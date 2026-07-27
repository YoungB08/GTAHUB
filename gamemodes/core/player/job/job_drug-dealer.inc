static
    // Crack data
    sellCrackCooldown[MAX_PLAYERS],
    BitArray:HasBeenOfferedCrack<MAX_PLAYERS>,
    crackSeller[MAX_PLAYERS],
    crackPrice[MAX_PLAYERS],
    crackAmount[MAX_PLAYERS],
    // Weed Data
    sellWeedCooldown[MAX_PLAYERS],
    BitArray:HasBeenOfferedWeed<MAX_PLAYERS>,
    weedSeller[MAX_PLAYERS],
    weedPrice[MAX_PLAYERS],
    weedAmount[MAX_PLAYERS]
;


#include <YSI_Coding\y_hooks>

hook OnPlayerConnect(playerid) {
    sellCrackCooldown[playerid] =
    crackPrice[playerid] =
    crackAmount[playerid] = -1;
    crackSeller[playerid] = INVALID_PLAYER_ID;
    Bit_Set(HasBeenOfferedCrack, playerid, false);

    sellWeedCooldown[playerid] =
    weedPrice[playerid] =
    weedAmount[playerid] = -1;
    weedSeller[playerid] = INVALID_PLAYER_ID;
    Bit_Set(HasBeenOfferedWeed, playerid, false);
    return 1;
}


CMD:sellcrack(playerid, params[]) {
    if (Player_GetJob(playerid) != TYPE_DRUGDEALER || Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return SendErrorMsg(playerid, "Chi drug dealer moi dung duoc lenh nay.");
    }

    new
        targetid,
        price,
        amount;

    if(sscanf(params, "rii", targetid, price, amount)) {
        return SendSyntaxMsg(playerid, "/sellcrack <playerid> <gia> <so luong> (TOI DA: 5)");
    }

    if(targetid == playerid) {
        return SendErrorMsg(playerid, "Ban khong the ban hang cho chinh minh.");
    }


    if(Player_GetCrack(playerid) < amount) {
        return SendErrorMsgF(playerid, "Ban chi co %i crack nhung dang muon ban %i!", Player_GetCrack(playerid), amount);
    }

    if(amount > 5) {
        return SendErrorMsg(playerid, "Ban chi co the ban toi da 50 gram moi lan.");
    }

    if(5000 > price > 20000) {
        return SendErrorMsg(playerid, "Gia toi thieu la $5,000 va toi da la $20,000.");
    }

    if(Player_GetCrack(targetid) >= MAX_CRACK) {
        return SendErrorMsg(playerid, "Khong the ban crack cho nguoi nay, ho da dat gioi han co the cam.");
    }

    if(gettime() < sellCrackCooldown[playerid]) {
        return SendErrorMsgF(playerid, "Ban vua ban crack gan day, vui long thu lai sau %i giay!", sellCrackCooldown[playerid] - gettime());
    }

    sellCrackCooldown[playerid] = gettime() + 60;

    Bit_Set(HasBeenOfferedCrack, targetid, true);
    crackSeller[targetid]	= playerid;
    crackPrice[targetid] 	= price;
    crackAmount[targetid] 	= amount;

    SendMsgF(playerid, COLOR_GREEN, "** CRACK **: "C_WHITE" Ban da rao ban cho "C_GREY"%p (%i) "C_WHITE""C_GREY"%i gram "C_WHITE"crack voi gia "C_GREEN"%m", targetid, targetid, amount * 10, price);
    SendMsgF(targetid, COLOR_GREEN, "** CRACK **: "C_WHITE" Drug dealer "C_GREY"%p (%i) "C_WHITE"rao ban cho ban "C_GREY"%i gram "C_WHITE"crack, dung "C_GREY"/buycrack "C_WHITE"de chap nhan.", playerid, playerid, amount * 10);
    return 1;
}

CMD:buycrack(playerid, params[]) {
    if(Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return SendErrorMsg(playerid, "Chi dan thuong moi dung duoc lenh nay.");
    }

    if(!Bit_Get(HasBeenOfferedCrack, playerid)) {
        return SendErrorMsg(playerid, "Chua ai rao ban crack cho ban.");
    }

    if(Player_GetCrack(playerid) >= MAX_CRACK) {
        return SendErrorMsg(playerid, "Ban khong the mua crack, ban da dat gioi han co the cam.");
    }

    if(GetPlayerMoney(playerid) < crackPrice[playerid]) {
        return SendErrorMsgF(playerid, "Ban khong du tien de mua crack tu "C_GREY"%p", crackSeller[playerid]);
    }

    Bit_Set(HasBeenOfferedCrack, playerid, false);
    GivePlayerMoney(playerid, -crackPrice[playerid]);
    GivePlayerMoney(crackSeller[playerid], crackPrice[playerid]);
    Player_GiveCrack(playerid, crackAmount[playerid]);

    SendMsgF(crackSeller[playerid], COLOR_GREEN, "** CRACK **: "C_WHITE"%p (%i) da chap nhan giao dich, ban nhan "C_GREEN"$%m"C_WHITE" khi ban "C_GREY"%i gram "C_WHITE"crack.",
        playerid, playerid, crackPrice[playerid], crackAmount[playerid] * 10);

    SendMsgF(playerid, COLOR_GREEN, "** CRACK **: "C_WHITE"Ban da mua thanh cong "C_GREY"%i gram "C_WHITE"crack voi gia "C_GREEN"$%m"C_WHITE" tu "C_GREY"%p(%i)",
        crackAmount[playerid] * 10, crackPrice[playerid], crackSeller[playerid], crackSeller[playerid]);
    return 1;
}

// WEED

new
    WeedFarmArea;

hook OnScriptInit() {
    WeedFarmArea = CreateDynamicRectangle(
        2442.1895,-1721.3822, 2477.2610,-1707.8583
    );

    CreateDynamicMapIcon(2463.1084, -1715.6919, 13.5065, 46, -1, .style = MAPICON_GLOBAL);
}

CMD:plantweed(playerid, params[]) {

    if (Player_GetJob(playerid) != TYPE_DRUGDEALER || Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return SendErrorMsg(playerid, "Chi drug dealer moi dung duoc lenh nay.");
    }

    if(GetPlayerState(playerid) != PLAYER_STATE_ONFOOT) {
        return SendErrorMsg(playerid, "Ban can di bo.");
    }

    if(!IsPlayerInDynamicArea(playerid, WeedFarmArea)) {
        return SendErrorMsg(playerid, "Ban can o weed farm de trong cay.");
    }

    new
        Float:x,
        Float:y,
        Float:z;

    GetPlayerPos(playerid, x, y, z);

    Weed_Create(playerid, x, y, z);
    return 1;
}

CMD:harvestweed(playerid, params[]) {
    if (Player_GetJob(playerid) != TYPE_DRUGDEALER || Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return SendErrorMsg(playerid, "Chi drug dealer moi dung duoc lenh nay.");
    }

    new
        weedId = Weed_GetClosest(playerid);

    if (weedId == -1) {
        return SendErrorMsg(playerid, "Ban khong dung gan cay weed nao cua minh!");
    }

    if (Weed_GetPercent(playerid, weedId) < 100) {
        return SendErrorMsg(playerid, "Chua the thu hoach cay can sa, cay chua lon het!");
    }

    Weed_ResetData(playerid, weedId);
    ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, false, false, false, false, 0, SYNC_ALL);

    Player_GiveWeed(playerid, 1);
    SendClientMessage(playerid, COLOR_GREEN, "** THU HOACH WEED ** "C_WHITE"Ban da thu hoach 10 gram weed.");
    return 1;
}

CMD:sellweed(playerid, params[]) {
    if (Player_GetJob(playerid) != TYPE_DRUGDEALER || Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return SendErrorMsg(playerid, "Chi drug dealer moi dung duoc lenh nay.");
    }

    new
        targetid,
        price,
        amount;

    if(sscanf(params, "uii", targetid, price, amount)) {
        return SendSyntaxMsg(playerid, "/sellweed <playerid> <gia> <so luong> (TOI DA: 5 = 50g)");
    }

    if(targetid == playerid) {
        return SendErrorMsg(playerid, "Ban khong the ban hang cho chinh minh.");
    }

    if(Player_GetWeed(playerid) < amount) {
        return SendErrorMsgF(playerid, "Ban chi co %i weed nhung dang muon ban %i!", Player_GetWeed(playerid), amount);
    }

    if(amount > 5) {
        return SendErrorMsg(playerid, "Ban chi co the ban toi da 50 gram moi lan.");
    }

    if(5_000 > price > 10_000) {
        return SendErrorMsg(playerid, "Gia toi thieu la $5,000 va toi da la $10,000.");
    }

    if(Player_GetWeed(targetid) >= MAX_WEED) {
        return SendErrorMsg(playerid, "Khong the ban weed cho nguoi nay, ho da dat gioi han co the cam.");
    }

    if(gettime() < sellWeedCooldown[playerid]) {
        return SendErrorMsgF(playerid, "Ban vua ban weed gan day, vui long thu lai sau %i giay!", sellWeedCooldown[playerid] - gettime());
    }

    sellWeedCooldown[playerid] = gettime() + 60;

    Bit_Set(HasBeenOfferedWeed, targetid, true);
    weedSeller[targetid]	= playerid;
    weedPrice[targetid] 	= price;
    weedAmount[targetid] 	= amount;

    new
        grams = (amount * 10);

    SendMsgF(
        playerid,
        COLOR_GREEN,
        "** WEED **: "C_WHITE" Ban da rao ban cho "C_GREY"%p (%i) "C_WHITE""C_GREY"%i gram "C_WHITE"weed voi gia "C_GREEN"%m",
        targetid,
        targetid,
        grams,
        price
    );
    SendMsgF(
        targetid,
        COLOR_GREEN,
        "** WEED **: "C_GREY"%p (%i) "C_WHITE"rao ban cho ban "C_GREY"%i gram "C_WHITE"weed voi gia "C_GREEN"%m"C_WHITE", dung "C_GREY"/buyweed "C_WHITE"de chap nhan.",
        playerid,
        playerid,
        grams,
        price
    );
    return 1;
}

CMD:buyweed(playerid, params[]) {
    if(Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return SendErrorMsg(playerid, "Chi dan thuong moi dung duoc lenh nay.");
    }

    if(!Bit_Get(HasBeenOfferedWeed, playerid)) {
        return SendErrorMsg(playerid, "Chua ai rao ban weed cho ban.");
    }

    if(Player_GetWeed(playerid) >= MAX_WEED) {
        return SendErrorMsg(playerid, "Ban khong the mua weed, ban da dat gioi han co the cam.");
    }

    if(GetPlayerMoney(playerid) < weedPrice[playerid]) {
        return SendErrorMsgF(playerid, "Ban khong du tien de mua weed tu ~y~%p", weedSeller[playerid]);
    }

    Bit_Set(HasBeenOfferedWeed, playerid, false);
    GivePlayerMoney(playerid, -weedPrice[playerid]);
    GivePlayerMoney(weedSeller[playerid], weedPrice[playerid]);
    Player_GiveWeed(playerid, weedAmount[playerid]);

    new
        grams = (weedAmount[playerid] * 10);

    SendMsgF(
        weedSeller[playerid],
        COLOR_GREEN,
        "** WEED **: "C_WHITE"Ban nhan "C_GREEN"$%m"C_WHITE" khi ban "C_GREY"%i gram "C_WHITE"weed cho "C_GREY"%p(%i)",
        weedPrice[playerid],
        grams,
        playerid,
        playerid
    );

    SendMsgF(
        playerid,
        COLOR_GREEN,
        "** WEED **: "C_WHITE"Ban da mua "C_GREY"%i gram "C_WHITE"weed voi gia "C_GREEN"$%m"C_WHITE" tu "C_GREY"%p(%i)",
        grams,
        weedPrice[playerid],
        weedSeller[playerid],
        weedSeller[playerid]
    );
    return 1;
}
