// =============================================================================
// CEF SA Mobile Phone System - GTAHUB
// Sự kiện JS -> Pawn: phone.call(number), phone.sendMessage(number, text), phone.transferMoney(target, amount), phone.close
// Sự kiện Pawn -> JS: phone.show(visible, data), phone.hide, phone.notification(app, title, text)
// =============================================================================

#if defined _cef_phone_included
    #endinput
#endif
#define _cef_phone_included

#include <YSI_Coding\y_hooks>

// --- Forwards ---
forward OnCEF_PhoneCall(playerid, browserid, const number[]);
forward OnCEF_PhoneSendMessage(playerid, browserid, const number[], const text[]);
forward OnCEF_PhoneTransferMoney(playerid, browserid, const target[], amount);
forward OnCEF_PhoneClose(playerid, browserid);

forward CEF_ShowPhone(playerid);

hook OnGameModeInit()
{
    CEF_RegisterEvent("phone.call", "OnCEF_PhoneCall", Argument_String);
    CEF_RegisterEvent("phone.sendMessage", "OnCEF_PhoneSendMessage", Argument_String, Argument_String);
    CEF_RegisterEvent("phone.transferMoney", "OnCEF_PhoneTransferMoney", Argument_String, Argument_Integer);
    CEF_RegisterEvent("phone.close", "OnCEF_PhoneClose");

    print("[CEF] Phone events registered.");
    return 1;
}

stock CEF_ShowPhone(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;

    CEF_CreateBrowser(playerid, CEF_BROWSER_PHONE, CEF_URL_PHONE, true, true);
    return 1;
}

public OnCEF_PhoneCall(playerid, browserid, const number[])
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid)) return 0;
    printf("[CEF] Player %d making phone call to: %s", playerid, number);
    return 1;
}

public OnCEF_PhoneSendMessage(playerid, browserid, const number[], const text[])
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid)) return 0;
    printf("[CEF] Player %d sending phone message to %s: %s", playerid, number, text);
    return 1;
}

public OnCEF_PhoneTransferMoney(playerid, browserid, const target[], amount)
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid)) return 0;
    printf("[CEF] Player %d SA Bank mobile transfer: Target=%s, Amount=%d", playerid, target, amount);
    return 1;
}

public OnCEF_PhoneClose(playerid, browserid)
{
    #pragma unused browserid
    if (IsPlayerConnected(playerid))
    {
        CEF_DestroyBrowser(playerid, CEF_BROWSER_PHONE);
    }
    return 1;
}
