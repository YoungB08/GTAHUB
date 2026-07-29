/*
    turf_system.inc
    ---------------
    Hệ thống Chiếm Đánh Địa Bàn (Turf War System).

    Tính năng:
    - Load turf từ DB, hiển thị trên map bằng GangZone.
    - /turfinfo – Xem thông tin turf hiện tại đang đứng.
    - /attackturf – Bắt đầu chiếm turf (yêu cầu 3+ thành viên gang cùng khu vực).
    - Thu thuế địa bàn: Mỗi 30 phút, gang có turf nhận thuế thu nhập.
    - Chiếm turf: Timer 60 giây, phía bên có nhiều người hơn thắng.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define MAX_TURFS               (10)
#define TURF_CAPTURE_TIME       (60)    // Giây để chiếm turf
#define TURF_MIN_MEMBERS        (2)     // Số thành viên tối thiểu để attack
#define TURF_TAX_INTERVAL       (1800)  // Giây – thuế địa bàn (30 phút)
#define TURF_TAX_PER_TURF       (5000)  // $ / turf / lần thu thuế
#define TURF_CAPTURE_COOLDOWN   (300)   // Giây – thời gian chờ sau khi chiếm xong

// Màu gang zones
#define TURF_COLOR_NEUTRAL      (0x80808060)
#define TURF_COLOR_CAPTURE_ING  (0xFFFF0080)

// =============================================================================
// DATA
// =============================================================================

enum E_TURF_DATA {
    E_TURF_ID,
    E_TURF_NAME[64],
    Float:E_TURF_X,
    Float:E_TURF_Y,
    Float:E_TURF_Z,
    Float:E_TURF_RADIUS,
    E_TURF_GANG_ID,           // -1 = neutral
    E_TURF_ZONE,              // GangZone ID
    bool:E_TURF_IS_UNDER_ATTACK,
    E_TURF_ATTACK_TIMER,      // Giây còn lại để chiếm
    E_TURF_ATTACKER_GANG,     // Gang đang chiếm
    E_TURF_TAX_TIMER          // Timer thu thuế
}

static TurfData[MAX_TURFS][E_TURF_DATA];
static TurfCount = 0;

static Statement:stmt_loadTurfs;
static Statement:stmt_saveTurf;

// Tax timer global
static TurfTaxTimer;

// =============================================================================
// HOOKS – MYSQL SETUP
// =============================================================================

hook OnMySQLConnected()
{
    mysql_query_file(MySQL_GetHandle(), "turfs.sql", false);

    static const loadQ[] = "\
    SELECT \
        turf_id, name, x, y, z, radius, gang_id \
    FROM \
        turfs \
    ORDER BY \
        turf_id ASC";

    stmt_loadTurfs = MySQL_PrepareStatement(MySQL_GetHandle(), loadQ);

    static const saveQ[] = "UPDATE turfs SET gang_id = ? WHERE turf_id = ?";
    stmt_saveTurf = MySQL_PrepareStatement(MySQL_GetHandle(), saveQ);

    return 1;
}

// =============================================================================
// LOAD TURFS ON GAMEMODE INIT
// =============================================================================

hook OnGameModeInit()
{
    Turf_LoadAll();
    return 1;
}

static Turf_LoadAll()
{
    inline OnTurfLoadRow()
    {
        if (TurfCount >= MAX_TURFS) return;

        new turf_id, name[64], gang_id;
        new Float:x, Float:y, Float:z, Float:radius;

        MySQL_BindResultInt(stmt_loadTurfs,   0, turf_id);
        MySQL_BindResultString(stmt_loadTurfs, 1, name, 64);
        MySQL_BindResultFloat(stmt_loadTurfs, 2, x);
        MySQL_BindResultFloat(stmt_loadTurfs, 3, y);
        MySQL_BindResultFloat(stmt_loadTurfs, 4, z);
        MySQL_BindResultFloat(stmt_loadTurfs, 5, radius);
        MySQL_BindResultInt(stmt_loadTurfs,   6, gang_id);

        while (MySQL_Statement_FetchRow(stmt_loadTurfs) && TurfCount < MAX_TURFS)
        {
            new i = TurfCount;
            TurfData[i][E_TURF_ID]        = turf_id;
            strcopy(TurfData[i][E_TURF_NAME], name, 64);
            TurfData[i][E_TURF_X]         = x;
            TurfData[i][E_TURF_Y]         = y;
            TurfData[i][E_TURF_Z]         = z;
            TurfData[i][E_TURF_RADIUS]    = radius;
            TurfData[i][E_TURF_GANG_ID]   = (gang_id == 0) ? -1 : gang_id;
            TurfData[i][E_TURF_IS_UNDER_ATTACK] = false;
            TurfData[i][E_TURF_ATTACK_TIMER]    = 0;
            TurfData[i][E_TURF_ATTACKER_GANG]   = -1;

            // Tạo GangZone
            TurfData[i][E_TURF_ZONE] = GangZoneCreate(
                x - radius, y - radius, x + radius, y + radius
            );

            // Màu hiển thị
            if (TurfData[i][E_TURF_GANG_ID] == -1)
                GangZoneShowForAll(TurfData[i][E_TURF_ZONE], TURF_COLOR_NEUTRAL);
            else
                GangZoneShowForAll(TurfData[i][E_TURF_ZONE], Gang_GetColor(TurfData[i][E_TURF_GANG_ID]));

            Logger_Log("[turf_system.inc] Loaded turf",
                Logger_S("name", name),
                Logger_I("gang_id", gang_id)
            );

            TurfCount++;

            // Re-fetch for next row
            MySQL_BindResultInt(stmt_loadTurfs,   0, turf_id);
            MySQL_BindResultString(stmt_loadTurfs, 1, name, 64);
            MySQL_BindResultFloat(stmt_loadTurfs, 2, x);
            MySQL_BindResultFloat(stmt_loadTurfs, 3, y);
            MySQL_BindResultFloat(stmt_loadTurfs, 4, z);
            MySQL_BindResultFloat(stmt_loadTurfs, 5, radius);
            MySQL_BindResultInt(stmt_loadTurfs,   6, gang_id);
        }
    }

    MySQL_ExecuteParallel_Inline(stmt_loadTurfs, using inline OnTurfLoadRow);
}

// =============================================================================
// HELPER – Tìm turf đang đứng
// =============================================================================

static GetPlayerTurf(playerid)
{
    for (new i = 0; i < TurfCount; i++)
    {
        if (IsPlayerInRangeOfPoint(playerid,
            TurfData[i][E_TURF_RADIUS],
            TurfData[i][E_TURF_X],
            TurfData[i][E_TURF_Y],
            TurfData[i][E_TURF_Z]))
        {
            return i;
        }
    }
    return -1;
}

// Đếm số thành viên gang trong turf
static CountGangMembersInTurf(gangid, turfIndex)
{
    new count = 0;
    foreach (new i : Player)
    {
        if (!Player_IsLoggedIn(i)) continue;
        if (Gang_GetPlayerGang(i) != gangid) continue;
        if (GetPlayerTurf(i) != turfIndex) continue;
        count++;
    }
    return count;
}

// =============================================================================
// COMMAND – /turfinfo – Xem thông tin turf đang đứng
// =============================================================================

CMD:turfinfo(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new turfIdx = GetPlayerTurf(playerid);
    if (turfIdx == -1)
        return SendErrorMsg(playerid, "Ban dang khong o trong dia ban nao."), 1;

    new ownerName[32];
    if (TurfData[turfIdx][E_TURF_GANG_ID] == -1)
        strcopy(ownerName, "Trung Lap", sizeof(ownerName));
    else
        Gang_GetName(TurfData[turfIdx][E_TURF_GANG_ID], ownerName, 32);

    new attackStatus[64];
    if (TurfData[turfIdx][E_TURF_IS_UNDER_ATTACK])
        format(attackStatus, sizeof(attackStatus), "DANG BI TAN CONG (%ds con lai)", TurfData[turfIdx][E_TURF_ATTACK_TIMER]);
    else
        strcopy(attackStatus, "Binh thuong", sizeof(attackStatus));

    new content[256];
    format(content, sizeof(content),
        "=== DIA BAN ===\n\n\
Ten:         %s\n\
So huu:      %s\n\
Trang thai:  %s\n\
Thue thu:    $%d / 30 phut",
        TurfData[turfIdx][E_TURF_NAME],
        ownerName,
        attackStatus,
        TURF_TAX_PER_TURF
    );

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Thong Tin Dia Ban", content, "Dong", "");
    return 1;
}

// =============================================================================
// COMMAND – /attackturf – Bắt đầu chiếm địa bàn
// =============================================================================

CMD:attackturf(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new gangid = Gang_GetPlayerGang(playerid);
    if (gangid == -1)
        return SendErrorMsg(playerid, "Ban can o trong 1 bang nhom de tan cong dia ban."), 1;

    new turfIdx = GetPlayerTurf(playerid);
    if (turfIdx == -1)
        return SendErrorMsg(playerid, "Ban phai o trong khu vuc dia ban de tan cong."), 1;

    if (TurfData[turfIdx][E_TURF_GANG_ID] == gangid)
        return SendErrorMsg(playerid, "Day la dia ban cua bang ban roi, khong the tan cong!"), 1;

    if (TurfData[turfIdx][E_TURF_IS_UNDER_ATTACK])
        return SendErrorMsg(playerid, "Dia ban nay dang bi tan cong roi!"), 1;

    new membersInTurf = CountGangMembersInTurf(gangid, turfIdx);
    if (membersInTurf < TURF_MIN_MEMBERS)
        return SendErrorMsgF(playerid, "Can it nhat %d thanh vien gang trong khu vuc de tan cong (hien co: %d).",
            TURF_MIN_MEMBERS, membersInTurf), 1;

    // Bắt đầu chiếm
    TurfData[turfIdx][E_TURF_IS_UNDER_ATTACK] = true;
    TurfData[turfIdx][E_TURF_ATTACK_TIMER]    = TURF_CAPTURE_TIME;
    TurfData[turfIdx][E_TURF_ATTACKER_GANG]   = gangid;

    GangZoneFlashForAll(TurfData[turfIdx][E_TURF_ZONE], TURF_COLOR_CAPTURE_ING);

    new gangName[32];
    Gang_GetName(gangid, gangName, 32);

    // Thông báo toàn server
    SendMsgAllF(COLOR_RED, "[DIA BAN] Bang %s dang tan cong dia ban '%s'! (%d giay)",
        gangName, TurfData[turfIdx][E_TURF_NAME], TURF_CAPTURE_TIME);

    return 1;
}

// =============================================================================
// HOOK – Xử lý turf capture timer mỗi giây
// =============================================================================

hook OnGameModeSecondUpdate()
{
    // Thu thuế địa bàn
    TurfTaxTimer++;
    if (TurfTaxTimer >= TURF_TAX_INTERVAL)
    {
        TurfTaxTimer = 0;
        Turf_CollectTax();
    }

    // Xử lý các turf đang bị tấn công
    for (new i = 0; i < TurfCount; i++)
    {
        if (!TurfData[i][E_TURF_IS_UNDER_ATTACK]) continue;

        TurfData[i][E_TURF_ATTACK_TIMER]--;

        if (TurfData[i][E_TURF_ATTACK_TIMER] <= 0)
        {
            // Hết giờ – xác định người thắng
            Turf_ResolveCapture(i);
        }
    }
    return 1;
}

static Turf_ResolveCapture(turfIdx)
{
    new attackerGang = TurfData[turfIdx][E_TURF_ATTACKER_GANG];
    new defenderGang = TurfData[turfIdx][E_TURF_GANG_ID];

    new attackers = CountGangMembersInTurf(attackerGang, turfIdx);
    new defenders = (defenderGang == -1) ? 0 : CountGangMembersInTurf(defenderGang, turfIdx);

    TurfData[turfIdx][E_TURF_IS_UNDER_ATTACK] = false;
    TurfData[turfIdx][E_TURF_ATTACK_TIMER]    = 0;
    TurfData[turfIdx][E_TURF_ATTACKER_GANG]   = -1;

    new gangName[32];
    GangZoneStopFlashForAll(TurfData[turfIdx][E_TURF_ZONE]);

    if (attackers > defenders)
    {
        // Bên tấn công thắng
        TurfData[turfIdx][E_TURF_GANG_ID] = attackerGang;
        Gang_GetName(attackerGang, gangName, 32);
        GangZoneShowForAll(TurfData[turfIdx][E_TURF_ZONE], Gang_GetColor(attackerGang));

        SendMsgAllF(COLOR_RED, "[DIA BAN] Bang %s da CHIEM DUOC dia ban '%s'!",
            gangName, TurfData[turfIdx][E_TURF_NAME]);

        // Lưu DB
        MySQL_BindInt(stmt_saveTurf, 0, attackerGang);
        MySQL_BindInt(stmt_saveTurf, 1, TurfData[turfIdx][E_TURF_ID]);
        MySQL_ExecuteThreaded(stmt_saveTurf);
    }
    else
    {
        // Bên phòng thủ giữ được
        if (defenderGang != -1)
        {
            Gang_GetName(defenderGang, gangName, 32);
            GangZoneShowForAll(TurfData[turfIdx][E_TURF_ZONE], Gang_GetColor(defenderGang));
        }
        else
        {
            gangName = "Khong co chu";
            GangZoneShowForAll(TurfData[turfIdx][E_TURF_ZONE], TURF_COLOR_NEUTRAL);
        }

        SendMsgAllF(COLOR_GREEN, "[DIA BAN] Dia ban '%s' da duoc bao ve thanh cong boi %s!",
            TurfData[turfIdx][E_TURF_NAME], gangName);
    }
}

static Turf_CollectTax()
{
    for (new i = 0; i < TurfCount; i++)
    {
        if (TurfData[i][E_TURF_GANG_ID] == -1) continue;

        // Phân phối thuế cho các thành viên gang đang online
        new gangid = TurfData[i][E_TURF_GANG_ID];
        new memberCount = 0;

        foreach (new j : Player)
        {
            if (!Player_IsLoggedIn(j)) continue;
            if (Gang_GetPlayerGang(j) != gangid) continue;
            memberCount++;
        }

        if (memberCount == 0) continue;

        new sharePerMember = TURF_TAX_PER_TURF / memberCount;

        foreach (new j : Player)
        {
            if (!Player_IsLoggedIn(j)) continue;
            if (Gang_GetPlayerGang(j) != gangid) continue;
            GivePlayerMoney(j, sharePerMember);
            SendMsgF(j, COLOR_GREEN, "[DIA BAN] Nhan thue dia ban '%s': $%d",
                TurfData[i][E_TURF_NAME], sharePerMember);
        }
    }
}
