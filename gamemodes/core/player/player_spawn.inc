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
hook OnPlayerConnect(playerid) {
    PlayerFirstSpawn[playerid] = true;
    return 1;
}

hook OnPlayerSpawn(playerid) {
    if (CallLocalFunction("Admin_IsPlayerSpectating", "i", playerid)) {
        return 1;
    }

    // Execute this when player spawns for the first time.
    if(PlayerFirstSpawn[playerid]) {
        Logger_Dbg(SERVER_DEBUG_HANDLER, "[OnPlayerSpawn / player_spawn.inc] calling OnPlayerFirstSpawn");
        CallLocalFunction("OnPlayerFirstSpawn", "i", playerid);
        PlayerFirstSpawn[playerid] = false;
    }

    new
        Float:x = SF_SPAWN_X,
        Float:y = SF_SPAWN_Y,
        Float:z = SF_SPAWN_Z,
        Float:ang = SF_SPAWN_A,
        world = SF_SPAWN_WORLD,
        interior = SF_SPAWN_INTERIOR;

    // Load vị trí lưu cuối cùng trước khi thoát game của người chơi
    if(PlayerPosition_HasSaved(playerid)) {
        PlayerPosition_GetSaved(playerid, x, y, z, ang, world, interior);
    }

    if(Player_GetClass(playerid) == TEAM_CIVILIAN) {
        if(Player_GetJob(playerid) == TYPE_NO_JOB) {
            Player_ShowJobMenu(playerid);
        }
    }

    Interaction_LoadSafeTeleport(playerid, x, y, z, ang, world, interior, false, 500);

    // Bỏ đóng băng và khôi phục camera phía sau người chơi sau khi spawn
    TogglePlayerControllable(playerid, true);
    SetCameraBehindPlayer(playerid);

    Logger_Dbg(PLAYER_DEBUG_HANDLER, "[OnPlayerSpawn / player_spawn.inc] player spawned",
        Logger_P(playerid),
        Logger_F("x", x),
        Logger_F("y", y),
        Logger_F("z", z)
    );
    return 1;
}
