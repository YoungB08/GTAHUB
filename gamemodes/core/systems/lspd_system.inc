/*
    lspd_system.inc
    ---------------
    Hệ thống LSPD (Los Santos Police Department) cơ bản cho Server RP.

    Tính năng:
    - On Duty / Off Duty toggle (/duty)
    - Khi On Duty: mặc đồng phục cảnh sát (skin tùy rank), nhận trang bị
    - MDC (Mobile Data Computer): tra cứu hồ sơ người chơi (/mdc)
    - Xuất phiếu phạt (/fine) – tích hợp với bank system
    - Thống kê duty time (giờ tổng đã on duty)

    Rank LSPD:
    0 = Cadet       | 1 = Officer
    2 = Corporal    | 3 = Sergeant
    4 = Lieutenant  | 5 = Captain (cao nhất)
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

// Skin theo rank
static const LSPDSkins[] = {
    280,  // Cadet
    281,  // Officer
    282,  // Corporal
    283,  // Sergeant
    284,  // Lieutenant
    285   // Captain
};

// Vũ khí khi on duty (theo rank)
// Cadet: Nightstick
// Officer+: Pistol + Nightstick
// Sergeant+: Shotgun + Pistol + Nightstick
#define LSPD_DUTY_AMMO_PISTOL       (100)
#define LSPD_DUTY_AMMO_NIGHTSTICK   (1)
#define LSPD_DUTY_AMMO_SHOTGUN      (30)

// Phiếu phạt tối đa/tối thiểu
#define LSPD_FINE_MIN   (500)
#define LSPD_FINE_MAX   (50000)

// =============================================================================
// DATA
// =============================================================================

static
    Statement:stmt_loadLSPD,
    Statement:stmt_saveLSPD;

static bool:LSPDOnDuty[MAX_PLAYERS];
static LSPDRank[MAX_PLAYERS];
static LSPDDutySeconds[MAX_PLAYERS];   // Tổng giây on duty session này

// =============================================================================
// HOOKS – MYSQL SETUP
// =============================================================================

hook OnMySQLConnected()
{
    static const loadQ[] = "\
    SELECT \
        lspd_rank, \
        lspd_duty_total \
    FROM \
        player_stats \
    WHERE \
        u_id = ?";

    stmt_loadLSPD = MySQL_PrepareStatement(MySQL_GetHandle(), loadQ);

    static const saveQ[] = "UPDATE player_stats SET lspd_rank = ?, lspd_duty_total = lspd_duty_total + ? WHERE u_id = ?";
    stmt_saveLSPD = MySQL_PrepareStatement(MySQL_GetHandle(), saveQ);

    return 1;
}

// =============================================================================
// HOOKS – PLAYER LIFECYCLE
// =============================================================================

hook OnPlayerConnect(playerid)
{
    LSPDOnDuty[playerid]     = false;
    LSPDRank[playerid]       = 0;
    LSPDDutySeconds[playerid]= 0;
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    // Lưu duty time khi thoát
    if (LSPDOnDuty[playerid])
    {
        LSPD_SaveDutyTime(playerid);
    }
    return 1;
}

hook OnPlayerLogin(playerid)
{
    // Chỉ load LSPD data nếu là cảnh sát
    if (Player_GetClass(playerid) == TEAM_POLICE)
        LSPD_LoadData(playerid);
    return 1;
}

hook OnPlayerSecondUpdate(playerid)
{
    if (LSPDOnDuty[playerid])
        LSPDDutySeconds[playerid]++;
    return 1;
}

// =============================================================================
// INTERNAL – LOAD / SAVE
// =============================================================================

static LSPD_LoadData(playerid)
{
    inline OnLSPDLoad()
    {
        new rank, dutyTotal;
        MySQL_BindResultInt(stmt_loadLSPD, 0, rank);
        MySQL_BindResultInt(stmt_loadLSPD, 1, dutyTotal);

        if (MySQL_Statement_FetchRow(stmt_loadLSPD))
        {
            LSPDRank[playerid] = rank;
        }
    }

    MySQL_BindInt(stmt_loadLSPD, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_loadLSPD, using inline OnLSPDLoad);
}

static LSPD_SaveDutyTime(playerid)
{
    MySQL_BindInt(stmt_saveLSPD, 0, LSPDRank[playerid]);
    MySQL_BindInt(stmt_saveLSPD, 1, LSPDDutySeconds[playerid]);
    MySQL_BindInt(stmt_saveLSPD, 2, Player_GetAccountID(playerid));
    MySQL_ExecuteThreaded(stmt_saveLSPD);
    LSPDDutySeconds[playerid] = 0;
}

// =============================================================================
// INTERNAL – Cấp / Thu trang bị duty
// =============================================================================

static LSPD_GiveEquipment(playerid)
{
    new rank = LSPDRank[playerid];

    // Nightstick (tất cả)
    GivePlayerWeapon(playerid, 3, LSPD_DUTY_AMMO_NIGHTSTICK);

    if (rank >= 1)
    {
        // Pistol
        GivePlayerWeapon(playerid, 22, LSPD_DUTY_AMMO_PISTOL);
    }
    if (rank >= 3)
    {
        // Shotgun
        GivePlayerWeapon(playerid, 25, LSPD_DUTY_AMMO_SHOTGUN);
    }
}

static LSPD_RemoveEquipment(playerid)
{
    ResetPlayerWeapons(playerid);
}

// =============================================================================
// PUBLIC API
// =============================================================================

stock LSPD_IsOnDuty(playerid)
{
    return LSPDOnDuty[playerid];
}

stock LSPD_GetRank(playerid)
{
    return LSPDRank[playerid];
}

stock LSPD_GetRankName(rank, output[], len)
{
    switch (rank) {
        case 0: format(output, len, "Cadet");
        case 1: format(output, len, "Officer");
        case 2: format(output, len, "Corporal");
        case 3: format(output, len, "Sergeant");
        case 4: format(output, len, "Lieutenant");
        case 5: format(output, len, "Captain");
        default: format(output, len, "Khong Xac Dinh");
    }
    return 1;
}

// =============================================================================
// COMMAND – /duty – Bật/Tắt On Duty
// =============================================================================

CMD:duty(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (Player_GetClass(playerid) != TEAM_POLICE)
        return SendErrorMsg(playerid, "Chi si quan Canh Sat moi co the on/off duty."), 1;

    LSPDOnDuty[playerid] = !LSPDOnDuty[playerid];

    if (LSPDOnDuty[playerid])
    {
        // ON DUTY
        new skin = LSPDSkins[LSPDRank[playerid]];
        SetPlayerSkin(playerid, skin);
        LSPD_GiveEquipment(playerid);

        new rankName[24];
        LSPD_GetRankName(LSPDRank[playerid], rankName, sizeof(rankName));
        SendMsgF(playerid, COLOR_BLUE,
            "[LSPD] Ban da len ca duty (Rank: %s). Trang bi da cap phat.",
            rankName);

        // Thông báo kênh bộ đàm
        foreach (new i : Player)
        {
            if (Player_GetClass(i) == TEAM_POLICE)
                SendMsgF(i, COLOR_BLUE, "[CANH SAT RADIO] Si quan %s da bat dau ca truc.", ReturnPlayerName(playerid));
        }
    }
    else
    {
        // OFF DUTY
        LSPD_RemoveEquipment(playerid);
        LSPD_SaveDutyTime(playerid);

        SendMsgF(playerid, COLOR_GREY, "[LSPD] Ban da ket thuc ca truc. Trang bi da thu hoi.");

        foreach (new i : Player)
        {
            if (Player_GetClass(i) == TEAM_POLICE)
                SendMsgF(i, COLOR_GREY, "[CANH SAT RADIO] Si quan %s ket thuc ca truc.", ReturnPlayerName(playerid));
        }
    }
    return 1;
}

// =============================================================================
// COMMAND – /mdc [playerid] – Tra cứu hồ sơ
// =============================================================================

CMD:mdc(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (!LSPD_IsOnDuty(playerid) && Admin_GetPlayerLevel(playerid) < 1)
        return SendErrorMsg(playerid, "Ban phai on duty de su dung MDC."), 1;

    new targetid;
    if (sscanf(params, "u", targetid))
        return SendErrorMsg(playerid, "Su dung: /mdc [playerid]"), 1;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi nay khong ton tai hoac da thoat."), 1;

    if (!Player_IsLoggedIn(targetid))
        return SendErrorMsg(playerid, "Nguoi choi nay chua dang nhap."), 1;

    // Thu thập thông tin
    new targetName[MAX_PLAYER_NAME];
    GetPlayerName(targetid, targetName, MAX_PLAYER_NAME);

    new wantedLevel = Player_GetWantedLevel(targetid);
    new kills       = Player_GetKills(targetid);
    new deaths      = Player_GetDeaths(targetid);
    new level       = Player_GetLevel(targetid);
    new jobID       = Player_GetJob(targetid);

    new hasCarLic     = _:Player_HasLicense(targetid, LICENSE_CAR);
    new hasGunLic     = _:Player_HasLicense(targetid, LICENSE_GUN);
    new hasTruckLic   = _:Player_HasLicense(targetid, LICENSE_TRUCK);
    new hasPlaneLic   = _:Player_HasLicense(targetid, LICENSE_PLANE);

    new content[512];
    format(content, sizeof(content),
        "=== HO SO NHAN THAN ===\n\n\
Ten:       %s (ID: %d)\n\
Level:     %d\n\
Job ID:    %d\n\n\
=== THONG TIN AN NINH ===\n\
Wanted:    %d sao\n\
Kills:     %d | Deaths: %d\n\n\
=== GIAY PHEP ===\n\
Bang lai xe:     %s\n\
Bang lai xe tai: %s\n\
Bang lai may bay:%s\n\
Giay phep sung:  %s",
        targetName, targetid,
        level,
        jobID,
        wantedLevel,
        kills, deaths,
        hasCarLic   ? "{33AA33}CO{FFFFFF}" : "{E85454}KHONG{FFFFFF}",
        hasTruckLic ? "{33AA33}CO{FFFFFF}" : "{E85454}KHONG{FFFFFF}",
        hasPlaneLic ? "{33AA33}CO{FFFFFF}" : "{E85454}KHONG{FFFFFF}",
        hasGunLic   ? "{33AA33}CO{FFFFFF}" : "{E85454}KHONG{FFFFFF}"
    );

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX,
        "MDC – Ho So Nhan Than",
        content, "Dong", "");

    return 1;
}

// =============================================================================
// COMMAND – /fine [playerid] [so_tien] [ly_do] – Phạt tiền IC
// =============================================================================

CMD:lspdfine(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (!LSPD_IsOnDuty(playerid) && Admin_GetPlayerLevel(playerid) < 4)
        return SendErrorMsg(playerid, "Ban phai on duty de phat tien."), 1;

    new targetid, amount;
    new reason[64];

    if (sscanf(params, "uis[64]", targetid, amount, reason))
        return SendErrorMsg(playerid, "Su dung: /lspdfine [playerid] [so_tien] [ly do]"), 1;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai hoac da thoat."), 1;

    if (amount < LSPD_FINE_MIN || amount > LSPD_FINE_MAX)
        return SendErrorMsgF(playerid, "So tien phat phai tu $%d den $%d.", LSPD_FINE_MIN, LSPD_FINE_MAX), 1;

    // Trừ từ tiền mặt trước, nếu không đủ thì trừ bank
    if (GetPlayerMoney(targetid) >= amount)
        GivePlayerMoney(targetid, -amount);
    else if (Player_GetBankMoney(targetid) >= amount)
        Player_RemoveBankMoney(targetid, amount);
    else
    {
        // Không đủ tiền – vẫn phạt (trừ về 0)
        GivePlayerMoney(targetid, -GetPlayerMoney(targetid));
    }

    SendMsgF(playerid, COLOR_BLUE,
        "[LSPD] Da phat %s $%d. Ly do: %s", ReturnPlayerName(targetid), amount, reason);

    SendMsgF(targetid, COLOR_RED,
        "[CANH SAT] Si quan %s da phat ban $%d. Ly do: %s",
        ReturnPlayerName(playerid), amount, reason);

    return 1;
}

ALTCMD:ticket(lspdfine);

// Alias /setrankLSPD (admin only)
ACMD:[4]setrank(playerid, params[])
{
    new targetid, rank;
    if (sscanf(params, "ui", targetid, rank))
        return SendErrorMsg(playerid, "Su dung: /setrank [playerid] [rank 0-5]"), 1;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai."), 1;

    if (rank < 0 || rank > 5)
        return SendErrorMsg(playerid, "Rank phai tu 0 den 5."), 1;

    LSPDRank[targetid] = rank;

    new rankName[24];
    LSPD_GetRankName(rank, rankName, sizeof(rankName));

    SendMsgF(playerid, COLOR_GREEN, "Da thang cap %s len rank %s.",
        ReturnPlayerName(targetid), rankName);
    SendMsgF(targetid, COLOR_GREEN, "Ban da duoc thang cap len rank %s.",
        rankName);
    return 1;
}
