// cuff closest player
CMD:cuffcw(playerid) {
    new
        targetid = Player_GetClosestWanted(playerid, 5.0),
        errorMessage[32],
        ret;

    ret = Player_CuffPlayer(playerid, targetid, errorMessage, sizeof(errorMessage));

    if(!ret) {
        SendErrorMsg(playerid, errorMessage);
    }
    return 1;
}

// cuff player by id
CMD:cuff(playerid, params[]) {
    new
        targetid;

    if(sscanf(params, "u", targetid)) {
        return SendSyntaxMsg(playerid, "/cuff (ten/id)");
    }

    new
        errorMessage[32],
        ret;

    ret = Player_CuffPlayer(playerid, targetid, errorMessage, sizeof(errorMessage));

    if(!ret) {
        SendErrorMsg(playerid, errorMessage);
    }
    return 1;
}

CMD:uncuff(playerid, params[]) {
    new
        targetid = Player_GetClosestPlayer(playerid, 4.0);

    if(Player_GetClass(playerid) != TEAM_POLICE) {
        return SendErrorMsg(playerid, "Lenh nay chi danh cho canh sat.");
    }

    if(targetid == INVALID_PLAYER_ID) {
        return SendErrorMsg(playerid, "Khong co nguoi choi nao gan day.");
    }

    if(!Player_IsCuffed(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi gan ban chua bi cong tay.");
    }

    SendServerMsgF(targetid, "Ban da duoc si quan "C_GREY"%p(%d) "C_WHITE"thao cong tay.", playerid, playerid);
    SendServerMsgF(playerid, "Ban da thao cong tay cho "C_GREY"%p(%d)", targetid, targetid);
    Player_Cuff(targetid, false);
    return 1;
}

// returns
// 0 -> fail
// -> playerid is not police.
// -> targetid == INVALID_PLAYER_ID.
// -> targetid is not in range.
// -> targetid is aleady cuffed.
// 1 -> success, if it passes all the checks above.
Player_CuffPlayer(playerid, targetid, errorMsg[], len = sizeof(errorMsg)) {
    if(Player_GetClass(playerid) != TEAM_POLICE) {
        strcat(errorMsg, "Chi canh sat moi duoc dung lenh nay!", len);
        return 0;
    }

    if(targetid == INVALID_PLAYER_ID)  {
        strcat(errorMsg, "Khong co doi tuong truy na gan ban", len);
        return 0;
    }

    // do nothing, can't cuff players with wanted level below 3
    if(GetPlayerWantedLevel(targetid) <= 3) {
        return 1;
    }

    new
        Float:x,
        Float:y,
        Float:z;

    GetPlayerPos(playerid, x, y, z);
    if(!IsPlayerInRangeOfPoint(targetid, 5.0, x, y, z)) {
        strcat(errorMsg, "Khong co doi tuong truy na gan ban", len);
        return 0;
    }

    if(Player_IsCuffed(targetid)) {
        strcat(errorMsg, "Nguoi choi nay da bi cong tay", len);
        return 0;
    }

    if(targetid == playerid) {
        strcat(errorMsg, "Ban khong the tu cong tay chinh minh!", len);
        return 0;
    }

    SendServerMsgF(targetid, "Ban da bi si quan "C_GREY"%p(%d) "C_WHITE"cong tay.", playerid, playerid);
    SendServerMsgF(playerid, "Ban da cong tay "C_GREY"%p(%d)", targetid, targetid);
    Player_Cuff(targetid, true);
    return 1;
}
