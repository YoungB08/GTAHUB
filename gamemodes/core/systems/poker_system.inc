#include <YSI_Coding\y_hooks>

#define POKER_MIN_BET       (100)
#define POKER_MAX_BET       (5000)

static Poker_GetHandName(rank, output[], len)
{
    switch(rank) {
        case 0: format(output, len, "High card");
        case 1: format(output, len, "One pair");
        case 2: format(output, len, "Two pair");
        case 3: format(output, len, "Three of a kind");
        case 4: format(output, len, "Straight");
        case 5: format(output, len, "Flush");
        case 6: format(output, len, "Full house");
        default: format(output, len, "Royal hand");
    }
    return 1;
}

static Poker_GetRank()
{
    new roll = random(100);

    if(roll < 40) return 0;
    if(roll < 65) return 1;
    if(roll < 80) return 2;
    if(roll < 90) return 3;
    if(roll < 95) return 4;
    if(roll < 98) return 5;
    if(roll < 99) return 6;
    return 7;
}

static Poker_GetMultiplier(rank)
{
    switch(rank) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 4;
        case 5: return 5;
        case 6: return 7;
    }
    return 10;
}

CMD:poker(playerid, params[])
{
    new bet;

    if(sscanf(params, "i", bet)) {
        bet = 500;
    }

    if(bet < POKER_MIN_BET || bet > POKER_MAX_BET) {
        return SendErrorMsgF(playerid, "Tien cuoc phai tu %m den %m.", POKER_MIN_BET, POKER_MAX_BET);
    }

    if(GetPlayerMoney(playerid) < bet) {
        return SendErrorMsg(playerid, "Ban khong du tien de choi poker.");
    }

    new
        rank = Poker_GetRank(),
        multiplier = Poker_GetMultiplier(rank),
        payout = bet * multiplier,
        handName[32];

    GivePlayerMoney(playerid, -bet);
    Poker_GetHandName(rank, handName, sizeof(handName));

    if(payout > 0) {
        payout = JobProgress_CalculateReward(playerid, JOB_PROGRESS_POKER, payout);
        GivePlayerMoney(playerid, payout);
        SendServerMsgF(playerid, "Poker: %s. Ban nhan %m.", handName, payout);
    }
    else {
        SendErrorMsgF(playerid, "Poker: %s. Ban thua %m.", handName, bet);
    }

    JobProgress_AddXP(playerid, JOB_PROGRESS_POKER, max(20, bet / 25));
    return 1;
}
