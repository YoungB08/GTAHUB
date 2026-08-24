#include <YSI_Coding\y_hooks>

#define SF_SPAWN_X          (-1985.5433)
#define SF_SPAWN_Y          (137.1518)
#define SF_SPAWN_Z          (27.6875)
#define SF_SPAWN_A          (90.0000)
#define SF_SPAWN_WORLD      (0)
#define SF_SPAWN_INTERIOR   (0)

static
    bool:PlayerFirstSpawn[MAX_PLAYERS] = true
;
forward OnSpawnUnfreezePlayer(playerid);
public OnSpawnUnfreezePlayer(playerid)
{
    if (IsPlayerConnected(playerid) && Char_IsActive(playerid))
    {
        TogglePlayerControllable(playerid, true);
        SetCameraBehindPlayer(playerid);
    }
    return 1;
}

hook OnPlayerConnect(playerid) {
    PlayerFirstSpawn[playerid] = true;
    return 1;
}

hook OnPlayerSpawn(playerid) {
    if (!Char_IsActive(playerid)) {
        return 1;
    }

    if (CallLocalFunction("Admin_IsPlayerSpectating", "i", playerid)) {
        return 1;
    }

    // Execute this when player spawns for the first time.
    if(PlayerFirstSpawn[playerid]) {
        Logger_Dbg(SERVER_DEBUG_HANDLER, "[OnPlayerSpawn / player_spawn.inc] calling OnPlayerFirstSpawn");
        CallLocalFunction("OnPlayerFirstSpawn", "i", playerid);
        PlayerFirstSpawn[playerid] = false;
    }

    // Lấy vị trí từ nhân vật IC làm mặc định
    new Float:x, Float:y, Float:z, Float:ang, world = 0, interior = 0;
    Char_GetPos(playerid, x, y, z, ang);

    // Ưu tiên vị trí lưu gần nhất trong player_position-save nếu hợp lệ
    if(PlayerPosition_HasSaved(playerid)) {
        new Float:sx, Float:sy, Float:sz, Float:sa, sw, sint;
        PlayerPosition_GetSaved(playerid, sx, sy, sz, sa, sw, sint);
        if (PlayerPosition_IsValid(sx, sy, sz)) {
            x = sx;
            y = sy;
            z = sz;
            ang = sa;
            world = sw;
            interior = sint;
        }
    }

    // Nếu tọa độ vẫn không hợp lệ -> reset về Bệnh viện Los Santos
    if(!PlayerPosition_IsValid(x, y, z)) {
        x = CHAR_DEFAULT_SPAWN_X;
        y = CHAR_DEFAULT_SPAWN_Y;
        z = CHAR_DEFAULT_SPAWN_Z;
        ang = CHAR_DEFAULT_SPAWN_A;
        world = 0;
        interior = 0;
    }


    // Đóng băng tạm thời trong 1.5 giây để nạp va chạm bản đồ & streamer (tránh rơi tự do vào void)
    TogglePlayerControllable(playerid, false);
    SetPlayerInterior(playerid, interior);
    SetPlayerVirtualWorld(playerid, world);
    SetPlayerPos(playerid, x, y, z);
    SetPlayerFacingAngle(playerid, ang);
    Streamer_UpdateEx(playerid, x, y, z, world, interior, -1, 1, 0);
    SetCameraBehindPlayer(playerid);

    SetTimerEx("OnSpawnUnfreezePlayer", 1500, false, "i", playerid);

    Logger_Dbg(PLAYER_DEBUG_HANDLER, "[OnPlayerSpawn / player_spawn.inc] player spawned",
        Logger_P(playerid),
        Logger_F("x", x),
        Logger_F("y", y),
        Logger_F("z", z)
    );
    return 1;
}
