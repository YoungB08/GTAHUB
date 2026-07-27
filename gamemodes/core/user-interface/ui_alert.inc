/*
 same function of notify but with different position.
*/

#include <YSI_Coding\y_hooks>

const MAX_ALERT_TD = 5;

forward Alert_Hide(const playerid);

enum E_ALERT {
	alert_time_id,
	bool:alert_Show[MAX_ALERT_TD],
	PlayerText:alert_TD[MAX_ALERT_TD]
}

static alert_[MAX_PLAYERS][E_ALERT];

public Alert_Hide(const playerid) {
	for(new j = 0; j < MAX_ALERT_TD; j ++) {
		alert_[playerid][alert_Show][j] = false;
		PlayerTextDrawHide(playerid, alert_[playerid][alert_TD][j]);
		PlayerTextDrawSetString(playerid, alert_[playerid][alert_TD][j], #);
	}
	return 1;
}


Alert_Send(playerid, const text[], time = 7000) {
	new
		l = 0, i = -0xFF;

	for(; l < MAX_ALERT_TD; l ++) {
		if(alert_[playerid][alert_Show][l] == false) {
			i = l;
			if(l > 0) KillTimer(alert_[playerid][alert_time_id]);
			break;
		}
	}
	if(i == -0xFF) {
		Alert_Hide(playerid);
		i = 0;
	}
	alert_[playerid][alert_time_id] = SetTimerEx("Alert_Hide", time, false, "d", playerid);
	PlayerTextDrawSetString(playerid, alert_[playerid][alert_TD][i], text);
	PlayerTextDrawShow(playerid, alert_[playerid][alert_TD][i]);
	alert_[playerid][alert_Show][i] = true;
	return 1;
}

hook OnPlayerConnect(playerid) {
	for(new i = 0; i < MAX_ALERT_TD; i ++) {
		alert_[playerid][alert_TD][i] = CreatePlayerTextDraw(playerid, 455.366676, 100.070495 - (float(18) * i), #); // cchanged on top right
		PlayerTextDrawLetterSize(playerid, alert_[playerid][alert_TD][i], 0.165666, 0.811852);
		PlayerTextDrawTextSize(playerid, alert_[playerid][alert_TD][i], 0.000000, 80.000000);
		PlayerTextDrawBackgroundColor(playerid, alert_[playerid][alert_TD][i], 255);
		PlayerTextDrawBoxColor(playerid, alert_[playerid][alert_TD][i], 168430280);
		PlayerTextDrawSetProportional(playerid, alert_[playerid][alert_TD][i], true);
		PlayerTextDrawAlignment(playerid, alert_[playerid][alert_TD][i], 2);
		PlayerTextDrawSetShadow(playerid, alert_[playerid][alert_TD][i], 0);
		PlayerTextDrawColor(playerid, alert_[playerid][alert_TD][i], -1);
		PlayerTextDrawUseBox(playerid, alert_[playerid][alert_TD][i], true);
		PlayerTextDrawFont(playerid, alert_[playerid][alert_TD][i], 1);
		alert_[playerid][alert_Show][i] = false;
	}
	CallLocalFunction("cb_prefix_OnPlayerConnect", "d", playerid);
	return 1;
}

hook OnPlayerDisconnect(playerid) {
	for(new i = 0; i < MAX_ALERT_TD; i ++) {
		PlayerTextDrawDestroy(playerid, alert_[playerid][alert_TD][i]);
	}
	CallLocalFunction("cb_prefix_OnPlayerDisconnect", "d", playerid);
	return 1;
}

CMD:testalert(playerid, const params[]) {
	if(isnull(params))
	{
	    return SendClientMessage(playerid, COLOR_MODERATION, "Cu phap: /testalert [noi dung]");
	}
    Alert_Send(playerid, params, 3000);
    return 1;
}
