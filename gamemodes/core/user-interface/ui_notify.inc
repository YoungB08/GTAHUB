#include <YSI_Coding\y_hooks>

const MAX_NOTIFY_TD = 10;

forward Notify_Hide(const playerid);

enum E_NOTIFY {
	notify_time_id,
	bool:notify_Show[MAX_NOTIFY_TD],
	PlayerText:notify_TD[MAX_NOTIFY_TD]
}

static notify_[MAX_PLAYERS][E_NOTIFY];

public Notify_Hide(const playerid) {
	for(new j = 0; j < MAX_NOTIFY_TD; j ++) {
		notify_[playerid][notify_Show][j] = false;
		PlayerTextDrawHide(playerid, notify_[playerid][notify_TD][j]);
		PlayerTextDrawSetString(playerid, notify_[playerid][notify_TD][j], #);
	}
	return 1;
}

Notify_Send(playerid, const text[], time = 7000) {
	new
		l = 0, i = -0xFF;

	for(; l < MAX_NOTIFY_TD; l ++) {
		if(notify_[playerid][notify_Show][l] == false) {
			i = l;
			if(l > 0) KillTimer(notify_[playerid][notify_time_id]);
			break;
		}
	}
	if(i == -0xFF) {
		Notify_Hide(playerid);
		i = 0;
	}
	notify_[playerid][notify_time_id] = SetTimerEx("Notify_Hide", time, false, "d", playerid);
	PlayerTextDrawSetString(playerid, notify_[playerid][notify_TD][i], text);
	PlayerTextDrawShow(playerid, notify_[playerid][notify_TD][i]);
	notify_[playerid][notify_Show][i] = true;
	return 1;
}

hook OnPlayerConnect(playerid) {
	for(new i = 0; i < MAX_NOTIFY_TD; i ++) {
		notify_[playerid][notify_TD][i] = CreatePlayerTextDraw(playerid, 573.366676, 313.070495 - (float(18) * i), #);
		PlayerTextDrawLetterSize(playerid, notify_[playerid][notify_TD][i], 0.165666, 0.811852);
		PlayerTextDrawTextSize(playerid, notify_[playerid][notify_TD][i], 0.000000, 80.000000);
		PlayerTextDrawBackgroundColor(playerid, notify_[playerid][notify_TD][i], 255);
		PlayerTextDrawBoxColor(playerid, notify_[playerid][notify_TD][i], 168430280);
		PlayerTextDrawSetProportional(playerid, notify_[playerid][notify_TD][i], true);
		PlayerTextDrawAlignment(playerid, notify_[playerid][notify_TD][i], 2);
		PlayerTextDrawSetShadow(playerid, notify_[playerid][notify_TD][i], 0);
		PlayerTextDrawColor(playerid, notify_[playerid][notify_TD][i], -1);
		PlayerTextDrawUseBox(playerid, notify_[playerid][notify_TD][i], true);
		PlayerTextDrawFont(playerid, notify_[playerid][notify_TD][i], 1);
		notify_[playerid][notify_Show][i] = false;
	}
	CallLocalFunction("cb_prefix_OnPlayerConnect", "d", playerid);
	return 1;
}

hook OnPlayerDisconnect(playerid) {
	for(new i = 0; i < MAX_NOTIFY_TD; i ++) {
		PlayerTextDrawDestroy(playerid, notify_[playerid][notify_TD][i]);
	}
	CallLocalFunction("cb_prefix_OnPlayerDisconnect", "d", playerid);
	return 1;
}

CMD:testnotify(playerid, const params[]) {
	if(isnull(params))
	{
	    return SendClientMessage(playerid, COLOR_MODERATION, "Cu phap: /testnotify [noi dung]");
	}
    Notify_Send(playerid, params, 3000);
    return 1;
}
