// =============================================================================
// CEF Core - GTAHUB
// Khởi tạo CEF, đăng ký resources, quản lý browser lifecycle & callbacks
// =============================================================================

#if defined _cef_core_included
    #endinput
#endif
#define _cef_core_included

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
#define CEF_BROWSER_CUSTOM_NAMETAG  (6)
#define CEF_BROWSER_BANKING         (7)
#define CEF_BROWSER_INTERACTION     (8)
#define CEF_BROWSER_INVENTORY       (9)
#define CEF_BROWSER_TRUNK           (10)
#define CEF_BROWSER_RADIAL          (11)
#define CEF_BROWSER_LICENSES        (12)
#define CEF_BROWSER_NOTIFY          (13)
#define CEF_BROWSER_PHONE           (14)
#define CEF_BROWSER_COUNT           (15)

// --- Resource URL Macros ---
#define CEF_URL_LOGIN               "http://cef/login/index.html"
#define CEF_URL_REGISTER            "http://cef/register/index.html"
#define CEF_URL_ACTIVE              "http://cef/active/index.html"
#define CEF_URL_CHAR_SELECT         "http://cef/character_selection/index.html"
#define CEF_URL_CHAR_CREATE         "http://cef/character_creation/index.html"
#define CEF_URL_STATUS_HUD          "http://cef/status_hud/index.html"
#define CEF_URL_CUSTOM_NAMETAG      "http://cef/custom_nametag/index.html"
#define CEF_URL_BANKING             "http://cef/banking/index.html"
#define CEF_URL_INTERACTION         "http://cef/interaction/index.html"
#define CEF_URL_INVENTORY           "http://cef/inventory/index.html"
#define CEF_URL_TRUNK               "http://cef/trunk/index.html"
#define CEF_URL_RADIAL              "http://cef/radial_menu/index.html"
#define CEF_URL_LICENSES            "http://cef/licenses/index.html"
#define CEF_URL_NOTIFY              "http://cef/notifications/index.html"
#define CEF_URL_PHONE               "http://cef/phone/index.html"

native CEF_NativeCreateBrowser(playerid, browserid, const url[], bool:focused,
    bool:controlsChat = true, Float:width = 0.0, Float:height = 0.0) = CEF_CreateBrowser;
native CEF_NativeDestroyBrowser(playerid, browserid) = CEF_DestroyBrowser;

static bool:gCEFBrowserRequested[MAX_PLAYERS][CEF_BROWSER_COUNT];

stock CEF_TrackedCreateBrowser(playerid, browserid, const url[], bool:focused,
    bool:controlsChat = true, Float:width = 0.0, Float:height = 0.0)
{
    if (browserid < 0 || browserid >= CEF_BROWSER_COUNT) return 0;

    gCEFBrowserRequested[playerid][browserid] = true;
    CEF_NativeCreateBrowser(playerid, browserid, url, focused, controlsChat, width, height);
    return 1;
}

stock CEF_TrackedDestroyBrowser(playerid, browserid)
{
    if (browserid < 0 || browserid >= CEF_BROWSER_COUNT) return 0;
    if (!gCEFBrowserRequested[playerid][browserid]) return 0;

    gCEFBrowserRequested[playerid][browserid] = false;
    CEF_NativeDestroyBrowser(playerid, browserid);
    return 1;
}

stock CEF_ResetTrackedBrowsers(playerid)
{
    for (new browserid = 0; browserid < CEF_BROWSER_COUNT; browserid++)
    {
        gCEFBrowserRequested[playerid][browserid] = false;
    }
    return 1;
}

stock CEF_TrackBrowserCreated(playerid, browserid, bool:success)
{
    if (browserid < 0 || browserid >= CEF_BROWSER_COUNT) return 0;
    if (!success) gCEFBrowserRequested[playerid][browserid] = false;
    return 1;
}

#define CEF_CreateBrowser CEF_TrackedCreateBrowser
#define CEF_DestroyBrowser CEF_TrackedDestroyBrowser

// --- Include CEF Modules ---
#include <cef_hud>
#include <cef_nametag>

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
    CEF_HideStatusHUD(playerid);
    CEF_HideCustomNametag(playerid);
    return 1;
}

stock CEF_EnableGameUI(playerid)
{
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

    // Mở Status HUD CEF Overlay & Custom Nametag
    CEF_ShowStatusHUD(playerid);
    CEF_ShowCustomNametag(playerid);
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
    CEF_AddResource("custom_nametag");
    CEF_AddResource("banking");
    CEF_AddResource("interaction");
    CEF_AddResource("inventory");
    CEF_AddResource("trunk");
    CEF_AddResource("radial_menu");
    CEF_AddResource("licenses");
    CEF_AddResource("notifications");
    CEF_AddResource("phone");
    CEF_AddResource("shared");

    CEF_RegisterEvent("GTAHUB:SetHUDFocus", "OnCEF_SetHUDFocus", Argument_Integer);

    print("[CEF] Resources registered: auth, character, status HUD, custom nametag");
    return 1;
}

// --- Hook OnPlayerConnect ---
hook OnPlayerConnect@cef_core(playerid)
{
    CEF_ResetTrackedBrowsers(playerid);
    gPlayerCEFReady[playerid] = false;
    gPlayerPendingUI[playerid] = CEF_PENDING_NONE;
    CEF_HUD_OnConnect(playerid);
    CEF_Nametag_OnConnect(playerid);
    CEF_DisableGameUI(playerid);
    return 1;
}

// --- Hook OnPlayerSpawn ---
hook OnPlayerSpawn@cef_core(playerid)
{
    if (Char_IsActive(playerid))
    {
        CEF_EnableGameUI(playerid);
    }
    return 1;
}

// --- Hook OnPlayerDisconnect: Dọn dẹp browsers ---
hook OnPlayerDisconnect@cef_core(playerid, reason)
{
    #pragma unused reason
    gPlayerCEFReady[playerid] = false;
    gPlayerPendingUI[playerid] = CEF_PENDING_NONE;
    CEF_HUD_OnDisconnect(playerid);
    CEF_Nametag_OnDisconnect(playerid);
    CEF_ResetTrackedBrowsers(playerid);
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
    if (!IsPlayerConnected(playerid)) return false;
    return gPlayerCEFReady[playerid] || CEF_PlayerHasPlugin(playerid);
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
    else if (Char_IsActive(playerid))
    {
        CEF_EnableGameUI(playerid);
    }
    else
    {
        printf("[CEF] OnCefReady: Player %d ready, waiting for login/character selection flow.", playerid);
    }
    return 1;
}

public OnCefBrowserCreated(playerid, browserid, bool:success, E_CEF_CREATE_STATUS:code, const reason[])
{
    #pragma unused reason
    printf("[CEF] OnCefBrowserCreated: playerid=%d, browserid=%d, success=%d, code=%d",
        playerid, browserid, success, _:code);
    CEF_TrackBrowserCreated(playerid, browserid, success);
    CEF_Nametag_OnBrowserCreated(playerid, browserid, success);
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
    CEF_DestroyBrowser(playerid, CEF_BROWSER_CUSTOM_NAMETAG);
}
