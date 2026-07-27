#include <YSI_Coding\y_hooks>

static
    PlayerBreakCuffTick[MAX_PLAYERS];

hook OnPlayerConnect(playerid) {
    PlayerBreakCuffTick[playerid] = 0;
    return 1;
}

CMD:breakcuff(playerid, params[]) {

    if (Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return SendErrorMsg(playerid, "Chi dan thuong moi co the pha cong tay.");
    }
    if (!Player_IsCuffed(playerid)) {
        return SendErrorMsg(playerid, "Ban chua bi cong tay.");
    }
    if (gettime() - PlayerBreakCuffTick[playerid] < 60) {
        return SendErrorMsg(playerid, "Ban vua dung lenh nay, vui long thu lai sau.");
    }
    if (Player_GetPicklock(playerid) < 1) {
        return SendErrorMsg(playerid, "Ban can picklock de thoat khoi cong tay.");
    }

    new
        bool:success_rate = RandomChance(65);
    if (!success_rate) {
        SendServerMsg(playerid, "Ban pha cong tay that bai.");
        PlayerBreakCuffTick[playerid] = gettime();
        return 1;
    }

    // Notify player
    SendServerMsg(playerid, "Ban da pha duoc cong tay! Hay chay khoi canh sat!");

    // Set data
    Player_GivePicklock(playerid, -1);
    Player_Cuff(playerid, false);

    PlayerBreakCuffTick[playerid] = gettime();
    return 1;
}
