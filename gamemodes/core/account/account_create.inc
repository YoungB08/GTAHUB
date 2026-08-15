#include <YSI_Coding\y_hooks>

static
    Statement:stmt_insertPlayerAccount;

hook OnMySQLConnected() {
    stmt_insertPlayerAccount = MySQL_PrepareStatement(MySQL_GetHandle(), "INSERT INTO players (username, password) VALUES (?, ?)");
    return 1;
}

// Prompts the player the register dialog -> Thay bằng CEF browser (với Dialog Fallback)
stock Account_PromptRegister(playerid) {
    print("[CEF] Account_PromptRegister: Showing CEF Register page for player...");
    CEF_ShowRegisterPage(playerid);

    CEF_SetPendingUI(playerid, CEF_PENDING_REGISTER);
    SetTimerEx("CEF_CheckRegisterFallbackTimer", 5500, false, "i", playerid);
}

forward CEF_CheckRegisterFallbackTimer(playerid);
public CEF_CheckRegisterFallbackTimer(playerid)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    if (CEF_GetPendingUI(playerid) == CEF_PENDING_REGISTER)
    {
        CEF_SetPendingUI(playerid, CEF_PENDING_NONE);
        if (!CEF_PlayerHasPlugin(playerid) && !CEF_IsPlayerCEFReady(playerid))
        {
            printf("[CEF] Fallback check: Player %d does NOT have CEF plugin installed. Falling back to SA-MP Dialog register.", playerid);
            Account_PromptRegister_DialogFallback(playerid);
        }
        else
        {
            printf("[CEF] Fallback check: Player %d has CEF plugin. Showing CEF Register page.", playerid);
            CEF_ShowRegisterPage(playerid);
        }
    }
    return 1;
}

stock Account_PromptRegister_DialogFallback(playerid)
{
    inline const _response(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused pid, dialogid, listitem
        if (!response) {
            Kick(playerid);
            return;
        }
        if (!(3 <= strlen(inputtext) <= 20) || IsNumeric(inputtext)) {
            SendErrorMsg(playerid, "Mat khau phai tu 3-20 ky tu va co chu cai.");
            Account_PromptRegister_DialogFallback(playerid);
            return;
        }
        bcrypt_hash(playerid, "OnPasswordHash", inputtext, BCRYPT_COST);
    }

    new string[256];
    static const RegisterMessage[] = "\
        "C_LGREEN"TAO TAI KHOAN (DIALOG)\
        \n\n"C_WHITE"Xin chao "C_GOLD"%p"C_WHITE", chao mung den voi "C_SAMPBLUE"%s"C_WHITE"!\
        \n\n"C_GREY"Ban dang tao tai khoan moi tren server.\
        \n"C_ORANGE"Luu y: Game cua ban dang chay che do Dialog (Chua cai dat CEF Plugin).\
        \n\n"C_GREEN"Nhap mat khau ban muon dung ben duoi.\
    ";

    format(string, sizeof(string), RegisterMessage, playerid, SERVER_NAME);
    Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_PASSWORD, SERVER_NAME, string, "Dang ky", "Thoat");
}

// Internal: kiểm tra và tạo tài khoản (gọi từ cef_register.inc qua)
stock Account_Create_Internal(playerid, const password[]) {
    if (!(3 <= strlen(password) <= 20)) {
        return 0;
    }
    if (IsNumeric(password)) {
        return 0;
    }
    bcrypt_hash(playerid, "OnPasswordHash", password, BCRYPT_COST);
    return 1;
}

// only used internally, when all checks passed this function will be called.
// it notifies player that they have successfully registered to the server.
// it inserts their data into the database.
// return n/a
static Account_InsertToDatabase(playerid, const hash[]) {
    inline const OnRegister() {
        Player_SetAccountID(playerid, cache_insert_id());
        Player_SetCharacterID(playerid, Player_GetMasterAccountID(playerid));
        Player_SetSkin(playerid, 1);
        CallLocalFunction("OnPlayerRegister", "i", playerid);
        Player_SetCharacterID(playerid, 0);

        Player_SetLoginStatus(playerid, true);
        CallLocalFunction("OnPlayerLogin", "i", playerid);

        SendServerMsg(playerid, "Tai khoan da duoc tao. Hay chon hoac tao nhan vat.");
        Account_ShowCharacterMenu_CEF(playerid);
    }

    new
        username[MAX_PLAYER_NAME];
    GetPlayerName(playerid, username, sizeof(username));

    // insert account data.
    MySQL_Bind(stmt_insertPlayerAccount, 0, username);
    MySQL_Bind(stmt_insertPlayerAccount, 1, hash);
    MySQL_ExecuteThreaded_Inline(stmt_insertPlayerAccount, using inline OnRegister);
}

forward OnPasswordHash(playerid);
public OnPasswordHash(playerid) {
    new hash[BCRYPT_HASH_LENGTH];
    bcrypt_get_hash(hash);
    Account_InsertToDatabase(playerid, hash);
}
