#include <YSI_Coding\y_hooks>

static
    bool:gPlayerLoginStatus[MAX_PLAYERS];

new PlayerAttempts[MAX_PLAYERS];

#define MAX_PASSWORD_ATTEMPT (5)

// Initialise
hook OnPlayerConnect(playerid) {
    gPlayerLoginStatus[playerid] = false;
    PlayerAttempts[playerid] = 0;
    return 1;
}

// Global - Mở login CEF (với Dialog Fallback nếu client không có CEF Plugin)
void:Account_PromptLogin(playerid, const hash[], len = sizeof(hash)) {
    // Lưu hash cho CEF verify
    CEF_Login_SetHash(playerid, hash, len);

    if (CEF_IsPlayerCEFReady(playerid))
    {
        print("[CEF] Player CEF is ready. Showing CEF Login page.");
        CEF_ShowLoginPage(playerid);
    }
    else
    {
        print("[CEF] Player plugin not ready yet on connect. Marking pending LOGIN.");
        CEF_SetPendingUI(playerid, CEF_PENDING_LOGIN);
        CEF_Login_SetPending(playerid, true);
        SetTimerEx("CEF_CheckLoginFallback", 3000, false, "i", playerid);
    }
}

// Fallback SA-MP Dialog khi client không cài CEF plugin
stock Account_PromptLogin_DialogFallback(playerid, const hash[])
{
    new playerHash[BCRYPT_HASH_LENGTH];
    strcpy(playerHash, hash, BCRYPT_HASH_LENGTH);

    inline const PromptLoginResponse(response, listitem, string:inputtext[]) {
        #pragma unused listitem
        if(!response) {
            Kick(playerid);
            return;
        }
        bcrypt_verify(playerid, "OnPasswordVerify_Fallback", inputtext, playerHash, "s", playerHash);
    }

    new string[256];
    static const LoginMessage[] = "\
        "C_SAMPBLUE"DANG NHAP TAI KHOAN (DIALOG)\
        \n\n"C_WHITE"Chao mung tro lai, "C_GOLD"%p"C_WHITE"!\
        \n\n"C_GREY"Nhap mat khau de truy cap tai khoan cua ban.\
        \n"C_ORANGE"Luu y: Game cua ban dang chay che do Dialog (Chua cai dat CEF Plugin).\
        \n\n"C_BLUE"Nhap mat khau ben duoi de tiep tuc.\
    ";

    format(string, sizeof(string), LoginMessage, playerid, SERVER_NAME);
    Dialog_ShowCallback(
        playerid,
        using inline PromptLoginResponse,
        DIALOG_STYLE_PASSWORD,
        SERVER_NAME,
        string,
        "Dang nhap",
        "Thoat"
    );
}

forward OnPasswordVerify_Fallback(playerid, bool:success, string:hash[]);
public OnPasswordVerify_Fallback(playerid, bool:success, string:hash[])
{
    if(!success) {
        PlayerAttempts[playerid]++;
        if(PlayerAttempts[playerid] >= MAX_PASSWORD_ATTEMPT) {
            SendErrorMsg(playerid, "Ban da bi kick vi dang nhap sai qua nhieu lan.");
            Kick(playerid);
            return 1;
        }
        Account_PromptLogin_DialogFallback(playerid, hash);
        SendErrorMsg(playerid, "Co loi xay ra, vui long thu lai.");
        return 1;
    }

    PlayerAttempts[playerid] = 0;
    Account_ShowCharacterMenu(playerid);
    return 1;
}

// Local
hook OnPlayerLogin(playerid) {
    gPlayerLoginStatus[playerid] = true;

    new
        query[128],
        accountid = Player_GetMasterAccountID(playerid),
        characterid = Player_GetAccountID(playerid);

    if(characterid > 0) {
        mysql_format(
            MySQL_GetHandle(),
            query,
            sizeof(query),
            "UPDATE players SET last_login = CURRENT_TIMESTAMP() WHERE u_id IN (%i, %i)",
            accountid,
            characterid
        );
        mysql_tquery(MySQL_GetHandle(), query);
    }
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
                Logger_I("uid", Player_GetMasterAccountID(playerid))
            );
            return;
        }

        Account_PromptLogin(playerid, hash, BCRYPT_HASH_LENGTH);

        SendErrorMsg(playerid, "Co loi xay ra, vui long thu lai.");

        Logger_Dbg(PLAYER_DEBUG_HANDLER, "player failed to login",
            Logger_I("attempts", PlayerAttempts[playerid]),
            Logger_P(playerid),
            Logger_I("uid", Player_GetMasterAccountID(playerid))
        );
        return;
    }

    Logger_Dbg(PLAYER_DEBUG_HANDLER, "player successfully logged in",
        Logger_P(playerid),
        Logger_I("uid", Player_GetMasterAccountID(playerid))
    );

    PlayerAttempts[playerid] = 0;
    Player_SetLoginStatus(playerid, true);
    CallLocalFunction("OnPlayerLogin", "i", playerid);
}

 // API
Player_IsLoggedIn(playerid) {
    return gPlayerLoginStatus[playerid];
}

Player_SetLoginStatus(playerid, bool:status) {
    return gPlayerLoginStatus[playerid] = status;
}
