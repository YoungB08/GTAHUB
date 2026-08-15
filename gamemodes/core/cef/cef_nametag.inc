// =============================================================================
// CEF Custom Nametag Overlay - GTAHUB
// Architecture: Single Transparent Fullscreen Browser Overlay + Server 3D Payload
// =============================================================================

#if defined _cef_nametag_included
    #endinput
#endif
#define _cef_nametag_included

#include <YSI_Coding\y_hooks>
#include <role/role.inc>

forward CEF_ShowCustomNametag(playerid);
forward CEF_HideCustomNametag(playerid);
forward CEF_UpdateNametags(playerid);
forward CEF_UpdateNametagsTimer(playerid);

static bool:gPlayerNametagCreated[MAX_PLAYERS];
static gNametagTimer[MAX_PLAYERS] = {-1, ...};

stock CEF_Nametag_OnConnect(playerid)
{
    gPlayerNametagCreated[playerid] = false;
    if (gNametagTimer[playerid] != -1) {
        KillTimer(gNametagTimer[playerid]);
        gNametagTimer[playerid] = -1;
    }
    return 1;
}

stock CEF_Nametag_OnDisconnect(playerid)
{
    gPlayerNametagCreated[playerid] = false;
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

    if (!gPlayerNametagCreated[playerid])
    {
        CEF_DestroyBrowser(playerid, CEF_BROWSER_CUSTOM_NAMETAG);
        CEF_CreateBrowser(playerid, CEF_BROWSER_CUSTOM_NAMETAG, CEF_URL_CUSTOM_NAMETAG, false, false);
    }
    return 1;
}

stock CEF_Nametag_OnBrowserCreated(playerid, browserid, bool:success)
{
    if (browserid == CEF_BROWSER_CUSTOM_NAMETAG && success)
    {
        gPlayerNametagCreated[playerid] = true;
        if (gNametagTimer[playerid] != -1) {
            KillTimer(gNametagTimer[playerid]);
        }
        // Run update timer at 50ms (~20 FPS server pipeline)
        gNametagTimer[playerid] = SetTimerEx("CEF_UpdateNametagsTimer", 50, true, "i", playerid);
    }
    return 1;
}

stock CEF_HideCustomNametag(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;

    if (gNametagTimer[playerid] != -1) {
        KillTimer(gNametagTimer[playerid]);
        gNametagTimer[playerid] = -1;
    }
    if (gPlayerNametagCreated[playerid])
    {
        CEF_DestroyBrowser(playerid, CEF_BROWSER_CUSTOM_NAMETAG);
        gPlayerNametagCreated[playerid] = false;
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
    if (!IsPlayerConnected(playerid) || !gPlayerNametagCreated[playerid])
        return 0;

    new Float:cx = 0.0, Float:cy = 0.0, Float:cz = 0.0;
    new Float:fx = 0.0, Float:fy = 0.0, Float:fz = 0.0;
    GetPlayerCameraPos(playerid, cx, cy, cz);
    GetPlayerCameraFrontVector(playerid, fx, fy, fz);

    CEF_EmitEvent(playerid, CEF_BROWSER_CUSTOM_NAMETAG, "GTAHUB:SetLocalPlayerId", CEF_INT(playerid));

    new Float:px = 0.0, Float:py = 0.0, Float:pz = 0.0;
    GetPlayerPos(playerid, px, py, pz);

    static gNametagJSONBuffer[4096];
    format(gNametagJSONBuffer, sizeof(gNametagJSONBuffer), "{\"cam\":{\"cx\":%.2f,\"cy\":%.2f,\"cz\":%.2f,\"fx\":%.4f,\"fy\":%.4f,\"fz\":%.4f},\"players\":[",
        cx, cy, cz, fx, fy, fz
    );

    new count = 0;

    foreach (new targetid : Player)
    {
        if (!IsPlayerConnected(targetid))
            continue;

        new bool:isLocal = (targetid == playerid);

        if (!IsPlayerRoleVisible(targetid))
            continue;

        new Float:tx = 0.0, Float:ty = 0.0, Float:tz = 0.0;
        GetPlayerPos(targetid, tx, ty, tz);

        new Float:dist = 0.0;
        if (!isLocal)
        {
            if (GetPlayerVirtualWorld(playerid) != GetPlayerVirtualWorld(targetid))
                continue;

            if (GetPlayerInterior(playerid) != GetPlayerInterior(targetid))
                continue;

            dist = VectorSize(px - tx, py - ty, pz - tz);
            if (dist > 35.0)
                continue;
        }

        // Target Head Position (Z + 1.15m)
        new Float:headZ = tz + 1.15;

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

        if (count > 0) strcat(gNametagJSONBuffer, ",");

        new entry[512];
        format(entry, sizeof(entry),
            "{\"id\":%d,\"name\":\"%s\",\"hp\":%d,\"arm\":%d,\"color\":%d,\"local\":%s,\"rainbow\":%s,\"dist\":%.1f,\"x\":%.2f,\"y\":%.2f,\"z\":%.2f,\"roles\":%s,\"voice\":%s,\"afk\":%s}",
            targetid, pName, floatround(hp), floatround(arm), nColor, isLocal ? "true" : "false", rainbow ? "true" : "false",
            dist, tx, ty, headZ, rolesStr, isVoice ? "true" : "false", isAFK ? "true" : "false"
        );

        if (strlen(gNametagJSONBuffer) + strlen(entry) + 5 < sizeof(gNametagJSONBuffer))
        {
            strcat(gNametagJSONBuffer, entry, sizeof(gNametagJSONBuffer));
            count++;
        }
    }

    strcat(gNametagJSONBuffer, "]}", sizeof(gNametagJSONBuffer));

    CEF_EmitEvent(playerid, CEF_BROWSER_CUSTOM_NAMETAG, "GTAHUB:UpdateNametags", CEF_STR(gNametagJSONBuffer));
    return 1;
}

hook OnPlayerSecondUpdate@cef_nt(playerid)
{
    if (gPlayerNametagCreated[playerid] && gNametagTimer[playerid] == -1)
    {
        CEF_UpdateNametags(playerid);
    }
    return 1;
}
