#include <YSI_Coding\y_hooks>

#define INTERACTION_INVALID_ID      (-1)
#define INTERACTION_LABEL_DISTANCE  (25.0)
#define INTERACTION_PROMPT_TIME     (-1)
#define INTERACTION_TELEPORT_DELAY  (1200)
#define INTERACTION_PROMPT_LOCK_MS  (2200)

enum {
    INTERACTION_TYPE_NONE = 0,
    INTERACTION_TYPE_ENTRANCE,
    INTERACTION_TYPE_HOUSE,
    INTERACTION_TYPE_ATM,
    INTERACTION_TYPE_PHONE,
    INTERACTION_TYPE_JOB,
    INTERACTION_TYPE_OBJECT
}

static
    PlayerInteractionType[MAX_PLAYERS],
    PlayerInteractionID[MAX_PLAYERS],
    PlayerTeleportToken[MAX_PLAYERS],
    PlayerPromptLockUntilTick[MAX_PLAYERS];

forward Interaction_UnfreezePlayer(playerid, token);
public Interaction_UnfreezePlayer(playerid, token)
{
    if(IsPlayerConnected(playerid) && PlayerTeleportToken[playerid] == token) {
        TogglePlayerControllable(playerid, true);
        SetCameraBehindPlayer(playerid);
    }
    return 1;
}

hook OnPlayerConnect(playerid)
{
    PlayerInteractionType[playerid] = INTERACTION_TYPE_NONE;
    PlayerInteractionID[playerid] = INTERACTION_INVALID_ID;
    PlayerTeleportToken[playerid] = 0;
    PlayerPromptLockUntilTick[playerid] = 0;
    return 1;
}

stock Interaction_BuildPointLabel(output[], len, const pointType[], const name[], const status[], const action[])
{
    format(
        output,
        len,
        C_ORANGE"%s\n"C_WHITE"%s\n"C_GREY"%s\n"C_YELLOW"Nhan "C_GREEN"'~k~~CONVERSATION_YES~' "C_YELLOW"de %s",
        pointType,
        name,
        status,
        action
    );
    return 1;
}

stock Interaction_ShowPrompt(playerid, type, interactionid, const caption[], const action[])
{
    if(GetTickCount() < PlayerPromptLockUntilTick[playerid]) {
        return 0;
    }

    if(
        PlayerInteractionType[playerid] != INTERACTION_TYPE_NONE &&
        PlayerInteractionType[playerid] != type &&
        type > PlayerInteractionType[playerid]
    ) {
        return 0;
    }

    new
        info[144];

    format(
        info,
        sizeof(info),
        "Nhan ~g~~k~~CONVERSATION_YES~~w~ de %s.~n~Nhan ~r~~k~~CONVERSATION_NO~~w~ de huy.",
        action
    );

    PlayerInteractionType[playerid] = type;
    PlayerInteractionID[playerid] = interactionid;
    return MessageBox_Show(playerid, TYPE_INFORMATION, caption, info, INTERACTION_PROMPT_TIME);
}

stock Interaction_LockPrompts(playerid, duration = INTERACTION_PROMPT_LOCK_MS)
{
    PlayerPromptLockUntilTick[playerid] = GetTickCount() + duration;
    return 1;
}

stock Interaction_ClearPrompt(playerid, type = INTERACTION_TYPE_NONE, interactionid = INTERACTION_INVALID_ID)
{
    if(type != INTERACTION_TYPE_NONE && PlayerInteractionType[playerid] != type) {
        return 0;
    }

    if(interactionid != INTERACTION_INVALID_ID && PlayerInteractionID[playerid] != interactionid) {
        return 0;
    }

    PlayerInteractionType[playerid] = INTERACTION_TYPE_NONE;
    PlayerInteractionID[playerid] = INTERACTION_INVALID_ID;
    HideMessageBox(playerid, TYPE_INFORMATION);
    return 1;
}

stock bool:Interaction_IsActive(playerid, type, interactionid = INTERACTION_INVALID_ID)
{
    if(PlayerInteractionType[playerid] != type) {
        return false;
    }

    if(interactionid != INTERACTION_INVALID_ID && PlayerInteractionID[playerid] != interactionid) {
        return false;
    }

    return true;
}

stock Interaction_GetActiveID(playerid)
{
    return PlayerInteractionID[playerid];
}

stock Interaction_LoadSafeTeleport(playerid, Float:x, Float:y, Float:z, Float:a, worldid, interiorid, bool:freezePlayer = true, delay = INTERACTION_TELEPORT_DELAY)
{
    new
        unfreezeDelay = delay + GetPlayerPing(playerid),
        token = PlayerTeleportToken[playerid];

    if(unfreezeDelay < 500) {
        unfreezeDelay = 500;
    }

    if(freezePlayer) {
        PlayerTeleportToken[playerid]++;
        token = PlayerTeleportToken[playerid];
        TogglePlayerControllable(playerid, false);
    }

    SetPlayerInterior(playerid, interiorid);
    SetPlayerVirtualWorld(playerid, worldid);
    Streamer_UpdateEx(playerid, x, y, z, worldid, interiorid, -1, unfreezeDelay, 0);
    SetPlayerPos(playerid, x, y, z);
    SetPlayerFacingAngle(playerid, a);
    SetCameraBehindPlayer(playerid);

    if(freezePlayer) {
        SetTimerEx("Interaction_UnfreezePlayer", unfreezeDelay, false, "ii", playerid, token);
        SetTimerEx("Interaction_UnfreezePlayer", unfreezeDelay + 1500, false, "ii", playerid, token);
    }
    return 1;
}
