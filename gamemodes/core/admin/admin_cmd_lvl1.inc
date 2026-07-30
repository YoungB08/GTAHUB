#include <YSI_Coding\y_hooks>
#include <YSI_Coding\y_timers>

ACMD:[1]get(playerid, params[])
{
	new
		targetid;

	if(sscanf(params, "u", targetid)) {
		return SendSyntaxMsg(playerid, "/get <playerid>");
	}

	if(!IsPlayerConnected(targetid)) {
		return SendErrorMsg(playerid, "Nguoi choi khong online.");
	}

	if(targetid == playerid) {
		return SendErrorMsg(playerid, "Ban khong the keo chinh minh.");
	}

	new
		Float:x,
		Float:y,
		Float:z;
	GetPlayerPos(playerid, x, y, z);

	SetPlayerPos(targetid, x, y, z);
	SetPlayerVirtualWorld(targetid, GetPlayerVirtualWorld(playerid));
	SetPlayerInterior(targetid, GetPlayerInterior(playerid));
	return 1;
}

ACMD:[1]gotopos(playerid, params[])
{
    new Float:x, Float:y, Float:z, interior = 0, world = 0;
    if(sscanf(params, "fffI(0)I(0)", x, y, z, interior, world)) {
        return SendSyntaxMsg(playerid, "/gotopos [x] [y] [z] (interior=0) (world=0)");
    }

    Interaction_LoadSafeTeleport(playerid, x, y, z, 0.0, world, interior, false, 500);
    SendServerMsgF(playerid, "[ADMIN] Da dich chuyen den vi tri X: %.2f, Y: %.2f, Z: %.2f (Interior: %d, World: %d)", x, y, z, interior, world);
    return 1;
}

ALTCMD:telepos(gotopos);

ACMD:[1]gotoid(playerid, params[])
{
    new targetid;
    if (sscanf(params, "u", targetid)) {
        return SendSyntaxMsg(playerid, "/gotoid <playerid | Partial Name>");
    }

    if (!IsPlayerConnected(targetid) || !Player_IsLoggedIn(targetid) || !Char_IsActive(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online hoac chua vao game.");
    }

    if (targetid == playerid) {
        return SendErrorMsg(playerid, "Ban khong the dich chuyen den chinh minh.");
    }

    new Float:x, Float:y, Float:z;
    new world = GetPlayerVirtualWorld(targetid);
    new interior = GetPlayerInterior(targetid);

    if (IsPlayerInAnyVehicle(targetid)) {
        new vehid = GetPlayerVehicleID(targetid);
        GetVehiclePos(vehid, x, y, z);
        x += 2.0; // Đặt admin đứng cạnh xe an toàn
    } else {
        GetPlayerPos(targetid, x, y, z);
        x += 1.0;
    }

    SetPlayerPos(playerid, x, y, z);
    SetPlayerVirtualWorld(playerid, world);
    SetPlayerInterior(playerid, interior);

    SendServerMsgF(playerid, "[ADMIN] Da dich chuyen den nguoi choi %s (ID: %d).", ReturnPlayerName(targetid), targetid);
    return 1;
}

ACMD:[1]goto(playerid, params[])
{
    new loc[32];
    if (sscanf(params, "s[32]", loc)) {
        return SendSyntaxMsg(playerid, "/goto <location | playerid>\nDanh sach: cityhall, vipls, dmv (drivingtest), ems (hospital), police (pd)");
    }

    // Nếu tham số là ID hoặc tên người chơi -> tự động gọi /gotoid
    new targetid;
    if (!sscanf(loc, "u", targetid) && IsPlayerConnected(targetid)) {
        return CallLocalFunction("ACMD_gotoid", "is", playerid, loc);
    }

    if (!strcmp(loc, "cityhall", true)) {
        Interaction_LoadSafeTeleport(playerid, 1481.0, -1771.0, 18.7, 0.0, 0, 0, false, 500);
        return SendServerMsg(playerid, "[ADMIN] Da dich chuyen den Toa Thi Chinh (City Hall).");
    }
    else if (!strcmp(loc, "vipls", true)) {
        Interaction_LoadSafeTeleport(playerid, 1172.5, -1323.8, 15.4, 270.0, 0, 0, false, 500);
        return SendServerMsg(playerid, "[ADMIN] Da dich chuyen den Khu Vuc VIP Los Santos.");
    }
    else if (!strcmp(loc, "dmv", true) || !strcmp(loc, "drivingtest", true)) {
        Interaction_LoadSafeTeleport(playerid, 854.5, -604.4, 18.4, 0.0, 0, 0, false, 500);
        return SendServerMsg(playerid, "[ADMIN] Da dich chuyen den Trung Tam Thi Bang Lai Xe (DMV / Driving Test).");
    }
    else if (!strcmp(loc, "ems", true) || !strcmp(loc, "hospital", true)) {
        Interaction_LoadSafeTeleport(playerid, 1172.0, -1323.0, 15.4, 270.0, 0, 0, false, 500);
        return SendServerMsg(playerid, "[ADMIN] Da dich chuyen den Benh Vien (EMS / Hospital).");
    }
    else if (!strcmp(loc, "police", true) || !strcmp(loc, "pd", true)) {
        Interaction_LoadSafeTeleport(playerid, 1555.0, -1675.0, 16.2, 90.0, 0, 0, false, 500);
        return SendServerMsg(playerid, "[ADMIN] Da dich chuyen den Tru So Canh Sat (LSPD / Police).");
    }

    return SendErrorMsg(playerid, "Dia diem khong ton tai! Cac dia diem: cityhall, vipls, dmv, drivingtest, ems, hospital, police, pd.");
}

ACMD:[1]kick(playerid, params[])
{
	new
		targetid,
		reason[32];

	if(sscanf(params, "uS[32](Khong co ly do)", targetid, reason)) {
		return SendSyntaxMsg(playerid, "/kick (ten/id) (ly do)");
	}

	if(!IsPlayerConnected(targetid)) {
		return SendErrorMsg(playerid, "Nguoi choi khong online.");
	}

	SendMsgAllF(COLOR_MODERATION, "[ADMIN KICK]: %p (%d) da bi kick vi: %s", targetid, targetid, reason);
	SendMsgF(targetid, COLOR_MODERATION, "[ADMIN KICK]: Ban da bi kick vi: %s", reason);
	Kick(targetid);
	return 1;
}

ACMD:[1]setwanted(playerid, params[])
{
	new
		targetid,
		level;

	if(sscanf(params, "ui", targetid, level)) {
		return SendSyntaxMsg(playerid, "/setwanted (ten/id) (muc truy na)");
	}
	if(!IsPlayerConnected(targetid)) {
		return SendErrorMsg(playerid, "Nguoi choi khong online.");
	}
	if(level != 0 && Player_IsLEO(targetid)) {
		return SendErrorMsg(playerid, "Khong the dat truy na cho luc luong chuc nang.");
	}
	SetPlayerWantedLevel(targetid, level);
	SendMsgF(playerid, COLOR_MODERATION, "[ADMIN]: Muc truy na cua %p (%d) da duoc dat thanh %d", targetid, targetid, level);
	SendMsgF(targetid, COLOR_MODERATION, "[ADMIN]: Muc truy na cua ban da duoc admin dat thanh %d", level);
	return 1;
}

ACMD:[1]respawn(playerid, params[])
{
	new target;
	if(sscanf(params, "u", target)) return SendSyntaxMsg(playerid, "/respawn (ten/id)");
	TogglePlayerSpectating(target, true);
	ForceSpawnPlayer(target);
	return 1;
}

ACMD:[1]freeze(playerid, params[])
{
	new
		targetid,
		bool:freezeState;

	if(sscanf(params, "ul", targetid, freezeState)) {
		return SendSyntaxMsg(playerid, "/freeze (ten/id) (0/1)");
	}

	if(!IsPlayerConnected(targetid)) {
		return SendErrorMsg(playerid, "Nguoi choi khong online.");
	}

	SendMsgAllF(
		COLOR_MODERATION,
		"[ADMIN FREEZE]: %p(%d) da bi %s boi Admin %p(%d)",
		targetid,
		targetid,
		(freezeState) ? ("dong bang") : ("mo dong bang"),
		playerid,
		playerid
	);

	// reverse it
	// because TogglePlayerControllable function
	// uses 0 to freeze and 1 to unfreeze while
	// the command takes 0 to unfreeze, 1 to freeze.
	TogglePlayerControllable(targetid, !freezeState);
 	return 1;
}

ACMD:[1]akill(playerid, params[])
{
	new
		targetid,
		Float:x,
		Float:y,
		Float:z;

	if(sscanf(params, "u", targetid)) {
		return SendSyntaxMsg(playerid, "/akill (ten/id)");
	}

	if(!IsPlayerConnected(targetid)) {
		return SendErrorMsg(playerid, "Nguoi choi khong online.");
	}

	GetPlayerPos(targetid, x, y, z);
	CreateExplosion(x, y, z, 12, 10.0);
	SetPlayerHealth(targetid, 0);

	SendMsgAllF(COLOR_BROWN, "%p(%d) da chet boi ban tay cua GOD", targetid, targetid);
	return 1;
}

ACMD:[1]afix(playerid, params[])
{
	if(!IsPlayerInAnyVehicle(playerid)) return SendErrorMsg(playerid, "Ban khong o trong xe nao.");
	RepairVehicle(GetPlayerVehicleID(playerid));
	return 1;
}

static
	bool:isReconnecting[MAX_PLAYERS],
	reconnectIP[MAX_PLAYERS][16]
;

hook OnPlayerConnect(playerid)
{
	isReconnecting[playerid] = false;
	reconnectIP[playerid][0] = '\0';
	return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
	if (isReconnecting[playerid]) {
		new cmd[32];
		format(cmd, sizeof(cmd), "unbanip %s", reconnectIP[playerid]);
		SendRconCommand(cmd);
		isReconnecting[playerid] = false;
		reconnectIP[playerid][0] = '\0';
	}
	return 1;
}

hook OnGameModeExit()
{
	for (new i = 0; i < MAX_PLAYERS; i++) {
		if (isReconnecting[i]) {
			new cmd[32];
			format(cmd, sizeof(cmd), "unbanip %s", reconnectIP[i]);
			SendRconCommand(cmd);
			isReconnecting[i] = false;
			reconnectIP[i][0] = '\0';
		}
	}
	return 1;
}

timer UnbanPlayerIP[1000](playerid)
{
	if (isReconnecting[playerid]) {
		new cmd[32];
		format(cmd, sizeof(cmd), "unbanip %s", reconnectIP[playerid]);
		SendRconCommand(cmd);
		isReconnecting[playerid] = false;
		reconnectIP[playerid][0] = '\0';
	}
	return 1;
}

ACMD:[1]relog(playerid, params[])
{
	new targetid = INVALID_PLAYER_ID;
	new reason[64] = "Khong co";

	if (isnull(params)) {
		targetid = playerid;
		format(reason, sizeof(reason), "Tu reconnect");
	} else {
		if (sscanf(params, "uS[64](Khong co ly do)", targetid, reason)) {
			if (sscanf(params, "u", targetid)) {
				return SendSyntaxMsg(playerid, "/relog [ten/id] [ly do]");
			}
		}
	}

	if (!IsPlayerConnected(targetid)) {
		return SendErrorMsg(playerid, "Nguoi choi ban muon reconnect khong online.");
	}

	new ip[16];
	GetPlayerIp(targetid, ip, sizeof(ip));

	isReconnecting[targetid] = true;
	format(reconnectIP[targetid], sizeof(reconnectIP[]), "%s", ip);

	SendMsgAllF(COLOR_MODERATION, "[ADMIN RECONNECT]: %p(%d) da duoc Admin %p(%d) reconnect. Ly do: %s", targetid, targetid, playerid, playerid, reason);
	SendMsgF(targetid, COLOR_MODERATION, "[ADMIN RECONNECT]: Ban dang duoc Admin reconnect. Ly do: %s", reason);

	new cmd[32];
	format(cmd, sizeof(cmd), "banip %s", ip);
	SendRconCommand(cmd);

	Kick(targetid);

	defer UnbanPlayerIP(targetid);
	return 1;
}

// =============================================================================
// COMMAND – /up [khoang_cach] [targetid] – Dich chuyen len cao de mo ket
// =============================================================================

ACMD:[1]up(playerid, params[])
{
    new Float:distance = 5.0, targetid = playerid;

    if (!sscanf(params, "fU(-1)", distance, targetid))
    {
        if (targetid == -1 || targetid == INVALID_PLAYER_ID)
            targetid = playerid;
    }
    else if (!sscanf(params, "u", targetid))
    {
        distance = 5.0;
    }
    else
    {
        distance = 5.0;
        targetid = playerid;
    }

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai hoac da thoat."), 1;

    if (distance < 1.0 || distance > 100.0)
        distance = 5.0;

    new Float:x, Float:y, Float:z;

    if (IsPlayerInAnyVehicle(targetid))
    {
        new vehid = GetPlayerVehicleID(targetid);
        GetVehiclePos(vehid, x, y, z);
        SetVehiclePos(vehid, x, y, z + distance);
    }
    else
    {
        GetPlayerPos(targetid, x, y, z);
        SetPlayerPos(targetid, x, y, z + distance);
    }

    if (targetid == playerid)
    {
        SendServerMsgF(playerid, "[ADMIN] Da dich chuyen ban len cao +%.1fm de mo ket.", distance);
    }
    else
    {
        SendServerMsgF(playerid, "[ADMIN] Da dich chuyen %s len cao +%.1fm de mo ket.", ReturnPlayerName(targetid), distance);
        SendServerMsgF(targetid, "[ADMIN] Admin %s da dich chuyen ban len cao +%.1fm de mo ket.", ReturnPlayerName(playerid), distance);
    }
    return 1;
}
