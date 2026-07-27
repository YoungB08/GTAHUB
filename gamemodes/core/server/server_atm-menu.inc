#include <YSI_Coding\y_hooks>

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    // don't run code below if player is not near
    // any ATM machine.
    if(!Player_IsNearATM(playerid)) {
        return 1;
    }

    // disable access if ATM is disabled
    new
        atmID = ATM_GetClosest(playerid),
        idx = ATM_FindIndexIDBySQLID(atmID);

    if(ATM_GetState(idx)) {
        return 1;
    }

    if(newkeys == KEY_YES) {
        ATM_MainMenu(playerid);
    }
    return 1;
}

void:ATM_MainMenu(playerid)
{
    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused inputtext
        if(!response) {
            return;
        }

        switch(listitem) {
            case 0: {
                ATM_DepositMenu(playerid);
            }
            case 1: {
                ATM_WithdrawMenu(playerid);
            }
        }
    }

    new
        title[32];
    format(title, sizeof(title), "ATM - So du: "C_GREEN"%m", Player_GetBankMoney(playerid));

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_LIST,
        title,
        "Gui tien\nRut tien",
        "Chon",
        "Dong"
    );
}

void:ATM_DepositMenu(playerid)
{
    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused listitem
        if(!response) {
            ATM_MainMenu(playerid);
            return ;
        }

        new
            amount = strval(inputtext);

        if(amount <= 0) {
            SendErrorMsg(playerid, "Du lieu nhap khong hop le.");
            ATM_DepositMenu(playerid);
            return;
        }

        if(GetPlayerMoney(playerid) < amount) {
            SendErrorMsg(playerid, "So tien nhap cao hon tien mat cua ban.");
            ATM_DepositMenu(playerid);
            return;
        }

        // Deduct money from player, add money to player's bank.
        Player_GiveBankMoney(playerid, amount, true);
        GivePlayerMoney(playerid, -amount);

        // Notify Player
        SendServerMsgF(playerid, "Ban da gui %m vao tai khoan ngan hang.", amount);
        SendServerMsgF(playerid, "So du hien tai: %m", Player_GetBankMoney(playerid));
    }

    new
        content[64 + 24];
    format(
        content,
        sizeof(content),
        "\
        "C_WHITE"So du: "C_GREEN"%m\n\n\
        "C_WHITE"Nhap so tien ban muon gui\
        ",
        Player_GetBankMoney(playerid)
    );

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_INPUT,
        "ATM Gui Tien",
        content,
        "Chon",
        "Quay Lai"
    );
}

void:ATM_WithdrawMenu(playerid)
{
    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused listitem
        if(!response) {
            ATM_MainMenu(playerid);
            return;
        }

        new
            amount = strval(inputtext);

        if(amount <= 0) {
            SendErrorMsg(playerid, "Du lieu nhap khong hop le.");
            ATM_WithdrawMenu(playerid);
            return;
        }

        if(Player_GetBankMoney(playerid) < amount) {
            SendErrorMsg(playerid, "So tien nhap cao hon so du ngan hang cua ban.");
            ATM_WithdrawMenu(playerid);
            return;
        }

        // Deduct money from bank, add money to player.
        Player_RemoveBankMoney(playerid, amount);
        GivePlayerMoney(playerid, amount);

        // Notify Player
        SendServerMsgF(playerid, "Ban da rut %m tu tai khoan ngan hang.", amount);
        SendServerMsgF(playerid, "So du hien tai: %m", Player_GetBankMoney(playerid));
    }

    new
        content[64 + 24];
    format(
        content,
        sizeof(content),
        "\
        "C_WHITE"So du: "C_GREEN"%m\n\n\
        "C_WHITE"Nhap so tien ban muon rut\
        ",
        Player_GetBankMoney(playerid)
    );

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_INPUT,
        "ATM Rut Tien",
        content,
        "Chon",
        "Quay Lai"
    );
}
