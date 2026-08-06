/*
    chat_rp.inc
    -----------
    Hệ thống kênh chat Roleplay IC / OOC.

    Kênh IC (In-Character):
        Chat thường     : Nói chuyện trong bán kính LOCAL_CHAT_DISTANCE (20.0m)
        /me  <action>   : Mô tả hành động nhân vật  (vd: * CJ bat tay voi ...)
        /do  <desc>     : Mô tả môi trường / trạng thái (vd: * [Ban co the thay...])
        /low <msg>      : Nói thầm – bán kính LOW_CHAT_DISTANCE (5.0m)
        /s   <msg>      : Hét to – bán kính SHOUT_CHAT_DISTANCE (50.0m)
        /r   <msg>      : Chat Global (toàn server)
        /radio <msg>    : Bộ đàm Faction (Gang/LSPD/EMS – chung 1 kênh theo faction)
        /dep <msg>      : Bộ đàm liên bộ Cảnh sát/Quân đội

    Kênh OOC (Out-Of-Character):
        /b   <msg>      : Local OOC – bán kính 20.0m
        /o   <msg>      : Global OOC – toàn server (có cooldown)
        /pm, /re        : đã có sẵn trong cmds_pm.inc

    Lưu ý thiết kế:
    - Chat thường (không lệnh) giữ nguyên xử lý tại chat_OnPlayerText.inc (bán kính local 20m).
    - Module này chỉ xử lý các lệnh /me, /do, /low, /s, /r, /radio, /dep, /b, /o.
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
#define COLOR_RP_RADIO          (0x55FF55FF)   // Xanh lá – /r chat global
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
stock SendLocalMessage(senderid, color, const msg[], Float:distance)
{
    new Float:x, Float:y, Float:z;
    GetPlayerPos(senderid, x, y, z);
    printf("[CEF Chat Debug] SendLocalMessage senderid=%d pos=(%.2f, %.2f, %.2f) msg='%s'",
        senderid, x, y, z, msg);

    new sentCount = 0;
    foreach (new i : Player)
    {
        if (!Player_IsLoggedIn(i)) {
            printf("[CEF Chat Debug] SendLocalMessage skip recipient %d: not logged in", i);
            continue;
        }
        if (GetPlayerVirtualWorld(i) != GetPlayerVirtualWorld(senderid)) {
            printf("[CEF Chat Debug] SendLocalMessage skip recipient %d: VW mismatch (%d != %d)",
                i, GetPlayerVirtualWorld(i), GetPlayerVirtualWorld(senderid));
            continue;
        }
        if (GetPlayerInterior(i) != GetPlayerInterior(senderid)) {
            printf("[CEF Chat Debug] SendLocalMessage skip recipient %d: Interior mismatch (%d != %d)",
                i, GetPlayerInterior(i), GetPlayerInterior(senderid));
            continue;
        }
        if (!IsPlayerInRangeOfPoint(i, distance, x, y, z)) {
            printf("[CEF Chat Debug] SendLocalMessage skip recipient %d: out of range", i);
            continue;
        }
        printf("[CEF Chat Debug] SendLocalMessage sending to recipient %d", i);
        SendClientMessage(i, color, msg);
        sentCount++;
    }
    printf("[CEF Chat Debug] SendLocalMessage finished, total recipients=%d", sentCount);
}

// =============================================================================
// /me – Mô tả hành động IC
// =============================================================================

CMD:me(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /me <hanh dong>"), 1;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    new msg[CHAT_FORMATTED_BYTES];
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

    new msg[CHAT_FORMATTED_BYTES];
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

    new msg[CHAT_FORMATTED_BYTES];
    format(msg, sizeof(msg), "[%d] %s (tham): %s", playerid, playerName, params);

    SendLocalMessage(playerid, COLOR_RP_LOW, msg, LOW_CHAT_DISTANCE);
    return 1;
}

// Alias /w
ALTCMD:w(low);

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

    new msg[CHAT_FORMATTED_BYTES];
    format(msg, sizeof(msg), "[%d] %s (HET): %s", playerid, playerName, params);

    SendLocalMessage(playerid, COLOR_RP_SHOUT, msg, SHOUT_CHAT_DISTANCE);
    return 1;
}

// Alias /shout
ALTCMD:shout(s);

// =============================================================================
// /r – Chat Global (Toàn máy chủ)
// =============================================================================

CMD:r(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /r <noi dung chat global>"), 1;

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    new msg[CHAT_FORMATTED_BYTES];
    format(msg, sizeof(msg), "[GLOBAL] [%d] %s: %s", playerid, playerName, params);

    SendClientMessageToAll(COLOR_RP_RADIO, msg);
    return 1;
}

ALTCMD:g(r);
ALTCMD:globalchat(r);

// =============================================================================
// /radio – Bộ đàm Faction (Gang / LSPD / EMS)
// =============================================================================

CMD:radio(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /radio <tin nhan bo dam>"), 1;

    new playerClass = Player_GetClass(playerid);

    new playerName[MAX_PLAYER_NAME];
    GetPlayerName(playerid, playerName, MAX_PLAYER_NAME);

    new msg[CHAT_FORMATTED_BYTES];
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

ALTCMD:fradio(radio);
ALTCMD:factionradio(radio);

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

    new msg[CHAT_FORMATTED_BYTES];
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

    new msg[CHAT_FORMATTED_BYTES];
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
