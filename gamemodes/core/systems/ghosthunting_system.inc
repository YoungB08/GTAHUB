// Ghost hunting puzzle
//  Written by s4marium

#include <YSI_Coding\y_hooks>

// Ghost Hunting System Configuration
#define MAX_GHOSTS_PER_PLAYER       (5)             // Maximum ghosts to hunt in one session
#define MAX_GHOST_LOCATIONS         (5)             // Available ghost spawn locations
#define GHOST_SESSION_START_HOUR    (20)            // Night starts at 8 PM (20:00)
#define GHOST_SESSION_END_HOUR      (6)             // Night ends at 6 AM (06:00)
#define GHOST_DETECTION_RANGE       (75.0)          // Detection range for weapon camera
#define GHOST_CAPTURE_REWARD        (500)          // Money reward per capture
#define GHOST_CAPTURE_XP            (50)           // XP reward per capture
#define GHOST_ESCAPE_DISTANCE       (5.0)           // Distance at which ghost escapes
#define GHOST_OBJECT_ID             (3092)          // Ghost object model ID
#define GHOST_LABEL_DISTANCE        (30.0)          // 3D text label distance
#define GHOST_LABEL_HEIGHT          (2.0)           // 3D text label height offset
#define GHOST_DETECTION_INCREMENT   (8)             // Detection percentage per scan

// Message Strings
#define MSG_NIGHT_START             "[GTAHUB]"C_WHITE" Troi da toi! Mua san ma da bat dau! Dung /starthunt de bat dau san."
#define MSG_HUNT_START              "[SAN MA]"C_WHITE" Phien san ma da bat dau! Hay bat ma truoc 6 gio sang!"
#define MSG_GHOST_LOCATION          "[SAN MA]"C_WHITE" Vi tri ma: %.2f, %.2f, %.2f"
#define MSG_ALL_GHOSTS_CAUGHT       "[SAN MA]"C_WHITE" Ban da bat du 5 con ma! Cho den 6 gio sang de ket thuc phien."
#define MSG_NO_MORE_GHOSTS          "[SAN MA]"C_WHITE" Khong con ma nao kha dung! Phien se som ket thuc."
#define MSG_GHOST_ESCAPED           "[SAN MA]"C_WHITE" Con ma da cam nhan duoc ban va bien mat!"
#define MSG_DETECTION_PROGRESS      "[SAN MA]"C_WHITE" Tiep tuc quet! Phat hien: %i%%"
#define MSG_NIGHT_ENDED             "[SAN MA]"C_WHITE" Mat troi da moc! Mua san ma da ket thuc!"

// Ghost States
enum ghostStateType {
    GHOST_STATE_INACTIVE = 0,
    GHOST_STATE_SPAWNED = 1,
    GHOST_STATE_DETECTED = 2,
    GHOST_STATE_CAUGHT = 3
}

// Ghost Data Structure
enum ghostDataType {
    ghostID,
    ghostStateType:ghostState,
    Float:ghostX,
    Float:ghostY,
    Float:ghostZ,
    ghostObject,
    Text3D:ghostLabel,
    Timer:ghostTimer
}

// Player Ghost Hunt Data
enum playerGhostDataType {
    ghostsHunted,
    ghostsCaught,
    Timer:ghostSessionTimer,
    currentGhostID,
    huntDetectionLevel,
    Float:sessionRewardMoney,
    Float:sessionRewardXP
}

// Static Arrays
static ghostData[MAX_GHOST_LOCATIONS][ghostDataType];
static playerGhostData[MAX_PLAYERS][playerGhostDataType];
static ghostCounter = 0;
static BitArray:playerHunting<MAX_PLAYERS>;
static lastNightHour = -1;
static Float:ghostLocations[MAX_GHOST_LOCATIONS][3] = {
    {1099.113525, -1414.430541, 13.624711},
    {1264.079345, -1338.927124, 13.271038},
    {1075.156738, -1281.489624, 13.382812},
    {819.469787, -1412.805786, 13.492121},
    {656.12895, -1405.314331, 13.398592}
};

// ========== INITIALIZATION ==========

hook OnGameModeInit() {
    GhostHunting_Initialize();
    repeat GhostHunting_CheckNightTime[1000]();
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerConnect(playerid) {
    GhostHunting_ResetPlayerData(playerid);
    return Y_HOOKS_CONTINUE_RETURN_1;
}

hook OnPlayerDisconnect(playerid, reason) {
    if(Bit_Get(playerHunting, playerid)) {
        stop playerGhostData[playerid][ghostSessionTimer];
        Bit_Set(playerHunting, playerid, false);
    }
    return Y_HOOKS_CONTINUE_RETURN_1;
}

GhostHunting_Initialize() {
    for(new i = 0; i < MAX_GHOST_LOCATIONS; i++) {
        ghostData[i][ghostState] = GHOST_STATE_INACTIVE;
        ghostData[i][ghostID] = i;
        ghostData[i][ghostObject] = INVALID_OBJECT_ID;
        ghostData[i][ghostLabel] = Text3D:INVALID_3DTEXT_ID;
    }
    ghostCounter = 0;
    lastNightHour = -1;
    print("[SAN MA] He thong da khoi tao thanh cong!");
    return 1;
}

// ========== HELPER FUNCTIONS ==========

stock bool:GhostHunting_IsValidSlot(slotID) {
    return (slotID >= 0 && slotID < MAX_GHOST_LOCATIONS);
}

stock bool:GhostHunting_IsNight(hour) {
    return (hour >= GHOST_SESSION_START_HOUR || hour < GHOST_SESSION_END_HOUR);
}

stock bool:GhostHunting_IsDaytime(hour) {
    return (hour >= GHOST_SESSION_END_HOUR && hour < GHOST_SESSION_START_HOUR);
}

stock GhostHunting_NotifyNightStart() {
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(IsPlayerConnected(i)) {
            SendClientMessage(i, COLOR_ORANGE, MSG_NIGHT_START);
        }
    }
}

stock GhostHunting_ResetPlayerData(playerid) {
    playerGhostData[playerid][ghostsHunted] = 0;
    playerGhostData[playerid][ghostsCaught] = 0;
    playerGhostData[playerid][huntDetectionLevel] = 0;
    playerGhostData[playerid][currentGhostID] = -1;
    playerGhostData[playerid][sessionRewardMoney] = 0.0;
    playerGhostData[playerid][sessionRewardXP] = 0.0;
    Bit_Set(playerHunting, playerid, false);
}

stock Float:GhostHunting_GetDistance(playerid, ghostIdx) {
    new Float:px, Float:py, Float:pz;
    new Float:gx, Float:gy, Float:gz;

    GetPlayerPos(playerid, px, py, pz);
    gx = ghostData[ghostIdx][ghostX];
    gy = ghostData[ghostIdx][ghostY];
    gz = ghostData[ghostIdx][ghostZ];

    return floatsqroot(
        ((px - gx) * (px - gx)) +
        ((py - gy) * (py - gy)) +
        ((pz - gz) * (pz - gz))
    );
}

// ========== NIGHTTIME DETECTION ==========

timer GhostHunting_CheckNightTime[1000]() {
    new hour, minute, second;
    gettime(hour, minute, second);

    new isNightTime = GhostHunting_IsNight(hour);

    if(isNightTime && lastNightHour != hour) {
        lastNightHour = hour;
        GhostHunting_NotifyNightStart();
        print("[SAN MA] Troi da toi! Nguoi choi co the bat dau phien san ma!");
    }

    if(!isNightTime && lastNightHour != -1) {
        lastNightHour = -1;
    }
}

// ========== GHOST SPAWN & MANAGEMENT ==========

stock GhostHunting_SpawnGhost(slotID) {
    if(!GhostHunting_IsValidSlot(slotID)) {
        return 0;
    }

    new Float:x = ghostLocations[slotID][0];
    new Float:y = ghostLocations[slotID][1];
    new Float:z = ghostLocations[slotID][2];

    ghostData[slotID][ghostX] = x;
    ghostData[slotID][ghostY] = y;
    ghostData[slotID][ghostZ] = z;
    ghostData[slotID][ghostState] = GHOST_STATE_SPAWNED;

    ghostData[slotID][ghostObject] = CreateDynamicObject(
        GHOST_OBJECT_ID,
        x, y, z,
        0, 0, 0
    );

    ghostData[slotID][ghostLabel] = CreateDynamic3DTextLabelEx(
        "[MA]\n"C_WHITE"Dung may anh de phat hien",
        COLOR_YELLOW,
        x, y, z + GHOST_LABEL_HEIGHT,
        GHOST_LABEL_DISTANCE,
        .testlos = 0
    );

    ghostCounter++;
    return 1;
}

stock GhostHunting_DespawnGhost(slotID) {
    if(!GhostHunting_IsValidSlot(slotID)) {
        return 0;
    }

    if(ghostData[slotID][ghostObject] != INVALID_OBJECT_ID) {
        DestroyDynamicObject(ghostData[slotID][ghostObject]);
        ghostData[slotID][ghostObject] = INVALID_OBJECT_ID;
    }

    if(ghostData[slotID][ghostLabel] != Text3D:INVALID_3DTEXT_ID) {
        DestroyDynamic3DTextLabel(ghostData[slotID][ghostLabel]);
        ghostData[slotID][ghostLabel] = Text3D:INVALID_3DTEXT_ID;
    }

    ghostData[slotID][ghostState] = GHOST_STATE_INACTIVE;
    ghostCounter--;
    return 1;
}

stock GhostHunting_GetInactiveGhostSlot() {
    new inactiveGhosts[MAX_GHOST_LOCATIONS];
    new inactiveCount = 0;

    for(new i = 0; i < MAX_GHOST_LOCATIONS; i++) {
        if(ghostData[i][ghostState] == GHOST_STATE_INACTIVE) {
            inactiveGhosts[inactiveCount++] = i;
        }
    }

    if(inactiveCount == 0) return -1;
    return inactiveGhosts[random(inactiveCount)];
}

// ========== GHOST DETECTION & HUNTING ==========

stock GhostHunting_StartHunt(playerid) {
    if(Bit_Get(playerHunting, playerid)) {
        return SendErrorMsg(playerid, "Ban dang trong phien san ma roi!");
    }

    new ghostSlot = GhostHunting_GetInactiveGhostSlot();

    if(ghostSlot == -1) {
        return SendErrorMsg(playerid, "Hien khong co ma nao de san!");
    }

    GhostHunting_SpawnGhost(ghostSlot);
    Bit_Set(playerHunting, playerid, true);
    playerGhostData[playerid][currentGhostID] = ghostSlot;
    playerGhostData[playerid][huntDetectionLevel] = 0;
    playerGhostData[playerid][ghostsHunted] = 0;
    playerGhostData[playerid][ghostsCaught] = 0;
    playerGhostData[playerid][sessionRewardMoney] = 0.0;
    playerGhostData[playerid][sessionRewardXP] = 0.0;
    playerGhostData[playerid][ghostSessionTimer] = repeat OnGhostSessionUpdate[500](playerid);

    SendClientMessage(playerid, COLOR_ORANGE, MSG_HUNT_START);

    new locMsg[128];
    format(locMsg, sizeof(locMsg), MSG_GHOST_LOCATION,
        ghostData[ghostSlot][ghostX],
        ghostData[ghostSlot][ghostY],
        ghostData[ghostSlot][ghostZ]);
    SendClientMessage(playerid, COLOR_ORANGE, locMsg);

    return 1;
}

stock GhostHunting_DetectGhost(playerid) {
    if(!Bit_Get(playerHunting, playerid)) {
        return 0;
    }

    new currentGhost = playerGhostData[playerid][currentGhostID];

    if(!GhostHunting_IsValidSlot(currentGhost)) {
        return 0;
    }

    new Float:detDist = GhostHunting_GetDistance(playerid, currentGhost);

    if(detDist <= GHOST_DETECTION_RANGE) {
        if(playerGhostData[playerid][huntDetectionLevel] < 100) {
            playerGhostData[playerid][huntDetectionLevel] += GHOST_DETECTION_INCREMENT;
        }

        if(playerGhostData[playerid][huntDetectionLevel] > 100) {
            playerGhostData[playerid][huntDetectionLevel] = 100;
        }

        return 1;
    }

    return 0;
}

stock GhostHunting_CaptureGhost(playerid) {
    if(!Bit_Get(playerHunting, playerid)) {
        return SendErrorMsg(playerid, "Ban khong trong phien san ma!");
    }

    new targetGhost = playerGhostData[playerid][currentGhostID];

    if(!GhostHunting_IsValidSlot(targetGhost)) {
        return SendErrorMsg(playerid, "Loi du lieu ma!");
    }

    new detectionLvl = playerGhostData[playerid][huntDetectionLevel];

    if(detectionLvl < 100) {
        new detMsg[128];
        format(detMsg, sizeof(detMsg),
            "[SAN MA]"C_WHITE" Ban can phat hien day du con ma truoc! Phat hien: %i%%",
            detectionLvl);
        SendClientMessage(playerid, COLOR_RED, detMsg);
        return 0;
    }

    playerGhostData[playerid][ghostsCaught]++;
    playerGhostData[playerid][ghostsHunted]++;
    playerGhostData[playerid][sessionRewardMoney] += GHOST_CAPTURE_REWARD;
    playerGhostData[playerid][sessionRewardXP] += GHOST_CAPTURE_XP;

    // Give rewards immediately to player
    GivePlayerMoney(playerid, GHOST_CAPTURE_REWARD);
    GivePlayerXp(playerid, GHOST_CAPTURE_XP);

    new rewardMsg[128];
    format(rewardMsg, sizeof(rewardMsg),
        "[SAN MA] "C_GREEN"THANH CONG! "C_WHITE"Ban da bat mot con ma! +$%i | +%i XP",
        GHOST_CAPTURE_REWARD, GHOST_CAPTURE_XP);
    SendClientMessage(playerid, COLOR_GREEN, rewardMsg);

    ApplyAnimation(playerid, "CAMERA", "camcrane_end", 4.0, false, false, false, true, 0, SYNC_ALL);
    GhostHunting_DespawnGhost(targetGhost);
    GhostHunting_SpawnNextGhost(playerid);

    return 1;
}

stock GhostHunting_EndSession(playerid) {
    if(!Bit_Get(playerHunting, playerid)) {
        return 0;
    }

    new sessionGhost = playerGhostData[playerid][currentGhostID];

    if(GhostHunting_IsValidSlot(sessionGhost)) {
        GhostHunting_DespawnGhost(sessionGhost);
    }

    stop playerGhostData[playerid][ghostSessionTimer];
    HideMessageBox(playerid, TYPE_MSGBOX);

    new totalGhosts = playerGhostData[playerid][ghostsCaught];

    if(totalGhosts > 0) {
        new endMsg[256];
        format(endMsg, sizeof(endMsg),
            "[SAN MA] "C_GREEN"PHIEN KET THUC! "C_WHITE"Ban da bat %i con ma va nhan day du thuong!",
            totalGhosts);
        SendClientMessage(playerid, COLOR_GREEN, endMsg);
    } else {
        SendClientMessage(playerid, COLOR_RED,
            "[SAN MA]"C_WHITE" Phien da ket thuc! Ban khong bat duoc con ma nao.");
    }

    Bit_Set(playerHunting, playerid, false);
    GhostHunting_ResetPlayerData(playerid);

    return 1;
}

stock GhostHunting_SpawnNextGhost(playerid) {
    if(playerGhostData[playerid][ghostsCaught] >= MAX_GHOSTS_PER_PLAYER) {
        SendClientMessage(playerid, COLOR_ORANGE, MSG_ALL_GHOSTS_CAUGHT);
        return 0;
    }

    new spawnSlot = GhostHunting_GetInactiveGhostSlot();

    if(spawnSlot == -1) {
        SendClientMessage(playerid, COLOR_RED, MSG_NO_MORE_GHOSTS);
        return 0;
    }

    GhostHunting_SpawnGhost(spawnSlot);
    playerGhostData[playerid][currentGhostID] = spawnSlot;
    playerGhostData[playerid][huntDetectionLevel] = 0;

    new ghostNum = playerGhostData[playerid][ghostsCaught] + 1;
    new numMsg[128];
    format(numMsg, sizeof(numMsg),
        "[SAN MA]"C_WHITE" Ma moi da xuat hien! Ma: %i/%i",
        ghostNum, MAX_GHOSTS_PER_PLAYER);
    SendClientMessage(playerid, COLOR_YELLOW, numMsg);

    new locMsg2[128];
    format(locMsg2, sizeof(locMsg2), MSG_GHOST_LOCATION,
        ghostData[spawnSlot][ghostX],
        ghostData[spawnSlot][ghostY],
        ghostData[spawnSlot][ghostZ]);
    SendClientMessage(playerid, COLOR_ORANGE, locMsg2);

    return 1;
}

// ========== SESSION UPDATE TIMER ==========

timer OnGhostSessionUpdate[500](playerid) {
    if(!Bit_Get(playerHunting, playerid)) {
        stop playerGhostData[playerid][ghostSessionTimer];
        return 0;
    }

    new hour, minute, second;
    gettime(hour, minute, second);

    if(GhostHunting_IsDaytime(hour)) {
        SendClientMessage(playerid, COLOR_ORANGE, MSG_NIGHT_ENDED);
        GhostHunting_EndSession(playerid);
        stop playerGhostData[playerid][ghostSessionTimer];
        return 0;
    }

    new timerGhost = playerGhostData[playerid][currentGhostID];

    if(GhostHunting_IsValidSlot(timerGhost)) {
        new Float:playerX, Float:playerY, Float:playerZ;
        new Float:ghostPosX, Float:ghostPosY, Float:ghostPosZ;

        GetPlayerPos(playerid, playerX, playerY, playerZ);
        ghostPosX = ghostData[timerGhost][ghostX];
        ghostPosY = ghostData[timerGhost][ghostY];
        ghostPosZ = ghostData[timerGhost][ghostZ];

        new Float:distToGhost = floatsqroot(
            ((playerX - ghostPosX) * (playerX - ghostPosX)) +
            ((playerY - ghostPosY) * (playerY - ghostPosY)) +
            ((playerZ - ghostPosZ) * (playerZ - ghostPosZ))
        );

        if(distToGhost < GHOST_ESCAPE_DISTANCE) {
            SendClientMessage(playerid, COLOR_RED, MSG_GHOST_ESCAPED);
            GhostHunting_DespawnGhost(timerGhost);

            if(!GhostHunting_SpawnNextGhost(playerid)) {
                GhostHunting_EndSession(playerid);
            }
            return 1;
        }
    }

    if(GetPlayerWeapon(playerid) == WEAPON_CAMERA && GhostHunting_IsValidSlot(timerGhost)) {
        GhostHunting_DetectGhost(playerid);
    }

    if(playerGhostData[playerid][ghostsCaught] < MAX_GHOSTS_PER_PLAYER) {
        new hudDetection = playerGhostData[playerid][huntDetectionLevel];
        new totalHunted = playerGhostData[playerid][ghostsHunted];
        new totalCaught = playerGhostData[playerid][ghostsCaught];
        new hudRewards = floatround(playerGhostData[playerid][sessionRewardMoney]);

        new hud[256];
        format(hud, sizeof(hud),
            "~w~Ma %i/%i~n~Phat hien: ~r~%i%%~w~ | Gio: ~g~%i:%02d~w~~n~Da bat: ~b~%i~w~ | Da nhan: ~g~$%i",
            totalHunted,
            MAX_GHOSTS_PER_PLAYER,
            hudDetection,
            hour,
            minute,
            totalCaught,
            hudRewards
        );

        MessageBox_ShowPersistent(playerid, TYPE_MSGBOX, "~y~[PHIEN SAN MA]", hud);
    } else {
        // All ghosts caught - hide the HUD popup
        HideMessageBox(playerid, TYPE_MSGBOX);
    }

    return 1;
}

// ========== WEAPON FIRE HANDLER ==========

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if((newkeys & KEY_FIRE) && !(oldkeys & KEY_FIRE)) {
        if(GetPlayerWeapon(playerid) == WEAPON_CAMERA && Bit_Get(playerHunting, playerid)) {
            new fireGhostID = playerGhostData[playerid][currentGhostID];

            if(!GhostHunting_IsValidSlot(fireGhostID)) {
                return 1;
            }

            new Float:playerDist = GhostHunting_GetDistance(playerid, fireGhostID);
            new fireDet = playerGhostData[playerid][huntDetectionLevel];

            if(playerDist <= GHOST_DETECTION_RANGE) {
                if(fireDet >= 100) {
                    SendClientMessage(playerid, COLOR_GREEN,
                        "[SAN MA]"C_WHITE" Da phat hien ma! Dang bat...");
                    ApplyAnimation(playerid, "CAMERA", "camcrane_end", 4.0, false, false, false, true, 0, SYNC_ALL);
                    GhostHunting_CaptureGhost(playerid);
                } else {
                    new fireMsg[128];
                    format(fireMsg, sizeof(fireMsg), MSG_DETECTION_PROGRESS, fireDet);
                    SendClientMessage(playerid, COLOR_YELLOW, fireMsg);
                }
            }
        }
    }

    return 1;
}

// ========== GETTERS / STATISTICS ==========

stock GhostHunting_GetGhostsCaught(playerid) {
    return playerGhostData[playerid][ghostsCaught];
}

stock GhostHunting_GetGhostsHunted(playerid) {
    return playerGhostData[playerid][ghostsHunted];
}

stock GhostHunting_GetDetectionLevel(playerid) {
    if(!Bit_Get(playerHunting, playerid)) {
        return 0;
    }
    return playerGhostData[playerid][huntDetectionLevel];
}

stock bool:GhostHunting_IsHunting(playerid) {
    return Bit_Get(playerHunting, playerid);
}

stock GhostHunting_GetGhostCount() {
    return ghostCounter;
}

// ========== COMMAND HANDLERS ==========
/*
stock Command_StartHunt(const playerid) {
    new hour, minute, second;
    gettime(hour, minute, second);

    if(!GhostHunting_IsNight(hour)) {
        SendErrorMsg(playerid, "Chi co the san ma vao ban dem (20:00 - 06:00)!");
        return 0;
    }

    if(Bit_Get(playerHunting, playerid)) {
        SendErrorMsg(playerid, "Ban dang trong phien san ma roi!");
        return 0;
    }

    GhostHunting_StartHunt(playerid);
    return 1;
}

stock Command_LeaveHunt(const playerid) {
    if(!Bit_Get(playerHunting, playerid)) {
        SendErrorMsg(playerid, "Ban hien khong trong phien san ma!");
        return 0;
    }

    SendClientMessage(playerid, COLOR_ORANGE,
        "[SAN MA]"C_WHITE" Ban da roi phien san ma.");
    GhostHunting_EndSession(playerid);
    return 1;
} */
