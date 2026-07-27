/*
    chat_rp.inc
    -----------
    Hệ thống kênh chat Roleplay IC / OOC.

    Kênh IC (In-Character):
        Chat thường     : Nói chuyện trong bán kính LOCAL_CHAT_DISTANCE (metres)
        /me  <action>   : Mô tả hành động nhân vật  (vd: * CJ bat tay voi ...)
        /do  <desc>     : Mô tả môi trường / trạng thái (vd: * [Ban co the thay...])
        /low <msg>      : Nói thầm – bán kính LOW_CHAT_DISTANCE nhỏ hơn
        /s   <msg>      : Hét to – bán kính SHOUT_CHAT_DISTANCE lớn hơn
        /r   <msg>      : Bộ đàm Faction (Gang/LSPD/EMS – chung 1 kênh theo faction)
        /dep <msg>      : Bộ đàm liên bộ Cảnh sát/Quân đội

    Kênh OOC (Out-Of-Character):
        /b   <msg>      : Local OOC – chỉ người trong phạm vi nghe được
        /o   <msg>      : Global OOC – toàn server (có cooldown)
        /pm  đã có sẵn trong cmds_pm.inc

    Lưu ý thiết kế:
    - Chat thường (không lệnh) giữ nguyên xử lý tại chat_OnPlayerText.inc (bán kính local).
    - Module này chỉ xử lý các lệnh /me, /do, /low, /s, /r, /dep, /b, /o.
    - Không import y_hooks do đây là file commands thuần túy.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define LOCAL_CHAT_DISTANCE     (20.0)   // Metres – chat thường
#define LOW_CHAT_DISTANCE       (5.0)    // Metres – nói thầm /low
#define SHOUT_CHAT_DISTANCE     (50.0)   // Metres – hét /s
#define LOCAL_OOC_DISTANCE      (20.0)   // Metres – /b OOC

// Màu sắc riêng cho từng kênh
#define COLOR_RP_ME             (0xC2A2DAFF)   // Tím – hành động /me
#define COLOR_RP_DO             (0x87CEEBFF)   // Xanh da trời – /do
#define COLOR_RP_LOW            (0xCCCCCCFF)   // Xám – /low
#define COLOR_RP_SHOUT          (0xFF6B6BFF)   // Đỏ cam – /s
#define COLOR_RP_LOCAL_CHAT     (0xFFFFFFFF)   // Trắng – chat thường local
#define COLOR_RP_RADIO          (0x55FF55FF)   // Xanh lá – /r bộ đàm
#define COLOR_RP_DEP_RADIO      (0x00BFFFFF)   // Xanh cyan – /dep liên bộ
#define COLOR_RP_LOCAL_OOC      (0xAFAFAFFF)   // Xám nhạt – /b
#define COLOR_RP_GLOBAL_OOC     (0xAFAFAFFF)   // Xám nhạt – /o

// Cooldown /o Global OOC (giây)
#define GLOBAL_OOC_COOLDOWN     (20)

static gGlobalOOCCooldown[MAX_PLAYERS];

// =============================================================================
// INTERNAL HELPERS
// =============================================================================

// Gửi tin nhắn cho tất cả người chơi trong phạm vi 3D
static SendLocalMessage(senderid, color, const msg[], Float:distance)
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(senderid, x, y, z);

    foreach (new i : Player)
    {
        if (!Player_IsLoggedIn(i)) continue;
        if (GetPlayerVirtualWorld(i) != GetPlayerVirtualWorld(senderid)) continue;
        if (GetPlayerInterior(i) != GetPlayerInterior(senderid)) continue;
        if (!IsPlayerInRangeOfPoint(i, distance, x, y, z)) continue;
        SendClientMessage(i, color, msg);
    }
}

// =============================================================================
// CHAT THƯỜNG – LOCAL IC (override OnPlayerText để giới hạn bán kính)
// =============================================================================

// Hook OnPlayerText: Chuyển global chat thành local IC chat
// Ghi chú: chat_OnPlayerText.inc vẫn load trước; ta hook bằng y_hooks để override
hook OnPlayerText(playerid, text[])
{
    // Chỉ xử lý khi đã đăng nhập
    if (!Player_IsLoggedIn(playerid)) return 1;

    // Đã có police radio ! và admin chat # xử lý bởi chat_OnPlayerText.inc
    // Ta xử lý phần còn lại ở đây thay vì gửi toàn server
    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    new msg[192];
    format(msg, sizeof(msg), "[%d] %s: %s", playerid, playerName, text);

    SendLocalMessage(playerid, COLOR_RP_LOCAL_CHAT, msg, LOCAL_CHAT_DISTANCE);

    return 0; // Chặn việc gửi toàn server
}

// =============================================================================
// /me – Hành động nhân vật IC
// =============================================================================

CMD:me(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /me <hanh dong>"), 1;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    new msg[192];
    format(msg, sizeof(msg), "* %s %s", playerName, params);

    SendLocalMessage(playerid, COLOR_RP_ME, msg, LOCAL_CHAT_DISTANCE);
    return 1;
}

// =============================================================================
// /do – Mô tả môi trường / trạng thái IC
// =============================================================================

CMD:do(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /do <mo ta>"), 1;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    new msg[192];
    format(msg, sizeof(msg), "* %s ((- %s -))", params, playerName);

    SendLocalMessage(playerid, COLOR_RP_DO, msg, LOCAL_CHAT_DISTANCE);
    return 1;
}

// =============================================================================
// /low – Nói thầm IC (bán kính nhỏ)
// =============================================================================

CMD:low(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /low <noi tham>"), 1;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    new msg[192];
    format(msg, sizeof(msg), "[%d] %s (tham): %s", playerid, playerName, params);

    SendLocalMessage(playerid, COLOR_RP_LOW, msg, LOW_CHAT_DISTANCE);
    return 1;
}

// Alias /w
ALTCMD:w(playerid, params);

// =============================================================================
// /s – Hét to IC (bán kính lớn)
// =============================================================================

CMD:s(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /s <het to>"), 1;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    new msg[192];
    format(msg, sizeof(msg), "[%d] %s (HET): %s", playerid, playerName, params);

    SendLocalMessage(playerid, COLOR_RP_SHOUT, msg, SHOUT_CHAT_DISTANCE);
    return 1;
}

// Alias /shout
ALTCMD:shout(playerid, params);

// =============================================================================
// /r – Bộ đàm Faction (Gang / LSPD / EMS)
// =============================================================================

CMD:r(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /r <tin nhan bo dam>"), 1;

    new playerClass = Player_GetClass(playerid);

    // Chỉ cho phép thành viên trong đội (Police, Army, Medic) hoặc civilian có gang dùng /r
    // Gang: faction radio chỉ cho civilian (gang members)
    // Hiện tại dùng team class để lọc

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    new msg[192];
    new radioLabel[32];

    switch (playerClass)
    {
        case TEAM_POLICE: format(radioLabel, sizeof(radioLabel), "[CANH SAT RADIO]");
        case TEAM_ARMY:   format(radioLabel, sizeof(radioLabel), "[QUAN DOI RADIO]");
        case TEAM_MEDIC:  format(radioLabel, sizeof(radioLabel), "[Y TE RADIO]");
        default:          format(radioLabel, sizeof(radioLabel), "[GANG RADIO]");
    }

    format(msg, sizeof(msg), "%s %s: %s", radioLabel, playerName, params);

    // Gửi đến những người cùng class
    foreach (new i : Player)
    {
        if (!Player_IsLoggedIn(i)) continue;

        // Civilian (gang) chỉ nhận nhau; các đội nhà nước nhận trong đội
        if (playerClass == TEAM_CIVILIAN && Player_GetClass(i) != TEAM_CIVILIAN) continue;
        if (playerClass != TEAM_CIVILIAN && Player_GetClass(i) != playerClass) continue;

        SendClientMessage(i, COLOR_RP_RADIO, msg);
    }
    return 1;
}

// =============================================================================
// /dep – Bộ đàm Liên Bộ (Cảnh sát + Quân đội)
// =============================================================================

CMD:dep(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new playerClass = Player_GetClass(playerid);
    if (playerClass != TEAM_POLICE && playerClass != TEAM_ARMY)
        return SendErrorMsg(playerid, "Lenh nay chi danh cho Canh sat va Quan doi."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /dep <tin nhan lien bo>"), 1;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    new msg[192];
    format(msg, sizeof(msg), "[LIEN BO] %s: %s", playerName, params);

    foreach (new i : Player)
    {
        if (!Player_IsLoggedIn(i)) continue;
        new iClass = Player_GetClass(i);
        if (iClass != TEAM_POLICE && iClass != TEAM_ARMY) continue;
        SendClientMessage(i, COLOR_RP_DEP_RADIO, msg);
    }
    return 1;
}

// =============================================================================
// /b – Local OOC (Out-Of-Character – gần nhau mới nghe)
// =============================================================================

CMD:b(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /b <nhan xet OOC>"), 1;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    new msg[192];
    format(msg, sizeof(msg), "(( [%d] %s: %s ))", playerid, playerName, params);

    SendLocalMessage(playerid, COLOR_RP_LOCAL_OOC, msg, LOCAL_OOC_DISTANCE);
    return 1;
}

// =============================================================================
// /o – Global OOC (toàn server, có cooldown)
// =============================================================================

CMD:o(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /o <nhan xet OOC toan server>"), 1;

    if (gettime() < gGlobalOOCCooldown[playerid])
    {
        new remaining = gGlobalOOCCooldown[playerid] - gettime();
        SendErrorMsgF(playerid, "Vui long doi %d giay truoc khi dung /o lai.", remaining);
        return 1;
    }

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    new msg[192];
    format(msg, sizeof(msg), "(( [%d] %s [OOC]: %s ))", playerid, playerName, params);

    SendClientMessageToAll(COLOR_RP_GLOBAL_OOC, msg);

    gGlobalOOCCooldown[playerid] = gettime() + GLOBAL_OOC_COOLDOWN;
    return 1;
}

// Reset cooldown khi kết nối lại
hook OnPlayerConnect(playerid)
{
    gGlobalOOCCooldown[playerid] = 0;
    return 1;
}
