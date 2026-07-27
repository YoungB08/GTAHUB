#include <YSI_Coding\y_hooks>

const DEATH_TAX_PERCENTAGE = 2;
const WEEKLY_TAX_PERCENTANGE = 5;

hook OnPlayerDeathEx(playerid, killerid, reason) {
    new
        cashInHand = GetPlayerMoney(playerid) + Player_GetBankMoney(playerid);

    if(cashInHand > 100_000) {
        return 1;
    }

    new
        tax = (cashInHand / 100) * DEATH_TAX_PERCENTAGE;

    MessageBox_ShowF(
        playerid,
        TYPE_MSGBOX,
        "~r~Bao Hiem Nhan Tho",
        "~w~Cam on ban da dong bao hiem nhan tho.\
        ~n~\
        Ban da bi tru ~r~%m",
        _,
        tax
    );

    GivePlayerMoney(playerid, -tax);
    Server_GiveMoney(tax);
    return 1;
}

hook OnServerWeekReset() {
    new
        cashInHand,
        tax;
    foreach(new playerid : Player) {
        if(!Player_IsLoggedIn(playerid)) {
            continue;
        }

        cashInHand = GetPlayerMoney(playerid) + Player_GetBankMoney(playerid);

        if(cashInHand > 100_000) {
            continue;
        }

        tax = (cashInHand / 100) * WEEKLY_TAX_PERCENTANGE;

        MessageBox_ShowF(
            playerid,
            TYPE_MSGBOX,
            "~r~Thue Hang Tuan",
            "~w~Cam on ban da dong thue hang tuan.\
            ~n~\
            Ban da bi tru ~r~%m\
            ~n~~n~\
            ~w~So tien nay duoc dung de tai tro cac hop dong hit ngau nhien.",
            _,
            tax
        );
        Server_GiveMoney(tax);
    }
}

stock Server_GetMoney() {
    return GetSVarInt("SERVER_MONEY");
}

Server_GiveMoney(amount) {
    SetSVarInt("SERVER_MONEY", Server_GetMoney() + amount);
}
