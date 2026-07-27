
#include <account_create>
#include <account_login>
#include <account_characters>
#include <account_vip>
#include <account_wanted-level>
#include <account_stats>

#include <YSI_Coding\y_hooks>

static
    Statement: stmt_savePlayerMoney,
    Player_AccountUID[MAX_PLAYERS],
    Player_CharacterUID[MAX_PLAYERS];

hook OnMySQLConnected() {
    stmt_savePlayerMoney = MySQL_PrepareStatement(MySQL_GetHandle(), "UPDATE player_stats SET money=? WHERE u_id=?");
    return 1;
}

hook OnPlayerConnect(playerid) {
    Player_AccountUID[playerid] = 0;
    Player_CharacterUID[playerid] = 0;
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    Player_AccountUID[playerid] = 0;
    Player_CharacterUID[playerid] = 0;
    return 1;
}

public OnPlayerMoneyChange(playerid, previous, current) {
    if(Player_GetAccountID(playerid) <= 0) {
        return 1;
    }

    MySQL_BindInt(stmt_savePlayerMoney, 0, current);
    MySQL_BindInt(stmt_savePlayerMoney, 1, Player_GetAccountID(playerid));
    MySQL_ExecuteThreaded(stmt_savePlayerMoney);
    return 1;
}

timer MoveCamera[500](playerid) {
    InterpolateCameraPos(playerid, 3.1435,-1955.6364,88.1194, 2106.8354,-1270.0221,100.7057, 1000 * 25, CAMERA_MOVE);
    InterpolateCameraLookAt(playerid, 2325.7061,-1063.7642,55.4202, 2325.7061,-1063.7642,45.4202, 1000 * 25, CAMERA_MOVE);
}

hook OnPlayerPassedBanCheck(playerid) {

    defer MoveCamera(playerid);
    inline const OnDataLoad() {
        new
            playerUID,
            playerHash[BCRYPT_HASH_LENGTH];

        if(cache_num_rows()) {
            cache_get_value_name_int(0, "u_id", playerUID);
            cache_get_value_name(0, "password", playerHash, sizeof(playerHash));
            Player_SetAccountID(playerid, playerUID);

            Account_PromptLogin(playerid, playerHash);
        }
        else {
            Account_PromptRegister(playerid);
        }
    }

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    MySQL_TQueryInline(
        MySQL_GetHandle(),
        using inline OnDataLoad,
        "SELECT u_id, password FROM players WHERE username = '%e' AND password <> '"CHARACTER_PASSWORD_MARKER"' LIMIT 1",
        playerName
    );
    return 1;
}

stock Player_SetAccountID(playerid, value) {
    return Player_AccountUID[playerid] = value;
}

stock Player_GetMasterAccountID(playerid) {
    return Player_AccountUID[playerid];
}

stock Player_SetCharacterID(playerid, value) {
    return Player_CharacterUID[playerid] = value;
}

stock Player_GetCharacterID(playerid) {
    return Player_CharacterUID[playerid];
}

stock Player_GetAccountID(playerid) {
    return Player_CharacterUID[playerid];
}
