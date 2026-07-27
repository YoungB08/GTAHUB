#include <YSI_Coding\y_hooks>

#define PHONE_PAYPHONE_FEE          (25)
#define PHONE_NUMBER_BASE           (500000)

enum E_PAYPHONE_DATA {
    E_PAYPHONE_NAME[32],
    Float:E_PAYPHONE_X,
    Float:E_PAYPHONE_Y,
    Float:E_PAYPHONE_Z,
    Float:E_PAYPHONE_A
}

static const PayPhoneData[][E_PAYPHONE_DATA] = {
    {"City Hall Payphone", 1472.9500, -1742.7000, 13.5469, 180.0},
    {"Unity Station Payphone", 1743.6400, -1861.1500, 13.5700, 90.0},
    {"Idlewood Payphone", 1933.2500, -1773.4200, 13.3828, 270.0}
};

static
    PlayerPhoneNumber[MAX_PLAYERS],
    PayPhoneArea[sizeof(PayPhoneData)],
    Statement:stmt_loadPhone,
    Statement:stmt_insertPhone,
    Statement:stmt_insertContact,
    Statement:stmt_loadContacts;

stock Phone_GetNumber(playerid)
{
    if(!IsPlayerConnected(playerid)) {
        return 0;
    }
    return PlayerPhoneNumber[playerid];
}

stock Phone_FindPlayerByNumber(number)
{
    foreach(new i : Player) {
        if(PlayerPhoneNumber[i] == number) {
            return i;
        }
    }
    return INVALID_PLAYER_ID;
}

static Phone_SaveNumber(playerid)
{
    MySQL_BindInt(stmt_insertPhone, 0, Player_GetAccountID(playerid));
    MySQL_BindInt(stmt_insertPhone, 1, PlayerPhoneNumber[playerid]);
    MySQL_BindInt(stmt_insertPhone, 2, PlayerPhoneNumber[playerid]);
    MySQL_ExecuteThreaded(stmt_insertPhone);
    return 1;
}

static Phone_Load(playerid)
{
    inline OnPhoneLoad()
    {
        new phoneNumber;
        MySQL_BindResultInt(stmt_loadPhone, 0, phoneNumber);

        if(MySQL_Statement_FetchRow(stmt_loadPhone)) {
            PlayerPhoneNumber[playerid] = phoneNumber;
        }
        else {
            PlayerPhoneNumber[playerid] = PHONE_NUMBER_BASE + Player_GetAccountID(playerid);
            Phone_SaveNumber(playerid);
        }
    }

    MySQL_BindInt(stmt_loadPhone, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_loadPhone, using inline OnPhoneLoad);
    return 1;
}

static Phone_SendSMS(playerid, number, const message[])
{
    new targetid = Phone_FindPlayerByNumber(number);

    if(targetid == INVALID_PLAYER_ID) {
        return SendErrorMsg(playerid, "So dien thoai nay hien khong online.");
    }

    SendServerMsgF(playerid, "SMS den %d: %s", number, message);
    SendServerMsgF(targetid, "SMS tu %d: %s", Phone_GetNumber(playerid), message);
    return 1;
}

stock Phone_CallNumber(playerid, number, bool:payphone = false)
{
    new targetid = Phone_FindPlayerByNumber(number);

    if(targetid == INVALID_PLAYER_ID) {
        return SendErrorMsg(playerid, "So dien thoai nay hien khong online.");
    }

    if(payphone) {
        if(GetPlayerMoney(playerid) < PHONE_PAYPHONE_FEE) {
            return SendErrorMsg(playerid, "Ban khong du tien de goi dien thoai cong cong.");
        }
        GivePlayerMoney(playerid, -PHONE_PAYPHONE_FEE);
    }

    new callerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, callerName, sizeof(callerName));

    SendServerMsgF(playerid, "Ban dang goi den %d.", number);
    SendServerMsgF(targetid, "Dien thoai reo: %s dang goi cho ban. Dung /call %d de goi lai.", callerName, Phone_GetNumber(playerid));
    return 1;
}

static Phone_ShowContacts(playerid)
{
    inline OnContactLoad()
    {
        new
            contactName[24],
            number,
            list[512],
            line[64],
            rows;

        list[0] = '\0';
        strcat(list, "Ten\tSo dien thoai\n", sizeof(list));

        MySQL_BindResult(stmt_loadContacts, 0, contactName, sizeof(contactName));
        MySQL_BindResultInt(stmt_loadContacts, 1, number);

        while(MySQL_Statement_FetchRow(stmt_loadContacts)) {
            format(line, sizeof(line), "%s\t%d\n", contactName, number);
            strcat(list, line, sizeof(list));
            rows++;
        }

        if(!rows) {
            Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Danh ba", "Danh ba cua ban dang trong.", "Dong", "");
        }
        else {
            Dialog_Show(playerid, DIALOG_STYLE_TABLIST_HEADERS, "Danh ba", list, "Dong", "");
        }
    }

    MySQL_BindInt(stmt_loadContacts, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_loadContacts, using inline OnContactLoad);
    return 1;
}

static Phone_ShowSMSDialog(playerid)
{
    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused listitem

        if(!response) {
            return 1;
        }

        new
            number,
            message[96];

        if(sscanf(inputtext, "is[96]", number, message)) {
            SendErrorMsg(playerid, "Nhap: so_dien_thoai noi_dung");
            Phone_ShowSMSDialog(playerid);
            return 1;
        }

        Phone_SendSMS(playerid, number, message);
        return 1;
    }

    Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_INPUT, "Gui SMS", "Nhap: so_dien_thoai noi_dung", "Gui", "Huy");
    return 1;
}

static Phone_ShowCallDialog(playerid, bool:payphone = false)
{
    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused listitem

        if(!response) {
            return 1;
        }

        new number = strval(inputtext);

        if(number <= 0) {
            SendErrorMsg(playerid, "So dien thoai khong hop le.");
            Phone_ShowCallDialog(playerid, payphone);
            return 1;
        }

        Phone_CallNumber(playerid, number, payphone);
        return 1;
    }

    new info[96];
    format(info, sizeof(info), "Nhap so dien thoai can goi.%s", payphone ? ("\nPhi cuoc: $25") : (""));
    Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_INPUT, payphone ? ("Dien thoai cong cong") : ("Goi dien"), info, "Goi", "Huy");
    return 1;
}

static Phone_ShowAddContactDialog(playerid)
{
    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused listitem

        if(!response) {
            return 1;
        }

        new
            contactName[24],
            number;

        if(sscanf(inputtext, "s[24]i", contactName, number)) {
            SendErrorMsg(playerid, "Nhap: ten so_dien_thoai");
            Phone_ShowAddContactDialog(playerid);
            return 1;
        }

        MySQL_BindInt(stmt_insertContact, 0, Player_GetAccountID(playerid));
        MySQL_Bind(stmt_insertContact, 1, contactName);
        MySQL_BindInt(stmt_insertContact, 2, number);
        MySQL_Bind(stmt_insertContact, 3, contactName);
        MySQL_ExecuteThreaded(stmt_insertContact);
        SendServerMsgF(playerid, "Da luu %s vao danh ba voi so %d.", contactName, number);
        return 1;
    }

    Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_INPUT, "Them danh ba", "Nhap: ten so_dien_thoai", "Luu", "Huy");
    return 1;
}

static Phone_ShowMobileMenu(playerid)
{
    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused inputtext

        if(!response) {
            return 1;
        }

        switch(listitem) {
            case 0: {
                new info[64];
                format(info, sizeof(info), "So dien thoai cua ban: %d", Phone_GetNumber(playerid));
                Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Dien thoai", info, "Dong", "");
            }
            case 1: Phone_ShowSMSDialog(playerid);
            case 2: Phone_ShowCallDialog(playerid);
            case 3: Phone_ShowContacts(playerid);
            case 4: Phone_ShowAddContactDialog(playerid);
            case 5: Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Thong bao", "Chua co thong bao moi.", "Dong", "");
        }
        return 1;
    }

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_LIST,
        "Dien thoai di dong",
        "So cua toi\nGui SMS\nGoi dien\nDanh ba\nThem danh ba\nThong bao",
        "Chon",
        "Dong"
    );
    return 1;
}

hook OnMySQLConnected()
{
    stmt_loadPhone = MySQL_PrepareStatement(MySQL_GetHandle(), "SELECT phone_number FROM player_phones WHERE u_id = ?");
    stmt_insertPhone = MySQL_PrepareStatement(MySQL_GetHandle(), "INSERT INTO player_phones (u_id, phone_number) VALUES (?, ?) ON DUPLICATE KEY UPDATE phone_number = ?");
    stmt_insertContact = MySQL_PrepareStatement(MySQL_GetHandle(), "INSERT INTO player_phone_contacts (u_id, contact_name, phone_number) VALUES (?, ?, ?) ON DUPLICATE KEY UPDATE contact_name = ?");
    stmt_loadContacts = MySQL_PrepareStatement(MySQL_GetHandle(), "SELECT contact_name, phone_number FROM player_phone_contacts WHERE u_id = ? ORDER BY contact_name ASC");
    return 1;
}

hook OnGameModeInit()
{
    new label[160];

    for(new i; i < sizeof(PayPhoneData); i++) {
        Interaction_BuildPointLabel(label, sizeof(label), "Phone booth", PayPhoneData[i][E_PAYPHONE_NAME], "Can nhap so de goi", "goi dien");

        CreateDynamicObject(
            1216,
            PayPhoneData[i][E_PAYPHONE_X],
            PayPhoneData[i][E_PAYPHONE_Y],
            PayPhoneData[i][E_PAYPHONE_Z] - 0.9,
            0.0,
            0.0,
            PayPhoneData[i][E_PAYPHONE_A]
        );

        PayPhoneArea[i] = CreateDynamicCircle(PayPhoneData[i][E_PAYPHONE_X], PayPhoneData[i][E_PAYPHONE_Y], 2.0, 0, 0);
        CreateDynamic3DTextLabel(
            label,
            COLOR_ORANGE,
            PayPhoneData[i][E_PAYPHONE_X],
            PayPhoneData[i][E_PAYPHONE_Y],
            PayPhoneData[i][E_PAYPHONE_Z] + 0.7,
            INTERACTION_LABEL_DISTANCE,
            .streamdistance = INTERACTION_LABEL_DISTANCE
        );
    }
    return 1;
}

hook OnPlayerConnect(playerid)
{
    PlayerPhoneNumber[playerid] = 0;
    return 1;
}

hook OnPlayerLogin(playerid)
{
    Phone_Load(playerid);
    return 1;
}

hook OnPlayerEnterDynArea(playerid, areaid)
{
    for(new i; i < sizeof(PayPhoneData); i++) {
        if(areaid == PayPhoneArea[i]) {
            Interaction_ShowPrompt(playerid, INTERACTION_TYPE_PHONE, i, "~y~~h~Phone booth", "goi dien");
            break;
        }
    }
    return 1;
}

hook OnPlayerLeaveDynArea(playerid, areaid)
{
    for(new i; i < sizeof(PayPhoneData); i++) {
        if(areaid == PayPhoneArea[i]) {
            Interaction_ClearPrompt(playerid, INTERACTION_TYPE_PHONE, i);
            break;
        }
    }
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if(PRESSED(KEY_NO) && Interaction_IsActive(playerid, INTERACTION_TYPE_PHONE)) {
        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_PHONE, Interaction_GetActiveID(playerid));
        return 1;
    }

    if(PRESSED(KEY_YES) && Interaction_IsActive(playerid, INTERACTION_TYPE_PHONE)) {
        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_PHONE, Interaction_GetActiveID(playerid));
        Phone_ShowCallDialog(playerid, true);
    }
    return 1;
}

CMD:phone(playerid, params[])
{
    #pragma unused params
    Phone_ShowMobileMenu(playerid);
    return 1;
}

CMD:sms(playerid, params[])
{
    new
        number,
        message[96];

    if(sscanf(params, "is[96]", number, message)) {
        return SendErrorMsg(playerid, "Dung: /sms [so dien thoai] [noi dung]");
    }

    Phone_SendSMS(playerid, number, message);
    return 1;
}

CMD:call(playerid, params[])
{
    new number;

    if(sscanf(params, "i", number)) {
        return SendErrorMsg(playerid, "Dung: /call [so dien thoai]");
    }

    Phone_CallNumber(playerid, number);
    return 1;
}

CMD:addcontact(playerid, params[])
{
    new
        contactName[24],
        number;

    if(sscanf(params, "s[24]i", contactName, number)) {
        return SendErrorMsg(playerid, "Dung: /addcontact [ten] [so dien thoai]");
    }

    MySQL_BindInt(stmt_insertContact, 0, Player_GetAccountID(playerid));
    MySQL_Bind(stmt_insertContact, 1, contactName);
    MySQL_BindInt(stmt_insertContact, 2, number);
    MySQL_Bind(stmt_insertContact, 3, contactName);
    MySQL_ExecuteThreaded(stmt_insertContact);
    SendServerMsgF(playerid, "Da them %s vao danh ba.", contactName);
    return 1;
}
