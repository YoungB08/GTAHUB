ACMD:[4]setvip(playerid, params[])
{
    new
        targetid,
        level,
        month;

    if(isnull(params)) {
        return SendSyntaxMsg(playerid, "/setvip month | perma");
    }

    if (!strcmp(params, "month", true, 5)) {
        if (sscanf(params, "{s[6]}uii", targetid, level, month))
            return SendSyntaxMsg(playerid, "/setvip month <playerid> <level> <so thang>");

        if (targetid == INVALID_PLAYER_ID)
            return SendErrorMsg(playerid, "Nguoi choi ban muon set VIP khong hop le.");

        if (VIP_GetPlayerLevel(targetid) > 0)
            return SendErrorMsg(playerid, "Nguoi choi nay da co VIP.");

        if (!(0 <= level < 3))
            return SendErrorMsg(playerid, "Level VIP khong hop le.");

        VIP_GivePlayer(targetid, level, 0, month);

        SendMsgF(targetid, COLOR_LIGHTGREEN, "** VIP ** "C_WHITE"Level VIP cua ban da duoc admin "C_GREY"%p(%i) "C_WHITE"dat thanh "C_GREY"%i", playerid, playerid, level);
        SendAdminMsgF("ADMIN "C_GREY"%p "C_WHITE"da set VIP cho "C_GREY"%p "C_WHITE"thanh level "C_GREY"%i", playerid, targetid, level);
    }
    if (!strcmp(params, "perma", true, 5)) {
        if (sscanf(params, "{s[6]}ii", targetid, level))
            return SendSyntaxMsg(playerid, "/setvip perma <playerid> <level>");

        if (targetid == INVALID_PLAYER_ID)
            return SendErrorMsg(playerid, "Nguoi choi ban muon set VIP khong hop le.");

        if (VIP_GetPlayerLevel(targetid) > 0)
            return SendErrorMsg(playerid, "Nguoi choi nay da co VIP.");

        if (!(0 <= level < 3))
            return SendErrorMsg(playerid, "Level VIP khong hop le.");

        VIP_GivePlayer(targetid, level, 1, 20);


        SendMsgF(targetid, COLOR_LIGHTGREEN, "** VIP ** "C_WHITE"Level VIP cua ban da duoc admin "C_GREY"%p(%i) "C_WHITE"dat thanh "C_GREY"%i", playerid, playerid, level);
        SendAdminMsgF("ADMIN "C_GREY"%p "C_WHITE"da set VIP cho "C_GREY"%p "C_WHITE"thanh level "C_GREY"%i", playerid, targetid, level);
    }
    return true;
}


ACMD:[4]givemoney(playerid, params[])
{
    new
        targetid,
        amount,
        reason[32];

    if(sscanf(params, "uiS(Khong co ly do)[32]", targetid, amount, reason)) {
        return SendSyntaxMsg(playerid, "/givemoney [playerid] [tien] [ly do]");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online.");
    }

    GivePlayerMoney(targetid, amount);
    SendMsgAllF(COLOR_MODERATION, "[ADMIN REWARD]: Admin da cho %p(%d) %m vi %s", targetid, targetid, amount, reason);
    return 1;
}

ACMD:[4]fine(playerid, params[])
{
    new
        targetid,
        amount,
        reason[32];

    if(sscanf(params, "uiS(Khong co ly do)[32]", targetid, amount, reason)) {
        return SendSyntaxMsg(playerid, "/fine [playerid] [tien] [ly do]");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online.");
    }

    if(GetPlayerMoney(targetid) < amount) {
        return SendErrorMsg(playerid, "Nguoi choi khong co du so tien do.");
    }

    GivePlayerMoney(targetid, -amount);
    SendMsgAllF(COLOR_MODERATION, "[ADMIN FINE]: Admin da phat %p(%d) %m vi %s", targetid, targetid, amount, reason);
    return 1;
}

ACMD:[4]getmoney(playerid, params[])
{
    new
        targetid;
    if(sscanf(params, "u", targetid)) {
        return SendSyntaxMsg(playerid, "/getmoney [playerid]");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online.");
    }

    SendServerMsgF(playerid, "Tien cua %p (%d) la %m", targetid, targetid, GetPlayerMoney(targetid));
    return 1;
}

ACMD:[4]motd(playerid, params[]) {

    if(isnull(params)) {
        return SendSyntaxMsg(playerid, "/motd <motd moi>");
    }

    new
        old_motd[128];

    MOTD_Get(old_motd, sizeof(old_motd));
    MOTD_Set(params, true);

    SendMsgF(playerid, COLOR_YELLOW, "(DA CAP NHAT MOTD)");
    SendMsgF(playerid, COLOR_YELLOW, "(MOTD CU) "C_BLUE"%s", old_motd);
    SendMsgF(playerid, COLOR_YELLOW, "(MOTD MOI) "C_BLUE"%s", params);
    return 1;
}
