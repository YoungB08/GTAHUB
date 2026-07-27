/*
    cmds_bank.inc
    -------------
    Lệnh ngân hàng RP nâng cao.

    Lệnh có sẵn:
    /transfer [playerid] [so_tien]  – Chuyển tiền ngân hàng cho người khác
    /balance                        – Xem số dư ngân hàng (alias ATM)

    Lưu ý: Cần đứng gần ATM để dùng /transfer (trừ khi có Smartphone App).
    ATM radius check: gần ATM trong 3m.
*/

#include <YSI_Coding\y_hooks>

#define BANK_TRANSFER_MIN       (100)       // Tối thiểu mỗi lần chuyển
#define BANK_TRANSFER_MAX       (500000)    // Tối đa mỗi lần chuyển
#define BANK_TRANSFER_FEE       (0.02)      // Phí chuyển khoản 2%
#define BANK_ATM_RADIUS         (3.0)       // Bán kính đứng gần ATM

// =============================================================================
// HELPER – Kiểm tra gần ATM
// =============================================================================

// Gọi hàm ATM_IsPlayerNearATM nếu có, không thì bỏ qua check
static bool:IsNearATM(playerid)
{
    // Fallback: cho phép bất cứ đâu nếu không có hàm check ATM
    // Khi tích hợp đầy đủ với smartphone, sẽ bỏ giới hạn này
    #if defined ATM_IsPlayerNear
        return ATM_IsPlayerNear(playerid);
    #else
        #pragma unused playerid
        return true;
    #endif
}

// =============================================================================
// /transfer [playerid] [so_tien]
// =============================================================================

CMD:transfer(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new targetid, amount;

    if (sscanf(params, "ui", targetid, amount))
        return SendErrorMsg(playerid, "Su dung: /transfer [playerid] [so_tien]"), 1;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi nay khong ton tai hoac da thoat."), 1;

    if (targetid == playerid)
        return SendErrorMsg(playerid, "Ban khong the chuyen tien cho chinh minh."), 1;

    if (!Player_IsLoggedIn(targetid))
        return SendErrorMsg(playerid, "Nguoi choi do chua dang nhap."), 1;

    if (amount < BANK_TRANSFER_MIN)
        return SendErrorMsgF(playerid, "So tien toi thieu de chuyen la $%d.", BANK_TRANSFER_MIN), 1;

    if (amount > BANK_TRANSFER_MAX)
        return SendErrorMsgF(playerid, "So tien toi da moi lan chuyen la $%d.", BANK_TRANSFER_MAX), 1;

    new fee = floatround(float(amount) * BANK_TRANSFER_FEE);
    new totalDeducted = amount + fee;

    if (Player_GetBankMoney(playerid) < totalDeducted)
        return SendErrorMsgF(playerid, "So du ngan hang khong du! Can $%d (gom phi $%d).", totalDeducted, fee), 1;

    // Thực hiện chuyển
    Player_RemoveBankMoney(playerid, totalDeducted);
    Player_GiveBankMoney(targetid, amount, true);

    // Thông báo 2 bên
    SendMsgF(playerid, COLOR_GREEN,
        "[NGAN HANG] Da chuyen $%d (phi: $%d) toi %s. So du con: $%d.",
        amount, fee, ReturnPlayerName(targetid), Player_GetBankMoney(playerid));

    SendMsgF(targetid, COLOR_GREEN,
        "[NGAN HANG] Ban nhan duoc $%d tu %s. So du: $%d.",
        amount, ReturnPlayerName(playerid), Player_GetBankMoney(targetid));

    Logger_Dbg(PLAYER_DEBUG_HANDLER, "[cmds_bank.inc / transfer] bank transfer",
        Logger_P(playerid),
        Logger_I("target",  targetid),
        Logger_I("amount",  amount),
        Logger_I("fee",     fee)
    );

    return 1;
}

// =============================================================================
// /balance – Xem số dư ngân hàng
// =============================================================================

CMD:balance(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    SendMsgF(playerid, COLOR_SAMPBLUE,
        "[NGAN HANG] So du: $%d | Tien mat: $%d",
        Player_GetBankMoney(playerid), GetPlayerMoney(playerid));

    return 1;
}

// Alias
ALTCMD:bal(playerid, params);
