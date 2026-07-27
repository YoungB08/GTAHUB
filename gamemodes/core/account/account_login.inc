#include <YSI_Coding\y_hooks>

static
    bool:gPlayerLoginStatus[MAX_PLAYERS],
    PlayerAttempts[MAX_PLAYERS];

#define MAX_PASSWORD_ATTEMPT (5)

// Initialise
hook OnPlayerConnect(playerid) {
    gPlayerLoginStatus[playerid] = false;
    PlayerAttempts[playerid] = 0;
    return 1;
}

// Global
void:Account_PromptLogin(playerid, const hash[], len = sizeof(hash)) {
    new
        playerHash[BCRYPT_HASH_LENGTH];
    // copy to local stack
    // fix with y_inline scopes.
    strcpy(playerHash, hash, len);

    inline const PromptLoginResponse(response, listitem, string:inputtext[]) {
         #pragma unused listitem

        // User clicked the 'leave' buton or pressed 'ESC' .
        if(!response) {
            Kick(playerid);
            return;
        }

        bcrypt_verify(playerid, "OnPasswordVerify", inputtext, playerHash, "s", playerHash);
    }

    new
        string[256];

    static const LoginMessage[] = "\
        "C_SAMPBLUE"DANG NHAP TAI KHOAN\
        \n\n"C_WHITE"Chao mung tro lai, "C_GOLD"%p"C_WHITE"!\
        \n\n"C_GREY"Nhap mat khau de truy cap tai khoan cua ban.\
        \n"C_GREY"Bao mat tai khoan cua ban rat quan trong.\
        \n\n"C_BLUE"Nhap mat khau ben duoi de tiep tuc.\
    ";

    format(
        string,
        sizeof(string),
        LoginMessage,
        playerid,
        SERVER_NAME
    );

    Dialog_ShowCallback(
        playerid,
        using inline PromptLoginResponse,   // Handler
        DIALOG_STYLE_PASSWORD,              // Style
        SERVER_NAME,                        // Title
        string,                             // Content
        "Dang nhap",                        // Button Left
        "Thoat"                             // Button Right
    );
}

// Local
static void:Account_NotifySuccess(playerid) {
    static const
        msg_title[] = "~g~Chao mung tro lai!",
        msg_content[] = "Xin chao ~p~%p, ~w~chao mung tro lai ~p~%s. ~n~~n~~y~Chuc ban choi vui!";
    MessageBox_ShowF(playerid, TYPE_MSGBOX, msg_title, msg_content, 8000, playerid, SERVER_NAME);
}


hook OnPlayerLogin(playerid) {
    gPlayerLoginStatus[playerid] = true;

    new
        Statement: stmt_updatePlayerLastLogin;

    stmt_updatePlayerLastLogin = MySQL_PrepareStatement(MySQL_GetHandle(), "UPDATE players SET last_login = CURRENT_TIMESTAMP() WHERE u_id = ?");
    MySQL_BindInt(stmt_updatePlayerLastLogin, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteThreaded(stmt_updatePlayerLastLogin);
}

forward OnPasswordVerify(playerid, bool:success, string:hash[]);
public OnPasswordVerify(playerid, bool:success, string:hash[]) {
    if(!success) {
        PlayerAttempts[playerid] ++;
        if(PlayerAttempts[playerid] >= MAX_PASSWORD_ATTEMPT) {
            SendErrorMsg(playerid, "Ban da bi kick vi dang nhap sai qua nhieu lan.");
            Kick(playerid);

            Logger_Dbg(PLAYER_DEBUG_HANDLER, "player has been kicked for too many login attempts",
                Logger_P(playerid),
                Logger_I("uid", Player_GetAccountID(playerid))
            );
            return;
        }

        Account_PromptLogin(playerid, hash, BCRYPT_HASH_LENGTH);

        SendErrorMsg(playerid, "Co loi xay ra, vui long thu lai.");

        Logger_Dbg(PLAYER_DEBUG_HANDLER, "player failed to login",
            Logger_I("attempts", PlayerAttempts[playerid]),
            Logger_P(playerid),
            Logger_I("uid", Player_GetAccountID(playerid))
        );
        return;
    }

    Logger_Dbg(PLAYER_DEBUG_HANDLER, "player successfully logged in",
        Logger_P(playerid),
        Logger_I("uid", Player_GetAccountID(playerid))
    );

    PlayerAttempts[playerid] = 0;
    Account_NotifySuccess(playerid);
    CallLocalFunction("OnPlayerLogin", "i", playerid); // Used in other modules to load other data.
}

 // API
Player_IsLoggedIn(playerid) {
    return gPlayerLoginStatus[playerid];
}

Player_SetLoginStatus(playerid, bool:status) {
    return gPlayerLoginStatus[playerid] = status;
}
