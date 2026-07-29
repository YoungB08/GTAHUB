// =============================================================================
// CEF Login - GTAHUB
// Xử lý đăng nhập qua CEF browser
// Sự kiện JS -> Pawn: GTAHUB:SubmitLogin(username, password, rememberMe)
// Sự kiện Pawn -> JS: GTAHUB:LoginResponse(success, message)
// =============================================================================

#include <YSI_Coding\y_hooks>

// --- Forward ---
forward OnCEF_SubmitLogin(playerid, browserid, const username[], const password[], bool:rememberMe);
forward OnCEF_LoginPasswordVerify(playerid, bool:success, string:hash[]);
forward CEF_OnPlayerReadyUI(playerid);
forward CEF_CheckLoginFallback(playerid);

// --- Pending state per player ---
static bool:gPlayerPendingLogin[MAX_PLAYERS];
new gLoginHash[MAX_PLAYERS][BCRYPT_HASH_LENGTH];

hook OnPlayerConnect(playerid)
{
    gPlayerPendingLogin[playerid] = false;
    return 1;
}

stock CEF_Login_SetPending(playerid, bool:status)
{
    gPlayerPendingLogin[playerid] = status;
    return 1;
}

// Gọi khi player hoàn tất handshake CEF
public CEF_OnPlayerReadyUI(playerid)
{
    if (gPlayerPendingLogin[playerid])
    {
        printf("[CEF] CEF_OnPlayerReadyUI: playerid=%d is now CEF ready, showing CEF login page!", playerid);
        gPlayerPendingLogin[playerid] = false;
        CEF_ShowLoginPage(playerid);
    }
    return 1;
}

// Kiểm tra fallback sau 2.5s nếu CEF chưa ready
public CEF_CheckLoginFallback(playerid)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    if (gPlayerPendingLogin[playerid])
    {
        gPlayerPendingLogin[playerid] = false;
        if (!CEF_PlayerHasPlugin(playerid))
        {
            printf("[CEF] Fallback check: Player %d does NOT have CEF plugin installed. Falling back to SA-MP Dialog login.", playerid);
            Account_PromptLogin_DialogFallback(playerid, gLoginHash[playerid]);
        }
        else
        {
            printf("[CEF] Fallback check: Player %d has CEF plugin. Showing CEF Login page.", playerid);
            CEF_ShowLoginPage(playerid);
        }
    }
    return 1;
}

// --- Đăng ký event khi GameMode khởi tạo ---
hook OnGameModeInit()
{
    // JS emit: cef.emit("GTAHUB:SubmitLogin", username, password, rememberMe)
    CEF_RegisterEvent("GTAHUB:SubmitLogin", "OnCEF_SubmitLogin",
        Argument_String,    // username
        Argument_String,    // password
        Argument_Bool       // rememberMe (bool)
    );

    // JS emit: cef.emit("GTAHUB:ToRegisterPage")
    CEF_RegisterEvent("GTAHUB:ToRegisterPage", "OnCEF_ToRegisterPage");

    // JS emit: cef.emit("GTAHUB:ForgotPassword")
    CEF_RegisterEvent("GTAHUB:ForgotPassword", "OnCEF_ForgotPassword");

    print("[CEF] Login events registered.");
    return 1;
}

// --- Mở trang Login cho player ---
stock CEF_ShowLoginPage(playerid)
{
    CEF_CloseAllBrowsers(playerid);
    // controls_chat = false để tránh treo/đứng máy khi gõ input trên một số GPU/SA-MP client
    CEF_CreateBrowser(playerid, CEF_BROWSER_LOGIN, CEF_URL_LOGIN, true, false);

    // Di chuyển camera background giống Account cũ
    TogglePlayerSpectating(playerid, true);

    // Phát sự kiện tự động điền Username (tên IC SA-MP)
    SetTimerEx("CEF_EmitUsernameDelayed", 300, false, "i", playerid);
    return 1;
}

forward CEF_EmitUsernameDelayed(playerid);
public CEF_EmitUsernameDelayed(playerid)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, sizeof(playerName));
    CEF_EmitEvent(playerid, CEF_BROWSER_LOGIN, "GTAHUB:SetUsername", CEF_STR(playerName));
    return 1;
}

// --- Callback: Player gửi form đăng nhập ---
public OnCEF_SubmitLogin(playerid, browserid, const username[], const password[], bool:rememberMe)
{
    #pragma unused browserid, rememberMe

    if (!IsPlayerConnected(playerid))
        return 0;

    // Validate input phía server
    if (isnull(username) || isnull(password))
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_LOGIN, "GTAHUB:LoginResponse",
            CEF_INT(0), CEF_STR("Ten tai khoan hoac mat khau khong duoc de trong!")
        );
        return 1;
    }

    if (strlen(password) < 3)
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_LOGIN, "GTAHUB:LoginResponse",
            CEF_INT(0), CEF_STR("Mat khau phai co it nhat 3 ky tu!")
        );
        return 1;
    }

    // Tìm tài khoản theo username được gửi từ client
    // Nếu username trùng với SA-MP name thì xác thực bình thường
    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    if (!strequal(username, playerName, true))
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_LOGIN, "GTAHUB:LoginResponse",
            CEF_INT(0), CEF_STR("Ten tai khoan khong khop voi ten nhan vat SA-MP cua ban!")
        );
        return 1;
    }

    // Kiểm tra hash đã được lưu trước khi player được redirect đến login
    // Thực hiện bcrypt verify với hash từ database
    CEF_Login_VerifyWithHash(playerid, password);
    return 1;
}

// --- Lưu hash tạm để verify ---
stock CEF_Login_SetHash(playerid, const hash[], len = sizeof(hash))
{
    #pragma unused len
    strcpy(gLoginHash[playerid], hash, BCRYPT_HASH_LENGTH);
}

// --- Thực hiện verify password với hash đã lưu ---
static CEF_Login_VerifyWithHash(playerid, const password[])
{
    if (isnull(gLoginHash[playerid]))
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_LOGIN, "GTAHUB:LoginResponse",
            CEF_INT(0), CEF_STR("Khong tim thay du lieu tai khoan. Vui long thu lai!")
        );
        return 0;
    }

    bcrypt_verify(playerid, "OnCEF_LoginPasswordVerify", password, gLoginHash[playerid]);
    return 1;
}

// --- Callback bcrypt verify ---
public OnCEF_LoginPasswordVerify(playerid, bool:success, string:hash[])
{
    #pragma unused hash

    if (!IsPlayerConnected(playerid))
        return 0;

    if (success)
    {
        PlayerAttempts_Reset(playerid);
        CEF_EmitEvent(playerid, CEF_BROWSER_LOGIN, "GTAHUB:LoginResponse",
            CEF_INT(1), CEF_STR("Dang nhap thanh cong! Dang tai...")
        );

        // Đóng login browser và chuyển sang character selection
        // Sau 1.5s JS sẽ tự emit GTAHUB:ToCharacterSelectionPage
        // Chúng ta handle ở đây luôn
        CEF_DestroyBrowser(playerid, CEF_BROWSER_LOGIN);
        Account_ShowCharacterMenu_CEF(playerid);
    }
    else
    {
        PlayerAttempts_Increment(playerid);

        if (PlayerAttempts_Get(playerid) >= MAX_PASSWORD_ATTEMPT)
        {
            CEF_EmitEvent(playerid, CEF_BROWSER_LOGIN, "GTAHUB:LoginResponse",
                CEF_INT(0), CEF_STR("Ban da bi kick vi dang nhap sai qua nhieu lan!")
            );
            SetTimerEx("CEF_KickPlayer", 2000, false, "i", playerid);
            return 1;
        }

        new errMsg[128];
        format(errMsg, sizeof(errMsg), "Sai mat khau! Con lai %d lan thu.", MAX_PASSWORD_ATTEMPT - PlayerAttempts_Get(playerid));

        CEF_EmitEvent(playerid, CEF_BROWSER_LOGIN, "GTAHUB:LoginResponse",
            CEF_INT(0), CEF_STR(errMsg)
        );
    }
    return 1;
}

// --- Forward và Public cho player attempts ---
forward PlayerAttempts_Reset(playerid);
public PlayerAttempts_Reset(playerid)
{
    PlayerAttempts[playerid] = 0;
    return 1;
}

forward PlayerAttempts_Increment(playerid);
public PlayerAttempts_Increment(playerid)
{
    PlayerAttempts[playerid]++;
    return 1;
}

forward PlayerAttempts_Get(playerid);
public PlayerAttempts_Get(playerid)
{
    return PlayerAttempts[playerid];
}

// --- Timer để kick player ---
forward CEF_KickPlayer(playerid);
public CEF_KickPlayer(playerid)
{
    if (IsPlayerConnected(playerid))
        Kick(playerid);
    return 1;
}

// --- CEF Event: Chuyển sang trang Register ---
forward OnCEF_ToRegisterPage(playerid, browserid);
public OnCEF_ToRegisterPage(playerid, browserid)
{
    #pragma unused browserid
    CEF_ShowRegisterPage(playerid);
    return 1;
}

// --- CEF Event: Quên mật khẩu ---
forward OnCEF_ForgotPassword(playerid, browserid);
public OnCEF_ForgotPassword(playerid, browserid)
{
    #pragma unused browserid
    SendClientMessage(playerid, 0xFFAA00FF, "[CEF] Tinh nang quen mat khau chua duoc ho tro. Lien he admin qua Discord.");
    return 1;
}
