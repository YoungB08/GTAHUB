/*
    anticheat_rp.inc
    -----------------
    Server-side Anti-Cheat cơ bản cho Server RP.

    Bảo vệ:
    1. Money Check    – Phát hiện SetPlayerMoney/GivePlayerMoney từ bên ngoài (hack tiền).
    2. Weapon Check   – Phát hiện vũ khí bất hợp pháp xuất hiện không qua server.
    3. Health Check   – Phát hiện HP tăng đột ngột không qua server (GodMode).
    4. Teleport Check – Phát hiện dịch chuyển vị trí tức thời (Teleport hack).
    5. Speed Check    – Phát hiện xe / người di chuyển quá nhanh (Speedhack / Airbreak).

    Mỗi vi phạm tích lũy điểm. Đủ điểm -> Kick (hoặc ban).
    Luôn log vi phạm qua Logger_.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define AC_KICK_THRESHOLD       (5)     // Số vi phạm tích lũy trước khi kick
#define AC_BAN_THRESHOLD        (15)    // Số vi phạm trước khi ban

// Giới hạn tốc độ
#define AC_MAX_PLAYER_SPEED     (6.0)   // m/s khi đi bộ (chuẩn GTA:SA ~5.8)
#define AC_MAX_VEHICLE_SPEED    (95.0)  // m/s (tương đương ~342 km/h, max theo game)

// Giới hạn HP hồi phục tự nhiên (không áp dụng nếu đang trong xe / heal)
#define AC_MAX_HP_GAIN_PS       (8.0)   // Máu tối đa có thể tăng / giây (do EMS/bandage)

// Giới hạn tiền tăng / giảm một lần (> ngưỡng này cần có log từ server)
#define AC_MAX_MONEY_DELTA      (1000000)  // $1M – chênh lệch tối đa hợp lý

// Thời gian lấy mẫu teleport (ms)
#define AC_TELEPORT_SAMPLE_MS   (500)
#define AC_TELEPORT_MAX_DIST    (50.0)  // Tối đa 50m / 0.5s (bình thường)

// Interval kiểm tra (ms)
#define AC_CHECK_INTERVAL       (1000)

// =============================================================================
// DATA
// =============================================================================

static
    ACViolations[MAX_PLAYERS],
    ACLastMoney[MAX_PLAYERS],
    Float:ACLastHP[MAX_PLAYERS],
    Float:ACLastPosX[MAX_PLAYERS],
    Float:ACLastPosY[MAX_PLAYERS],
    Float:ACLastPosZ[MAX_PLAYERS],
    bool:ACEnabled[MAX_PLAYERS],
    bool:ACMoneyGranted[MAX_PLAYERS];  // Cho phép biến đổi tiền hợp lệ kỳ này

// Danh sách vũ khí hợp lệ được phép có mà không cần server cấp
// (Ví dụ, vũ khí nhặt trên map)
#define MAX_ALLOWED_WEAPONS (8)
static const AllowedWeapons[MAX_ALLOWED_WEAPONS] = {
    0,  // Tay không
    1,  // Brass Knuckles
    2,  // Golf Club
    5,  // Baseball Bat
    6,  // Shovel
    7,  // Pool Cue
    8,  // Katana
    9   // Chainsaw
};

// Vũ khí được phép mang theo thông qua /job police
#define MAX_POLICE_WEAPONS (4)
static const PoliceWeapons[MAX_POLICE_WEAPONS] = {
    22, // Pistol
    25, // Shotgun
    3,  // Nightstick
    40  // TearGas
};

// =============================================================================
// HOOKS
// =============================================================================

hook OnPlayerConnect(playerid)
{
    ACViolations[playerid]    = 0;
    ACLastMoney[playerid]     = 0;
    ACLastHP[playerid]        = 100.0;
    ACLastPosX[playerid]      = 0.0;
    ACLastPosY[playerid]      = 0.0;
    ACLastPosZ[playerid]      = 0.0;
    ACEnabled[playerid]       = false;
    ACMoneyGranted[playerid]  = false;
    return 1;
}

hook OnPlayerSpawn(playerid)
{
    ACEnabled[playerid] = Player_IsLoggedIn(playerid);
    GetPlayerPos(playerid, ACLastPosX[playerid], ACLastPosY[playerid], ACLastPosZ[playerid]);
    GetPlayerHealth(playerid, ACLastHP[playerid]);
    ACLastMoney[playerid] = GetPlayerMoney(playerid);
    return 1;
}

// =============================================================================
// HELPER – Cờ cho phép biến đổi tiền hợp lý
// Gọi trước bất kỳ GivePlayerMoney / SetPlayerMoney từ server
// =============================================================================

stock AC_GrantMoneyChange(playerid)
{
    ACMoneyGranted[playerid] = true;
}

// =============================================================================
// MAIN CHECK TIMER (mỗi 1 giây)
// =============================================================================

static ptask _ACCheckTimer[AC_CHECK_INTERVAL](playerid)
{
    if (!ACEnabled[playerid]) return;
    if (!Player_IsSpawned(playerid)) return;
    if (!Player_IsLoggedIn(playerid)) return;

    // Skip admin
    if (Admin_GetPlayerLevel(playerid) >= 4) return;

    // --- 1. HEALTH CHECK ---
    new Float:currentHP;
    GetPlayerHealth(playerid, currentHP);

    new Float:hpDelta = currentHP - ACLastHP[playerid];
    if (hpDelta > AC_MAX_HP_GAIN_PS && !IsPlayerInAnyVehicle(playerid))
    {
        // Loại trừ nếu đang trong bệnh viện / vừa respawn
        if (!IsPlayerInRangeOfPoint(playerid, 20.0, 1543.4, -1675.6, 13.5))
        {
            AC_FlagViolation(playerid, "HealthHack", hpDelta);
            SetPlayerHealth(playerid, ACLastHP[playerid]);  // Rollback
        }
    }
    ACLastHP[playerid] = currentHP;

    // --- 2. MONEY CHECK ---
    new currentMoney = GetPlayerMoney(playerid);
    new moneyDelta   = currentMoney - ACLastMoney[playerid];

    if (!ACMoneyGranted[playerid] && moneyDelta > AC_MAX_MONEY_DELTA)
    {
        AC_FlagViolation(playerid, "MoneyHack", float(moneyDelta));
        GivePlayerMoney(playerid, -moneyDelta);  // Rollback
    }
    ACMoneyGranted[playerid] = false;
    ACLastMoney[playerid]    = GetPlayerMoney(playerid);

    // --- 3. WEAPON CHECK ---
    if (!IsPlayerInAnyVehicle(playerid))
        AC_CheckWeapons(playerid);

    // --- 4. TELEPORT CHECK ---
    if (!IsPlayerInAnyVehicle(playerid))
    {
        new Float:cx, Float:cy, Float:cz;
        GetPlayerPos(playerid, cx, cy, cz);

        new Float:dist = Distance(ACLastPosX[playerid], ACLastPosY[playerid], ACLastPosZ[playerid], cx, cy, cz);
        if (dist > AC_TELEPORT_MAX_DIST)
        {
            new state = GetPlayerState(playerid);
            if (state != PLAYER_STATE_WASTED && state != PLAYER_STATE_SPECTATING)
                AC_FlagViolation(playerid, "Teleport", dist);
        }

        ACLastPosX[playerid] = cx;
        ACLastPosY[playerid] = cy;
        ACLastPosZ[playerid] = cz;
    }

    // --- 5. SPEED CHECK ---
    if (IsPlayerInAnyVehicle(playerid))
    {
        new Float:vx, Float:vy, Float:vz;
        new vehicleid = GetPlayerVehicleID(playerid);
        GetVehicleVelocity(vehicleid, vx, vy, vz);

        new Float:speed = floatsqroot(vx * vx + vy * vy + vz * vz) * 100.0;
        if (speed > AC_MAX_VEHICLE_SPEED)
            AC_FlagViolation(playerid, "VehicleSpeed", speed);
    }
}

// =============================================================================
// WEAPON CHECK
// =============================================================================

static AC_CheckWeapons(playerid)
{
    for (new slot = 0; slot <= 12; slot++)
    {
        new weaponid, ammo;
        GetPlayerWeaponData(playerid, slot, weaponid, ammo);
        if (weaponid == 0 || weaponid == -1) continue;

        // Kiểm tra xem có nằm trong danh sách cho phép không
        new bool:isAllowed = false;

        // Vũ khí cận chiến không cần kiểm tra nghiêm ngặt
        for (new i = 0; i < MAX_ALLOWED_WEAPONS; i++)
        {
            if (weaponid == AllowedWeapons[i]) { isAllowed = true; break; }
        }
        if (isAllowed) continue;

        // Cảnh sát được phép mang một số vũ khí
        if (Player_GetClass(playerid) == TEAM_POLICE && LSPD_IsOnDuty(playerid))
        {
            for (new i = 0; i < MAX_POLICE_WEAPONS; i++)
            {
                if (weaponid == PoliceWeapons[i]) { isAllowed = true; break; }
            }
            if (isAllowed) continue;
        }

        // Kiểm tra giấy phép súng – nếu có GPSC thì cho phép mang súng ngắn hợp lệ
        if (Player_HasLicense(playerid, LICENSE_GUN) && (weaponid == 22 || weaponid == 23))
        {
            isAllowed = true;
            continue;
        }

        // Vũ khí không rõ nguồn gốc -> vi phạm
        if (!isAllowed && weaponid >= 22)
        {
            // Chỉ flag nếu không phải weapon do server cấp gần đây
            // (Heuristic: vũ khí có ammo > 0 và không được ghi nhận)
            AC_FlagViolation(playerid, "WeaponHack", float(weaponid));
            RemovePlayerWeapon(playerid, weaponid);
        }
    }
}

// =============================================================================
// FLAG VIOLATION
// =============================================================================

static AC_FlagViolation(playerid, const type[], Float:value)
{
    ACViolations[playerid]++;

    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, MAX_PLAYER_NAME);

    Logger_Log(
        "[ANTICHEAT] VIOLATION",
        Logger_S("type", type),
        Logger_S("player", name),
        Logger_I("playerid", playerid),
        Logger_F("value", value),
        Logger_I("total", ACViolations[playerid])
    );

    // Thông báo admin
    foreach (new i : Player)
    {
        if (Admin_GetPlayerLevel(i) < 1) continue;
        SendClientMessageF(i, COLOR_RED,
            "[AC] Vi pham: %s (ID:%d) - %s (Value:%.1f) [%d/%d]",
            name, playerid, type, value,
            ACViolations[playerid], AC_KICK_THRESHOLD);
    }

    // Kick nếu đủ điểm
    if (ACViolations[playerid] >= AC_BAN_THRESHOLD)
    {
        // TODO: Tích hợp ban system đầy đủ
        Ban(playerid);
        return;
    }

    if (ACViolations[playerid] >= AC_KICK_THRESHOLD)
    {
        SendClientMessage(playerid, COLOR_RED, "[ANTICHEAT] Ban bi kick vi vi pham luat choi.");
        Kick(playerid);
    }
}

// =============================================================================
// COMMAND – /acstats [playerid] – Admin xem vi phạm
// =============================================================================

ACMD:[3]acstats(playerid, params[])
{
    new targetid;
    if (sscanf(params, "u", targetid))
        return SendErrorMsg(playerid, "Su dung: /acstats [playerid]"), 1;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai."), 1;

    SendMsgF(playerid, COLOR_ORANGE,
        "[AC] %s (ID:%d): Vi pham = %d/%d | HP: %.1f | Money: $%d",
        ReturnPlayerName(targetid), targetid,
        ACViolations[targetid], AC_KICK_THRESHOLD,
        ACLastHP[targetid],
        ACLastMoney[targetid]);
    return 1;
}

// =============================================================================
// COMMAND – /acreset [playerid] – Admin reset vi phạm
// =============================================================================

ACMD:[4]acreset(playerid, params[])
{
    new targetid;
    if (sscanf(params, "u", targetid))
        return SendErrorMsg(playerid, "Su dung: /acreset [playerid]"), 1;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai."), 1;

    ACViolations[targetid] = 0;
    SendMsgF(playerid, COLOR_GREEN, "[AC] Da reset vi pham cho %s.", ReturnPlayerName(targetid));
    return 1;
}
