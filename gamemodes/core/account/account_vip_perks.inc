/*
    account_vip_perks.inc
    ----------------------
    Hệ thống VIP System & Quyền Lợi (VIP Perks & Benefits).

    Cấp VIP:
    - VIP 1: Silver VIP
    - VIP 2: Gold VIP
    - VIP 3: Platinum VIP

    Quyền lợi:
    - Silver:   +10% Lương Paycheck, 10% Giảm phí ngân hàng
    - Gold:     +20% Lương Paycheck, 20% Giảm phí ngân hàng, Lệnh /vipcolor
    - Platinum: +30% Lương Paycheck, 50% Giảm phí ngân hàng, Lệnh /vipcolor, /vipchat

    Lệnh:
    - /myvip       – Xem cấp độ VIP & Hạn sử dụng
    - /vipcolor    – Đổi màu tên OOC (Gold / Platinum)
    - /vipchat     – Kênh chat riêng giữa các thành viên VIP (Platinum)
    - /setvip      – Admin cấp VIP cho người chơi
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define VIP_LEVEL_NONE      (0)
#define VIP_LEVEL_SILVER    (1)
#define VIP_LEVEL_GOLD      (2)
#define VIP_LEVEL_PLATINUM  (3)

// Màu sắc VIP độc quyền
static const VIPColors[] = {
    0xFFD700FF, // Gold
    0xE5E4E2FF, // Platinum
    0x00FFFFFF, // Cyan Spark
    0xFF007FFF  // Neon Rose
};

// Cooldown VIP Chat (10s)
static VIPChatCooldown[MAX_PLAYERS];

// =============================================================================
// PUBLIC API – Tra cứu VIP Bonus Paycheck
// =============================================================================

stock Float:VIP_GetPaycheckMultiplier(playerid)
{
    new vipLevel = VIP_GetPlayerLevel(playerid);
    switch (vipLevel)
    {
        case VIP_LEVEL_SILVER:   return 1.10; // +10%
        case VIP_LEVEL_GOLD:     return 1.20; // +20%
        case VIP_LEVEL_PLATINUM: return 1.30; // +30%
    }
    return 1.0; // Normal
}

// =============================================================================
// COMMAND – /myvip – Xem cấp độ VIP
// =============================================================================

CMD:myvip(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new vipLevel = VIP_GetPlayerLevel(playerid);
    new content[512];

    if (vipLevel == VIP_LEVEL_NONE)
    {
        format(content, sizeof(content),
            "Cap do VIP: {AFAFAF}Thuong (Khong co VIP){FFFFFF}\n\n\
Quyen loi khi nang cap VIP:\n\
- Silver VIP:   +10%% Luong Paycheck\n\
- Gold VIP:     +20%% Luong Paycheck + Mau ten Doc Quyen (/vipcolor)\n\
- Platinum VIP: +30%% Luong Paycheck + Kenh Chat VIP (/vipchat)\n\n\
Lien he Admin hoac Donate de nang cap VIP!"
        );
    }
    else
    {
        static const vipNames[][] = { "Khong", "Silver VIP", "Gold VIP", "Platinum VIP" };
        format(content, sizeof(content),
            "=== DASHBOARD VIP ===\n\n\
Cap do VIP: {FFAA00}%s{FFFFFF}\n\
Bonus Luong: {33AA33}+%.0f%%{FFFFFF}\n\n\
Cac lenh kha dung:\n\
%s%s",
            vipNames[vipLevel],
            (VIP_GetPaycheckMultiplier(playerid) - 1.0) * 100.0,
            (vipLevel >= VIP_LEVEL_GOLD) ? "- /vipcolor : Chon mau ten OOC doc quyen\n" : "",
            (vipLevel >= VIP_LEVEL_PLATINUM) ? "- /vipchat : Kenh chat noi bo Platinum VIP\n" : ""
        );
    }

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Thong Tin Tai Khoan VIP", content, "Dong", "");
    return 1;
}

// =============================================================================
// COMMAND – /vipcolor – Đổi màu tên OOC độc quyền
// =============================================================================

CMD:vipcolor(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (VIP_GetPlayerLevel(playerid) < VIP_LEVEL_GOLD)
        return SendErrorMsg(playerid, "Ban can toi thieu Gold VIP (Level 2) de doi mau ten doc quyen."), 1;

    inline OnColorSelect(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext
        if (!response) return 1;

        SetPlayerColor(playerid, VIPColors[listitem]);
        SendMsgF(playerid, COLOR_GREEN, "[VIP PERKS] Da doi mau ten OOC thanh cong!");
    }

    Dialog_ShowCallback(playerid, using inline OnColorSelect,
        DIALOG_STYLE_LIST,
        "Chon Mau Ten VIP Doc Quyen",
        "Gold VIP (Vang Hoang Gia)\n\
        Platinum (Bach Kim Sang)\n\
        Cyan Spark (Xanh Da Troi)\n\
        Neon Rose (Hong Neon)",
        "Chon", "Huy");
    return 1;
}

// =============================================================================
// COMMAND – /vipchat – Chat kênh VIP (Platinum VIP)
// =============================================================================

CMD:vipchat(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (VIP_GetPlayerLevel(playerid) < VIP_LEVEL_PLATINUM)
        return SendErrorMsg(playerid, "Chi Platinum VIP (Level 3) moi duoc dung /vipchat."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /vipchat [noi dung]"), 1;

    if (gettime() < VIPChatCooldown[playerid])
    {
        new rem = VIPChatCooldown[playerid] - gettime();
        return SendErrorMsgF(playerid, "Vui long doi %d giay truoc khi gui tin tiep theo.", rem), 1;
    }

    VIPChatCooldown[playerid] = gettime() + 10; // 10s cooldown

    // Send to all Platinum VIPs
    foreach (new i : Player)
    {
        if (VIP_GetPlayerLevel(i) >= VIP_LEVEL_PLATINUM || Admin_GetPlayerLevel(i) >= 1)
        {
            SendMsgF(i, 0xE5E4E2FF, "[VIP CHAT] %s: %s", ReturnPlayerName(playerid), params);
        }
    }
    return 1;
}

// =============================================================================
// COMMAND – /setvip [playerid] [level] – Admin cấp VIP
// =============================================================================

ACMD:[4]setviplevel(playerid, params[])
{
    new targetid, level;
    if (sscanf(params, "ui", targetid, level))
        return SendErrorMsg(playerid, "Su dung: /setviplevel [playerid] [level 0-3]"), 1;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai."), 1;

    if (level < 0 || level > 3)
        return SendErrorMsg(playerid, "Level VIP phai tu 0 den 3."), 1;

    if (level == VIP_LEVEL_NONE) {
        VIP_RemovePlayer(targetid);
    }
    else {
        VIP_GivePlayer(targetid, level, 1, 20);
    }

    static const names[][] = { "Thuong", "Silver VIP", "Gold VIP", "Platinum VIP" };
    SendMsgF(playerid, COLOR_GREEN, "[ADMIN] Da cap VIP Level %d (%s) cho %s.",
        level, names[level], ReturnPlayerName(targetid));
    SendMsgF(targetid, COLOR_GREEN, "[VIP PERKS] Admin %s da cap cho ban cap do VIP %s!",
        ReturnPlayerName(playerid), names[level]);
    return 1;
}
