#define LOTTO_MAX_JACKPOT 		(15000) //+15, 000
#define LOTTO_MIN_JACKPOT		(5000) //+5, 000
#define LOTTO_START     		(25000) //25, 000 starting jackpot
#define MAX_LOTTO_INPUT			(MAX_PLAYERS)

#define TICKET_PRICE      		(500)

new
	LottoJackpot = LOTTO_START,
	BitArray:lottoUsedNumbers<MAX_LOTTO_INPUT>,
	BitArray:PlayerBoughtTicket<MAX_PLAYERS>,
	PlayerLottoNumber[MAX_PLAYERS] = {0, ...},
    lottoDrawCounter = 0,
    lottoLastTick = 0;

#include <YSI_Coding\y_hooks>

hook OnGameModeInit() {

    // initiate the global data on start up.
    lottoLastTick = gettime();
    lottoDrawCounter = 0;
    LottoJackpot = LOTTO_START;
    return 1;
}

hook OnPlayerConnect(playerid) {
    PlayerLottoNumber[playerid] = 0;
    Bit_Set(PlayerBoughtTicket, playerid, false);
    return 1;
}

hook OnServerSecondUpdate() {

    // only show the message every 72 seconds
    if(gettime() - lottoLastTick < (72)) {
        // go to next hook.
        return 1;
    }

    lottoDrawCounter++;

    // draw every ~6 minutes.
    if(lottoDrawCounter >= 5) { // draws every ~6 minutes (5 * 72s)
        new
            genRandomNum = 10,//random(MAX_LOTTO_INPUT - 1) + 1,
            winnerID = INVALID_PLAYER_ID;

        foreach(new i : Player) {

            // set winner id.
            if(PlayerLottoNumber[i] == genRandomNum) {
                winnerID = i;
            }

            // reset all data
            PlayerLottoNumber[i] = 0;
            Bit_Vet(PlayerBoughtTicket, i);
        }

        for(new i; i < MAX_LOTTO_INPUT; i++) {
            Bit_Vet(lottoUsedNumbers, i);
        }

        if(winnerID != INVALID_PLAYER_ID) {
            MessageBox_ShowAllF(
                TYPE_INFORMATION,
                "~y~Thong tin Lotto",
                "~y~CHUC MUNG! ~p~%p ~w~da trung ~g~%m ~w~tu xo so!~n~\
                ~w~Hay mua ve cho ky quay tiep theo ~p~/lotto [1 - %i]",
                _,
                winnerID, LottoJackpot, MAX_LOTTO_INPUT
            );

            GivePlayerMoney(winnerID, LottoJackpot);
            LottoJackpot = LOTTO_START;

            ApplyAnimation(winnerID, "OTB", "wtchrace_win", 4.1, false, true, true, false, 3300); // Winning animation
        }

        if(winnerID == INVALID_PLAYER_ID) {

            // increase the prize.
            LottoJackpot = LottoJackpot + random(LOTTO_MAX_JACKPOT - LOTTO_MIN_JACKPOT) + LOTTO_MIN_JACKPOT;

            // Notify players.
            MessageBox_ShowAllF(
                TYPE_INFORMATION,
                "~y~Thong tin Lotto",
                "Khong co ai trung lotto trong ky quay nay. Jackpot tang len ~g~%m~n~\
                ~w~Dung ~p~/lotto [1 - %i] ~w~de mua ve va co co hoi trung tien!",
                _,
                LottoJackpot, MAX_LOTTO_INPUT
            );
        }
        lottoDrawCounter = 0;
        lottoLastTick = gettime();
        return 1;
    }

    MessageBox_ShowAllF(
        TYPE_INFORMATION,
        "~y~Thong tin Lotto",
        "~w~Jackpot lotto hien tai la ~g~%m~n~\
        ~w~Dung ~p~/lotto [1 - %i] ~w~de mua ve va co co hoi trung tien!",
        _,
        LottoJackpot, MAX_LOTTO_INPUT
    );
    lottoLastTick = gettime();
	return 1;
}

CMD:lotto(playerid, params[]) {
    if(isnull(params) || !IsNumeric(params)) {
      	SendMsgF(playerid, COLOR_YELLOW, "Cu phap: "C_WHITE"/lotto <1 - %i>", MAX_LOTTO_INPUT);
      	SendMsgF(playerid, COLOR_WHITE, "Jackpot hien tai la "C_GREEN"%m", LottoJackpot);
        return 1;
    }

    new
    	lottoID = strval(params);

    if(GetPlayerMoney(playerid) < TICKET_PRICE) {
    	return SendErrorMsg(playerid, "Ban khong du tien de mua ve!");
    }

    if(lottoID <= 0 || lottoID > MAX_LOTTO_INPUT) {
    	return SendErrorMsgF(playerid, "Ban chi co the chon so tu 1 den %i", MAX_LOTTO_INPUT);
    }

    if(Bit_Get(lottoUsedNumbers, lottoID)) {
		return SendErrorMsgF(playerid, "So lotto "C_GREY"#%i "C_WHITE"da co nguoi chon, hay thu so khac!", lottoID);
    }

    if(Bit_Get(PlayerBoughtTicket, playerid)) {
    	return SendErrorMsg(playerid, "Ban da mua ve lotto roi!");
    }


    Bit_Let(lottoUsedNumbers, lottoID);
    Bit_Let(PlayerBoughtTicket, playerid);
    PlayerLottoNumber[playerid] = lottoID;

    GivePlayerMoney(playerid, -TICKET_PRICE); //Takes away the ticket cost.

    SendMsgF(playerid, COLOR_YELLOW, "** LOTTO ** "C_WHITE"Ban da mua ve xo so! So lotto cua ban la "C_GREY"#%i"C_WHITE", hay cho ky quay tiep theo!", lottoID);
    SendMsgF(playerid, COLOR_YELLOW, "** LOTTO ** "C_WHITE"Ky quay dien ra moi "C_GREY"~6 "C_WHITE"phut va se cong bo nguoi thang. Jackpot hien tai la "C_GREEN"%m", LottoJackpot);
    return 1;
}
