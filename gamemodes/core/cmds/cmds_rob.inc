#include <YSI_Coding\y_hooks>

static
    PlayerRobTick[MAX_PLAYERS];

hook OnPlayerConnect(playerid) {
    PlayerRobTick[playerid] = 0;
    return 1;
}

CMD:rob(playerid, params[])
{
    new
        targetid = Player_GetClosestPlayer(playerid, 5.0);

    if (!IsPlayerConnected(targetid) || !Player_IsSpawned(targetid)) {
        return SendErrorMsg(playerid, "Khong co nguoi choi gan day de cuop.");
    }

    if(GetPlayerState(targetid) != PLAYER_STATE_ONFOOT) {
        return SendErrorMsg(playerid, "Nguoi choi ban muon cuop khong di bo.");
    }

    if (gettime() - PlayerRobTick[playerid] < 45) {
        return SendErrorMsg(playerid, "Ban vua cuop gan day, vui long thu lai sau.");
    }

    if(GetPlayerMoney(targetid) < 1000) {
        return SendErrorMsg(playerid, "Nguoi choi nay khong co du tien.");
    }

    if (Player_GetWallet(targetid) > 0) {
        SendMsgF(playerid, COLOR_ORANGE, "(cuop nguoi choi) "C_WHITE"Ban cuop "C_GREY"%p (%i) "C_WHITE"that bai vi nguoi nay co vi bao ve, chay di!", targetid, targetid);
        SendMsgF(targetid, COLOR_ORANGE, "(cuop nguoi choi) "C_GREY"%p (%i) "C_WHITE"da thu cuop ban nhung that bai, duoi theo di! - Vi con lai: %i", playerid, playerid, Player_GetWallet(playerid));
        PlayerRobTick[playerid] = gettime();
        return 1;
    }

    new
        bool:success_rate = RandomChance(65);

    if (!success_rate) {
        SendMsgF(playerid, COLOR_ORANGE, "(cuop nguoi choi) "C_WHITE"Ban cuop "C_GREY"%p (%i) "C_WHITE"that bai, chay di!", targetid, targetid);
        SendMsgF(targetid, COLOR_ORANGE, "(cuop nguoi choi) "C_GREY"%p (%i) "C_WHITE"da thu cuop ban nhung that bai, duoi theo di!", playerid, playerid);
        PlayerRobTick[playerid] = gettime();
        return 1;
    }

    new max_robbed = (GetPlayerMoney(targetid) / 100) * 3;
    
    // Cap the maximum base robbery amount to $10,000 to prevent economy breaking
    if (max_robbed > 10000) {
        max_robbed = 10000;
    }

    new
        // 3% of player's money + $300 on top, up to a max of $10,300.
        robbedcash = random(max_robbed) + 300;

    CallLocalFunction("OnPlayerRobPlayer", "iii", playerid, targetid, robbedcash);

    PlayerRobTick[playerid] = gettime();
    return true;
}
