#include <YSI_Coding\y_hooks>

static
    BitArray:PlayerPMDisabled<MAX_PLAYERS>,
    PlayerLastPM[MAX_PLAYERS];

hook OnPlayerConnect(playerid) {
    Bit_Set(PlayerPMDisabled, playerid, false);
    PlayerLastPM[playerid] = INVALID_PLAYER_ID;
    return 1;
}

CMD:dnd(playerid, params[]) {

    if(!Bit_Get(PlayerPMDisabled, playerid)) {
        SendServerMsg(playerid, "Ban da chan tin nhan rieng.");
        Bit_Let(PlayerPMDisabled, playerid);
    } else {
        SendServerMsg(playerid, "Ban da mo lai tin nhan rieng.");
        Bit_Vet (PlayerPMDisabled, playerid);
    }
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    if(PlayerLastPM[playerid] == INVALID_PLAYER_ID) return 1;

    new target = PlayerLastPM[PlayerLastPM[playerid]];
    if(IsPlayerConnected(target)) {
        PlayerLastPM[target] = INVALID_PLAYER_ID;
    }
    PlayerLastPM[playerid] = INVALID_PLAYER_ID;
    return 1;
}

CMD:pm(playerid, params[]) {
    new
        targetid, msg[128];
    if (sscanf(params, "us[128]", targetid, msg)) {
        return SendSyntaxMsg(playerid, "/pm <playerid> <noi dung>");
    }

    if (!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi ban muon PM khong hop le.");
    }

    if (targetid == playerid) {
        return SendErrorMsg(playerid, "Ban khong the PM cho chinh minh.");
    }

    if (Bit_Get(PlayerPMDisabled, playerid)) {
        return SendErrorMsg(playerid, "Nguoi choi nay da tat PM.");
    }

    SendMsgF(playerid, COLOR_YELLOW, "** PM DEN "C_GREY"%p (%i) ** "C_WHITE"%s", targetid, targetid, msg);
    SendMsgF(targetid, COLOR_YELLOW, "** PM TU "C_GREY"%p (%i) ** "C_WHITE"%s", playerid, playerid, msg);
    PlayerPlaySound(targetid, 17802, 0.0, 0.0, 0.0);
    GameTextForPlayer(targetid, "~n~~n~~n~~y~Da nhan tin nhan", 1500, 5);
    PlayerLastPM[targetid] = playerid;
    return 1;
}

CMD:re(playerid, params[])
{
    new msg[128];
    new targetid = PlayerLastPM[playerid];
    if(sscanf(params, "s[128]", msg)) {
        return SendSyntaxMsg(playerid, "/re <noi dung>");
    }
    if(targetid == -1) {
        return SendErrorMsg(playerid, "Nguoi choi ban muon PM da thoat.");
    }
    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi ban muon PM khong hop le.");
    }
    SendMsgF(playerid, COLOR_YELLOW, "** PM DEN "C_GREY"%p (%i) ** "C_WHITE"%s", targetid, targetid, msg);
    SendMsgF(targetid, COLOR_YELLOW, "** PM TU "C_GREY"%p (%i) ** "C_WHITE"%s", playerid, playerid, msg);
    PlayerPlaySound(targetid, 17802, 0.0, 0.0, 0.0);
    GameTextForPlayer(targetid, "~n~~n~~n~~y~Da nhan tin nhan", 1500, 5);
    PlayerLastPM[targetid] = playerid;
    return 1;
}

ALTCMD:reply(re);
