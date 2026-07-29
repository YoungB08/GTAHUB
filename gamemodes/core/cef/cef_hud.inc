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

forward OnCEF_SetHUDFocus(playerid, browserid, focusState);

// --- Mở Status HUD overlay ---
stock CEF_ShowStatusHUD(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;

    // Đóng browser cũ nếu có
    CEF_DestroyBrowser(playerid, CEF_BROWSER_STATUS_HUD);

    // Ẩn riêng từng thành phần HUD zin của GTA SA (Máu, Giáp, Tiền, Vũ khí, Đạn, Truy nã, Nhịp thở)
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_HEALTH, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_ARMOUR, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_MONEY, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_WEAPON, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_AMMO, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_WANTED_STARS, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_BREATH, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_RADAR, true);

    // Mở phím F7 (VK_F7 = 118)
    CEF_EnableKey(playerid, 118, true);

    // Tạo browser overlay (focused = false, controls_chat = false để chatbox SA-MP luôn hoạt động)
    CEF_CreateBrowser(playerid, CEF_BROWSER_STATUS_HUD, CEF_URL_STATUS_HUD, false, false);

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

    // Gửi event qua tham số trực tiếp & chuỗi JSON
    CEF_EmitEvent(playerid, CEF_BROWSER_STATUS_HUD, "GTAHUB:UpdateStatusData",
        CEF_INT(floatround(hp)),
        CEF_INT(floatround(arm)),
        CEF_INT(Survival_GetHunger(playerid)),
        CEF_INT(Survival_GetThirst(playerid)),
        CEF_INT(GetPlayerMoney(playerid)),
        CEF_INT(playerid),
        CEF_INT(GetPlayerPing(playerid)),
        CEF_INT(onlineCount),
        CEF_INT(GetPlayerWantedLevel(playerid)),
        CEF_INT(weaponId),
        CEF_STR(ammoStr)
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

static bool:gPlayerHUDFocused[MAX_PLAYERS];

// --- Event callback: Bật/Tắt con trỏ chuột khi mở/đóng cài đặt HUD ---
public OnCEF_SetHUDFocus(playerid, browserid, focusState)
{
    #pragma unused browserid
    if (!IsPlayerConnected(playerid)) return 0;
    gPlayerHUDFocused[playerid] = bool:focusState;
    CEF_FocusBrowser(playerid, CEF_BROWSER_STATUS_HUD, gPlayerHUDFocused[playerid]);
    return 1;
}

// --- Lệnh mở/đóng Cài đặt Status HUD ---
CMD:hud(playerid, params[])
{
    if (!IsPlayerConnected(playerid))
        return 1;

    if (!Char_IsActive(playerid))
        return SendErrorMsg(playerid, "Ban phai chon nhan vat vao game de dung lenh /hud."), 1;

    gPlayerHUDFocused[playerid] = !gPlayerHUDFocused[playerid];
    CEF_FocusBrowser(playerid, CEF_BROWSER_STATUS_HUD, gPlayerHUDFocused[playerid]);
    CEF_EmitEvent(playerid, CEF_BROWSER_STATUS_HUD, "GTAHUB:ToggleHUDPanel");
    return 1;
}
