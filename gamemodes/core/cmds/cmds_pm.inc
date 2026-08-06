#include <YSI_Coding\y_hooks>

static
    BitArray:PlayerPMDisabled<MAX_PLAYERS>,
    List:g_PlayerPMHistory[MAX_PLAYERS];

static stock PM_AddHistory(playerid, targetid)
{
    if (!list_valid(g_PlayerPMHistory[playerid]))
    {
        g_PlayerPMHistory[playerid] = list_new();
    }
    new index = list_find(g_PlayerPMHistory[playerid], targetid);
    if (index != -1)
    {
        list_remove(g_PlayerPMHistory[playerid], index);
    }
    list_add(g_PlayerPMHistory[playerid], targetid);
    while (list_size(g_PlayerPMHistory[playerid]) > 5)
    {
        list_remove(g_PlayerPMHistory[playerid], 0);
    }
    return 1;
}

hook OnPlayerConnect(playerid) {
    Bit_Set(PlayerPMDisabled, playerid, false);
    if (list_valid(g_PlayerPMHistory[playerid]))
    {
        list_delete(g_PlayerPMHistory[playerid]);
    }
    g_PlayerPMHistory[playerid] = list_new();
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    if (list_valid(g_PlayerPMHistory[playerid]))
    {
        list_delete(g_PlayerPMHistory[playerid]);
        g_PlayerPMHistory[playerid] = List:0;
    }
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

CMD:pm(playerid, params[]) {
    new targetid, msg[128];
    if (sscanf(params, "us[128]", targetid, msg)) {
        return SendSyntaxMsg(playerid, "/pm <playerid> <noi dung>");
    }

    if (!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi ban muon PM khong hop le.");
    }

    if (targetid == playerid) {
        return SendErrorMsg(playerid, "Ban khong the PM cho chinh minh.");
    }

    if (Bit_Get(PlayerPMDisabled, targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi nay da tat PM.");
    }

    SendMsgF(playerid, COLOR_YELLOW, "** PM DEN "C_GREY"%p (%i) ** "C_WHITE"%s", targetid, targetid, msg);
    SendMsgF(targetid, COLOR_YELLOW, "** PM TU "C_GREY"%p (%i) ** "C_WHITE"%s", playerid, playerid, msg);
    PlayerPlaySound(targetid, 17802, 0.0, 0.0, 0.0);
    GameTextForPlayer(targetid, "~n~~n~~n~~y~Da nhan tin nhan", 1500, 5);

    PM_AddHistory(playerid, targetid);
    PM_AddHistory(targetid, playerid);
    return 1;
}

CMD:re(playerid, params[])
{
    new msg[128];
    if (sscanf(params, "s[128]", msg)) {
        return SendSyntaxMsg(playerid, "/re <noi dung>");
    }

    if (!list_valid(g_PlayerPMHistory[playerid]) || list_size(g_PlayerPMHistory[playerid]) == 0) {
        return SendErrorMsg(playerid, "Ban chua co tin nhan PM nao gan day.");
    }

    new size = list_size(g_PlayerPMHistory[playerid]);
    new targetid = list_get(g_PlayerPMHistory[playerid], size - 1);
    if (!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi ban muon PM da thoat.");
    }

    if (Bit_Get(PlayerPMDisabled, targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi nay da tat PM.");
    }

    SendMsgF(playerid, COLOR_YELLOW, "** PM DEN "C_GREY"%p (%i) ** "C_WHITE"%s", targetid, targetid, msg);
    SendMsgF(targetid, COLOR_YELLOW, "** PM TU "C_GREY"%p (%i) ** "C_WHITE"%s", playerid, playerid, msg);
    PlayerPlaySound(targetid, 17802, 0.0, 0.0, 0.0);
    GameTextForPlayer(targetid, "~n~~n~~n~~y~Da nhan tin nhan", 1500, 5);

    PM_AddHistory(playerid, targetid);
    PM_AddHistory(targetid, playerid);
    return 1;
}

ALTCMD:reply(re);

CMD:pmlist(playerid, params[])
{
    if (!list_valid(g_PlayerPMHistory[playerid]) || list_size(g_PlayerPMHistory[playerid]) == 0) {
        return SendServerMsg(playerid, "Danh sach PM gan day cua ban dang trong.");
    }

    SendServerMsg(playerid, "--- DANH SACH PM GAN DAY ---");
    new size = list_size(g_PlayerPMHistory[playerid]);
    new itemNo = 1;
    for (new i = size - 1; i >= 0; i--) {
        new targetid = list_get(g_PlayerPMHistory[playerid], i);
        if (IsPlayerConnected(targetid)) {
            SendMsgF(playerid, COLOR_WHITE, "[%d] ID %d - %p (Online)", itemNo++, targetid, targetid);
        } else {
            SendMsgF(playerid, COLOR_GREY, "[%d] ID %d (Offline)", itemNo++, targetid);
        }
    }
    return 1;
}

ALTCMD:pmhistory(pmlist);
ALTCMD:replylist(pmlist);
