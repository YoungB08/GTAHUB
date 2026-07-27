CMD:rconsetadmin(playerid, params[])
{
    if(IsPlayerAdmin(playerid))
    {
        new target, level;
        if(sscanf(params, "ui", target, level)) return SendSyntaxMsg(playerid, "/rconsetadmin [player] [level]");
        if(target == INVALID_PLAYER_ID) return SendErrorMsg(playerid, "NGUOI CHOI KHONG HOP LE!");
        if(Admin_GetPlayerStoredLevel(target) == level) {
        return SendErrorMsg(playerid, "Nguoi choi nay dang o dung cap admin do.");
        }
        if(level < 0 || level > MAX_ADMIN_LEVELS) {
            return SendErrorMsgF(playerid, "Cap admin toi da la %d", MAX_ADMIN_LEVELS);
        }
        Admin_SetPlayerLevel(target, level);
        SendServerMsgF(playerid, "Ban da set %p (%d) thanh %s", target, target, Admin_GetRankName(target));
    }
    else return 0;
    return 1;
}
