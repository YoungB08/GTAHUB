#include <YSI_Coding\y_hooks>

static
    HitAmount[MAX_PLAYERS],
    HitReason[MAX_PLAYERS][32],
    HitPlacedBy[MAX_PLAYERS][MAX_PLAYER_NAME],
    BitArray:HasHit<MAX_PLAYERS>
;

hook OnMySQLConnected() {
    return 1;
}

hook OnPlayerConnect(playerid) {
    _InitHitmanData(playerid);
    return 1;
}

hook OnPlayerLogin(playerid) {
    Hitman_LoadPlayerHit(playerid);
    return 1;
}

hook OnPlayerDeathEx(playerid, killerid, reason) {
    if (killerid != INVALID_PLAYER_ID) {
        if (Bit_Get(HasHit, playerid) && Player_GetJob(killerid) == TYPE_HITMAN) {
            SendMsgAllF(COLOR_ORANGE, "** HIT **: "C_GREY"%p (%i) "C_WHITE"da hoan thanh hop dong tren "C_GREY"%p (%i) "C_WHITE"va nhan "C_GREEN"$%i.", killerid, killerid, playerid, playerid, HitAmount[playerid]);
            // Reward killer.
            GivePlayerMoney(killerid, HitAmount[playerid]);

            Hitman_DeletePlayerHit(playerid);

            // Reset data.
            _InitHitmanData(playerid);

        }
    }
    return 1;
}

hook OnPlayerDisconnect(playerid, reason){
    if (Bit_Get(HasHit, playerid))  {
        Hitman_SavePlayerHit(playerid);
        SendMsgAllF(COLOR_ORANGE, "** HIT **: "C_WHITE"Hop dong tren "C_GREY"%p (%i) "C_WHITE"duoc luu lai vi nguoi choi da thoat.", playerid, playerid );
    }
    return 1;
}

static _InitHitmanData(playerid) {
    Bit_Set(HasHit, playerid, false);
    HitAmount[playerid] = 0;
    HitReason[playerid][0] = EOS;
    HitPlacedBy[playerid][0] = EOS;
}

static Hitman_SavePlayerHit(playerid) {
    if(!Bit_Get(HasHit, playerid) || Player_GetAccountID(playerid) <= 0) {
        return 0;
    }

    new
        query[512],
        accountID = Player_GetAccountID(playerid);

    mysql_format(
        MySQL_GetHandle(),
        query,
        sizeof(query),
        "INSERT INTO player_hits (u_id, amount, reason, placed_by) VALUES (%i, %i, '%e', '%e') ON DUPLICATE KEY UPDATE amount = %i, reason = '%e', placed_by = '%e'",
        accountID,
        HitAmount[playerid],
        HitReason[playerid],
        HitPlacedBy[playerid],
        HitAmount[playerid],
        HitReason[playerid],
        HitPlacedBy[playerid]
    );
    mysql_tquery(MySQL_GetHandle(), query);
    return 1;
}

static Hitman_DeletePlayerHit(playerid) {
    if(Player_GetAccountID(playerid) <= 0) {
        return 0;
    }

    new
        query[96];

    mysql_format(MySQL_GetHandle(), query, sizeof(query), "DELETE FROM player_hits WHERE u_id = %i", Player_GetAccountID(playerid));
    mysql_tquery(MySQL_GetHandle(), query);
    return 1;
}

static Hitman_LoadPlayerHit(playerid) {
    new
        query[128],
        accountID = Player_GetAccountID(playerid);

    if(accountID <= 0) {
        return 0;
    }

    inline const OnHitLoad() {
        if(!IsPlayerConnected(playerid) || Player_GetAccountID(playerid) != accountID) {
            return 1;
        }

        if(!cache_num_rows()) {
            return 1;
        }

        cache_get_value_name_int(0, "amount", HitAmount[playerid]);
        cache_get_value_name(0, "reason", HitReason[playerid], sizeof(HitReason[]));
        cache_get_value_name(0, "placed_by", HitPlacedBy[playerid], sizeof(HitPlacedBy[]));

        Bit_Set(HasHit, playerid, true);
        SendServerMsgF(playerid, "Ban dang co hop dong hit tri gia "C_GREEN"%m"C_WHITE", ly do: "C_GREY"%s", HitAmount[playerid], HitReason[playerid]);
        return 1;
    }

    mysql_format(MySQL_GetHandle(), query, sizeof(query), "SELECT amount, reason, placed_by FROM player_hits WHERE u_id = %i", accountID);
    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnHitLoad, query);
    return 1;
}

CMD:hitlist(playerid, params[]) {
    new
        count = 0;

    if (Player_GetJob(playerid) != TYPE_HITMAN || Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return SendErrorMsg(playerid, "Ban khong duoc dung lenh nay neu khong phai hitman.");
    }

    new
        tmpHitlist[96],
        hitlistContent[512];

    strcat(hitlistContent, "Nguoi choi\tTien thuong\tLy do\n", sizeof(hitlistContent));
    foreach(new i : Player) {
        if(!Bit_Get(HasHit, i)) {
            continue;
        }

        format(tmpHitlist, sizeof(tmpHitlist), "%p\t%m\t%s\n", i, HitAmount[i], HitReason[i]);
        strcat(hitlistContent, tmpHitlist, sizeof(hitlistContent));
        count ++;
    }
    if(count == 0) {
        MessageBox_Show(playerid, TYPE_MSGBOX, "~y~Hitlist", "Hien khong co hop dong nao.");
    } else {
        Dialog_Show(
            playerid,
            DIALOG_STYLE_TABLIST_HEADERS,
            "Hitlist",
            hitlistContent,
            "Dong",
            ""
        );
    }
    return true;
}

CMD:placehit(playerid, params[])
{
    new
        targetid, hitamount, reason[32];

    if (Player_GetJob(playerid) == TYPE_HITMAN && Player_GetClass(playerid) == TEAM_CIVILIAN ) {
        return SendErrorMsg(playerid, "Hitman khong duoc tu dat hop dong len nguoi khac.");
    }

    if (sscanf(params, "ris[32]", targetid, hitamount, reason)) {
        return SendSyntaxMsg(playerid, "/placehit <playerid> <so tien> <ly do>");
    }

    if(targetid == playerid) {
        return SendErrorMsg(playerid, "Ban khong the dat hit len chinh minh.");
    }

    if(strlen(reason) > 32) {
        return SendErrorMsg(playerid, "Ly do qua dai. "C_YELLOW"MEO: "C_GREY"Toi da 32 ky tu.");
    }

    if (!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi ban dat hit khong online.");
    }

    if (!Player_IsSpawned(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi ban dat hit chua spawn.");
    }

    if (GetPlayerMoney(playerid) < hitamount) {
        return SendErrorMsg(playerid, "Ban khong du tien de tra hop dong, hay kiem them tien!");
    }

    if (hitamount < 10000) {
        return SendErrorMsg(playerid, "Hitman se khong nhan gia nay, hay tang tien thuong!");
    }

    if (Bit_Get(HasHit, targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi nay da co hop dong hit.");
    }

    new
        name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, MAX_PLAYER_NAME);

    strcpy(HitReason[targetid], reason, sizeof(reason));
    strcpy(HitPlacedBy[targetid], name, sizeof(name));
    HitAmount[targetid]  = hitamount;
    Bit_Set(HasHit, targetid, true);
    Hitman_SavePlayerHit(targetid);
    GivePlayerMoney(playerid, -hitamount);

    SendMsgF(playerid, COLOR_ORANGE, "Hit: "C_WHITE"Ban da dat hit len "C_GREY"%p (%i) "C_WHITE"voi gia "C_GREEN"$%i"C_WHITE", ly do: "C_GREY"%s", targetid, targetid, hitamount, reason);
    SendMsgAllF(COLOR_ORANGE, "Hit: "C_GREY"%p (%i) "C_WHITE"da dat hit len "C_GREY"%p (%i) "C_WHITE"voi gia "C_GREEN"$%i"C_WHITE", ly do: "C_GREY"%s", playerid, playerid, targetid, targetid, hitamount, reason);
    return true;
}
