#include <YSI_Coding\y_hooks>

static
    Statement:stmt_SavePlayerPosition,
    Statement:stmt_LoadPlayerPosition,
    bool:PlayerPositionLoaded[MAX_PLAYERS],
    bool:PlayerHasSavedPosition[MAX_PLAYERS],
    Float:PlayerSavedX[MAX_PLAYERS],
    Float:PlayerSavedY[MAX_PLAYERS],
    Float:PlayerSavedZ[MAX_PLAYERS],
    Float:PlayerSavedA[MAX_PLAYERS],
    PlayerSavedWorld[MAX_PLAYERS],
    PlayerSavedInterior[MAX_PLAYERS];

hook OnMySQLConnected()
{
    stmt_SavePlayerPosition = MySQL_PrepareStatement(
        MySQL_GetHandle(),
        "\
            INSERT INTO player_positions \
                (u_id, x, y, z, angle, world, interior) \
            VALUES \
                (?, ?, ?, ?, ?, ?, ?) \
            ON DUPLICATE KEY UPDATE \
                x = ?, y = ?, z = ?, angle = ?, world = ?, interior = ?"
    );

    stmt_LoadPlayerPosition = MySQL_PrepareStatement(
        MySQL_GetHandle(),
        "SELECT x, y, z, angle, world, interior FROM player_positions WHERE u_id = ? LIMIT 1"
    );
    return 1;
}

hook OnPlayerConnect(playerid)
{
    PlayerPositionLoaded[playerid] = false;
    PlayerHasSavedPosition[playerid] = false;
    PlayerSavedX[playerid] = 0.0;
    PlayerSavedY[playerid] = 0.0;
    PlayerSavedZ[playerid] = 0.0;
    PlayerSavedA[playerid] = 0.0;
    PlayerSavedWorld[playerid] = 0;
    PlayerSavedInterior[playerid] = 0;
    return 1;
}

hook OnPlayerLogin(playerid)
{
    inline const OnPositionLoad()
    {
        MySQL_BindResultFloat(stmt_LoadPlayerPosition, 0, PlayerSavedX[playerid]);
        MySQL_BindResultFloat(stmt_LoadPlayerPosition, 1, PlayerSavedY[playerid]);
        MySQL_BindResultFloat(stmt_LoadPlayerPosition, 2, PlayerSavedZ[playerid]);
        MySQL_BindResultFloat(stmt_LoadPlayerPosition, 3, PlayerSavedA[playerid]);
        MySQL_BindResultInt(stmt_LoadPlayerPosition, 4, PlayerSavedWorld[playerid]);
        MySQL_BindResultInt(stmt_LoadPlayerPosition, 5, PlayerSavedInterior[playerid]);

        PlayerHasSavedPosition[playerid] = MySQL_Statement_FetchRow(stmt_LoadPlayerPosition) ? true : false;
        PlayerPositionLoaded[playerid] = true;
    }

    MySQL_BindInt(stmt_LoadPlayerPosition, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_LoadPlayerPosition, using inline OnPositionLoad);
    return 1;
}

hook OnPlayerAutoSave(playerid)
{
    PlayerPosition_Save(playerid);
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    #pragma unused reason
    PlayerPosition_Save(playerid);
    return 1;
}

stock PlayerPosition_Save(playerid)
{
    if(Player_GetAccountID(playerid) <= 0) {
        return 0;
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:a,
        world = GetPlayerVirtualWorld(playerid),
        interior = GetPlayerInterior(playerid);

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    MySQL_BindInt(stmt_SavePlayerPosition, 0, Player_GetAccountID(playerid));
    MySQL_BindFloat(stmt_SavePlayerPosition, 1, x);
    MySQL_BindFloat(stmt_SavePlayerPosition, 2, y);
    MySQL_BindFloat(stmt_SavePlayerPosition, 3, z);
    MySQL_BindFloat(stmt_SavePlayerPosition, 4, a);
    MySQL_BindInt(stmt_SavePlayerPosition, 5, world);
    MySQL_BindInt(stmt_SavePlayerPosition, 6, interior);
    MySQL_BindFloat(stmt_SavePlayerPosition, 7, x);
    MySQL_BindFloat(stmt_SavePlayerPosition, 8, y);
    MySQL_BindFloat(stmt_SavePlayerPosition, 9, z);
    MySQL_BindFloat(stmt_SavePlayerPosition, 10, a);
    MySQL_BindInt(stmt_SavePlayerPosition, 11, world);
    MySQL_BindInt(stmt_SavePlayerPosition, 12, interior);
    MySQL_ExecuteThreaded(stmt_SavePlayerPosition);
    return 1;
}

stock bool:PlayerPosition_HasSaved(playerid)
{
    return PlayerPositionLoaded[playerid] && PlayerHasSavedPosition[playerid];
}

stock PlayerPosition_GetSaved(playerid, &Float:x, &Float:y, &Float:z, &Float:a, &world, &interior)
{
    if(!PlayerPosition_HasSaved(playerid)) {
        return 0;
    }

    x = PlayerSavedX[playerid];
    y = PlayerSavedY[playerid];
    z = PlayerSavedZ[playerid];
    a = PlayerSavedA[playerid];
    world = PlayerSavedWorld[playerid];
    interior = PlayerSavedInterior[playerid];
    return 1;
}
