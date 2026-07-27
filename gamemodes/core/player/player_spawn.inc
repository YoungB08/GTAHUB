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
        Float:ang = SF_SPAWN_A;

    if(Player_GetClass(playerid) == TEAM_CIVILIAN) {
        if(Player_GetJob(playerid) == TYPE_NO_JOB) {
            Player_ShowJobMenu(playerid);
        }
    }

    SetPlayerPos(playerid, x, y, z);
    SetPlayerFacingAngle(playerid, ang);
    SetPlayerInterior(playerid, SF_SPAWN_INTERIOR);
    SetPlayerVirtualWorld(playerid, SF_SPAWN_WORLD);
    SetCameraBehindPlayer(playerid);

    Logger_Dbg(PLAYER_DEBUG_HANDLER, "[OnPlayerSpawn / player_spawn.inc] player spawned",
        Logger_P(playerid),
        Logger_S("spawn", "sf-fixed"),
        Logger_F("x", x),
        Logger_F("y", y),
        Logger_F("z", z)
    );
    return 1;
}
