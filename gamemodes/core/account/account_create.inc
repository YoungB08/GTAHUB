#include <YSI_Coding\y_hooks>

static
    Statement:stmt_insertPlayerAccount;

hook OnMySQLConnected() {
    stmt_insertPlayerAccount = MySQL_PrepareStatement(MySQL_GetHandle(), "INSERT INTO players (username, password) VALUES (?, ?)");
    return 1;
}

// Prompts the player the register dialog.
// return n/a
stock Account_PromptRegister(playerid) {
    inline const _response(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused pid, dialogid, listitem
        if (!response) {
            Kick(playerid);
            return;
        }
        Account_Create(playerid, inputtext);
    }

    new
        string[256];

    static
        const RegisterMessage[] = "\
        "C_LGREEN"TAO TAI KHOAN\
        \n\n"C_WHITE"Xin chao "C_GOLD"%p"C_WHITE", chao mung den voi "C_SAMPBLUE"%s"C_WHITE"!\
        \n\n"C_GREY"Ban dang tao tai khoan moi tren server.\
        \n"C_GREY"Hay chon mat khau an toan cho tai khoan.\
        \n\n"C_GREEN"Nhap mat khau ban muon dung ben duoi.\
        \n"C_ORANGE"Luu y: Hay giu mat khau cua ban that an toan!\
    ";

    format(string, sizeof(string), RegisterMessage, playerid, SERVER_NAME);

    Dialog_ShowCallback(playerid,
        using inline _response, // handler
        DIALOG_STYLE_PASSWORD,  // style
        SERVER_NAME,            // title
        string,                 // content
        "Dang ky",              // button left
        "Thoat");               // button right
}

// only used internally, it checks for validations before calling OnPasswordHash callback.
// it checks for pass length, must be more than 3 but less than 20.
// password can't be all just numbers.
// return n/a
static Account_Create(playerid, const password[]) {

    if (!(3 <= strlen(password) <= 20)) {
        SendErrorMsg(playerid, "Do dai mat khau khong hop le. Mat khau phai tu 3-20 ky tu.");
        Account_PromptRegister(playerid);
        return;
    }
    if (IsNumeric(password)) {
        SendErrorMsg(playerid, "Mat khau khong hop le. Mat khau phai co chu cai.");
        Account_PromptRegister(playerid);
        return;
    }

    bcrypt_hash(playerid, "OnPasswordHash", password, BCRYPT_COST);
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
