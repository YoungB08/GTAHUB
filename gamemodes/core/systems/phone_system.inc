/*
    phone_system.inc
    ----------------
    Hệ thống Điện thoại cơ bản cho Server RP.

    Tính năng:
    - Mỗi player có số điện thoại 7 chữ số duy nhất (0900000 - 0999999)
    - /phone     – Mở menu điện thoại (số điện thoại, cuộc gọi, SMS)
    - /sms [so]  [tin nhan] – Gửi tin nhắn tới số điện thoại
    - /call [so] – Gọi điện thoại (nhận realtime dialog)
    - /911       – Gọi khẩn cấp (thông báo tới tất cả LSPD on duty)
    - /addcontact [so] [ten] – Lưu danh bạ
    - /myphone   – Xem số điện thoại của bản thân
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define PHONE_NUMBER_MIN        (9000000)
#define PHONE_NUMBER_MAX        (9999999)
#define MAX_SMS_LENGTH          (128)
#define PHONE_CALL_TIMEOUT      (30)    // Giây trước khi gọi tự cúp

// =============================================================================
// DATA
// =============================================================================

static
    Statement:stmt_loadPhone,
    Statement:stmt_insertPhone,
    Statement:stmt_loadContacts,
    Statement:stmt_saveContact;

// Số điện thoại player
static PhoneNumber[MAX_PLAYERS][12]; // "09XXXXXXX\0"

// Cuộc gọi hiện tại: -1 nếu không có
static PhoneCallTarget[MAX_PLAYERS];    // Người đang gọi
static PhoneCallTimer[MAX_PLAYERS];     // Giây còn lại

// Danh bạ runtime (tối đa 20 số)
#define MAX_CONTACTS_PER_PLAYER (20)
enum E_CONTACT {
    E_CONTACT_NAME[32],
    E_CONTACT_NUMBER[12]
}
static ContactList[MAX_PLAYERS][MAX_CONTACTS_PER_PLAYER][E_CONTACT];
static ContactCount[MAX_PLAYERS];

// =============================================================================
// HOOKS – MYSQL SETUP
// =============================================================================

hook OnMySQLConnected()
{
    mysql_query_file(MySQL_GetHandle(), "phone.sql", false);

    static const loadQ[] = "SELECT phone_num FROM phone_numbers WHERE u_id = ?";
    stmt_loadPhone = MySQL_PrepareStatement(MySQL_GetHandle(), loadQ);

    static const insertQ[] = "INSERT IGNORE INTO phone_numbers (u_id, phone_num) VALUES (?, ?)";
    stmt_insertPhone = MySQL_PrepareStatement(MySQL_GetHandle(), insertQ);

    static const contactsQ[] = "SELECT name, number FROM phone_contacts WHERE owner_uid = ? LIMIT 20";
    stmt_loadContacts = MySQL_PrepareStatement(MySQL_GetHandle(), contactsQ);

    static const saveContactQ[] = "INSERT INTO phone_contacts (owner_uid, name, number) VALUES (?, ?, ?)";
    stmt_saveContact = MySQL_PrepareStatement(MySQL_GetHandle(), saveContactQ);

    return 1;
}

// =============================================================================
// HOOKS – PLAYER LIFECYCLE
// =============================================================================

hook OnPlayerConnect(playerid)
{
    PhoneNumber[playerid][0] = '\0';
    PhoneCallTarget[playerid] = -1;
    PhoneCallTimer[playerid] = 0;
    ContactCount[playerid] = 0;
    return 1;
}

hook OnPlayerLogin(playerid)
{
    Phone_Load(playerid);
    return 1;
}

hook OnPlayerRegister(playerid)
{
    Phone_GenerateNew(playerid);
    return 1;
}

hook OnPlayerSecondUpdate(playerid)
{
    // Đếm ngược cuộc gọi không được bắt
    if (PhoneCallTarget[playerid] != -1)
    {
        PhoneCallTimer[playerid]--;
        if (PhoneCallTimer[playerid] <= 0)
        {
            // Cúp máy sau timeout
            PhoneCallTarget[playerid] = -1;
            PhoneCallTimer[playerid] = 0;
            SendErrorMsg(playerid, "[DIEN THOAI] Cuoc goi khong duoc tra loi.");
        }
    }
    return 1;
}

// =============================================================================
// INTERNAL – LOAD / GENERATE
// =============================================================================

static Phone_Load(playerid)
{
    inline OnPhoneLoad()
    {
        new numStr[12];
        MySQL_BindResultString(stmt_loadPhone, 0, numStr, 12);

        if (MySQL_Statement_FetchRow(stmt_loadPhone))
        {
            strcopy(PhoneNumber[playerid], numStr, 12);
            Phone_UI_SetNumber(playerid, PhoneNumber[playerid]);
        }
        else
        {
            // Chưa có số – tạo mới
            Phone_GenerateNew(playerid);
        }

        // Load danh bạ
        Phone_LoadContacts(playerid);
    }

    MySQL_BindInt(stmt_loadPhone, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_loadPhone, using inline OnPhoneLoad);
}

static Phone_GenerateNew(playerid)
{
    new number = PHONE_NUMBER_MIN + random(PHONE_NUMBER_MAX - PHONE_NUMBER_MIN);
    format(PhoneNumber[playerid], 12, "0%d", number);

    MySQL_BindInt(stmt_insertPhone, 0, Player_GetAccountID(playerid));
    MySQL_BindString(stmt_insertPhone, 1, PhoneNumber[playerid], 12);
    MySQL_ExecuteThreaded(stmt_insertPhone);
    Phone_UI_SetNumber(playerid, PhoneNumber[playerid]);
}

static Phone_LoadContacts(playerid)
{
    inline OnContactLoad()
    {
        new name[32], number[12];
        MySQL_BindResultString(stmt_loadContacts, 0, name, 32);
        MySQL_BindResultString(stmt_loadContacts, 1, number, 12);

        ContactCount[playerid] = 0;
        while (MySQL_Statement_FetchRow(stmt_loadContacts) && ContactCount[playerid] < MAX_CONTACTS_PER_PLAYER)
        {
            strcopy(ContactList[playerid][ContactCount[playerid]][E_CONTACT_NAME], name, 32);
            strcopy(ContactList[playerid][ContactCount[playerid]][E_CONTACT_NUMBER], number, 12);
            ContactCount[playerid]++;

            MySQL_BindResultString(stmt_loadContacts, 0, name, 32);
            MySQL_BindResultString(stmt_loadContacts, 1, number, 12);
        }
    }

    MySQL_BindInt(stmt_loadContacts, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_loadContacts, using inline OnContactLoad);
}

// =============================================================================
// HELPER – Tìm player theo số điện thoại
// =============================================================================

static FindPlayerByPhone(const phoneNum[])
{
    foreach (new i : Player)
    {
        if (!Player_IsLoggedIn(i)) continue;
        if (!strcmp(PhoneNumber[i], phoneNum, true, 12))
            return i;
    }
    return -1;
}

// =============================================================================
// COMMAND – /myphone – Xem số điện thoại bản thân
// =============================================================================

CMD:myphone(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(PhoneNumber[playerid]))
        return SendErrorMsg(playerid, "Ban chua co so dien thoai. Lien he admin."), 1;

    SendMsgF(playerid, COLOR_SAMPBLUE, "[DIEN THOAI] So cua ban: %s", PhoneNumber[playerid]);
    return 1;
}

// =============================================================================
// COMMAND – /sms [so] [tin nhan]
// =============================================================================

CMD:sms(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new phoneNum[12], message[MAX_SMS_LENGTH];
    if (sscanf(params, "s[12]s[128]", phoneNum, message))
        return SendErrorMsg(playerid, "Su dung: /sms [so_dien_thoai] [tin nhan]"), 1;

    new targetid = FindPlayerByPhone(phoneNum);
    if (targetid == -1)
        return SendErrorMsg(playerid, "So dien thoai nay hien khong online hoac khong ton tai."), 1;

    SendMsgF(playerid, COLOR_GREY, "[SMS -> %s] %s", phoneNum, message);
    SendMsgF(targetid, COLOR_YELLOW, "[SMS tu %s]: %s", PhoneNumber[playerid], message);

    return 1;
}

// =============================================================================
// COMMAND – /call [so] – Gọi điện
// =============================================================================

CMD:call(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new phoneNum[12];
    if (sscanf(params, "s[12]", phoneNum))
        return SendErrorMsg(playerid, "Su dung: /call [so_dien_thoai]"), 1;

    if (PhoneCallTarget[playerid] != -1)
        return SendErrorMsg(playerid, "Ban dang co cuoc goi khac. Dung /hangup truoc."), 1;

    new targetid = FindPlayerByPhone(phoneNum);
    if (targetid == -1)
        return SendErrorMsg(playerid, "So dien thoai nay hien khong online."), 1;

    if (PhoneCallTarget[targetid] != -1)
        return SendErrorMsg(playerid, "Nguoi nay dang ban. Vui long thu lai sau."), 1;

    // Thiết lập cuộc gọi
    PhoneCallTarget[playerid] = targetid;
    PhoneCallTimer[playerid]  = PHONE_CALL_TIMEOUT;

    SendMsgF(playerid, COLOR_GREEN, "[DIEN THOAI] Dang goi toi %s...", phoneNum);
    SendMsgF(targetid, COLOR_GREEN, "[DIEN THOAI] Co cuoc goi den tu %s. Dung /answer de nghe hoac /reject tu choi.",
        PhoneNumber[playerid]);

    // Dialog chấp nhận cho target
    inline OnCallResponse(response, listitem, string:inputtext[])
    {
        #pragma unused listitem, inputtext
        if (response)
        {
            // Chấp nhận
            if (!IsPlayerConnected(playerid) || PhoneCallTarget[playerid] != targetid)
            {
                SendErrorMsg(targetid, "[DIEN THOAI] Cuoc goi da bi ngat.");
                return 1;
            }
            PhoneCallTarget[targetid] = playerid;
            PhoneCallTimer[targetid]  = 0;
            PhoneCallTimer[playerid]  = 0;
            SendMsgF(playerid, COLOR_GREEN, "[DIEN THOAI] Ket noi! Dang dam thoai voi %s. Dung /hangup de cat may.", phoneNum);
            SendMsgF(targetid, COLOR_GREEN, "[DIEN THOAI] Da ket noi! Dang dam thoai voi %s. Dung /hangup de cat may.", PhoneNumber[playerid]);
        }
        else
        {
            // Từ chối
            PhoneCallTarget[playerid] = -1;
            PhoneCallTimer[playerid]  = 0;
            SendMsgF(playerid, COLOR_GREY, "[DIEN THOAI] Cuoc goi tu choi.");
            SendMsgF(targetid, COLOR_GREY, "[DIEN THOAI] Ban da tu choi cuoc goi.");
        }
        return 1;
    }

    Dialog_ShowCallback(targetid, using inline OnCallResponse,
        DIALOG_STYLE_MSGBOX,
        "Cuoc Goi Den",
        "Co cuoc goi den. Ban co muon nghe khong?",
        "Nghe may", "Tu choi");

    return 1;
}

// =============================================================================
// COMMAND – /hangup – Cúp máy
// =============================================================================

CMD:hangup(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (PhoneCallTarget[playerid] == -1)
        return SendErrorMsg(playerid, "Ban khong co cuoc goi nao dang mo."), 1;

    new targetid = PhoneCallTarget[playerid];
    PhoneCallTarget[playerid] = -1;
    PhoneCallTimer[playerid] = 0;

    SendMsgF(playerid, COLOR_GREY, "[DIEN THOAI] Da cat may.");

    if (IsPlayerConnected(targetid) && PhoneCallTarget[targetid] == playerid)
    {
        PhoneCallTarget[targetid] = -1;
        PhoneCallTimer[targetid] = 0;
        SendMsgF(targetid, COLOR_GREY, "[DIEN THOAI] Nguoi goi da cat may.");
    }
    return 1;
}

// =============================================================================
// COMMAND – /911 – Gọi cảnh sát khẩn cấp
// =============================================================================

CMD:e911(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new reason[64] = "Khong ro ly do";
    if (!isnull(params)) strcopy(reason, params, 64);

    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    new zoneName[32];
    new MapZone:zoneid = GetPlayerMapZone(playerid);
    if (!GetMapZoneName(zoneid, zoneName, sizeof(zoneName))) {
        format(zoneName, sizeof(zoneName), "San Andreas");
    }

    // Gửi tới tất cả cảnh sát on duty
    new notified = 0;
    foreach (new i : Player)
    {
        if (!Player_IsLoggedIn(i)) continue;
        if (Player_GetClass(i) != TEAM_POLICE) continue;
        if (!LSPD_IsOnDuty(i)) continue;

        SendMsgF(i, COLOR_RED,
            "[911] Tu: %s (ID:%d) | Khu vuc: %s | Ly do: %s",
            ReturnPlayerName(playerid), playerid, zoneName, reason);
        SetPlayerCheckpoint(i, px, py, pz, 5.0);
        notified++;
    }

    if (notified > 0)
        SendMsgF(playerid, COLOR_GREEN, "[911] Da gui bao canh sat. Co %d si quan duoc thong bao.", notified);
    else
        SendErrorMsg(playerid, "[911] Hien khong co canh sat truc tuyem. Xin thu lai sau.");

    return 1;
}

// =============================================================================
// COMMAND – /addcontact [so] [ten] – Thêm danh bạ
// =============================================================================

CMD:addcontact(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new phoneNum[12], name[32];
    if (sscanf(params, "s[12]s[32]", phoneNum, name))
        return SendErrorMsg(playerid, "Su dung: /addcontact [so_dien_thoai] [ten]"), 1;

    if (ContactCount[playerid] >= MAX_CONTACTS_PER_PLAYER)
        return SendErrorMsg(playerid, "Danh ba da day! Toi da " #MAX_CONTACTS_PER_PLAYER " so."), 1;

    new idx = ContactCount[playerid];
    strcopy(ContactList[playerid][idx][E_CONTACT_NAME], name, 32);
    strcopy(ContactList[playerid][idx][E_CONTACT_NUMBER], phoneNum, 12);
    ContactCount[playerid]++;

    MySQL_BindInt(stmt_saveContact, 0, Player_GetAccountID(playerid));
    MySQL_BindString(stmt_saveContact, 1, name, 32);
    MySQL_BindString(stmt_saveContact, 2, phoneNum, 12);
    MySQL_ExecuteThreaded(stmt_saveContact);

    SendMsgF(playerid, COLOR_GREEN, "[DANH BA] Da luu: %s - %s", name, phoneNum);
    return 1;
}

stock Phone_GetNumber(playerid)
{
    return strval(PhoneNumber[playerid]);
}

stock Phone_CallNumber(playerid, number)
{
    new phoneNum[12];
    if (number >= PHONE_NUMBER_MIN && number <= PHONE_NUMBER_MAX) {
        format(phoneNum, sizeof(phoneNum), "0%d", number);
    }
    else {
        format(phoneNum, sizeof(phoneNum), "%d", number);
    }
    return cmd_call(playerid, phoneNum);
}

// Alias /dialup /callme
ALTCMD:dialup(call);
