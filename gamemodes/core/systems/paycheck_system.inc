/*
    paycheck_system.inc
    -------------------
    Hệ thống Paycheck (Lương định kỳ) cho Server RP.

    Cơ chế:
    - Mỗi PAYCHECK_INTERVAL phút (mặc định 60), mỗi player đang online nhận lương.
    - Lương được tính dựa trên Job/Class của nhân vật:
      * TEAM_POLICE  – Lương cảnh sát cố định + bonus theo level
      * TEAM_ARMY    – Lương quân đội
      * TEAM_MEDIC   – Lương bác sĩ
      * TEAM_CIVILIAN– Lương dân thường (tính theo job hiện tại)
    - Sau khi nhận lương, thông báo hiện lên màn hình.
    - Tích hợp /paycheck để xem thông tin lương hiện tại.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

// Khoảng thời gian trả lương (giây) – mặc định 60 phút
#define PAYCHECK_INTERVAL   (60 * 60)

// Lương cơ bản theo class ($/lần)
#define PAYCHECK_POLICE     (15000)
#define PAYCHECK_ARMY       (18000)
#define PAYCHECK_MEDIC      (14000)
#define PAYCHECK_CIVILIAN   (8000)

// Bonus % theo level XP (mỗi 10 level tăng 5%)
#define PAYCHECK_LEVEL_BONUS_PER_10_LEVELS  (0.05)

// Trừ thuế nhà (nếu có nhà) – tính cố định theo giờ
#define PAYCHECK_HOUSE_TAX  (2000)

// =============================================================================
// STATIC VARIABLES
// =============================================================================

// Đếm giây cho mỗi player để biết khi nào trả lương
static PaycheckTimer[MAX_PLAYERS];

// =============================================================================
// HOOK – Đếm mỗi giây
// =============================================================================

hook OnPlayerSecondUpdate(playerid)
{
    if (!Player_IsLoggedIn(playerid) || !Player_IsSpawned(playerid)) return 1;

    PaycheckTimer[playerid]++;

    if (PaycheckTimer[playerid] >= PAYCHECK_INTERVAL)
    {
        PaycheckTimer[playerid] = 0;
        Paycheck_Give(playerid);
    }
    return 1;
}

// =============================================================================
// HOOK – Reset timer khi kết nối / login
// =============================================================================

hook OnPlayerConnect(playerid)
{
    PaycheckTimer[playerid] = 0;
    return 1;
}

hook OnPlayerLogin(playerid)
{
    PaycheckTimer[playerid] = 0;
    return 1;
}

// =============================================================================
// INTERNAL – Tính và phát lương
// =============================================================================

static Paycheck_Give(playerid)
{
    new playerClass = Player_GetClass(playerid);
    new level       = Player_GetLevel(playerid);

    // Lương cơ bản theo class
    new basePay;
    new className[32];

    switch (playerClass)
    {
        case TEAM_POLICE:
        {
            basePay = PAYCHECK_POLICE;
            className = "Canh Sat";
        }
        case TEAM_ARMY:
        {
            basePay = PAYCHECK_ARMY;
            className = "Quan Doi";
        }
        case TEAM_MEDIC:
        {
            basePay = PAYCHECK_MEDIC;
            className = "Y Te";
        }
        default:
        {
            // Civilian – lương dựa vào job
            basePay = PAYCHECK_CIVILIAN;
            className = "Dan Thuong";
        }
    }

    // Bonus level: mỗi 10 level +5% lương
    new Float:levelBonus = float(level / 10) * PAYCHECK_LEVEL_BONUS_PER_10_LEVELS;
    new bonusAmount       = floatround(float(basePay) * levelBonus);
    new totalPay          = basePay + bonusAmount;

    // Trừ thuế
    new tax = 0;
    // TODO: kiểm tra có nhà hay không để trừ PAYCHECK_HOUSE_TAX
    // if (House_PlayerOwnsAny(playerid)) tax += PAYCHECK_HOUSE_TAX;

    new netPay = totalPay - tax;
    if (netPay < 0) netPay = 0;

    GivePlayerMoney(playerid, netPay);
    ShowPlayerCashText(playerid, netPay);

    ShowPlayerPopup(playerid, "LUONG THANG", 
        "Luong co ban", 
        6000, false);

    // Gửi thông báo chi tiết qua chat
    SendMsgF(playerid, COLOR_GREEN,
        "[LUONG] Nghe: %s | Luong: $%d | Bonus: $%d | Thue: $%d | NHAN: $%d",
        className, basePay, bonusAmount, tax, netPay);

    Logger_Dbg(PLAYER_DEBUG_HANDLER, "[Paycheck_Give] paycheck issued",
        Logger_P(playerid),
        Logger_I("class",   playerClass),
        Logger_I("level",   level),
        Logger_I("net_pay", netPay)
    );
}

// =============================================================================
// COMMAND – /paycheck – Xem thông tin lương hiện tại
// =============================================================================

CMD:paycheck(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new playerClass = Player_GetClass(playerid);
    new level       = Player_GetLevel(playerid);

    new basePay;
    new className[32];
    switch (playerClass)
    {
        case TEAM_POLICE:  { basePay = PAYCHECK_POLICE;  className = "Canh Sat"; }
        case TEAM_ARMY:    { basePay = PAYCHECK_ARMY;    className = "Quan Doi"; }
        case TEAM_MEDIC:   { basePay = PAYCHECK_MEDIC;   className = "Y Te"; }
        default:           { basePay = PAYCHECK_CIVILIAN; className = "Dan Thuong"; }
    }

    new Float:levelBonus = float(level / 10) * PAYCHECK_LEVEL_BONUS_PER_10_LEVELS;
    new bonusAmount       = floatround(float(basePay) * levelBonus);
    new totalPay          = basePay + bonusAmount;

    new remaining = PAYCHECK_INTERVAL - PaycheckTimer[playerid];
    new mins      = remaining / 60;
    new secs      = remaining % 60;

    new content[256];
    format(content, sizeof(content),
        "Thong Tin Luong Hien Tai:\n\n\
Nghe: %s  |  Level: %d\n\
Luong co ban: $%d\n\
Bonus level:  $%d\n\
Tong nhan:    $%d\n\n\
Luong tiep theo: %d phut %d giay",
        className, level, basePay, bonusAmount, totalPay, mins, secs
    );

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Thong Tin Luong", content, "Dong", "");
    return 1;
}

// Alias /salary
ALTCMD:salary(paycheck);
