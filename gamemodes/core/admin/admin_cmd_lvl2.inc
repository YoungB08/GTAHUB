ACMD:[2]jail(playerid, params[])
{
    new
        targetid,
        seconds,
        reason[32];

    if(sscanf(params, "uiS[32](Khong co ly do)", targetid, seconds, reason)) {
        return SendSyntaxMsg(playerid, "/jail (ten/id) (giay) (ly do)");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online!");
    }


    SendMsgAllF(COLOR_MODERATION, "[ADMIN JAIL]: %p (%d) da bi jail trong %d giay", targetid, targetid, seconds);
    SendMsgAllF(COLOR_MODERATION, "[ADMIN JAIL]: ly do: %s", reason);

    SendMsgF(targetid, COLOR_MODERATION, "[ADMIN JAIL]: Ban da bi jail trong %d giay", seconds);
    SendMsgF(targetid, COLOR_MODERATION, "[ADMIN JAIL]: ly do: %s", reason);

    Player_SendToJail(targetid, seconds, true);
    return 1;
}

ACMD:[2]unjail(playerid, params[])
{
    new
        targetid;

    if(sscanf(params, "u", targetid)) {
        return SendSyntaxMsg(playerid, "/unjail (ten/id)");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online!");
    }

    if(!Player_IsInJail(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi nay khong bi jail!");
    }

    SendMsgAllF(COLOR_MODERATION, "[UNJAIL]: %s %p (%d) da unjail %p (%d)", Admin_GetRankName(playerid), playerid, playerid, targetid, targetid);
    Player_ReleaseFromJail(targetid);
    return 1;
}
