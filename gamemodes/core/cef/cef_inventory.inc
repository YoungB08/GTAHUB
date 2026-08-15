// =============================================================================
// CEF Inventory 48-Slot System - GTAHUB
// Sự kiện JS -> Pawn: inventory.useItem(slotId), inventory.dropItem(slotId, count), inventory.giveItem(slotId, targetId, count), inventory.swapSlots(fromSlot, toSlot), inventory.close
// Sự kiện Pawn -> JS: inventory.show(payload), inventory.hide, inventory.setSkin(skinId, playerId, playerName)
// =============================================================================

#if defined _cef_inventory_included
    #endinput
#endif
#define _cef_inventory_included

#include <YSI_Coding\y_hooks>

// --- Forwards ---
forward OnCEF_InventoryUseItem(playerid, browserid, slotId);
forward OnCEF_InventoryDropItem(playerid, browserid, slotId, count);
forward OnCEF_InventoryGiveItem(playerid, browserid, slotId, targetId, count);
forward OnCEF_InventorySwapSlots(playerid, browserid, fromSlot, toSlot);
forward OnCEF_InventoryClose(playerid, browserid);

forward CEF_ShowInventory(playerid);

hook OnGameModeInit()
{
    CEF_RegisterEvent("inventory.useItem", "OnCEF_InventoryUseItem", Argument_Integer);
    CEF_RegisterEvent("inventory.dropItem", "OnCEF_InventoryDropItem", Argument_Integer, Argument_Integer);
    CEF_RegisterEvent("inventory.giveItem", "OnCEF_InventoryGiveItem", Argument_Integer, Argument_Integer, Argument_Integer);
    CEF_RegisterEvent("inventory.swapSlots", "OnCEF_InventorySwapSlots", Argument_Integer, Argument_Integer);
    CEF_RegisterEvent("inventory.close", "OnCEF_InventoryClose");

    print("[CEF] Inventory events registered.");
    return 1;
}

stock CEF_ShowInventory(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;

    CEF_CreateBrowser(playerid, CEF_BROWSER_INVENTORY, CEF_URL_INVENTORY, true, true);

    new skinId = GetPlayerSkin(playerid);
    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));

    CEF_EmitEvent(playerid, CEF_BROWSER_INVENTORY, "inventory.setSkin", CEF_INT(skinId), CEF_INT(playerid), CEF_STR(name));
    return 1;
}

public OnCEF_InventoryUseItem(playerid, browserid, slotId)
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid)) return 0;
    printf("[CEF] Player %d used item in slot %d", playerid, slotId);
    return 1;
}

public OnCEF_InventoryDropItem(playerid, browserid, slotId, count)
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid)) return 0;
    printf("[CEF] Player %d dropped %d items from slot %d", playerid, count, slotId);
    return 1;
}

public OnCEF_InventoryGiveItem(playerid, browserid, slotId, targetId, count)
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid)) return 0;
    printf("[CEF] Player %d gave %d items from slot %d to Target %d", playerid, count, slotId, targetId);
    return 1;
}

public OnCEF_InventorySwapSlots(playerid, browserid, fromSlot, toSlot)
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid)) return 0;
    printf("[CEF] Player %d swapped slot %d -> %d", playerid, fromSlot, toSlot);
    return 1;
}

public OnCEF_InventoryClose(playerid, browserid)
{
    #pragma unused browserid
    if (IsPlayerConnected(playerid))
    {
        CEF_DestroyBrowser(playerid, CEF_BROWSER_INVENTORY);
    }
    return 1;
}
