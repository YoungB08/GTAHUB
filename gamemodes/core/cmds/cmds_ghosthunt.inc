CMD:starthunt(playerid, params[])
{
    new hour, minute, second;
    gettime(hour, minute, second);

    // Check if it's night time (8 PM - 6 AM)
    if(!(hour >= 20 || hour < 6)) {
        return SendErrorMsg(playerid, "San ma chi mo vao ban dem (20:00 - 06:00)!");
    }

    if(GhostHunting_IsHunting(playerid)) {
        return SendErrorMsg(playerid, "Ban dang trong phien san ma roi!");
    }

    if(!GhostHunting_StartHunt(playerid)) {
        return SendErrorMsg(playerid, "Khong the bat dau san ma!");
    }

    return 1;
}

CMD:leavehunt(playerid, params[])
{
    if(!GhostHunting_IsHunting(playerid)) {
        return SendErrorMsg(playerid, "Ban hien khong o trong phien san ma!");
    }

    SendClientMessage(playerid, COLOR_ORANGE, "[SAN MA]"C_WHITE" Ban da roi phien san ma.");
    GhostHunting_EndSession(playerid);

    return 1;
}

CMD:giveweaponcam(playerid, params[])
{
    if(!GhostHunting_IsHunting(playerid)) {
        return SendErrorMsg(playerid, "Ban phai dang san ma moi dung duoc lenh nay!");
    }

    // Give thermal vision camera (weapon ID 43)
    GivePlayerWeapon(playerid, 43, 900);

    new msg[128];
    format(msg, sizeof(msg), "[SAN MA]"C_WHITE" Ban da nhan camera nhiet! Hay dung no de phat hien ma!");
    SendClientMessage(playerid, COLOR_ORANGE, msg);

    return 1;
}
