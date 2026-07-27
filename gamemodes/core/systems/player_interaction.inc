#include <YSI_Coding\y_hooks>

static PlayerInteractTarget[MAX_PLAYERS];

static PlayerInteract_GetJobName(jobid, output[], len)
{
    switch(jobid) {
        case TYPE_DRUGDEALER: format(output, len, "Buon Ma Tuy");
        case TYPE_WEAPONDEALER: format(output, len, "Buon Vu Khi");
        case TYPE_HITMAN: format(output, len, "Hitman");
        case TYPE_TERRORIST: format(output, len, "Khung Bo");
        case TYPE_RAPIST: format(output, len, "Ke Truyen Benh");
        case TYPE_MECHANIC: format(output, len, "Tho May");
        default: format(output, len, "Chua co");
    }
    return 1;
}

static bool:PlayerInteract_IsValidTarget(playerid, targetid)
{
    if(!IsPlayerConnected(targetid) || targetid == playerid) {
        return false;
    }

    return Player_IsNearPlayer(playerid, targetid, 4.0) ? true : false;
}

static PlayerInteract_ShowStats(playerid, targetid)
{
    new
        targetName[MAX_PLAYER_NAME],
        jobName[24],
        info[384];

    GetPlayerName(targetid, targetName, sizeof(targetName));
    PlayerInteract_GetJobName(Player_GetJob(targetid), jobName, sizeof(jobName));

    format(
        info,
        sizeof(info),
        "Ten: %s\nLevel: %d\nXP: %d/%d\nTien mat: %m\nNgan hang: %m\nWanted: %d\nNghe chinh: %s\nSo dien thoai: %d",
        targetName,
        GetPlayerLevel(targetid),
        GetPlayerXp(targetid),
        GetPlayerXpRequired(targetid),
        GetPlayerMoney(targetid),
        Player_GetBankMoney(targetid),
        GetPlayerWantedLevel(targetid),
        jobName,
        Phone_GetNumber(targetid)
    );

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Thong tin nhan vat", info, "Dong", "");
    return 1;
}

static PlayerInteract_ShowGiveMoney(playerid, targetid)
{
    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused listitem

        if(!response) {
            return 1;
        }

        if(!PlayerInteract_IsValidTarget(playerid, targetid)) {
            SendErrorMsg(playerid, "Nguoi choi khong con o gan ban.");
            return 1;
        }

        new amount = strval(inputtext);

        if(amount <= 0) {
            SendErrorMsg(playerid, "So tien khong hop le.");
            return 1;
        }

        if(GetPlayerMoney(playerid) < amount) {
            SendErrorMsg(playerid, "Ban khong du tien mat.");
            return 1;
        }

        GivePlayerMoney(playerid, -amount);
        GivePlayerMoney(targetid, amount);
        SendServerMsgF(playerid, "Ban da dua %m cho nguoi choi.", amount);
        SendServerMsgF(targetid, "Ban da nhan %m tu nguoi choi.", amount);
        return 1;
    }

    Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_INPUT, "Dua tien", "Nhap so tien muon dua", "Dua", "Huy");
    return 1;
}

static PlayerInteract_ShowMenu(playerid, targetid)
{
    PlayerInteractTarget[playerid] = targetid;

    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused inputtext

        if(!response) {
            return 1;
        }

        new target = PlayerInteractTarget[playerid];

        if(!PlayerInteract_IsValidTarget(playerid, target)) {
            SendErrorMsg(playerid, "Nguoi choi khong con o gan ban.");
            return 1;
        }

        switch(listitem) {
            case 0: PlayerInteract_ShowStats(playerid, target);
            case 1: License_ShowList(playerid, target);
            case 2: PlayerInteract_ShowGiveMoney(playerid, target);
            case 3: Phone_CallNumber(playerid, Phone_GetNumber(target));
            case 4: {
                SendServerMsgF(playerid, "Dung /sms %d [noi dung] de nhan tin nguoi choi nay.", Phone_GetNumber(target));
            }
            case 5: {
                SendServerMsg(playerid, "Da gui loi moi job/faction/party neu he thong lien quan dang san sang.");
                SendServerMsg(target, "Ban vua nhan mot loi moi job/faction/party.");
            }
        }
        return 1;
    }

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_LIST,
        "Tuong tac nguoi choi",
        "Thong tin nhan vat\nXem giay phep\nDua tien\nGoi dien\nNhan tin\nMoi job/faction/party",
        "Chon",
        "Dong"
    );
    return 1;
}

hook OnPlayerConnect(playerid)
{
    PlayerInteractTarget[playerid] = INVALID_PLAYER_ID;
    return 1;
}

CMD:interact(playerid, params[])
{
    new targetid;

    if(sscanf(params, "u", targetid)) {
        targetid = Player_GetClosestPlayer(playerid, 4.0);
    }

    if(!PlayerInteract_IsValidTarget(playerid, targetid)) {
        return SendErrorMsg(playerid, "Khong co nguoi choi nao o gan de tuong tac.");
    }

    PlayerInteract_ShowMenu(playerid, targetid);
    return 1;
}
