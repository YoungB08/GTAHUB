// =============================================================================
// CEF Custom Nametag Overlay - GTAHUB (Native C++ World2D Plugin Engine)
// Dùng CEF_CreateWorld2DBrowser và CEF_SetWorld2DBrowserPos
// C++ Client Plugin tự động tính toán vị trí 3D->2D WorldToScreen 100% mượt 60-144 FPS
// =============================================================================

#if defined _cef_nametag_included
    #endinput
#endif
#define _cef_nametag_included

#include <YSI_Coding\y_hooks>
#include <role/role.inc>

#define CEF_WORLD2D_NAMETAG_BASE    (9000)

static bool:gWorld2DNametagActive[MAX_PLAYERS][MAX_PLAYERS];
static gNametagTimer[MAX_PLAYERS] = {-1, ...};

forward CEF_ShowCustomNametag(playerid);
forward CEF_HideCustomNametag(playerid);
forward CEF_UpdateNametags(playerid);
forward CEF_UpdateNametagsTimer(playerid);

stock CEF_Nametag_OnConnect(playerid)
{
    for (new i = 0; i < MAX_PLAYERS; i++) {
        gWorld2DNametagActive[playerid][i] = false;
    }
    if (gNametagTimer[playerid] != -1) {
        KillTimer(gNametagTimer[playerid]);
        gNametagTimer[playerid] = -1;
    }
    return 1;
}

stock CEF_Nametag_OnDisconnect(playerid)
{
    for (new i = 0; i < MAX_PLAYERS; i++) {
        if (gWorld2DNametagActive[playerid][i]) {
            CEF_DestroyBrowser(playerid, CEF_WORLD2D_NAMETAG_BASE + i);
            gWorld2DNametagActive[playerid][i] = false;
        }
    }
    if (gNametagTimer[playerid] != -1) {
        KillTimer(gNametagTimer[playerid]);
        gNametagTimer[playerid] = -1;
    }
    return 1;
}

stock CEF_ShowCustomNametag(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;

    ShowNameTags(false);
    foreach (new targetid : Player)
    {
        ShowPlayerNameTagForPlayer(playerid, targetid, false);
    }

    if (gNametagTimer[playerid] != -1) {
        KillTimer(gNametagTimer[playerid]);
    }
    gNametagTimer[playerid] = SetTimerEx("CEF_UpdateNametagsTimer", 50, true, "i", playerid);
    return 1;
}

stock CEF_Nametag_OnBrowserCreated(playerid, browserid, bool:success)
{
    #pragma unused playerid, browserid, success
    return 1;
}

stock CEF_HideCustomNametag(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;

    for (new i = 0; i < MAX_PLAYERS; i++) {
        if (gWorld2DNametagActive[playerid][i]) {
            CEF_DestroyBrowser(playerid, CEF_WORLD2D_NAMETAG_BASE + i);
            gWorld2DNametagActive[playerid][i] = false;
        }
    }

    if (gNametagTimer[playerid] != -1) {
        KillTimer(gNametagTimer[playerid]);
        gNametagTimer[playerid] = -1;
    }
    return 1;
}

public CEF_UpdateNametagsTimer(playerid)
{
    if (!IsPlayerConnected(playerid))
    {
        if (gNametagTimer[playerid] != -1) {
            KillTimer(gNametagTimer[playerid]);
            gNametagTimer[playerid] = -1;
        }
        return 0;
    }
    return CEF_UpdateNametags(playerid);
}

public CEF_UpdateNametags(playerid)
{
    if (!IsPlayerConnected(playerid))
        return 0;

    new Float:px = 0.0, Float:py = 0.0, Float:pz = 0.0;
    GetPlayerPos(playerid, px, py, pz);

    foreach (new targetid : Player)
    {
        if (!IsPlayerConnected(targetid))
        {
            if (gWorld2DNametagActive[playerid][targetid]) {
                CEF_DestroyBrowser(playerid, CEF_WORLD2D_NAMETAG_BASE + targetid);
                gWorld2DNametagActive[playerid][targetid] = false;
            }
            continue;
        }

        new bool:isLocal = (targetid == playerid);

        if (!IsPlayerRoleVisible(targetid))
        {
            if (gWorld2DNametagActive[playerid][targetid]) {
                CEF_DestroyBrowser(playerid, CEF_WORLD2D_NAMETAG_BASE + targetid);
                gWorld2DNametagActive[playerid][targetid] = false;
            }
            continue;
        }

        new Float:tx = 0.0, Float:ty = 0.0, Float:tz = 0.0;
        GetPlayerPos(targetid, tx, ty, tz);

        if (!isLocal)
        {
            if (GetPlayerVirtualWorld(playerid) != GetPlayerVirtualWorld(targetid) ||
                GetPlayerInterior(playerid) != GetPlayerInterior(targetid))
            {
                if (gWorld2DNametagActive[playerid][targetid]) {
                    CEF_DestroyBrowser(playerid, CEF_WORLD2D_NAMETAG_BASE + targetid);
                    gWorld2DNametagActive[playerid][targetid] = false;
                }
                continue;
            }

            new Float:dist = VectorSize(px - tx, py - ty, pz - tz);
            if (dist > 35.0)
            {
                if (gWorld2DNametagActive[playerid][targetid]) {
                    CEF_DestroyBrowser(playerid, CEF_WORLD2D_NAMETAG_BASE + targetid);
                    gWorld2DNametagActive[playerid][targetid] = false;
                }
                continue;
            }
        }

        new Float:headZ = tz + 1.15;
        new w2dBid = CEF_WORLD2D_NAMETAG_BASE + targetid;

        // Khởi tạo hoặc cập nhật vị trí qua C++ Native World2D Engine
        if (!gWorld2DNametagActive[playerid][targetid])
        {
            CEF_CreateWorld2DBrowser(playerid, w2dBid, CEF_URL_CUSTOM_NAMETAG, tx, ty, headZ, 260.0, 90.0, 0.0, 0.5, 1.0);
            gWorld2DNametagActive[playerid][targetid] = true;
        }
        else
        {
            CEF_SetWorld2DBrowserPos(playerid, w2dBid, tx, ty, headZ);
        }

        new pName[MAX_PLAYER_NAME];
        GetPlayerName(targetid, pName, sizeof(pName));

        new Float:hp = 100.0, Float:arm = 0.0;
        GetPlayerHealth(targetid, hp);
        GetPlayerArmour(targetid, arm);

        new nColor = -1;
        GetPlayerNametagColor(targetid, nColor);

        new bool:rainbow = IsPlayerRainbowActive(targetid, -1);

        new rolesStr[512];
        GetPlayerRoleJSON(targetid, rolesStr, sizeof(rolesStr));

        new bool:isAFK = false;
        #if defined IsPlayerAFK
            isAFK = IsPlayerAFK(targetid);
        #endif

        new bool:isVoice = false;
        #if defined IsPlayerSpeaking
            isVoice = IsPlayerSpeaking(targetid);
        #endif

        CEF_EmitEvent(playerid, w2dBid, "GTAHUB:SetNametagData",
            CEF_INT(targetid),
            CEF_STR(pName),
            CEF_INT(floatround(hp)),
            CEF_INT(floatround(arm)),
            CEF_INT(nColor),
            CEF_BOOL(isLocal),
            CEF_BOOL(rainbow),
            CEF_STR(rolesStr),
            CEF_BOOL(isVoice),
            CEF_BOOL(isAFK)
        );
    }
    return 1;
}

// Cập nhật vị trí World2D Nametag liên tục theo gói tin di chuyển (Player Sync Packet)
hook OnPlayerUpdate@cef_nt_sync(playerid)
{
    new Float:x = 0.0, Float:y = 0.0, Float:z = 0.0;
    GetPlayerPos(playerid, x, y, z);
    new Float:headZ = z + 1.15;
    new w2dBid = CEF_WORLD2D_NAMETAG_BASE + playerid;

    foreach (new observerid : Player)
    {
        if (gWorld2DNametagActive[observerid][playerid])
        {
            CEF_SetWorld2DBrowserPos(observerid, w2dBid, x, y, headZ);
        }
    }
    return 1;
}
