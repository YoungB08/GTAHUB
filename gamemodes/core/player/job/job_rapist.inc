#include <job_rapist-impl>

static
    HasJustUseRape[MAX_PLAYERS];

CMD:rape(playerid, params[])
{
	if (Player_GetJob(playerid) != TYPE_RAPIST || Player_GetClass(playerid) != TEAM_CIVILIAN) {
		return SendErrorMsg(playerid, "Ban khong duoc dung lenh nay neu khong phai nghe truyen benh.");
    }

	if (isnull(params)) {
		return SendSyntaxMsg(playerid, "/rape <playerid>");
    }

	new
		targetid = strval(params);

	if (targetid == INVALID_PLAYER_ID) {
		return SendErrorMsg(playerid, "Nguoi choi muc tieu khong online.");
    }

	if (!Player_IsSpawned(targetid)) {
		return SendErrorMsg(playerid, "Nguoi choi muc tieu chua spawn.");
    }

	if (Disease_PlayerHas(targetid)) {
		return SendErrorMsg(playerid, "Nguoi choi muc tieu da nhiem benh.");
    }

	if (!Player_IsNearPlayer(playerid, targetid, 4.0)) {
		return SendErrorMsg(playerid, "Nguoi choi muc tieu khong o gan ban.");
    }

	if (targetid == playerid) {
		return SendErrorMsg(playerid, "Ban khong the dung lenh nay len chinh minh.");
    }

	if (gettime() - HasJustUseRape[playerid] < 20) {
		return SendErrorMsgF(playerid, "Vui long cho "C_GREY"%i giay "C_WHITE"truoc khi dung lai lenh nay.", 20 - (gettime() - HasJustUseRape[playerid]));
    }

	if (Player_GetCondom(targetid) > 0)
	{
		Player_GiveCondom(targetid, -1);
		SendMsgF(playerid, COLOR_LIGHTGREEN, "** TRUYEN BENH **: "C_GREY"Ban thu lam nhiem benh "C_GREY"%p (%i) "C_WHITE"nhung that bai vi nguoi do co condom.", targetid, targetid);
  	 	SendMsgF(targetid, COLOR_LIGHTGREEN, "** TRUYEN BENH **: "C_GREY"%p(%i) "C_WHITE"da thu lam ban nhiem benh, may man ban co condom, con lai "C_GREY"%i "C_WHITE"condom", playerid, playerid, Player_GetCondom(targetid));
  	 	return true;
 	}

	Disease_GivePlayer(targetid, random(5) + 1);

	SendMsgAllF(COLOR_LIGHTGREEN, "** TRUYEN BENH **: "C_GREY"%p (%i) "C_WHITE"da lam nhiem benh "C_GREY"%p (%i) "C_WHITE"voi "C_LGREEN"%s", playerid, playerid, targetid, targetid, Disease_GetName(targetid));

	SendMsgF(playerid, COLOR_LIGHTGREEN, "** TRUYEN BENH **: "C_WHITE"Ban da lam nhiem benh "C_GREY"%p (%i) "C_WHITE"voi "C_LGREEN"%s", targetid, targetid, Disease_GetName(targetid));
	SendMsgF(targetid, COLOR_LIGHTGREEN, "** TRUYEN BENH **: "C_GREY"%p (%i) "C_WHITE"da lam ban nhiem "C_LGREEN"%s"C_WHITE", hay den benh vien de chua tri som!", playerid, playerid, Disease_GetName(targetid));

	HasJustUseRape[playerid] = gettime();
	return true;
}
