// =============================================================================
// CEF Interaction HUD (Range & Entrance Hints) - GTAHUB
// Sự kiện JS -> Pawn: interaction.response(bool:accepted)
// Sự kiện Pawn -> JS: interaction.showHint(key, title, subtitle, context), interaction.hideHint
// =============================================================================

#if defined _cef_interaction_included
    #endinput
#endif
#define _cef_interaction_included

#include <YSI_Coding\y_hooks>

// --- Forwards ---
forward OnCEF_InteractionResponse(playerid, browserid, bool:accepted);
forward CEF_ShowInteraction(playerid, const key[], const title[], const subtitle[], const context[]);
forward CEF_HideInteraction(playerid);

// --- State Tracking per Player ---
static bool:gPlayerInteractionActive[MAX_PLAYERS];

hook OnPlayerConnect(playerid)
{
    gPlayerInteractionActive[playerid] = false;
    return 1;
}

hook OnGameModeInit()
{
    // Đăng ký event JS emit: cef.emit("interaction.response", accepted)
    CEF_RegisterEvent("interaction.response", "OnCEF_InteractionResponse", Argument_Bool);
    print("[CEF] Interaction HUD events registered.");
    return 1;
}

// --- Mở HUD Tương Tác Near Range & Entrance ---
stock CEF_ShowInteraction(playerid, const key[], const title[], const subtitle[], const context[])
{
    if (!IsPlayerConnected(playerid)) return 0;

    if (!gPlayerInteractionActive[playerid])
    {
        CEF_CreateBrowser(playerid, CEF_BROWSER_INTERACTION, CEF_URL_INTERACTION, false, false);
        gPlayerInteractionActive[playerid] = true;
    }

    CEF_EmitEvent(playerid, CEF_BROWSER_INTERACTION, "interaction.showHint",
        CEF_STR(key), CEF_STR(title), CEF_STR(subtitle), CEF_STR(context)
    );
    return 1;
}

// --- Ẩn HUD Tương Tác ---
stock CEF_HideInteraction(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;
    if (gPlayerInteractionActive[playerid])
    {
        CEF_EmitEvent(playerid, CEF_BROWSER_INTERACTION, "interaction.hideHint");
        CEF_DestroyBrowser(playerid, CEF_BROWSER_INTERACTION);
        gPlayerInteractionActive[playerid] = false;
    }
    return 1;
}

// --- Callback khi Khách chọn Y/N hoặc phím E ---
public OnCEF_InteractionResponse(playerid, browserid, bool:accepted)
{
    #pragma unused browserid

    if (!IsPlayerConnected(playerid)) return 0;

    if (accepted)
    {
        // Cho phép các module (House, Business, Entrance) hook xử lý
        CallLocalFunction("OnPlayerInteractionAccept", "i", playerid);
    }
    else
    {
        CEF_HideInteraction(playerid);
    }
    return 1;
}
