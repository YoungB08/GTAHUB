// =============================================================================
// CEF HUD - GTAHUB
// Tích hợp Status HUD (máu, giáp, đói, khát, tiền, coin, vũ khí, số người chơi...)
// =============================================================================

#include <YSI_Coding\y_hooks>

forward CEF_ShowStatusHUD(playerid);
forward CEF_HideStatusHUD(playerid);
forward CEF_UpdateStatusHUD(playerid);
forward CEF_UpdateStatusHUDDelayed(playerid);

// --- Forward Getters từ player_survival ---
forward Survival_GetHunger(playerid);
forward Survival_GetThirst(playerid);

// --- Mở Status HUD overlay ---
stock CEF_ShowStatusHUD(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;

    // Đóng browser cũ nếu có
    CEF_DestroyBrowser(playerid, CEF_BROWSER_STATUS_HUD);

    // Ẩn toàn bộ HUD zin, ngoại trừ minimap (RADAR)
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_ALL, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_RADAR, true);

    // Tạo browser overlay (focused = false, controls_chat = true)
    CEF_CreateBrowser(playerid, CEF_BROWSER_STATUS_HUD, CEF_URL_STATUS_HUD, false, true);

    // Cập nhật chỉ số sau 500ms khi CEF browser sẵn sàng
    SetTimerEx("CEF_UpdateStatusHUDDelayed", 500, false, "i", playerid);
    return 1;
}

// --- Ẩn / Đóng Status HUD overlay ---
stock CEF_HideStatusHUD(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;
    CEF_DestroyBrowser(playerid, CEF_BROWSER_STATUS_HUD);
    return 1;
}

public CEF_UpdateStatusHUDDelayed(playerid)
{
    CEF_UpdateStatusHUD(playerid);
    return 1;
}

// --- Cập nhật thông tin HUD lên CEF ---
public CEF_UpdateStatusHUD(playerid)
{
    if (!IsPlayerConnected(playerid) || !Char_IsActive(playerid))
        return 0;

    new Float:hp = 100.0, Float:arm = 0.0;
    GetPlayerHealth(playerid, hp);
    GetPlayerArmour(playerid, arm);

    new weaponId = GetPlayerWeapon(playerid);
    new ammo = GetPlayerAmmo(playerid);
    new ammoStr[32];
    if (weaponId == 0)
    {
        format(ammoStr, sizeof(ammoStr), "-");
    }
    else
    {
        format(ammoStr, sizeof(ammoStr), "%d", ammo);
    }

    new onlineCount = 0;
    foreach(new i : Player)
    {
        onlineCount++;
    }

    new json[384];
    format(json, sizeof(json),
        "{\"health\":%d,\"armor\":%d,\"hunger\":%d,\"thirst\":%d,\"stamina\":100,\"cash\":%d,\"coin\":0,\"id\":%d,\"ping\":%d,\"online\":%d,\"wanted\":%d,\"weaponId\":%d,\"weaponAmmo\":\"%s\"}",
        floatround(hp),
        floatround(arm),
        Survival_GetHunger(playerid),
        Survival_GetThirst(playerid),
        GetPlayerMoney(playerid),
        playerid,
        GetPlayerPing(playerid),
        onlineCount,
        GetPlayerWantedLevel(playerid),
        weaponId,
        ammoStr
    );

    CEF_EmitEvent(playerid, CEF_BROWSER_STATUS_HUD, "GTAHUB:UpdateStatus", CEF_STR(json));
    return 1;
}

// Hook cập nhật thông tin HUD mỗi 1 giây
hook OnPlayerSecondUpdate(playerid)
{
    if (Char_IsActive(playerid))
    {
        CEF_UpdateStatusHUD(playerid);
    }
    return 1;
}
