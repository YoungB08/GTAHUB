#include <YSI_Coding\y_hooks>

const
    CHANGE_SKILL_PRICE  = 7_500;

hook OnGameModeInit() {
    Create3DTextLabel("Nhan "C_YELLOW"~k~~CONVERSATION_YES~"C_WHITE" de doi ky nang", COLOR_WHITE, 359.7139, 173.5916, 1009.3893, 40.0, 26, false);
    new BotAnna = CreateActor(141, 359.7139, 173.5916, 1008.3893, 271.1871);
    SetActorVirtualWorld(BotAnna, 26);
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if(PRESSED(KEY_YES)) {
        if(IsPlayerInRangeOfPoint(playerid, 5.0, 359.7139, 173.5916, 1008.3893)) {
            Cityhall_ChangeSkill(playerid);
        }
    }
    return 1;
}

void:Cityhall_ChangeSkill(playerid) {
    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused listitem, inputtext
        if(!response) {
            return;
        }

        if(GetPlayerMoney(playerid) < CHANGE_SKILL_PRICE) {
            SendErrorMsg(playerid, "Khong the doi ky nang, ban khong du tien.");
            return;
        }

        if(!License_PlayerHas(playerid, LICENSE_JOB)) {
            SendErrorMsg(playerid, "Ban can giay phep nghe dac biet de doi ky nang tai Toa Thi Chinh.");
            return;
        }

        Player_ShowJobMenu(playerid);
        GivePlayerMoney(playerid, -CHANGE_SKILL_PRICE);
    }

    new
        string[64 + 24];

    format(
        string,
        sizeof(string),
        "Ban co chac muon doi ky nang khong?\n\n\
        Phi doi ky nang: "C_GREEN"%m",
        CHANGE_SKILL_PRICE
    );

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_MSGBOX,
        "Doi ky nang",
        string,
        "Chac chan",
        "Khong"
    );
}
