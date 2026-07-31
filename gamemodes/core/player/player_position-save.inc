#include <YSI_Coding\y_hooks>

stock bool:PlayerPosition_IsValid(Float:x, Float:y, Float:z)
{
    // Kiểm tra NaN
    if (x != x || y != y || z != z) return false;
    // Tọa độ 0,0,0
    if (x == 0.0 && y == 0.0 && z == 0.0) return false;
    // Tọa độ kẹt camera / độ cao bất thường (z > 1000m hoặc z < -100m) hoặc ngoài giới hạn bản đồ (-6000 đến +6000)
    if (z > 1000.0 || z < -100.0) return false;
    if (floatabs(x) > 6000.0 || floatabs(y) > 6000.0) return false;
    // Tọa độ camera selection mặc định của SA-MP (2931.9829, -1922.0767)
    if (floatabs(x - 2931.9829) < 2.0 && floatabs(y - (-1922.0767)) < 2.0) return false;
    return true;
}

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
    mysql_query_file(MySQL_GetHandle(), "player_positions.sql", false);

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
    if (Player_GetAccountID(playerid) <= 0) return 1;

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

hook OnPlayerSecondUpdate(playerid)
{
    // Cập nhật RAM Cache vị trí hợp lệ định kỳ (mỗi 15 giây)
    if (Char_IsActive(playerid) && !CallLocalFunction("Admin_IsPlayerSpectating", "i", playerid))
    {
        new Float:x, Float:y, Float:z, Float:a;
        GetPlayerPos(playerid, x, y, z);
        GetPlayerFacingAngle(playerid, a);

        if (PlayerPosition_IsValid(x, y, z))
        {
            PlayerSavedX[playerid]        = x;
            PlayerSavedY[playerid]        = y;
            PlayerSavedZ[playerid]        = z;
            PlayerSavedA[playerid]        = a;
            PlayerSavedWorld[playerid]    = GetPlayerVirtualWorld(playerid);
            PlayerSavedInterior[playerid] = GetPlayerInterior(playerid);
            PlayerHasSavedPosition[playerid] = true;
            PlayerPositionLoaded[playerid]   = true;
        }
    }
    return 1;
}

hook OnPlayerAutoSave(playerid)
{
    if (Char_IsActive(playerid))
    {
        PlayerPosition_Save(playerid, false);
    }
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    if (!Char_IsActive(playerid)) return 1;

    // reason = 1: Người chơi tự thoát bằng /q hoặc ESC menu (lấy trực tiếp vị trí hiện tại)
    // reason = 0 hoặc 2: Crash, Timeout, Kick/Ban (dùng RAM cache vị trí hợp lệ gần nhất)
    new bool:isNormalQuit = (reason == 1);
    PlayerPosition_Save(playerid, isNormalQuit);
    return 1;
}

stock PlayerPosition_Save(playerid, bool:forceFresh = true)
{
    if(Player_GetAccountID(playerid) <= 0 || !Player_IsLoggedIn(playerid) || !Char_IsActive(playerid)) {
        return 0;
    }

    new
        Float:x, Float:y, Float:z, Float:a,
        world, interior;

    if (forceFresh && !CallLocalFunction("Admin_IsPlayerSpectating", "i", playerid))
    {
        GetPlayerPos(playerid, x, y, z);
        GetPlayerFacingAngle(playerid, a);
        world = GetPlayerVirtualWorld(playerid);
        interior = GetPlayerInterior(playerid);

        if (!PlayerPosition_IsValid(x, y, z))
        {
            if (!PlayerPosition_HasSaved(playerid)) return 0;
            x = PlayerSavedX[playerid];
            y = PlayerSavedY[playerid];
            z = PlayerSavedZ[playerid];
            a = PlayerSavedA[playerid];
            world = PlayerSavedWorld[playerid];
            interior = PlayerSavedInterior[playerid];
        }
    }
    else
    {
        if (!PlayerPosition_HasSaved(playerid)) return 0;
        x = PlayerSavedX[playerid];
        y = PlayerSavedY[playerid];
        z = PlayerSavedZ[playerid];
        a = PlayerSavedA[playerid];
        world = PlayerSavedWorld[playerid];
        interior = PlayerSavedInterior[playerid];
    }

    if (!PlayerPosition_IsValid(x, y, z)) return 0;

    // Lưu ngay lập tức vào bộ đệm bộ nhớ
    PlayerSavedX[playerid] = x;
    PlayerSavedY[playerid] = y;
    PlayerSavedZ[playerid] = z;
    PlayerSavedA[playerid] = a;
    PlayerSavedWorld[playerid] = world;
    PlayerSavedInterior[playerid] = interior;
    PlayerHasSavedPosition[playerid] = true;
    PlayerPositionLoaded[playerid] = true;

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
