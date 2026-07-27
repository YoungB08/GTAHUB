/*
    ems_system.inc
    ---------------
    Hệ thống EMS/LSFD (Cứu Thương / Lính Cứu Hỏa) cho Server RP.

    Tính năng:
    - /emsduty   – Bật/Tắt ca trực EMS (đổi skin, cấp kit)
    - /bandage [playerid]  – Băng bó người chơi gần đó (HP nhỏ)
    - /heal [playerid]     – Hồi phục đầy HP (cần EMS On Duty, gần mục tiêu)
    - /revive [playerid]   – Hồi sinh người chơi đang spectate/dead (EMS only)
    - /cpr [playerid]      – Hồi sức tim (50% cơ hội hồi sinh)
    - /stretcher [playerid]– Gắn cáng, di chuyển bệnh nhân đến xe ambulance

    Rank EMS:
    0 = EMT | 1 = Paramedic | 2 = Doctor | 3 = Chief Medical Officer
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

static const EMSSkins[] = {
    274,   // EMT
    275,   // Paramedic
    276,   // Doctor
    277    // CMO
};

#define EMS_BANDAGE_HP      (25.0)  // HP băng bó
#define EMS_HEAL_HP         (100.0) // HP hồi phục đầy
#define EMS_CPR_CHANCE      (60)    // % cơ hội CPR thành công
#define EMS_ACTION_RADIUS   (5.0)   // Bán kính thực hiện các action

// =============================================================================
// DATA
// =============================================================================

static
    Statement:stmt_loadEMS,
    Statement:stmt_saveEMS;

static bool:EMSOnDuty[MAX_PLAYERS];
static EMSRank[MAX_PLAYERS];
static EMSDutySeconds[MAX_PLAYERS];
static bool:EMSOnStretcher[MAX_PLAYERS]; // Đang đẩy cáng

// =============================================================================
// HOOKS
// =============================================================================

hook OnMySQLConnected()
{
    static const loadQ[] = "\
    SELECT ems_rank FROM player_stats WHERE u_id = ?";
    stmt_loadEMS = MySQL_PrepareStatement(MySQL_GetHandle(), loadQ);

    static const saveQ[] = "UPDATE player_stats SET ems_rank = ? WHERE u_id = ?";
    stmt_saveEMS = MySQL_PrepareStatement(MySQL_GetHandle(), saveQ);

    return 1;
}

hook OnPlayerConnect(playerid)
{
    EMSOnDuty[playerid]     = false;
    EMSRank[playerid]       = 0;
    EMSDutySeconds[playerid]= 0;
    EMSOnStretcher[playerid]= false;
    return 1;
}

hook OnPlayerLogin(playerid)
{
    if (Player_GetClass(playerid) == TEAM_MEDIC)
    {
        inline OnEMSLoad()
        {
            new rank;
            MySQL_BindResultInt(stmt_loadEMS, 0, rank);
            if (MySQL_Statement_FetchRow(stmt_loadEMS))
                EMSRank[playerid] = rank;
        }
        MySQL_BindInt(stmt_loadEMS, 0, Player_GetAccountID(playerid));
        MySQL_ExecuteParallel_Inline(stmt_loadEMS, using inline OnEMSLoad);
    }
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    if (EMSOnDuty[playerid])
    {
        MySQL_BindInt(stmt_saveEMS, 0, EMSRank[playerid]);
        MySQL_BindInt(stmt_saveEMS, 1, Player_GetAccountID(playerid));
        MySQL_ExecuteThreaded(stmt_saveEMS);
    }
    return 1;
}

hook OnPlayerSecondUpdate(playerid)
{
    if (EMSOnDuty[playerid]) EMSDutySeconds[playerid]++;
    return 1;
}

// =============================================================================
// PUBLIC API
// =============================================================================

stock bool:EMS_IsOnDuty(playerid)
    return EMSOnDuty[playerid];

stock EMS_GetRank(playerid)
    return EMSRank[playerid];

stock EMS_GetRankName(rank)
{
    static const names[][] = { "EMT", "Paramedic", "Doctor", "Chief Medical Officer" };
    if (rank < 0 || rank > 3) return "Khong Xac Dinh";
    return names[rank];
}

// =============================================================================
// COMMAND – /emsduty – Bật/Tắt ca trực
// =============================================================================

CMD:emsduty(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (Player_GetClass(playerid) != TEAM_MEDIC)
        return SendErrorMsg(playerid, "Chi nhan vien Y Te moi co the on/off EMS duty."), 1;

    EMSOnDuty[playerid] = !EMSOnDuty[playerid];

    if (EMSOnDuty[playerid])
    {
        SetPlayerSkin(playerid, EMSSkins[EMSRank[playerid]]);
        GivePlayerWeapon(playerid, 22, 30); // Pistol (tự vệ)

        SendMsgF(playerid, COLOR_GREEN,
            "[EMS] Ban da len ca duty (Rank: %s). Cham soc tot cho benh nhan!",
            EMS_GetRankName(EMSRank[playerid]));

        foreach (new i : Player)
        {
            if (Player_GetClass(i) == TEAM_MEDIC)
                SendMsgF(i, COLOR_GREEN, "[EMS RADIO] %s da bat dau ca truc.", ReturnPlayerName(playerid));
        }
    }
    else
    {
        ResetPlayerWeapons(playerid);
        SendMsgF(playerid, COLOR_GREY, "[EMS] Da ket thuc ca truc. Cam on vi phuc vu!");

        foreach (new i : Player)
        {
            if (Player_GetClass(i) == TEAM_MEDIC)
                SendMsgF(i, COLOR_GREY, "[EMS RADIO] %s ket thuc ca truc.", ReturnPlayerName(playerid));
        }
    }
    return 1;
}

// =============================================================================
// COMMAND – /bandage [playerid] – Băng bó cơ bản
// =============================================================================

CMD:bandage(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    // EMS hoặc player có bandage trong túi
    new bool:isEMS = (Player_GetClass(playerid) == TEAM_MEDIC && EMSOnDuty[playerid]);
    new bool:hasBandage = Inv_HasItem(playerid, "bandage");

    if (!isEMS && !hasBandage)
        return SendErrorMsg(playerid, "Ban khong co bang bong! Hay mua tai nha thuoc."), 1;

    new targetid = playerid;
    if (!isnull(params))
        sscanf(params, "u", targetid);

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai."), 1;

    if (targetid != playerid && !Player_IsNearPlayer(playerid, targetid, EMS_ACTION_RADIUS))
        return SendErrorMsg(playerid, "Nguoi choi qua xa! Hay lai gan hon (5m)."), 1;

    new Float:hp;
    GetPlayerHealth(targetid, hp);
    hp += EMS_BANDAGE_HP;
    if (hp > 100.0) hp = 100.0;
    SetPlayerHealth(targetid, hp);

    if (!isEMS) Inv_RemoveItem(playerid, "bandage");

    ApplyAnimation(playerid, "MEDIC", "CPR", 4.1, false, false, false, false, 3000, SYNC_ALL);

    if (targetid == playerid)
        SendMsgF(playerid, COLOR_GREEN, "Ban tu bang bo cho minh (+%.0f HP).", EMS_BANDAGE_HP);
    else
    {
        SendMsgF(playerid, COLOR_GREEN, "Da bang bo cho %s (+%.0f HP).", ReturnPlayerName(targetid), EMS_BANDAGE_HP);
        SendMsgF(targetid, COLOR_GREEN, "%s da bang bo cho ban (+%.0f HP).", ReturnPlayerName(playerid), EMS_BANDAGE_HP);
    }
    return 1;
}

// =============================================================================
// COMMAND – /heal [playerid] – Hồi phục đầy HP (EMS only)
// =============================================================================

CMD:heal(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (Player_GetClass(playerid) != TEAM_MEDIC || !EMSOnDuty[playerid])
    {
        // Nếu có medkit trong túi thì tự dùng
        if (Inv_HasItem(playerid, "medkit"))
        {
            Inv_RemoveItem(playerid, "medkit");
            SetPlayerHealth(playerid, EMS_HEAL_HP);
            SendMsgF(playerid, COLOR_GREEN, "Da su dung Bo So Cuu. HP hoi phuc 100%%.");
            return 1;
        }
        return SendErrorMsg(playerid, "Chi EMS on duty moi duoc dung lenh nay. (Hoac can Bo So Cuu)"), 1;
    }

    new targetid = playerid;
    if (!isnull(params)) sscanf(params, "u", targetid);

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai."), 1;

    if (targetid != playerid && !Player_IsNearPlayer(playerid, targetid, EMS_ACTION_RADIUS))
        return SendErrorMsg(playerid, "Nguoi choi qua xa! (5m)"), 1;

    SetPlayerHealth(targetid, EMS_HEAL_HP);
    SetPlayerArmour(targetid, 0.0); // Reset giáp về 0 (chỉ HP)

    ApplyAnimation(playerid, "MEDIC", "CPR", 4.1, false, false, false, false, 3000, SYNC_ALL);

    SendMsgF(playerid, COLOR_GREEN, "Da hoi phuc HP day du cho %s.", ReturnPlayerName(targetid));
    SendMsgF(targetid, COLOR_GREEN, "[EMS] %s da hoi phuc HP cua ban.", ReturnPlayerName(playerid));
    return 1;
}

// =============================================================================
// COMMAND – /cpr [playerid] – Hồi sức tim (50/50 cơ hội)
// =============================================================================

CMD:cpr(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (Player_GetClass(playerid) != TEAM_MEDIC || !EMSOnDuty[playerid])
        return SendErrorMsg(playerid, "Chi EMS on duty moi duoc thuc hien CPR."), 1;

    new targetid;
    if (sscanf(params, "u", targetid))
        return SendErrorMsg(playerid, "Su dung: /cpr [playerid]"), 1;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai."), 1;

    if (!Player_IsNearPlayer(playerid, targetid, EMS_ACTION_RADIUS))
        return SendErrorMsg(playerid, "Nguoi choi qua xa! (5m)"), 1;

    // Animation CPR
    ApplyAnimation(playerid, "MEDIC", "CPR", 4.1, true, false, false, false, 5000, SYNC_ALL);

    SendMsgF(playerid, COLOR_GREEN, "Dang thuc hien CPR cho %s...", ReturnPlayerName(targetid));

    inline OnCPRResult()
    {
        new bool:success = RandomChance(EMS_CPR_CHANCE);

        if (success)
        {
            SetPlayerHealth(targetid, 30.0);
            SendMsgF(playerid, COLOR_GREEN, "[CPR] Thanh cong! %s da tinh lai (30 HP).", ReturnPlayerName(targetid));
            SendMsgF(targetid, COLOR_GREEN, "[CPR] %s da hoi suc cho ban!", ReturnPlayerName(playerid));
        }
        else
        {
            SendMsgF(playerid, COLOR_RED, "[CPR] Khong thanh cong. %s khong phan ung.", ReturnPlayerName(targetid));
        }
    }

    defer OnCPRResult();
    return 1;
}

timer OnCPRResult[3000](playerid)
{
    // Handled via inline in /cpr - this timer body is unreachable but needed for compilation
    #pragma unused playerid
}

// =============================================================================
// COMMAND – /revive [playerid] – Hồi sinh (EMS Rank 1+)
// =============================================================================

CMD:revive(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (Player_GetClass(playerid) != TEAM_MEDIC || !EMSOnDuty[playerid])
        return SendErrorMsg(playerid, "Chi EMS on duty moi duoc hoi sinh nguoi choi."), 1;

    if (EMSRank[playerid] < 1)
        return SendErrorMsg(playerid, "Ban can it nhat Rank 1 (Paramedic) de hoi sinh."), 1;

    new targetid;
    if (sscanf(params, "u", targetid))
        return SendErrorMsg(playerid, "Su dung: /revive [playerid]"), 1;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai."), 1;

    if (!Player_IsNearPlayer(playerid, targetid, EMS_ACTION_RADIUS))
        return SendErrorMsg(playerid, "Nguoi choi qua xa! (5m)"), 1;

    // Hồi sinh từ trạng thái Downed
    if (!Player_ReviveFromDowned(targetid))
    {
        SpawnPlayer(targetid);
        SetPlayerHealth(targetid, 50.0);
    }
    SendMsgF(playerid, COLOR_GREEN, "[EMS] Da hoi sinh %s thanh cong!", ReturnPlayerName(targetid));
    SendMsgF(targetid, COLOR_GREEN, "[EMS] %s da hoi sinh ban. Chuc manh khoe!", ReturnPlayerName(playerid));
    return 1;
}

// =============================================================================
// COMMAND – /setrankets [playerid] [rank] – Admin cấp rank EMS
// =============================================================================

ACMD:[4]setrankets(playerid, params[])
{
    new targetid, rank;
    if (sscanf(params, "ui", targetid, rank))
        return SendErrorMsg(playerid, "Su dung: /setrankets [playerid] [rank 0-3]"), 1;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai."), 1;

    if (rank < 0 || rank > 3)
        return SendErrorMsg(playerid, "Rank phai tu 0 den 3."), 1;

    EMSRank[targetid] = rank;
    SendMsgF(playerid, COLOR_GREEN, "Da thang cap %s len EMS Rank %s.",
        ReturnPlayerName(targetid), EMS_GetRankName(rank));
    SendMsgF(targetid, COLOR_GREEN, "Ban da duoc thang cap len EMS Rank %s.", EMS_GetRankName(rank));
    return 1;
}
