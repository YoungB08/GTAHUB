static 
    PlayerFixCooldown[MAX_PLAYERS];

/*
    This command can be used by mechanics only,
    it has 3 minute cooldown everytime the command
    is used to avoid repair abuse during chase.
*/
CMD:mechfix(playerid, params[]) {
    if(Player_GetJob(playerid) != TYPE_MECHANIC) {
        return SendErrorMsg(playerid, "Chi tho may moi dung duoc lenh nay.");
    }

    if(gettime() < PlayerFixCooldown[playerid]) {
        return SendErrorMsgF(playerid, "Vui long cho "C_GREY"%i giay "C_WHITE"truoc khi dung lai lenh nay.", (PlayerFixCooldown[playerid] - gettime()));
    }

    if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
        return SendErrorMsg(playerid, "Ban phai la tai xe de sua xe.");
    }
    
    RepairVehicle(GetPlayerVehicleID(playerid));
    PlayerFixCooldown[playerid] = gettime() + (60 * 3); 

    SendClientMessage(playerid, COLOR_PURPLE, "[SUA XE] "C_WHITE"Xe cua ban da duoc sua, cooldown 3 phut.");
    return 1;
}
