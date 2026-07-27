/*
    cmds_admin_rp.inc
    -----------------
    Công cụ Admin RP: Ticket/Support System.

    Lệnh player:
    /report [ly do]   – Gửi ticket hỗ trợ tới admin
    /reports          – Admin: Xem danh sách ticket đang mở
    /closereport [id] – Admin: Đóng / giải quyết ticket
    /warn [playerid] [ly do] – Admin: Cảnh cáo OOC (tích lũy 3 = ban)
    /warns [playerid]        – Xem cảnh cáo của người chơi
    /aduty            – Admin: Toggle chế độ Admin Duty (OOC)
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define MAX_REPORTS         (50)
#define MAX_WARNS           (3)     // Bao nhiêu warn thì ban
#define WARN_BAN_DURATION   (1440)  // Phút – ban khi đủ warn (24h)

// =============================================================================
// DATA
// =============================================================================

enum E_REPORT {
    E_RPT_ID,
    E_RPT_PLAYER,
    E_RPT_NAME[MAX_PLAYER_NAME],
    E_RPT_REASON[128],
    E_RPT_TIME,
    bool:E_RPT_OPEN
}

static ReportList[MAX_REPORTS][E_REPORT];
static ReportCount = 0;
static NextReportID = 1;

// Warns tích lũy
static PlayerWarnCount[MAX_PLAYERS];
static bool:AdminDuty[MAX_PLAYERS];

// =============================================================================
// HOOKS
// =============================================================================

hook OnPlayerConnect(playerid)
{
    PlayerWarnCount[playerid] = 0;
    AdminDuty[playerid] = false;
    return 1;
}

// =============================================================================
// COMMAND – /report [ly do]
// =============================================================================

CMD:report(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /report [mo ta van de]"), 1;

    if (ReportCount >= MAX_REPORTS)
        return SendErrorMsg(playerid, "He thong ticket dang qua tai. Vui long thu lai sau."), 1;

    new idx = ReportCount;
    ReportList[idx][E_RPT_ID]     = NextReportID++;
    ReportList[idx][E_RPT_PLAYER] = playerid;
    GetPlayerName(playerid, ReportList[idx][E_RPT_NAME], MAX_PLAYER_NAME);
    strcopy(ReportList[idx][E_RPT_REASON], params, 128);
    ReportList[idx][E_RPT_TIME]   = gettime();
    ReportList[idx][E_RPT_OPEN]   = true;
    ReportCount++;

    SendMsgF(playerid, COLOR_GREEN,
        "[REPORT #%d] Da gui bao cao. Admin se xu ly som. Vui long doi!", ReportList[idx][E_RPT_ID]);

    // Thông báo tất cả admin online
    foreach (new i : Player)
    {
        if (Admin_GetPlayerLevel(i) < 1) continue;
        SendMsgF(i, COLOR_MODERATION,
            "[REPORT #%d] Tu: %s (ID:%d) | %s",
            ReportList[idx][E_RPT_ID], ReportList[idx][E_RPT_NAME], playerid, params);
    }

    return 1;
}

// =============================================================================
// COMMAND – /reports – Admin xem ticket
// =============================================================================

CMD:reports(playerid, params[])
{
    if (Admin_GetPlayerLevel(playerid) < 1)
        return SendErrorMsg(playerid, "Chi admin moi duoc xem reports."), 1;

    if (ReportCount == 0)
        return SendMsgF(playerid, COLOR_GREEN, "Khong co report nao dang mo."), 1;

    new content[1024], line[128];
    new openCount = 0;

    for (new i = 0; i < ReportCount; i++)
    {
        if (!ReportList[i][E_RPT_OPEN]) continue;

        format(line, sizeof(line), "#%d | %s (ID:%d) | %s\n",
            ReportList[i][E_RPT_ID],
            ReportList[i][E_RPT_NAME],
            ReportList[i][E_RPT_PLAYER],
            ReportList[i][E_RPT_REASON]);
        strcat(content, line, sizeof(content));
        openCount++;
    }

    if (openCount == 0)
    {
        SendMsgF(playerid, COLOR_GREEN, "Tat ca reports da duoc xu ly.");
        return 1;
    }

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX,
        "Danh Sach Reports Dang Mo",
        content, "Dong", "");
    return 1;
}

// =============================================================================
// COMMAND – /closereport [id]
// =============================================================================

CMD:closereport(playerid, params[])
{
    if (Admin_GetPlayerLevel(playerid) < 1)
        return SendErrorMsg(playerid, "Chi admin moi co the dong report."), 1;

    new reportID;
    if (sscanf(params, "i", reportID))
        return SendErrorMsg(playerid, "Su dung: /closereport [report_id]"), 1;

    for (new i = 0; i < ReportCount; i++)
    {
        if (ReportList[i][E_RPT_ID] != reportID) continue;
        if (!ReportList[i][E_RPT_OPEN])
        {
            SendErrorMsg(playerid, "Report nay da duoc xu ly roi.");
            return 1;
        }

        ReportList[i][E_RPT_OPEN] = false;

        SendMsgF(playerid, COLOR_GREEN, "[REPORT #%d] Da dong boi ban.", reportID);

        // Thông báo cho người gửi
        new rptPlayer = ReportList[i][E_RPT_PLAYER];
        if (IsPlayerConnected(rptPlayer))
            SendMsgF(rptPlayer, COLOR_GREEN, "[REPORT #%d] Da duoc admin %s xu ly.",
                reportID, ReturnPlayerName(playerid));

        return 1;
    }

    SendErrorMsg(playerid, "Khong tim thay report voi ID do.");
    return 1;
}

// =============================================================================
// COMMAND – /warn [playerid] [ly do]
// =============================================================================

CMD:warn(playerid, params[])
{
    if (Admin_GetPlayerLevel(playerid) < 1)
        return SendErrorMsg(playerid, "Chi admin moi co the warn."), 1;

    new targetid;
    new reason[64];
    if (sscanf(params, "us[64]", targetid, reason))
        return SendErrorMsg(playerid, "Su dung: /warn [playerid] [ly do]"), 1;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai hoac da thoat."), 1;

    PlayerWarnCount[targetid]++;

    new warnNum = PlayerWarnCount[targetid];

    // Thông báo
    SendMsgF(playerid, COLOR_ORANGE, "[WARN] Da canh cao %s (Warn %d/%d). Ly do: %s",
        ReturnPlayerName(targetid), warnNum, MAX_WARNS, reason);

    SendMsgF(targetid, COLOR_RED,
        "[CANH CAO OOC] Admin %s da canh cao ban (Warn %d/%d). Ly do: %s",
        ReturnPlayerName(playerid), warnNum, MAX_WARNS, reason);

    if (warnNum >= MAX_WARNS)
    {
        // Ban
        SendMsgAllF(COLOR_RED,
            "[BAN] %s (ID:%d) da bi ban %d phut vi du %d warns. Ly do: %s",
            ReturnPlayerName(targetid), targetid, WARN_BAN_DURATION, MAX_WARNS, reason);

        // TODO: Gọi hệ thống ban: Ban_AddPlayer(targetid, WARN_BAN_DURATION, reason);
        Kick(targetid);
    }
    return 1;
}

// =============================================================================
// COMMAND – /warns [playerid] – Xem cảnh cáo
// =============================================================================

CMD:warns(playerid, params[])
{
    new targetid;
    if (sscanf(params, "u", targetid))
        targetid = playerid;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai hoac da thoat."), 1;

    if (targetid != playerid && Admin_GetPlayerLevel(playerid) < 1)
        return SendErrorMsg(playerid, "Ban khong co quyen xem warn cua nguoi khac."), 1;

    SendMsgF(playerid, COLOR_ORANGE, "[WARNS] %s: %d/%d canh cao.",
        ReturnPlayerName(targetid), PlayerWarnCount[targetid], MAX_WARNS);
    return 1;
}

// =============================================================================
// COMMAND – /aduty – Toggle Admin Duty Mode
// =============================================================================

CMD:aduty(playerid, params[])
{
    if (Admin_GetPlayerLevel(playerid) < 1)
        return SendErrorMsg(playerid, "Chi admin moi co the dung lenh nay."), 1;

    AdminDuty[playerid] = !AdminDuty[playerid];

    if (AdminDuty[playerid])
    {
        SetPlayerColor(playerid, 0xFF000099);   // Màu đỏ khi aduty
        SendMsgF(playerid, COLOR_MODERATION, "[ADMIN] Ban da bat che do Admin Duty (OOC). Nguoi choi thay ban mau do.");
    }
    else
    {
        // Khôi phục màu bình thường
        SetPlayerColor(playerid, GetPlayerColor(playerid));
        SendMsgF(playerid, COLOR_GREY, "[ADMIN] Da tat Admin Duty.");
    }
    return 1;
}
