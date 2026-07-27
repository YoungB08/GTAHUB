ALTCMD:arcw(arrestcw);
ALTCMD:ar(arrest);

CMD:arrestcw(playerid, params[])
{
    new
        targetid = Player_GetClosestWanted(playerid, 5.0),
        errorMessage[45],
        ret;

    if(targetid == INVALID_PLAYER_ID) {
        return SendErrorMsg(playerid, "Khong co nguoi choi gan day de bat.");
    }

    ret = Player_Arrest(playerid, targetid, errorMessage, sizeof(errorMessage));

    if(!ret) {
        return SendErrorMsg(playerid, errorMessage);
    }
    return 1;
}

CMD:arrest(playerid, params[])
{
    new
        targetid;
    if(sscanf(params, "u", targetid)) {
        return SendSyntaxMsg(playerid, "/arrest (ten/id)");
    }

    new
        errorMessage[45],
        ret;
    ret = Player_Arrest(playerid, targetid, errorMessage, sizeof(errorMessage));

    if(!ret) {
        return SendErrorMsg(playerid, errorMessage);
    }
    return 1;
}
