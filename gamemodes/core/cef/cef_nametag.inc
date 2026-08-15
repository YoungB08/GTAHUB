// =============================================================================
// CEF Custom Nametag Overlay - GTAHUB
// Hiển thị Nametag Custom qua CEF Overlay trên đầu Player (Pure Pawn + CEF)
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
        gNametagTimer[playerid] = SetTimerEx("CEF_UpdateNametagsTimer", 20, true, "i", playerid);
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

    static gNametagJSONBuffer[3072];
    format(gNametagJSONBuffer, sizeof(gNametagJSONBuffer), "{\"cam\":{\"cx\":%.2f,\"cy\":%.2f,\"cz\":%.2f,\"fx\":%.4f,\"fy\":%.4f,\"fz\":%.4f},\"players\":[",
        cx, cy, cz, fx, fy, fz
    );

    new count = 0;

    // Quét toàn bộ người chơi trong Server (Multi-Player Support)
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

            dist = VectorSize(px - tx, py - ty, pz - tz);
            if (dist > 35.0)
                continue;
        }

        // Tọa độ đầu người chơi (Z + 1.15m)
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

        if (count > 0) strcat(gNametagJSONBuffer, ",");

        new entry[512];
        format(entry, sizeof(entry),
            "{\"id\":%d,\"name\":\"%s\",\"visible\":true,\"health\":%d,\"armour\":%d,\"color\":%d,\"isLocal\":%s,\"rainbow\":%s,\"x\":%.2f,\"y\":%.2f,\"z\":%.2f,\"dist\":%.1f,\"roles\":%s}",
            targetid, pName, floatround(hp), floatround(arm), nColor, isLocal ? "true" : "false", rainbow ? "true" : "false",
            tx, ty, headZ, dist, rolesStr
        );
        strcat(gNametagJSONBuffer, entry, sizeof(gNametagJSONBuffer));
        count++;
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
