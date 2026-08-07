/**
 * @file ompvoice_system.inc
 * @brief Hệ thống Voice Chat cho OMPVoiceCore.
 */

#if defined _ompvoice_system_included
    #endinput
#endif
#define _ompvoice_system_included

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS & DEFINES
// =============================================================================

#if !defined COLOR_INFO
    #define COLOR_INFO  0x4FC3F7FF
#endif

#if !defined COLOR_OK
    #define COLOR_OK    0x66BB6AFF
#endif

#if !defined COLOR_ERROR
    #define COLOR_ERROR 0xEF5350FF
#endif

// =============================================================================
// GLOBAL DATA
// =============================================================================

static g_ProximityChannel = -1;
static bool:g_VoiceEnabled[MAX_PLAYERS];

// =============================================================================
// HOOKS
// =============================================================================

hook OnGameModeInit()
{
    // The current client transmits on channel 1, so create proximity first.
    g_ProximityChannel = OV_CreateChannel(25.0);
    printf("[OMPVoiceCore] Proximity channel: %d", g_ProximityChannel);
    return 1;
}

hook OnGameModeExit()
{
    if (g_ProximityChannel > 0)
    {
        OV_DestroyChannel(g_ProximityChannel);
        g_ProximityChannel = -1;
    }
    return 1;
}

hook OnPlayerConnect(playerid)
{
    g_VoiceEnabled[playerid] = true;
    OV_EnableVoice(playerid, true);
    OV_SetVolume(playerid, 1.0);
    OV_SetTalkKey(playerid, 0x5A); // Z key.
    OV_ShowHudIcon(playerid, true);

    SendClientMessage(playerid, COLOR_INFO, "Voice enabled. Hold Z to talk, F8 toggles local loopback.");
    SendClientMessage(playerid, COLOR_INFO, "Commands: /voicehelp, /voiceon, /voiceoff, /vcall [id], /hangup, /mute [id], /unmute [id]");
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    OV_EndPhoneCall(playerid);
    g_VoiceEnabled[playerid] = false;
    return 1;
}

// =============================================================================
// COMMANDS
// =============================================================================

CMD:voicehelp(playerid, params[])
{
    SendClientMessage(playerid, COLOR_INFO, "Hold Z to transmit. Use F8 for a one-client loopback test.");
    SendClientMessage(playerid, COLOR_INFO, "Use /ovdiag for server diagnostics. Voice UDP uses port 7775.");
    return 1;
}

CMD:voiceon(playerid, params[])
{
    g_VoiceEnabled[playerid] = true;
    OV_EnableVoice(playerid, true);
    OV_ShowHudIcon(playerid, true);
    SendClientMessage(playerid, COLOR_OK, "Voice enabled.");
    return 1;
}

CMD:voiceoff(playerid, params[])
{
    g_VoiceEnabled[playerid] = false;
    OV_EnableVoice(playerid, false);
    OV_ShowHudIcon(playerid, false);
    SendClientMessage(playerid, COLOR_OK, "Voice disabled.");
    return 1;
}

CMD:vcall(playerid, params[])
{
    new targetid;
    if (sscanf(params, "u", targetid) || targetid == INVALID_PLAYER_ID)
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Usage: /vcall [connected player id]"), 1;
    }
    if (targetid == playerid || !IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Usage: /vcall [connected player id]"), 1;
    }
    if (!OV_StartPhoneCall(playerid, targetid))
    {
        return SendClientMessage(playerid, COLOR_ERROR, "The call could not be started; one participant may already be in a call."), 1;
    }
    SendClientMessage(playerid, COLOR_OK, "Voice call started. Use /hangup to end it.");
    SendClientMessage(targetid, COLOR_OK, "An OMPVoiceCore call was connected. Use /hangup to end it.");
    return 1;
}

CMD:mute(playerid, params[])
{
    new targetid;
    if (sscanf(params, "u", targetid) || targetid == INVALID_PLAYER_ID)
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Usage: /mute [connected player id]"), 1;
    }
    if (targetid == playerid || !IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Usage: /mute [connected player id]"), 1;
    }
    OV_SetMuted(playerid, targetid, true);
    SendClientMessage(playerid, COLOR_OK, "Player voice muted.");
    return 1;
}

CMD:unmute(playerid, params[])
{
    new targetid;
    if (sscanf(params, "u", targetid) || targetid == INVALID_PLAYER_ID)
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Usage: /unmute [connected player id]"), 1;
    }
    if (targetid == playerid || !IsPlayerConnected(targetid))
    {
        return SendClientMessage(playerid, COLOR_ERROR, "Usage: /unmute [connected player id]"), 1;
    }
    OV_SetMuted(playerid, targetid, false);
    SendClientMessage(playerid, COLOR_OK, "Player voice unmuted.");
    return 1;
}
