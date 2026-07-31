#include <YSI_Coding\y_hooks>
#include <formatex>

static Text: LeaveJoinTD;
#pragma unused LeaveJoinTD

hook OnGameModeInit() {

    return 1;
}

hook OnGameModeExit()
{
    return 1;
}

hook OnPlayerConnect(playerid) {
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {

    return 1;
}

hook OnPlayerSpawn(playerid) {
    return 1;
}

hook OnPlayerDeathEx(playerid, killerid, reason) {
    return 1;
}
