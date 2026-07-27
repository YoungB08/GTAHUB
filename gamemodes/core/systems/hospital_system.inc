#include <YSI_Coding\y_hooks>

static
    bool:BoughtChangeSkin[MAX_PLAYERS];

const
    HOSPITAL_CURE_PRICE = 1_500,
    HOSPITAL_HEAL_PRICE = 500,
    HOSPITAL_SKIN_CHANGE = 10_000;

hook OnGameModeInit() {
    Create3DTextLabel("Nhan "C_YELLOW"~k~~CONVERSATION_YES~"C_WHITE" de xem dich vu", COLOR_WHITE, 1173.1469, -1323.4918, 16.3958, 40.0, 0, false);
    CreateActor(70, 1173.1469, -1323.4918, 15.3958, 270.2290);
    return 1;
}

hook OnPlayerSpawn(playerid) {
	if(BoughtChangeSkin[playerid]) {
		BoughtChangeSkin[playerid] = false;
		Class_SelectionState(playerid, false);
		Player_SetSkin(playerid, Player_GetSkin(playerid), true);
	}
	return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if(PRESSED(KEY_YES)) {
        if(IsPlayerInRangeOfPoint(playerid, 5.0, 1173.1469, -1323.4918, 15.3958)) {
            ShowHospitalMenu(playerid);
        }
    }
    return 1;
}

stock ShowHospitalMenu(playerid) {

    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused inputtext
        if(!response) {
            return 1;
        }

        switch(listitem) {
            case 0: {
                new
                    Float:health;
                GetPlayerHealth(playerid, health);

                if(GetPlayerMoney(playerid) < HOSPITAL_HEAL_PRICE) {
                    return SendErrorMsg(playerid, "Khong du tien!");
                }

                if(health >= 100.0) {
                    return SendErrorMsg(playerid, "Mau cua ban da day, hien khong the hoi mau.");
                }

                SetPlayerHealth(playerid, 100);
                GivePlayerMoney(playerid, -HOSPITAL_HEAL_PRICE);
            }
            case 1: {
                if(GetPlayerMoney(playerid) < HOSPITAL_SKIN_CHANGE) {
                    return SendErrorMsg(playerid, "Khong du tien!");
                }

                if(GetPlayerWantedLevel(playerid) != 0) {
                    return SendErrorMsg(playerid, "Ban phai khong bi truy na de doi skin!");
                }

                BoughtChangeSkin[playerid] = true;
                Class_SelectionState(playerid, true);

                ForceClassSelection(playerid);
                TogglePlayerSpectating(playerid, true);
                TogglePlayerSpectating(playerid, false);

                GivePlayerMoney(playerid, -HOSPITAL_SKIN_CHANGE);
            }
            case 2: {
                if(GetPlayerMoney(playerid) < HOSPITAL_CURE_PRICE) {
                    return SendErrorMsg(playerid, "Khong du tien!");
                }

                if(!Disease_PlayerHas(playerid)) {
                    return SendErrorMsg(playerid, "Ban khong mac benh nao.");
                }

                Disease_GivePlayer(playerid, DISEASE_NONE);
                GivePlayerMoney(playerid, -HOSPITAL_CURE_PRICE);
            }
        }

    }

    new
        content[128 + 32];

    format(
        content,
        sizeof(content),
        "\
        Dich vu\tGia\n\
        "C_WHITE"Hoi mau\t"C_GREEN"%m\n\
        "C_WHITE"Doi skin/class\t"C_GREEN"%m\n\
        "C_WHITE"Chua benh\t"C_GREEN"%m\
        ", HOSPITAL_HEAL_PRICE, HOSPITAL_SKIN_CHANGE, HOSPITAL_CURE_PRICE
    );

    Dialog_ShowCallback(
        playerid, using inline _response,
        DIALOG_STYLE_TABLIST_HEADERS,
        "Benh vien",
        content,
        "Chon",
        "Huy"
        );

    return 1;
}
