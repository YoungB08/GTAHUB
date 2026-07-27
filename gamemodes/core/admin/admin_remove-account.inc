void:Acc_Remove(username[], playerid = INVALID_PLAYER_ID)
{
    new 
        string[45 + MAX_PLAYER_NAME],
        name[MAX_PLAYER_NAME];
    strcpy(name, username, MAX_PLAYER_NAME);

    inline OnPlayerRemoveAcc() {
        if (!cache_num_rows()) {
            if (playerid != INVALID_PLAYER_ID) {
                SendServerMsgF(playerid, "Tai khoan (%s) khong ton tai trong database.", name);
                Logger_Dbg(PLAYER_DEBUG_HANDLER, "Attempted to remove Account", Logger_S("username", name));
                return;
            }
        }

        new accountid;
        cache_get_value_name_int(0, "u_id", accountid);

        mysql_format(MySQL_GetHandle(), string, sizeof(string), "DELETE FROM players WHERE u_id = %i OR account_id = %i", accountid, accountid);
        mysql_tquery(MySQL_GetHandle(), string, "", "");
        if (playerid != INVALID_PLAYER_ID) {
            SendServerMsgF(playerid, "Ban da xoa thanh cong tai khoan cua "C_GREY"%s"C_WHITE".", name);
            Logger_Dbg(PLAYER_DEBUG_HANDLER, "Account has been successfully removed the Account", Logger_S("username", name));
        }
    }

    static const query[] = "\
        SELECT \
            u_id \
        FROM \
            players \
        WHERE \
            username = '%e' AND is_character = 0"
    ;
    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnPlayerRemoveAcc, query, username, playerid);
}

ACMD:[5]deleteacc(playerid, params[])
{
    if (isnull(params) || strval(params)) {
        return SendSyntaxMsg(playerid, "/deleteacc <ten player>");
    }
    if (strlen(params) > MAX_PLAYER_NAME) {
        return SendErrorMsg(playerid, "MAX_PLAYER_NAME = 24, hay nhap lai ten!");
    }

    Acc_Remove(params, playerid);
    return 1;
}
