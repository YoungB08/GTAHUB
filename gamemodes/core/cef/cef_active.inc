// =============================================================================
// CEF Active (Account Verification / OTP) - GTAHUB
// Xử lý kích hoạt tài khoản qua OTP Gmail trên CEF browser
// JS -> Pawn: GTAHUB:VerifyGmail(code)
// JS -> Pawn: GTAHUB:ResendVerificationCode()
// Pawn -> JS: GTAHUB:VerificationResponse(success, message)
// =============================================================================

#if defined _cef_active_included
    #endinput
#endif
#define _cef_active_included

#include <YSI_Coding\y_hooks>

// --- Forwards ---
forward OnCEF_VerifyGmail(playerid, browserid, const code[]);
forward OnCEF_ResendVerificationCode(playerid, browserid);
forward CEF_ShowActivePage(playerid);

// --- Đăng ký event khi GameMode khởi tạo ---
hook OnGameModeInit()
{
    // JS emit: cef.emit("GTAHUB:VerifyGmail", code)
    CEF_RegisterEvent("GTAHUB:VerifyGmail", "OnCEF_VerifyGmail",
        Argument_String     // Mã OTP 6 chữ số
    );

    // JS emit: cef.emit("GTAHUB:ResendVerificationCode")
    CEF_RegisterEvent("GTAHUB:ResendVerificationCode", "OnCEF_ResendVerificationCode");

    print("[CEF] Active/OTP events registered.");
    return 1;
}

// --- Mở trang Kích Hoạt (Active/OTP) cho player ---
stock CEF_ShowActivePage(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;

    CEF_DisableGameUI(playerid);
    TogglePlayerSpectating(playerid, true);

    // Đóng các browser UI khác
    CEF_DestroyBrowser(playerid, CEF_BROWSER_LOGIN);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_REGISTER);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_CHAR_SELECT);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_CHAR_CREATE);

    // Tạo browser Active OTP
    CEF_CreateBrowser(playerid, CEF_BROWSER_ACTIVE, CEF_URL_ACTIVE, true, true);
    return 1;
}

// --- Callback: Player gửi mã OTP xác thực ---
public OnCEF_VerifyGmail(playerid, browserid, const code[])
{
    #pragma unused browserid

    if (!IsPlayerConnected(playerid))
        return 0;

    // Validate định dạng mã OTP phía server
    if (isnull(code) || strlen(code) != 6 || !IsNumeric(code))
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_ACTIVE, "GTAHUB:VerificationResponse",
            CEF_INT(0), CEF_STR("Ma OTP phai la mot day so gom dung 6 chu so!")
        );
        return 1;
    }

    // TODO: Đối chiếu mã OTP với Database/Session của player
    // Ở đây gửi phản hồi xác thực thành công mẫu
    CEF_EmitEvent(playerid, CEF_BROWSER_ACTIVE, "GTAHUB:VerificationResponse",
        CEF_INT(1), CEF_STR("Xac thuc thanh cong! Tai khoan cua ban da duoc kich hoat.")
    );

    return 1;
}

// --- Callback: Player yêu cầu gửi lại mã OTP ---
public OnCEF_ResendVerificationCode(playerid, browserid)
{
    #pragma unused browserid

    if (!IsPlayerConnected(playerid))
        return 0;

    // TODO: Tạo OTP mới và gửi về email của player
    printf("[CEF] Player %d requested resending OTP verification code.", playerid);
    return 1;
}
