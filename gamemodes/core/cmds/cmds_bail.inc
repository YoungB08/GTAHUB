CMD:bail(playerid) {
    if(!Player_HasServedSentence(playerid)) {
        return SendErrorMsg(playerid, "Ban phai chap hanh xong thoi gian bat buoc moi duoc bao lanh.");
    }

    if(!Player_IsInJail(playerid)) {
        return SendErrorMsg(playerid, "Ban khong o trong tu!");
    }

    if(GetPlayerMoney(playerid) < Player_GetBailAmount(playerid)) {
        return SendErrorMsgF(playerid, "Ban khong co ~g~$%i ~w~de nop tien bao lanh.", Player_GetBailAmount(playerid));
    }

    Player_ReleaseFromJail(playerid);
    GivePlayerMoney(playerid, -Player_GetBailAmount(playerid));
    Player_SetBailAmount(playerid, 0);
    return 1;
}
