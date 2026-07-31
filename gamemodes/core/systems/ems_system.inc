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

const
    HOSPITAL_CURE_PRICE  = 1_500,
    HOSPITAL_HEAL_PRICE  = 500,
    HOSPITAL_SKIN_CHANGE = 10_000;

static bool:BoughtChangeSkin[MAX_PLAYERS];

hook OnMySQLConnected()
{
    static const loadQ[] = "\
    SELECT ems_rank FROM player_stats WHERE u_id = ?";
    stmt_loadEMS = MySQL_PrepareStatement(MySQL_GetHandle(), loadQ);

    static const saveQ[] = "UPDATE player_stats SET ems_rank = ? WHERE u_id = ?";
    stmt_saveEMS = MySQL_PrepareStatement(MySQL_GetHandle(), saveQ);
    return 1;
}

hook OnGameModeInit()
{
    Create3DTextLabel("Nhan "C_YELLOW"Y"C_WHITE" de xem dich vu Benh Vien", COLOR_WHITE, 1173.1469, -1323.4918, 16.3958, 40.0, 0, false);
    CreateActor(70, 1173.1469, -1323.4918, 15.3958, 270.2290);
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if (PRESSED(KEY_YES))
    {
        if (IsPlayerInRangeOfPoint(playerid, 5.0, 1173.1469, -1323.4918, 15.3958))
        {
            ShowHospitalMenu(playerid);
        }
    }
    return 1;
}

stock ShowHospitalMenu(playerid)
{
    inline const _response(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext
        if(!response) return 1;

        switch(listitem)
        {
            case 0:
            {
                new Float:health;
                GetPlayerHealth(playerid, health);

                if (GetPlayerMoney(playerid) < HOSPITAL_HEAL_PRICE)
                    return SendErrorMsg(playerid, "Khong du tien!");

                if (health >= 100.0)
                    return SendErrorMsg(playerid, "Mau cua ban da day, hien khong the hoi mau.");

                SetPlayerHealth(playerid, 100);
                GivePlayerMoney(playerid, -HOSPITAL_HEAL_PRICE);
                SendMsgF(playerid, COLOR_GREEN, "[BENH VIEN] Ban da duoc hoi phuc day mau voi gia $%d.", HOSPITAL_HEAL_PRICE);
            }
            case 1:
            {
                if (GetPlayerMoney(playerid) < HOSPITAL_SKIN_CHANGE)
                    return SendErrorMsg(playerid, "Khong du tien!");

                if (GetPlayerWantedLevel(playerid) != 0)
                    return SendErrorMsg(playerid, "Ban phai khong bi truy na de doi skin!");

                BoughtChangeSkin[playerid] = true;
                Class_SelectionState(playerid, true);

                ForceClassSelection(playerid);
                TogglePlayerSpectating(playerid, true);
                TogglePlayerSpectating(playerid, false);

                GivePlayerMoney(playerid, -HOSPITAL_SKIN_CHANGE);
            }
            case 2:
            {
                if (GetPlayerMoney(playerid) < HOSPITAL_CURE_PRICE)
                    return SendErrorMsg(playerid, "Khong du tien!");

                if (!Disease_PlayerHas(playerid))
                    return SendErrorMsg(playerid, "Ban khong mac benh nao.");

                Disease_GivePlayer(playerid, DISEASE_NONE);
                GivePlayerMoney(playerid, -HOSPITAL_CURE_PRICE);
                SendMsgF(playerid, COLOR_GREEN, "[BENH VIEN] Ban da duoc chua khoi benh voi gia $%d.", HOSPITAL_CURE_PRICE);
            }
        }
        return 1;
    }

    Dialog_ShowCallback(playerid, using inline _response,
        DIALOG_STYLE_LIST,
        "=== DICH VU BENH VIEN ===",
        "1. Hoi phuc mau ($500)\n2. Thay doi trang phuc IC ($10,000)\n3. Dieu tri benh truyen nhiem ($1,500)",
        "Chon", "Thoat");
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

stock EMS_IsOnDuty(playerid)
{
    return EMSOnDuty[playerid];
}

stock EMS_GetRank(playerid)
{
    return EMSRank[playerid];
}

stock EMS_GetRankName(rank, output[], len)
{
    switch (rank) {
        case 0: format(output, len, "EMT");
        case 1: format(output, len, "Paramedic");
        case 2: format(output, len, "Doctor");
        case 3: format(output, len, "Chief Medical Officer");
        default: format(output, len, "Khong Xac Dinh");
    }
    return 1;
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

        new rankName[24];
        EMS_GetRankName(EMSRank[playerid], rankName, sizeof(rankName));

        SendMsgF(playerid, COLOR_GREEN,
            "[EMS] Ban da len ca duty (Rank: %s). Cham soc tot cho benh nhan!",
            rankName);

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

    defer EMS_OnCPRResult(playerid, targetid);
    return 1;
}

timer EMS_OnCPRResult[3000](playerid, targetid)
{
    if (!IsPlayerConnected(playerid) || !IsPlayerConnected(targetid)) {
        return 1;
    }

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

    return 1;
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

    new rankName[24];
    EMS_GetRankName(rank, rankName, sizeof(rankName));

    SendMsgF(playerid, COLOR_GREEN, "Da thang cap %s len EMS Rank %s.",
        ReturnPlayerName(targetid), rankName);
    SendMsgF(targetid, COLOR_GREEN, "Ban da duoc thang cap len EMS Rank %s.", rankName);
    return 1;
}
