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
    strcpy(joinString[4], joinString[3]);
    strcpy(joinString[3], joinString[2]);
    strcpy(joinString[2], joinString[1]);
    strcpy(joinString[1], joinString[0]);

    new
        leave_reason[16];

    switch(reason) {
        case 0: leave_reason = "~y~MAT" ;
        case 1: leave_reason = "~r~RA";
        case 2: leave_reason = "~p~KICK" ;
    }

    new
        string[7 + ((sizeof(joinString[]) * 5))];

    formatex(joinString[0], sizeof(joinString[]), "~n~%s: ~w~%p (%i)", leave_reason, playerid, playerid);

    strcat(string, joinString[4], sizeof(string));
    strcat(string, joinString[3], sizeof(string));
    strcat(string, joinString[2], sizeof(string));
    strcat(string, joinString[1], sizeof(string));
    strcat(string, joinString[0], sizeof(string));

    TextDrawSetString(LeaveJoinTD, string);
    return 1;
}

hook OnPlayerSpawn(playerid) {
    return 1;
}

hook OnPlayerDeathEx(playerid, killerid, reason) {
    return 1;
}
