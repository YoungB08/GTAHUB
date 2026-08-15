// =============================================================================
// CEF Vehicle Trunk System - GTAHUB
// Sự kiện JS -> Pawn: trunk.moveItem(fromType, fromSlot, toSlot, count), trunk.close
// Sự kiện Pawn -> JS: trunk.show(payload), trunk.hide
// =============================================================================

#if defined _cef_trunk_included
    #endinput
#endif
#define _cef_trunk_included

#include <YSI_Coding\y_hooks>

// --- Forwards ---
forward OnCEF_TrunkMoveItem(playerid, browserid, const fromType[], fromSlot, toSlot, count);
forward OnCEF_TrunkClose(playerid, browserid);

forward CEF_ShowTrunk(playerid, vehicleid);

hook OnGameModeInit()
{
    CEF_RegisterEvent("trunk.moveItem", "OnCEF_TrunkMoveItem", Argument_String, Argument_Integer, Argument_Integer, Argument_Integer);
    CEF_RegisterEvent("trunk.close", "OnCEF_TrunkClose");

    print("[CEF] Trunk events registered.");
    return 1;
}

stock CEF_ShowTrunk(playerid, vehicleid)
{
    #pragma unused vehicleid
    if (!IsPlayerConnected(playerid)) return 0;

    CEF_CreateBrowser(playerid, CEF_BROWSER_TRUNK, CEF_URL_TRUNK, true, true);
    return 1;
}

public OnCEF_TrunkMoveItem(playerid, browserid, const fromType[], fromSlot, toSlot, count)
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid)) return 0;
    printf("[CEF] Player %d moved item: Type=%s, From=%d, To=%d, Count=%d", playerid, fromType, fromSlot, toSlot, count);
    return 1;
}

public OnCEF_TrunkClose(playerid, browserid)
{
    #pragma unused browserid
    if (IsPlayerConnected(playerid))
    {
        CEF_DestroyBrowser(playerid, CEF_BROWSER_TRUNK);
    }
    return 1;
}
