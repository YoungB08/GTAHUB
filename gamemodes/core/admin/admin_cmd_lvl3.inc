ACMD:[3]setname(playerid, params[])
{
    new
        targetid,
        newName[MAX_PLAYER_NAME + 1],
        string[128];

    if(sscanf(params, "us[25]", targetid, newName)) {
        return SendSyntaxMsg(playerid, "/setname (ten/id) (ten moi)");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online.");
    }

    mysql_format(MySQL_GetHandle(), string, sizeof(string), "UPDATE players SET username = '%e' WHERE u_id = %i", newName, Player_GetAccountID(targetid));
    mysql_tquery(MySQL_GetHandle(), string);

    SendMsgF(targetid, COLOR_MODERATION, "[ADMIN]: Ten cua ban da duoc doi thanh %s", newName);
    SendMsgF(playerid, COLOR_MODERATION, "[ADMIN]: Ban da doi ten cua %p (%d) thanh %s", targetid, targetid, newName);

    SetPlayerName(targetid, newName);
    return 1;
}

ACMD:[3]setarmy(playerid, params[]) {
    new
        targetid;

    if(sscanf(params, "u", targetid)) {
        return SendSyntaxMsg(playerid, "/setarmy <ten/id>");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online.");
    }

    if(!Player_AddToArmyDB(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi nay da o trong army.");
    }

    SendMsgF(targetid, COLOR_MODERATION, "[ADMIN]: %p(%i) da them ban vao army.", playerid, playerid);
    SendMsgF(playerid, COLOR_MODERATION, "[ADMIN]: Ban da them %p(%i) vao army.", targetid, targetid);
    return 1;
}

ACMD:[3]revokearmy(playerid, params[]) {
    new
        targetid;

    if(sscanf(params, "u", targetid)) {
        return SendSyntaxMsg(playerid, "/revokearmy <ten/id>");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online.");
    }

    if(!Player_RemoveFromArmyDB(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi nay khong o trong army.");
    }

    SendMsgF(targetid, COLOR_MODERATION, "[ADMIN]: %p(%i) da xoa ban khoi army.", playerid, playerid);
    SendMsgF(playerid, COLOR_MODERATION, "[ADMIN]: Ban da xoa %p(%i) khoi army.", targetid, targetid);
    return 1;
}

ACMD:[3]getallciv(playerid, params[])
{
    new
        Float:x,
        Float:y,
        Float:z;

    foreach(new i : Player) {
        if(Player_GetClass(i) == TEAM_CIVILIAN) {
            GetPlayerPos(playerid, x, y, z);
            SetPlayerPos(i, x, y + i, z);
        }
    }
    return 1;
}

ACMD:[3]disarm(playerid, params[])
{
    new
        targetid;

    if(sscanf(params, "u", targetid)) {
        return SendSyntaxMsg(playerid, "/disarm (ten/id)");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online.");
    }

    SendMsgF(playerid, COLOR_MODERATION, "[ADMIN DISARM]: Ban da thu vu khi cua %p (%d)", targetid, targetid);
    SendMsgF(targetid, COLOR_MODERATION, "[ADMIN DISARM]: Ban da bi admin thu vu khi.");

    ResetPlayerWeapons(targetid);
    return 1;
}

ACMD:[3]sethealth(playerid, params[])
{
    new
        targetid,
        Float:amount;

    if(sscanf(params, "uf", targetid, amount)) {
        return SendSyntaxMsg(playerid, "/sethealth (ten/id) (mau)");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online.");
    }

    SendMsgF(playerid, COLOR_MODERATION, "[ADMIN HEAL]: Mau cua %p (%d) da duoc dat thanh %f", targetid, targetid, amount);
    SendMsgF(targetid, COLOR_MODERATION, "[ADMIN HEAL]: Mau cua ban da duoc dat thanh %f", amount);

    SetPlayerHealth(targetid, amount);
    return 1;
}

ACMD:[3]setarmour(playerid, params[])
{
    new
        targetid,
        Float:amount;

    if(sscanf(params, "uf", targetid, amount)) {
        return SendSyntaxMsg(playerid, "/setarmour (ten/id) (giap)");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online.");
    }

    SendMsgF(playerid, COLOR_MODERATION, "[ADMIN]: Giap cua %p (%d) da duoc dat thanh %f", targetid, targetid, amount);
    SendMsgF(targetid, COLOR_MODERATION, "[ADMIN]: Giap cua ban da duoc dat thanh %f", amount);

    SetPlayerArmour(targetid, amount);
    return 1;
}
