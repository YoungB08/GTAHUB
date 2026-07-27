#include <YSI_Coding\y_hooks>

static
    PlayerBreakOutTick[MAX_PLAYERS];


hook OnPlayerConnect(playerid) {
    PlayerBreakOutTick[playerid] = 0;
    return 1;
}

CMD:breakout(playerid, params[]) {

    if(!Player_IsInJail(playerid)) {
        return SendErrorMsg(playerid, "Ban khong o trong tu.");
    }

    if(gettime() - PlayerBreakOutTick[playerid] < 60) {
        return SendErrorMsg(playerid, "Ban vua dung lenh nay, vui long cho...");
    }

    if (Player_GetPicklock(playerid) < 1) {
        return SendErrorMsg(playerid, "Ban can picklock de vuot nguc.");
    }

    new
        bool:success_rate = RandomChance(65);
    if (!success_rate) {
        SendServerMsg(playerid, "Ban vuot nguc that bai.");
        PlayerBreakOutTick[playerid] = gettime();
        return 1;
    }


    Player_GivePicklock(playerid, -1);
    Player_ReleaseFromJail(playerid);

    PlayerBreakOutTick[playerid] = gettime();
    return 1;
}
