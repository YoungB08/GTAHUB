// =============================================================================
// CEF Register - GTAHUB
// Xử lý đăng ký tài khoản qua CEF browser
// Sự kiện JS -> Pawn: GTAHUB:SubmitRegister(username, email, password, confirmPassword)
// Sự kiện Pawn -> JS: GTAHUB:RegisterResponse(success, message)
// =============================================================================

#include <YSI_Coding\y_hooks>

// --- Forward ---
forward OnCEF_SubmitRegister(playerid, browserid, const username[], const email[], const password[], const confirmPassword[]);
forward OnCEF_RegisterPasswordHash(playerid);
forward OnCEF_ToLoginPage(playerid, browserid);
forward OnCEF_RegisterReady(playerid, browserid);
forward CEF_EmitRegisterUsername(playerid);

// --- Biến lưu mật khẩu tạm khi register ---
static gRegisterPassword[MAX_PLAYERS][128];

// --- Đăng ký event khi GameMode khởi tạo ---
hook OnGameModeInit()
{
    // JS emit: cef.emit("GTAHUB:RegisterReady")
    CEF_RegisterEvent("GTAHUB:RegisterReady", "OnCEF_RegisterReady");

    // JS emit: cef.emit("GTAHUB:SubmitRegister", username, email, password, confirmPassword)
    CEF_RegisterEvent("GTAHUB:SubmitRegister", "OnCEF_SubmitRegister",
        Argument_String,    // username (từ SA-MP name)
        Argument_String,    // email (lưu vào DB sau)
        Argument_String,    // password
        Argument_String     // confirmPassword
    );

    // JS emit: cef.emit("GTAHUB:ToLoginPage") - quay về login
    CEF_RegisterEvent("GTAHUB:ToLoginPage", "OnCEF_ToLoginPage");

    print("[CEF] Register events registered.");
    return 1;
}

// --- CEF Event: Trang Register đã sẵn sàng ---
public OnCEF_RegisterReady(playerid, browserid)
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid)) return 0;
    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, sizeof(playerName));
    CEF_EmitEvent(playerid, CEF_BROWSER_REGISTER, "GTAHUB:SetUsername", CEF_STR(playerName));
    return 1;
}

// --- Mở trang Register cho player ---
stock CEF_ShowRegisterPage(playerid)
{
    CEF_DisableGameUI(playerid);

    SetPlayerCameraPos(playerid, 2106.8354, -1270.0221, 100.7057);
    SetPlayerCameraLookAt(playerid, 2325.7061, -1063.7642, 55.4202);

    CEF_DestroyBrowser(playerid, CEF_BROWSER_LOGIN);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_ACTIVE);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_CHAR_SELECT);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_CHAR_CREATE);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_REGISTER);

    CEF_CreateBrowser(playerid, CEF_BROWSER_REGISTER, CEF_URL_REGISTER, true, true);

    SetTimerEx("CEF_EmitRegisterUsername", 300, false, "i", playerid);
    SetTimerEx("CEF_EmitRegisterUsername", 800, false, "i", playerid);
    return 1;
}

public CEF_EmitRegisterUsername(playerid)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, sizeof(playerName));
    CEF_EmitEvent(playerid, CEF_BROWSER_REGISTER, "GTAHUB:SetUsername", CEF_STR(playerName));
    return 1;
}

// --- Callback: Player gửi form đăng ký ---
public OnCEF_SubmitRegister(playerid, browserid, const username[], const email[], const password[], const confirmPassword[])
{
    #pragma unused browserid, email

    if (!IsPlayerConnected(playerid))
        return 0;

    // Validate input phía server
    if (isnull(password))
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_REGISTER, "GTAHUB:RegisterResponse",
            CEF_INT(0), CEF_STR("Mat khau khong duoc de trong!")
        );
        return 1;
    }

    if (!strequal(password, confirmPassword))
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_REGISTER, "GTAHUB:RegisterResponse",
            CEF_INT(0), CEF_STR("Mat khau xac nhan khong khop!")
        );
        return 1;
    }

    new passLen = strlen(password);
    if (!(3 <= passLen <= 20))
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_REGISTER, "GTAHUB:RegisterResponse",
            CEF_INT(0), CEF_STR("Do dai mat khau phai tu 3-20 ky tu!")
        );
        return 1;
    }

    if (IsNumeric(password))
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_REGISTER, "GTAHUB:RegisterResponse",
            CEF_INT(0), CEF_STR("Mat khau khong duoc chi co so! Can co chu cai.")
        );
        return 1;
    }

    // Lưu password để dùng sau callback hash
    strcpy(gRegisterPassword[playerid], password, sizeof(gRegisterPassword[]));

    // Hash password
    bcrypt_hash(playerid, "OnCEF_RegisterPasswordHash", password, BCRYPT_COST);
    return 1;
}

// --- Callback bcrypt hash hoàn tất ---
public OnCEF_RegisterPasswordHash(playerid)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    new hash[BCRYPT_HASH_LENGTH];
    bcrypt_get_hash(hash);

    // Xóa password tạm
    gRegisterPassword[playerid][0] = '\0';

    // Tạo tài khoản trong database
    CEF_Register_InsertToDatabase(playerid, hash);
    return 1;
}

// --- Tạo tài khoản trong DB ---
static CEF_Register_InsertToDatabase(playerid, const hash[])
{
    inline const OnCEFRegisterInserted()
    {
        Player_SetAccountID(playerid, cache_insert_id());
        Player_SetCharacterID(playerid, Player_GetMasterAccountID(playerid));
        Player_SetSkin(playerid, 1);
        CallLocalFunction("OnPlayerRegister", "i", playerid);
        Player_SetCharacterID(playerid, 0);

        CEF_EmitEvent(playerid, CEF_BROWSER_REGISTER, "GTAHUB:RegisterResponse",
            CEF_INT(1), CEF_STR("Dang ky tai khoan thanh cong! Dang chuyen sang chon nhan vat...")
        );

        // Sau khi JS nhận success, nó sẽ chuyển sang character selection
        // Chúng ta cũng handle từ Pawn sau 2s
        SetTimerEx("CEF_Register_ShowCharMenu", 2000, false, "i", playerid);
    }

    new username[MAX_PLAYER_NAME];
    GetPlayerName(playerid, username, MAX_PLAYER_NAME);

    MySQL_TQueryInline(
        MySQL_GetHandle(),
        using inline OnCEFRegisterInserted,
        "INSERT INTO players (username, password) VALUES ('%e', '%e')",
        username, hash
    );
}

// --- Timer: Chuyển sang character menu sau register ---
forward CEF_Register_ShowCharMenu(playerid);
public CEF_Register_ShowCharMenu(playerid)
{
    if (IsPlayerConnected(playerid))
    {
        CEF_DestroyBrowser(playerid, CEF_BROWSER_REGISTER);
        Account_ShowCharacterMenu_CEF(playerid);
    }
    return 1;
}

// --- CEF Event: Quay về trang Login ---
public OnCEF_ToLoginPage(playerid, browserid)
{
    #pragma unused browserid
    CEF_DestroyBrowser(playerid, CEF_BROWSER_REGISTER);
    CEF_ShowLoginPage(playerid);
    return 1;
}
