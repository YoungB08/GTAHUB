ACMD:[2]tempban(playerid, params[])
{
    if (isnull(params) || IsNumeric(params))  {
        return SendSyntaxMsg(playerid, "/tempban <tuy chon: d(ngay), m(thang), y(nam)>");
    }

    new
        targetid = INVALID_PLAYER_ID,
        reason[MAX_BAN_REASON] = '\0',
        duration = -1,
        bool:success = false,
        interval = -1;

    if (!strcmp(params, "d", true, 1)) {
        if (sscanf(params, "{s[2]}riS(Not specified)["#MAX_BAN_REASON"]", targetid, duration, reason)) {
            return SendSyntaxMsg(playerid, "/tempban d <playerid> <thoi gian> <ly do>");
        }

        if (!IsPlayerConnected(targetid)) {
            return SendErrorMsg(playerid, "Nguoi choi ban muon ban khong online.");
        }
        if (targetid == playerid) {
            return SendErrorMsg(playerid, "Ban khong the tu ban chinh minh.");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "Ly do qua dai, toi da 32 ky tu.");
        }
        interval = 0;
    }
    if (!strcmp(params, "m", true, 1)) {
        if (sscanf(params, "{s[2]}riS(Not specified)["#MAX_BAN_REASON"]", targetid, duration, reason)) {
            return SendSyntaxMsg(playerid, "/tempban m <playerid> <thoi gian> <ly do>");
        }
        if (!IsPlayerConnected(targetid)) {
            return SendErrorMsg(playerid, "Nguoi choi ban muon ban khong online.");
        }
        if (targetid == playerid) {
            return SendErrorMsg(playerid, "Ban khong the tu ban chinh minh.");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "Ly do qua dai, toi da 32 ky tu.");
        }

        interval = 1;
    }
    if (!strcmp(params, "y", true, 1))  {
        if (sscanf(params, "{s[2]}riS(Not specified)["#MAX_BAN_REASON"]", targetid, duration, reason)) {
            return SendSyntaxMsg(playerid, "/tempban y <playerid> <thoi gian> <ly do>");
        }
        if (!IsPlayerConnected(targetid)) {
            return SendErrorMsg(playerid, "Nguoi choi ban muon ban khong online.");
        }
        if (targetid == playerid) {
            return SendErrorMsg(playerid, "Ban khong the tu ban chinh minh.");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "Ly do qua dai, toi da 32 ky tu.");
        }
        interval = 2;
    }

    if(interval != -1) {
        success = bool:Ban_AddUser(targetid, playerid, reason, interval, duration);
    }

    if(success) {
        SendMsgF(playerid, COLOR_MODERATION, "[ADMIN]: Ban da khoa tai khoan cua %p vi %s", targetid, reason);
        SendMsgF(targetid, COLOR_MODERATION, "[ADMIN]: Ban da bi admin ban vi %s", reason);

        SendMsgAllF(COLOR_MODERATION, "[ADMIN]: %p(%d) da bi ban vi %s", targetid, targetid, reason);
        Kick(targetid);
    }
    return 1;
}

ACMD:[2]offban(playerid, params[])
{
    if (isnull(params) || IsNumeric(params))  {
        return SendSyntaxMsg(playerid, "/offban <tuy chon: d(ngay), m(thang), y(nam)>");
    }
    new
        targetname[MAX_PLAYER_NAME],
        reason[MAX_BAN_REASON] = '\0',
        duration = -1,
        bool:success = false,
        interval = -1;
    if (!strcmp(params, "d", true, 1)) {
        if (sscanf(params, "{s[2]}s[24]iS(Not specified)["#MAX_BAN_REASON"]", targetname, duration, reason)) {
            return SendSyntaxMsg(playerid, "/offban d <ten player> <thoi gian> <ly do>");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "Ly do qua dai, toi da 32 ky tu.");
        }
        interval = 0;
    }
    if (!strcmp(params, "m", true, 1)) {
        if (sscanf(params, "{s[2]}s[24]iS(Not specified)["#MAX_BAN_REASON"]", targetname, duration, reason)) {
            return SendSyntaxMsg(playerid, "/offban m <ten player> <thoi gian> <ly do>");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "Ly do qua dai, toi da 32 ky tu.");
        }

        interval = 1;
    }
    if (!strcmp(params, "y", true, 1))  {
        if (sscanf(params, "{s[2]}s[24]iS(Not specified)["#MAX_BAN_REASON"]", targetname, duration, reason)) {
            return SendSyntaxMsg(playerid, "/offban y <ten player> <thoi gian> <ly do>");
        }
        if (strlen(reason) > MAX_BAN_REASON) {
            return SendErrorMsg(playerid, "Ly do qua dai, toi da 32 ky tu.");
        }
        interval = 2;
    }

    if(interval != -1) {
        success = !!OffBan_AddUser(targetname, playerid, reason, interval, duration);
    }

    if(success) {
    }
    return 1;
}

ACMD:[3]unban(playerid, params[])
{
    if (isnull(params) || strval(params)) {
        return SendSyntaxMsg(playerid, "/unban <ten player>");
    }
    if (strlen(params) > MAX_PLAYER_NAME) {
        return SendErrorMsg(playerid, "MAX_PLAYER_NAME = 24, hay nhap lai ten!");
    }

    Ban_RemoveUser(params, playerid);
    return 1;
}
