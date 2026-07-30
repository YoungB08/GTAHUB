#include <YSI_Coding\y_hooks>

stock bool:Player_IsValidSkin(skinid)
{
    if (skinid < 0 || skinid > 311 || skinid == 74) return false;
    return true;
}

static
    PlayerSkin[MAX_PLAYERS] = -1,
    bool:PlayerNew[MAX_PLAYERS] = false,
    Statement: stmt_updatePlayerSkin,
    Statement: stmt_loadPlayerSkin;

hook OnMySQLConnected() {
    stmt_updatePlayerSkin = MySQL_PrepareStatement(MySQL_GetHandle(), "UPDATE player_stats SET skin = ? WHERE u_id = ?");
    stmt_loadPlayerSkin = MySQL_PrepareStatement(MySQL_GetHandle(), "SELECT skin FROM player_stats WHERE u_id = ?");
    return 1;
}

hook OnPlayerRegister(playerid) {
    PlayerNew[playerid] = true;
    return 1;
}

hook OnPlayerConnect(playerid) {
    PlayerSkin[playerid] = -1;
    PlayerNew[playerid] = false;
    return 1;
}

hook OnPlayerLogin(playerid) {
    if (Player_GetAccountID(playerid) <= 0) return 1;

    inline const OnSkinLoad() {
        new
            playerSkin;

        MySQL_BindResultInt(stmt_loadPlayerSkin, 0, playerSkin);

        if(MySQL_Statement_FetchRow(stmt_loadPlayerSkin)) {
            Player_SetSkin(playerid, playerSkin);

            Logger_Dbg(PLAYER_DEBUG_HANDLER, "[OnPlayerLogin / player_skin.inc] skin loaded",
                Logger_I("skinid", playerSkin),
                Logger_P(playerid)
            );
        }
    }
    MySQL_BindInt(stmt_loadPlayerSkin, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_loadPlayerSkin, using inline OnSkinLoad);
    return 1;
}

hook OnPlayerFirstSpawn(playerid) {
    if (Char_IsActive(playerid)) return 1;

    new
        skinid = Player_GetSkin(playerid);
    SetPlayerSkin(playerid, skinid);

    if(PlayerNew[playerid]) {
        Player_SetSkin(playerid, skinid, true);
    }
    return 1;
}

stock Player_SetSkin(playerid, skinid, bool:save = false) {
    if (!Player_IsValidSkin(skinid)) {
        skinid = 1;
    }
    PlayerSkin[playerid] = skinid;
    SetPlayerSkin(playerid, skinid);

    if(save && Player_GetAccountID(playerid) > 0) {
        MySQL_BindInt(stmt_updatePlayerSkin, 0, skinid);
        MySQL_BindInt(stmt_updatePlayerSkin, 1, Player_GetAccountID(playerid));
        MySQL_ExecuteThreaded(stmt_updatePlayerSkin);
    }
    return 1;
}

stock Player_GetSkin(playerid) {
    new skinid = PlayerSkin[playerid];
    if (!Player_IsValidSkin(skinid)) {
        return 1;
    }
    return skinid;
}

// (helper removed; revert to original behavior)