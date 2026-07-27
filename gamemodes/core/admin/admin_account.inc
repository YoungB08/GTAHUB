
#include <YSI_Coding\y_hooks>

#define MAX_ADMIN_LEVELS (5)

enum {
    ADM_LVL_1 = 1,
    ADM_LVL_2,
    ADM_LVL_3,
    ADM_LVL_4,
    ADM_LVL_5
}

static
    PlayerAdminLevel[MAX_PLAYERS],
    PlayerTempAdminLevel[MAX_PLAYERS],
    BitArray: adminOnDuty<MAX_PLAYERS>,
    Statement: stmt_retrieveAdmin;

hook OnGameModeInit() {
    static const query_retrieveAdmin[] = "\
        SELECT \
            admin_level \
        FROM  \
            admins \
        WHERE \
            u_id = ? \
        "
    ;
    stmt_retrieveAdmin = MySQL_PrepareStatement(MySQL_GetHandle(), query_retrieveAdmin);
    return 1;
}

hook OnPlayerConnect(playerid) {
    PlayerAdminLevel[playerid] = 0 ;
    PlayerTempAdminLevel[playerid] = 0;
    Bit_Set(adminOnDuty, playerid, false);
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    PlayerAdminLevel[playerid] = 0;
    PlayerTempAdminLevel[playerid] = 0;
    Bit_Set(adminOnDuty, playerid, false);
    return 1;
}

hook OnPlayerLogin(playerid) {
    inline OnPlayerAdminLoad() {
        new
            adminLevel;

        MySQL_BindResultInt(stmt_retrieveAdmin, 0, adminLevel);

        if(MySQL_Statement_FetchRow(stmt_retrieveAdmin)) {
            Admin_SetPlayerLevel(playerid, adminLevel);
        }

    }
    MySQL_BindInt(stmt_retrieveAdmin, 0, Player_GetMasterAccountID(playerid));
    MySQL_ExecuteThreaded_Inline(stmt_retrieveAdmin, using inline OnPlayerAdminLoad);
}

/*

    Admin Messaging
        - SendAdminMessage
        - SendAdminMessageF(fmat[], {Float,_}:...);
*/

stock SendAdminMsg(const fmat[]) {
    foreach(new i : Player) {
        if (i != INVALID_PLAYER_ID && Admin_GetPlayerLevel(i) > 0)
        {
            SendMsgF(i, COLOR_MODERATION, "(Admin) "C_WHITE" %s", fmat);
        }
    }
    return 1;
}

stock SendAdminMsgF(const fmat[], {Float,_}:...) {
    new
        out[144];

    foreach(new i : Player) {
        if (i != INVALID_PLAYER_ID && Admin_GetPlayerLevel(i) > 0) {
            formatex(out, sizeof(out), fmat, ___(1));
            SendAdminMsg(out);
        }
    }
    return 1;
}

stock Admin_SetPlayerLevel(playerid, plevel) {
    PlayerAdminLevel[playerid] = plevel;
    PlayerTempAdminLevel[playerid] = 0;

    new string[256];

    static const query[] = "\
    INSERT INTO \
        admins(u_id, admin_level) \
    VALUES \
        (%i, %i) \
    ON DUPLICATE KEY UPDATE \
        admin_level = %i \
    ";

    mysql_format(MySQL_GetHandle(), string, sizeof(string), query, Player_GetMasterAccountID(playerid), plevel, plevel);
    mysql_tquery(MySQL_GetHandle(), string);
}

stock Admin_SetPlayerTempLevel(playerid, plevel) {
    PlayerTempAdminLevel[playerid] = plevel;
    return 1;
}

stock Admin_GetPlayerStoredLevel(playerid)
    return PlayerAdminLevel[playerid];

stock Admin_GetPlayerTempLevel(playerid)
    return PlayerTempAdminLevel[playerid];

stock Admin_GetRankName(playerid) {
    new admin_RankNames[6][24] = {
        "Khong co",      // 0
        "Admin Tap Su",  // 1
        "Admin Thuong",  // 2
        "Admin Cao Cap", // 3
        "Truong Admin",  // 4
        "Developer"      // 5
    };

    return admin_RankNames[Admin_GetPlayerLevel(playerid)];
}

stock Admin_GetPlayerLevel(playerid) {
    if(PlayerTempAdminLevel[playerid] > PlayerAdminLevel[playerid]) {
        return PlayerTempAdminLevel[playerid];
    }
    return PlayerAdminLevel[playerid];
}

stock SendAdminCmdUsage(playerid, const cmd[], const params[]) {

    if(Admin_GetPlayerLevel(playerid) == 5) {
        return 0;
    }

   // loop through all players
    foreach(new i : Player) {
        // only send message if admin level is more than 1
        if (Admin_GetPlayerLevel(i) >= 1) {
            SendMsgF(i, COLOR_MODERATION, "(Lenh Admin) "C_GREY"%p(%d) "C_WHITE"da dung "C_GREY"/%s %s", playerid, playerid, cmd, params);
        }
    }
    return 1;
}

stock Admin_SetPlayerDuty(playerid, bool:toggle) {
    Bit_Set(adminOnDuty, playerid, toggle);

    if(toggle) {
        SetPlayerColor(playerid, COLOR_MODERATION);
    }
    else if(Player_IsSpawned(playerid)) {
        Player_UpdateColour(playerid);
    }
    return 1;
}

stock Admin_IsOnDuty(playerid)
    return Bit_Get(adminOnDuty, playerid);
