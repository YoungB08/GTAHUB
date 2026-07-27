
//    Written by s4marium

#include <YSI_Coding\y_hooks>

#define DUMPSTER_SEARCH_TIME        (7)         // Time to search dumpster in seconds
#define DUMPSTER_COOLDOWN_TIME      (600)       // Cooldown time in seconds (10 minutes)
#define DUMPSTER_DETECTION_RANGE    (5.5)      // Range to detect player near dumpster

// Dumpster data enumeration
enum E_DUMPSTER_DATA {
    Float:DumpsterX,
    Float:DumpsterY,
    Float:DumpsterZ,
    DumpsterLastSearched,           // UNIX timestamp of last search
    Text3D:DumpsterLabel
};

// Reward item data
enum E_REWARD_DATA {
    RewardType,                    // 0 = money, 1 = item
    RewardID,                      // Item weapon ID or money amount
    RewardName[32],
    RewardChance                   // Chance percentage (0-100)
};

// Dumpster rewards table
static const RewardTable[][E_REWARD_DATA] = {
    {0, 1000,  "Tien mat",           15},    // 1000 cash
    {0, 2000,  "Tien mat",           12},    // 2000 cash
    {0, 3000,  "Tien mat",           10},    // 3000 cash
    {0, 4000,  "Tien mat",           8},     // 4000 cash
    {0, 5000,  "Tien mat",           5},     // 5000 cash
    {1, 24,    "Desert Eagle",       10},    // Deagle
    {1, 25,    "Shotgun",            8},     // Combat Shotgun
    {1, 22,    "Pistol",             15},    // 9mm Pistol
    {1, 16,    "Grenade",            7},     // Grenade
    {1, 18,    "Molotov",            4},     // Molotov Cocktail
    {0, 7500,  "Tien mat",           3},     // 7500 cash (rare)
    {0, 10000, "Tien mat",           2},     // 10000 cash (very rare)
    {1, 29,    "Micro SMG",          1}      // Micro SMG
};

// Dumpster locations
static const Float:DumpsterLocations[][3] = {
    {-169.9766, 1027.1953, 19.4453},
    {-160.2656, 1122.5391, 19.5391},
    {-149.8516, 1133.7656, 19.5391},
    {-170.1719, 1169.0547, 19.5391},
    {-218.0312, 1164.9219, 19.5391},
    {-149.8203, 1164.1094, 19.5391},
    {-162.0938, 1175.1406, 19.5391},
    {-155.6953, 1234.4219, 19.4766},
    {-123.8125, 1079.3984, 19.5000},
    {-90.6016, 1128.2188, 19.4609},
    {-88.8594, 1165.3828, 19.4609},
    {-44.3047, 1179.2734, 19.2422},
    {-24.8203, 1165.4062, 19.2422},
    {776.8281, 1866.1562, 3.8906},
    {338.0547, 39.8438, 6.2656},
    {339.7188, 52.9844, 3.2656},
    {341.4219, 55.0078, 3.2656},
    {1353.1016, 201.7578, 19.2656},
    {1353.5781, 204.2266, 19.2656},
    {244.1250, -313.1250, 1.2734},
    {251.4453, -262.5469, 1.2734},
    {254.0000, -263.2578, 1.2734},
    {306.0625, -255.1016, 1.2578},
    {308.6719, -255.0938, 1.2578}
};

static
    DumpsterData[sizeof(DumpsterLocations)][E_DUMPSTER_DATA],
    PlayerSearchingDumpster[MAX_PLAYERS] = {-1, ...},
    PlayerSearchProgress[MAX_PLAYERS],
    PlayerSearchTimer[MAX_PLAYERS],
    PlayerBar:DumpsterProgressBar[MAX_PLAYERS],
    PlayerText:DumpsterSearchTitle[MAX_PLAYERS];

// Timer for updating search progress
forward DumpsterSearchUpdate(playerid);

// Initialize hook - called when gamemode starts
hook OnGameModeInit() {
    new label[128];

    for(new i = 0; i < sizeof(DumpsterLocations); i++) {
        DumpsterData[i][DumpsterX] = DumpsterLocations[i][0];
        DumpsterData[i][DumpsterY] = DumpsterLocations[i][1];
        DumpsterData[i][DumpsterZ] = DumpsterLocations[i][2];
        DumpsterData[i][DumpsterLastSearched] = 0;

        format(label, sizeof(label), "Thung rac\n{FFFFFF}\nNhan Y de luc tim");
        DumpsterData[i][DumpsterLabel] = CreateDynamic3DTextLabel(
            label,
            0x2ECC71FF,
            DumpsterData[i][DumpsterX],
            DumpsterData[i][DumpsterY],
            DumpsterData[i][DumpsterZ] + 1.0,
            15.0,
            .testlos = 1
        );
    }

    printf("[THUNG RAC] Da khoi tao %d vi tri thung rac", sizeof(DumpsterLocations));
    return 1;
}

// Reset player data on connect
hook OnPlayerConnect(playerid) {
    PlayerSearchingDumpster[playerid] = -1;
    PlayerSearchProgress[playerid] = 0;

    DumpsterProgressBar[playerid] = CreatePlayerProgressBar(playerid, 272.000000, 363.000000, 107.500000, 9.199998, -1347440641, 100.0000, 0);
    HidePlayerProgressBar(playerid, DumpsterProgressBar[playerid]);

    DumpsterSearchTitle[playerid] = CreatePlayerTextDraw(playerid, 270.000000, 350.000000, "Dang luc thung rac...");
    PlayerTextDrawFont(playerid, DumpsterSearchTitle[playerid], TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, DumpsterSearchTitle[playerid], 0.249999, 1.100000);
    PlayerTextDrawColor(playerid, DumpsterSearchTitle[playerid], -1);
    PlayerTextDrawSetOutline(playerid, DumpsterSearchTitle[playerid], 1);
    PlayerTextDrawSetProportional(playerid, DumpsterSearchTitle[playerid], true);
    PlayerTextDrawHide(playerid, DumpsterSearchTitle[playerid]);
    return 1;
}

// Clean up on player disconnect
hook OnPlayerDisconnect(playerid, reason) {
    if(PlayerSearchingDumpster[playerid] != -1) {
        KillTimer(PlayerSearchTimer[playerid]);
        PlayerSearchingDumpster[playerid] = -1;
        PlayerSearchProgress[playerid] = 0;
    }
    return 1;
}

// Update 3D text labels based on dumpster availability
hook OnPlayerUpdate(playerid) {
    if(!IsPlayerConnected(playerid) || IsPlayerNPC(playerid)) {
        return 1;
    }

    // Don't update labels while searching
    if(PlayerSearchingDumpster[playerid] != -1) {
        return 1;
    }

    // Check proximity to dumpsters and update labels
    new currentTime = gettime();

    for(new i = 0; i < sizeof(DumpsterLocations); i++) {
        if(IsPlayerInRangeOfPoint(playerid, DUMPSTER_DETECTION_RANGE,
            DumpsterData[i][DumpsterX],
            DumpsterData[i][DumpsterY],
            DumpsterData[i][DumpsterZ])) {

            new labelText[128];
            new timeDiff = currentTime - DumpsterData[i][DumpsterLastSearched];

            // Check if dumpster is on cooldown
            if(timeDiff < DUMPSTER_COOLDOWN_TIME) {
                new timeRemaining = DUMPSTER_COOLDOWN_TIME - timeDiff;
                new minutes = timeRemaining / 60;
                new seconds = timeRemaining % 60;

                format(labelText, sizeof(labelText),
                    "Thung rac\n{FFFFFF}Co the luc sau: {FF0000}%02d:%02d",
                    minutes, seconds
                );

                UpdateDynamic3DTextLabelText(DumpsterData[i][DumpsterLabel], 0xFF0000FF, labelText);
            } else {
                format(labelText, sizeof(labelText),
                    "Thung rac\n{FFFFFF}\n{00FF00}Nhan Y de luc tim"
                );

                UpdateDynamic3DTextLabelText(DumpsterData[i][DumpsterLabel], 0x2ECC71FF, labelText);
            }
        }
    }

    return 1;
}

// Handle Y key press for dumpster search
hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if((newkeys & KEY_YES) && !(oldkeys & KEY_YES)) {
        // Player is already searching
        if(PlayerSearchingDumpster[playerid] != -1) {
            return SendMsgF(playerid, 0xE74C3CFF, "LOI: {FFFFFF}Ban dang luc thung rac roi!");
        }

        new id = ClosestDumpster(playerid);
        if(id == -1) {
            return 1;  // Not near any dumpster, don't send error
        }

        // Check if THIS SPECIFIC dumpster is on cooldown
        new currentTime = gettime();
        new timeDiff = currentTime - DumpsterData[id][DumpsterLastSearched];

        if(timeDiff < DUMPSTER_COOLDOWN_TIME) {
            new timeRemaining = DUMPSTER_COOLDOWN_TIME - timeDiff;
            new minutes = timeRemaining / 60;
            new seconds = timeRemaining % 60;

            return SendMsgF(playerid, 0xE74C3CFF, "LOI: {FFFFFF}Thung rac nay co the luc sau {00FF00}%02d:%02d", minutes, seconds);
        }

        // Start searching
        StartDumpsterSearch(playerid, id);
        return 1;
    }

    return 1;
}

// Start dumpster search for a player
static StartDumpsterSearch(playerid, dumpsterid) {
    PlayerSearchingDumpster[playerid] = dumpsterid;
    PlayerSearchProgress[playerid] = 0;

    SendMsgF(playerid, 0x2ECC71FF, "THUNG RAC: {FFFFFF}Dang luc thung rac... Dung di chuyen!");

    TogglePlayerControllable(playerid, false);

    ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.0, true, false, false, false, 0, 1);

    PlayerTextDrawShow(playerid, DumpsterSearchTitle[playerid]);
    ShowPlayerProgressBar(playerid, DumpsterProgressBar[playerid]);
    SetPlayerProgressBarValue(playerid, DumpsterProgressBar[playerid], 0.0);

    // Start timer for progress updates (every 1 second for DUMPSTER_SEARCH_TIME seconds)
    PlayerSearchTimer[playerid] = SetTimerEx("DumpsterSearchUpdate", 1000, true, "i", playerid);
}

// Update search progress (called every second)
public DumpsterSearchUpdate(playerid) {
    if(PlayerSearchingDumpster[playerid] == -1) {
        return 1;
    }

    new dumpsterid = PlayerSearchingDumpster[playerid];
    new Float:x = DumpsterData[dumpsterid][DumpsterX];
    new Float:y = DumpsterData[dumpsterid][DumpsterY];
    new Float:z = DumpsterData[dumpsterid][DumpsterZ];

    if(!IsPlayerInRangeOfPoint(playerid, DUMPSTER_DETECTION_RANGE * 2, x, y, z)) {
        CancelDumpsterSearch(playerid, "Ban da di qua xa thung rac!");
        return 1;
    }

    PlayerSearchProgress[playerid]++;

    // Update progress bar (0-100 over DUMPSTER_SEARCH_TIME seconds)
    new Float:progressPercent = (Float:PlayerSearchProgress[playerid] / Float:DUMPSTER_SEARCH_TIME) * 100.0;
    if(progressPercent > 100.0) progressPercent = 100.0;
    SetPlayerProgressBarValue(playerid, DumpsterProgressBar[playerid], progressPercent);

    // Check if search is complete
    if(PlayerSearchProgress[playerid] >= DUMPSTER_SEARCH_TIME) {
        CompleteDumpsterSearch(playerid);
        return 1;
    }

    return 1;
}

// Complete the dumpster search and give reward
static CompleteDumpsterSearch(playerid) {
    if(PlayerSearchingDumpster[playerid] == -1) {
        return;
    }

    new dumpsterid = PlayerSearchingDumpster[playerid];

    KillTimer(PlayerSearchTimer[playerid]);

    ClearAnimations(playerid);
    TogglePlayerControllable(playerid, true);
    PlayerTextDrawHide(playerid, DumpsterSearchTitle[playerid]);
    HidePlayerProgressBar(playerid, DumpsterProgressBar[playerid]);

    // Random reward
    new rand = random(100);
    new cumulativeChance = 0;
    new rewardType = 0;
    new rewardValue = 0;
    new rewardName[32] = "Khong ro";

    for(new i = 0; i < sizeof(RewardTable); i++) {
        cumulativeChance += RewardTable[i][RewardChance];
        if(rand <= cumulativeChance) {
            rewardType = RewardTable[i][RewardType];
            rewardValue = RewardTable[i][RewardID];
            strmid(rewardName, RewardTable[i][RewardName], 0, strlen(RewardTable[i][RewardName]));
            break;
        }
    }

    // Give reward
    if(rewardType == 0) {
        GivePlayerMoney(playerid, rewardValue);
        SendMsgF(playerid, 0x2ECC71FF, "THUNG RAC: {FFFFFF}Ban tim thay {00FF00}$%d {FFFFFF}trong thung rac!", rewardValue);
    } else {
        GivePlayerWeapon(playerid, rewardValue, 30);
        SendMsgF(playerid, 0x2ECC71FF, "THUNG RAC: {FFFFFF}Ban tim thay {FFFF00}%s {FFFFFF}trong thung rac!", rewardName);
    }

    // Update dumpster cooldown (this specific dumpster goes on cooldown)
    DumpsterData[dumpsterid][DumpsterLastSearched] = gettime();

    // Reset player search data
    PlayerSearchingDumpster[playerid] = -1;
    PlayerSearchProgress[playerid] = 0;
}

// Cancel dumpster search
static CancelDumpsterSearch(playerid, const cancelReason[]) {
    if(PlayerSearchingDumpster[playerid] == -1) {
        return;
    }

    KillTimer(PlayerSearchTimer[playerid]);

    ClearAnimations(playerid);
    TogglePlayerControllable(playerid, true);
    PlayerTextDrawHide(playerid, DumpsterSearchTitle[playerid]);
    HidePlayerProgressBar(playerid, DumpsterProgressBar[playerid]);

    PlayerSearchingDumpster[playerid] = -1;
    PlayerSearchProgress[playerid] = 0;

    SendMsgF(playerid, 0xE74C3CFF, "LOI: {FFFFFF}%s", cancelReason);
}

// Find closest dumpster
static ClosestDumpster(playerid) {
    new closestid = -1;
    new Float:closestdist = 999999.0;

    for(new i = 0; i < sizeof(DumpsterLocations); i++) {
        new Float:dist = GetPlayerDistanceFromPoint(playerid,
            DumpsterData[i][DumpsterX],
            DumpsterData[i][DumpsterY],
            DumpsterData[i][DumpsterZ]
        );

        if(dist < closestdist && dist <= DUMPSTER_DETECTION_RANGE) {
            closestdist = dist;
            closestid = i;
        }
    }

    return closestid;
}

// Command to search dumpsters
CMD:testse(playerid, params[]) {
    if(IsPlayerInAnyVehicle(playerid)) {
        return SendMsgF(playerid, 0xE74C3CFF, "LOI: {FFFFFF}Ban khong the dung lenh nay khi dang o trong xe.");
    }

    if(PlayerSearchingDumpster[playerid] != -1) {
        return SendMsgF(playerid, 0xE74C3CFF, "LOI: {FFFFFF}Ban dang luc thung rac roi!");
    }

    new id = ClosestDumpster(playerid);
    if(id == -1) {
        return SendMsgF(playerid, 0xE74C3CFF, "LOI: {FFFFFF}Ban khong o gan thung rac nao.");
    }

    // Check if THIS SPECIFIC dumpster is on cooldown
    new currentTime = gettime();
    new timeDiff = currentTime - DumpsterData[id][DumpsterLastSearched];

    if(timeDiff < DUMPSTER_COOLDOWN_TIME) {
        new timeRemaining = DUMPSTER_COOLDOWN_TIME - timeDiff;
        new minutes = timeRemaining / 60;
        new seconds = timeRemaining % 60;

        return SendMsgF(playerid, 0xE74C3CFF, "LOI: {FFFFFF}Thung rac nay co the luc sau {00FF00}%02d:%02d", minutes, seconds);
    }

    // Start searching
    StartDumpsterSearch(playerid, id);
    return 1;
}
