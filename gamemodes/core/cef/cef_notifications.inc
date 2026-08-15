// =============================================================================
// CEF Compact Toast Notifications - GTAHUB
// Sự kiện Pawn -> JS: notify.show(type, text, durationMs), notify.clear
// =============================================================================

#if defined _cef_notifications_included
    #endinput
#endif
#define _cef_notifications_included

#include <YSI_Coding\y_hooks>

static bool:gPlayerNotifyCreated[MAX_PLAYERS];

hook OnPlayerConnect(playerid)
{
    gPlayerNotifyCreated[playerid] = false;
    return 1;
}

stock CEF_ShowNotification(playerid, const type[], const text[], durationMs = 4000)
{
    if (!IsPlayerConnected(playerid)) return 0;

    if (!gPlayerNotifyCreated[playerid])
    {
        CEF_CreateBrowser(playerid, CEF_BROWSER_NOTIFY, CEF_URL_NOTIFY, false, false);
        gPlayerNotifyCreated[playerid] = true;
    }

    CEF_EmitEvent(playerid, CEF_BROWSER_NOTIFY, "notify.show", CEF_STR(type), CEF_STR(text), CEF_INT(durationMs));
    return 1;
}

stock CEF_ClearNotifications(playerid)
{
    if (!IsPlayerConnected(playerid) || !gPlayerNotifyCreated[playerid]) return 0;

    CEF_EmitEvent(playerid, CEF_BROWSER_NOTIFY, "notify.clear");
    return 1;
}
