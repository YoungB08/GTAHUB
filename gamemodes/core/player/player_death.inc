/*
    player_death.inc
    ----------------
    Hệ thống Tử Vong & Vết Thương (Death & Injury System) cho Server RP.

    Mô tả:
    Khi người chơi chết (HP = 0), thay vì hồi sinh ngay lập tức tại bệnh viện và mất trí nhớ,
    họ sẽ chuyển sang trạng thái "Bị thương nặng" (Downed / Brutally Wounded).
    - Người chơi bị đóng băng tại vị trí chết, nằm gục với animation đau đớn.
    - Thời gian đếm ngược 5 phút chờ EMS đến cứu chữa (/revive).
    - Có thể chọn chết hẳn bằng lệnh /acceptdeath sau 1 phút chờ đợi.
    - Nếu chết hẳn, người chơi sẽ hồi sinh tại bệnh viện, mất 10% tiền mặt mang theo.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define DEATH_WAIT_TIME         (300)       // 5 phút (300 giây) chờ EMS
#define ACCEPT_DEATH_TIME       (60)        // Sau 60 giây mới được /acceptdeath
#define DEATH_MONEY_PENALTY     (0.10)      // Phạt 10% tiền mặt khi Accept Death

// =============================================================================
// DATA
// =============================================================================

static bool:IsPlayerDowned[MAX_PLAYERS];
static DownedTimeLeft[MAX_PLAYERS];
static Float:DeathPosX[MAX_PLAYERS];
static Float:DeathPosY[MAX_PLAYERS];
static Float:DeathPosZ[MAX_PLAYERS];
static Float:DeathPosA[MAX_PLAYERS];
static DeathVW[MAX_PLAYERS];
static DeathInt[MAX_PLAYERS];

// =============================================================================
// HOOKS
// =============================================================================

hook OnPlayerConnect(playerid)
{
    IsPlayerDowned[playerid] = false;
    DownedTimeLeft[playerid] = 0;
    return 1;
}

hook OnPlayerDeath(playerid, killerid, reason)
{
    // Lưu lại vị trí khi chết
    GetPlayerPos(playerid, DeathPosX[playerid], DeathPosY[playerid], DeathPosZ[playerid]);
    GetPlayerFacingAngle(playerid, DeathPosA[playerid]);
    DeathVW[playerid]  = GetPlayerVirtualWorld(playerid);
    DeathInt[playerid] = GetPlayerInterior(playerid);

    // Bật trạng thái bị thương nặng
    IsPlayerDowned[playerid] = true;
    DownedTimeLeft[playerid] = DEATH_WAIT_TIME;

    // Gửi thông báo cho EMS
    new name[MAX_PLAYER_NAME];
    GetPlayerName(playerid, name, sizeof(name));
    foreach (new i : Player)
    {
        if (Player_GetClass(i) == TEAM_MEDIC && EMS_IsOnDuty(i))
        {
            SendMsgF(i, COLOR_RED, "[911] Nan nhan %s dang trong tinh trang nguy kich! Can cap cuu gap!", name);
        }
    }

    return 1;
}

hook OnPlayerSpawn(playerid)
{
    if (IsPlayerDowned[playerid])
    {
        // Chặn không cho hồi sinh bình thường, đẩy về lại vị trí chết
        SetPlayerPos(playerid, DeathPosX[playerid], DeathPosY[playerid], DeathPosZ[playerid]);
        SetPlayerFacingAngle(playerid, DeathPosA[playerid]);
        SetPlayerVirtualWorld(playerid, DeathVW[playerid]);
        SetPlayerInterior(playerid, DeathInt[playerid]);

        SetPlayerHealth(playerid, 10.0); // Giữ máu thấp
        TogglePlayerControllable(playerid, false); // Đóng băng di chuyển

        // Animation gục ngã nằm chết (CRACK: crckdeth2)
        ApplyAnimation(playerid, "CRACK", "crckdeth2", 4.1, false, true, true, true, 0, true);

        SendMsgF(playerid, COLOR_RED, "=====================================================");
        SendMsgF(playerid, COLOR_RED, "BAN DANG BI THUONG NANG (BRUTALLY WOUNDED)!");
        SendMsgF(playerid, COLOR_RED, "Hay doi EMS den cuu chua. Thoi gian cho: 5 phut.");
        SendMsgF(playerid, COLOR_RED, "Sau 60 giay, ban co the dung lenh /acceptdeath de chap nhan tu vong.");
        SendMsgF(playerid, COLOR_RED, "=====================================================");
    }
    return 1;
}

hook OnPlayerSecondUpdate(playerid)
{
    if (IsPlayerDowned[playerid])
    {
        // Duy trì animation nằm gục chết liên tục
        ApplyAnimation(playerid, "CRACK", "crckdeth2", 4.1, false, true, true, true, 0, true);

        if (DownedTimeLeft[playerid] > 0)
        {
            DownedTimeLeft[playerid]--;
            
            // Nhắc nhở mỗi 60 giây
            if (DownedTimeLeft[playerid] > 0 && DownedTimeLeft[playerid] % 60 == 0)
            {
                SendMsgF(playerid, COLOR_RED, "[DOWNED] Thoi gian cho EMS con lai: %d phut.", DownedTimeLeft[playerid] / 60);
            }

            // Hết giờ -> Chết hẳn
            if (DownedTimeLeft[playerid] == 0)
            {
                Player_AcceptDeath(playerid, "He thong tu dong chap nhan tu vong do het thoi gian cho.");
            }
        }
    }
    return 1;
}

// =============================================================================
// LỆNH & HÀM HỖ TRỢ
// =============================================================================

CMD:acceptdeath(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (!IsPlayerDowned[playerid])
        return SendErrorMsg(playerid, "Ban khong trong trang thai bi thuong nang."), 1;

    new waited = DEATH_WAIT_TIME - DownedTimeLeft[playerid];
    if (waited < ACCEPT_DEATH_TIME)
    {
        new rem = ACCEPT_DEATH_TIME - waited;
        return SendErrorMsgF(playerid, "Ban phai doi them %d giay nua moi co the chap nhan tu vong.", rem), 1;
    }

    Player_AcceptDeath(playerid, "Ban da tu chap nhan tu vong.");
    return 1;
}

static Player_AcceptDeath(playerid, const reason[])
{
    IsPlayerDowned[playerid] = false;
    DownedTimeLeft[playerid] = 0;
    TogglePlayerControllable(playerid, true);
    ClearAnimations(playerid);

    // Phạt tiền
    new money = GetPlayerMoney(playerid);
    new penalty = floatround(float(money) * DEATH_MONEY_PENALTY);
    if (penalty > 0)
    {
        GivePlayerMoney(playerid, -penalty);
    }

    SendMsgF(playerid, COLOR_RED, "[TU VONG] %s", reason);
    SendMsgF(playerid, COLOR_RED, "[TU VONG] Ban bi mat $%d (10%% tien mat mang theo) de chi tra vien phi.", penalty);

    // Xóa vị trí chết, cho phép respawn tại bệnh viện (1172.1, -1323.2, 14.5 - Vị trí mẫu)
    SetPlayerPos(playerid, 1172.1, -1323.2, 14.5);
    SetPlayerFacingAngle(playerid, 270.0);
    SetPlayerVirtualWorld(playerid, 0);
    SetPlayerInterior(playerid, 0);
    SetPlayerHealth(playerid, 50.0); // Hồi sinh với 50 HP
}

// Hàm này được gọi từ ems_system.inc (lệnh /revive)
stock Player_ReviveFromDowned(playerid)
{
    if (!IsPlayerDowned[playerid]) return false;

    IsPlayerDowned[playerid] = false;
    DownedTimeLeft[playerid] = 0;
    TogglePlayerControllable(playerid, true);
    ClearAnimations(playerid);
    
    // Y tế hồi phục 50 HP (cần /heal để đầy)
    SetPlayerHealth(playerid, 50.0);
    
    return true;
}
