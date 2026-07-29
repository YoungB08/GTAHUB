// =============================================================================
// CEF Core - GTAHUB
// Khởi tạo CEF, đăng ký resources, quản lý browser lifecycle & callbacks
// =============================================================================

#include <cef>
#include <YSI_Coding\y_hooks>

// --- Forwards ---
forward bool:CEF_IsPlayerCEFReady(playerid);
forward bool:Char_IsActive(playerid);
forward CEF_ShowLoginPage(playerid);
forward CEF_ShowRegisterPage(playerid);
forward CEF_ShowCharacterSelection(playerid);

// --- Browser ID Constants ---
#define CEF_BROWSER_LOGIN           (0)
#define CEF_BROWSER_REGISTER        (1)
#define CEF_BROWSER_ACTIVE          (2)
#define CEF_BROWSER_CHAR_SELECT     (3)
#define CEF_BROWSER_CHAR_CREATE     (4)
#define CEF_BROWSER_STATUS_HUD      (5)

// --- Resource URL Macros ---
#define CEF_URL_LOGIN               "http://cef/login/index.html"
#define CEF_URL_REGISTER            "http://cef/register/index.html"
#define CEF_URL_ACTIVE              "http://cef/active/index.html"
#define CEF_URL_CHAR_SELECT         "http://cef/character_selection/index.html"
#define CEF_URL_CHAR_CREATE         "http://cef/character_creation/index.html"
#define CEF_URL_STATUS_HUD          "http://cef/status_hud/index.html"

// --- Include CEF Modules ---
#include <cef_hud>

// --- Pending UI States ---
enum {
    CEF_PENDING_NONE = 0,
    CEF_PENDING_LOGIN,
    CEF_PENDING_REGISTER,
    CEF_PENDING_CHAR_SELECT
};

// --- State tracking per player ---
static bool:gPlayerCEFReady[MAX_PLAYERS];
static gPlayerPendingUI[MAX_PLAYERS];

// --- Helpers: Tắt/Bật Chat & HUD khi ở trong CEF ---
stock CEF_DisableGameUI(playerid)
{
    CEF_ToggleChatInput(playerid, true);
    CEF_ClearChat(playerid);
    CEF_HideStatusHUD(playerid);
    return 1;
}

stock CEF_EnableGameUI(playerid)
{
    CEF_ToggleChatInput(playerid, true);

    // Ẩn riêng từng thành phần HUD mặc định của GTA SA (Máu, Giáp, Tiền, Vũ khí, Đạn, Truy nã, Nhịp thở)
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_HEALTH, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_ARMOUR, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_MONEY, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_WEAPON, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_AMMO, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_WANTED_STARS, false);
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_BREATH, false);

    // Giữ lại Minimap / Radar
    CEF_ToggleHudComponent(playerid, HUD_COMPONENT_RADAR, true);

    // Mở Status HUD CEF Overlay
    CEF_ShowStatusHUD(playerid);
    return 1;
}

// --- Hook OnGameModeInit: Đăng ký tất cả resources & events ---
hook OnGameModeInit()
{
    CEF_AddResource("login");
    CEF_AddResource("register");
    CEF_AddResource("active");
    CEF_AddResource("character_selection");
    CEF_AddResource("character_creation");
    CEF_AddResource("status_hud");

    CEF_RegisterEvent("GTAHUB:SetHUDFocus", "OnCEF_SetHUDFocus", Argument_Integer);

    print("[CEF] Resources registered: login, register, active, character_selection, character_creation, status_hud");
    return 1;
}

// --- Hook OnPlayerConnect ---
hook OnPlayerConnect(playerid)
{
    gPlayerCEFReady[playerid] = false;
    gPlayerPendingUI[playerid] = CEF_PENDING_NONE;
    CEF_DisableGameUI(playerid);
    return 1;
}

// --- Hook OnPlayerSpawn: Mở lại Chat & HUD khi đã spawn vào game ---
hook OnPlayerSpawn(playerid)
{
    if (Char_IsActive(playerid))
    {
        CEF_CloseAllBrowsers(playerid);
        TogglePlayerSpectating(playerid, false);
        CEF_EnableGameUI(playerid);
    }
    return 1;
}

// --- Hook OnPlayerDisconnect: Dọn dẹp browsers ---
hook OnPlayerDisconnect(playerid, reason)
{
    #pragma unused reason
    gPlayerCEFReady[playerid] = false;
    gPlayerPendingUI[playerid] = CEF_PENDING_NONE;
    CEF_DestroyBrowser(playerid, CEF_BROWSER_LOGIN);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_REGISTER);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_ACTIVE);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_CHAR_SELECT);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_CHAR_CREATE);
    return 1;
}

// --- Getter / Setter cho Pending UI ---
stock CEF_SetPendingUI(playerid, uiState)
{
    gPlayerPendingUI[playerid] = uiState;
    return 1;
}

stock CEF_GetPendingUI(playerid)
{
    return gPlayerPendingUI[playerid];
}

stock bool:CEF_IsPlayerCEFReady(playerid)
{
    return gPlayerCEFReady[playerid];
}

// --- CEF Callbacks Logging & Event Dispatch ---
public OnCefInitialize(playerid, bool:success, E_CEF_INIT_REASON:reason, const message[])
{
    printf("[CEF] OnCefInitialize: playerid=%d, success=%d, reason=%d, message='%s'", playerid, success, _:reason, message);
    return 1;
}

public OnCefReady(playerid)
{
    printf("[CEF] OnCefReady: playerid=%d client is fully ready!", playerid);
    gPlayerCEFReady[playerid] = true;

    new pendingUI = gPlayerPendingUI[playerid];
    gPlayerPendingUI[playerid] = CEF_PENDING_NONE;

    if (pendingUI == CEF_PENDING_LOGIN)
    {
        printf("[CEF] OnCefReady: Triggering Pending LOGIN UI for player %d", playerid);
        CEF_ShowLoginPage(playerid);
    }
    else if (pendingUI == CEF_PENDING_REGISTER)
    {
        printf("[CEF] OnCefReady: Triggering Pending REGISTER UI for player %d", playerid);
        CEF_ShowRegisterPage(playerid);
    }
    else if (pendingUI == CEF_PENDING_CHAR_SELECT)
    {
        printf("[CEF] OnCefReady: Triggering Pending CHARACTER SELECTION UI for player %d", playerid);
        CEF_ShowCharacterSelection(playerid);
    }
    return 1;
}

public OnCefBrowserCreated(playerid, browserid, bool:success, E_CEF_CREATE_STATUS:code, const reason[])
{
    printf("[CEF] OnCefBrowserCreated: playerid=%d, browserid=%d, success=%d, code=%d, reason='%s'",
        playerid, browserid, success, _:code, reason);
    return 1;
}

// --- Utility: Đóng tất cả CEF browsers của player ---
stock CEF_CloseAllBrowsers(playerid)
{
    CEF_DestroyBrowser(playerid, CEF_BROWSER_LOGIN);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_REGISTER);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_ACTIVE);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_CHAR_SELECT);
    CEF_DestroyBrowser(playerid, CEF_BROWSER_CHAR_CREATE);
}
