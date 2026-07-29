/*
    player_survival.inc
    -------------------
    Hệ thống Sinh tồn: Đói (Hunger) và Khát (Thirst).

    Mô tả:
    - Mỗi phút, người chơi sẽ bị trừ 1-2 điểm Đói và Khát.
    - Thanh đói/khát tối đa là 100.
    - Khi Đói hoặc Khát về 0, người chơi sẽ bị mất máu từ từ (1 HP mỗi phút).
    - Tích hợp với lệnh /use (của hệ thống inventory) để ăn/uống phục hồi.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define MAX_SURVIVAL_STAT       (100)
#define HUNGER_DECREASE_RATE    (2)     // Trừ 2 điểm mỗi phút
#define THIRST_DECREASE_RATE    (3)     // Trừ 3 điểm mỗi phút
#define SURVIVAL_DAMAGE_RATE    (2.0)   // Trừ 2 HP nếu Đói hoặc Khát = 0

// =============================================================================
// DATA
// =============================================================================

static PlayerHunger[MAX_PLAYERS];
static PlayerThirst[MAX_PLAYERS];
static PlayerSurvivalTimer[MAX_PLAYERS];

// HUD TextDraw
static PlayerText:ptdHunger[MAX_PLAYERS] = {PlayerText:INVALID_TEXT_DRAW, ...};
static PlayerText:ptdThirst[MAX_PLAYERS] = {PlayerText:INVALID_TEXT_DRAW, ...};

// =============================================================================
// HOOKS
// =============================================================================

hook OnPlayerConnect(playerid)
{
    PlayerHunger[playerid] = MAX_SURVIVAL_STAT;
    PlayerThirst[playerid] = MAX_SURVIVAL_STAT;
    PlayerSurvivalTimer[playerid] = 0;

    // Tạo HUD TextDraw
    ptdHunger[playerid] = CreatePlayerTextDraw(playerid, 550.0, 390.0, "Doi: 100");
    PlayerTextDrawLetterSize(playerid, ptdHunger[playerid], 0.22, 1.0);
    PlayerTextDrawColor(playerid, ptdHunger[playerid], 0xFFCC00FF);
    PlayerTextDrawSetShadow(playerid, ptdHunger[playerid], 1);
    PlayerTextDrawFont(playerid, ptdHunger[playerid], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, ptdHunger[playerid], true);

    ptdThirst[playerid] = CreatePlayerTextDraw(playerid, 550.0, 402.0, "Khat: 100");
    PlayerTextDrawLetterSize(playerid, ptdThirst[playerid], 0.22, 1.0);
    PlayerTextDrawColor(playerid, ptdThirst[playerid], 0x00CCFFFF);
    PlayerTextDrawSetShadow(playerid, ptdThirst[playerid], 1);
    PlayerTextDrawFont(playerid, ptdThirst[playerid], TEXT_DRAW_FONT_2);
    PlayerTextDrawSetProportional(playerid, ptdThirst[playerid], true);

    return 1;
}

hook OnPlayerSpawn(playerid)
{
    // Phục hồi 50% khi hồi sinh nếu đang quá thấp
    if (PlayerHunger[playerid] < 50) PlayerHunger[playerid] = 50;
    if (PlayerThirst[playerid] < 50) PlayerThirst[playerid] = 50;
    Survival_UpdateHUD(playerid);
    // Ẩn TextDraw đói khát cũ (vì đã dùng CEF Status HUD mới)
    // PlayerTextDrawShow(playerid, ptdHunger[playerid]);
    // PlayerTextDrawShow(playerid, ptdThirst[playerid]);
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    if (ptdHunger[playerid] != PlayerText:INVALID_TEXT_DRAW)
    {
        PlayerTextDrawDestroy(playerid, ptdHunger[playerid]);
        ptdHunger[playerid] = PlayerText:INVALID_TEXT_DRAW;
    }
    if (ptdThirst[playerid] != PlayerText:INVALID_TEXT_DRAW)
    {
        PlayerTextDrawDestroy(playerid, ptdThirst[playerid]);
        ptdThirst[playerid] = PlayerText:INVALID_TEXT_DRAW;
    }
    return 1;
}


// Chạy mỗi 60 giây (1 phút)
hook OnPlayerSecondUpdate(playerid)
{
    if (!Player_IsLoggedIn(playerid)) return 1;

    PlayerSurvivalTimer[playerid]++;
    if (PlayerSurvivalTimer[playerid] >= 60)
    {
        PlayerSurvivalTimer[playerid] = 0;

        // Trừ Đói
        if (PlayerHunger[playerid] > 0)
        {
            PlayerHunger[playerid] -= HUNGER_DECREASE_RATE;
            if (PlayerHunger[playerid] < 0) PlayerHunger[playerid] = 0;
        }

        // Trừ Khát
        if (PlayerThirst[playerid] > 0)
        {
            PlayerThirst[playerid] -= THIRST_DECREASE_RATE;
            if (PlayerThirst[playerid] < 0) PlayerThirst[playerid] = 0;
        }

        // Thông báo
        if (PlayerHunger[playerid] == 10)
            SendMsgF(playerid, COLOR_YELLOW, "* Ban dang cam thay doi bung. Hay tim do an.");
        if (PlayerThirst[playerid] == 10)
            SendMsgF(playerid, COLOR_YELLOW, "* Ban dang cam thay khao khat. Hay uong nuoc.");

        // Gây sát thương nếu về 0
        if (PlayerHunger[playerid] == 0 || PlayerThirst[playerid] == 0)
        {
            new Float:hp;
            GetPlayerHealth(playerid, hp);
            if (hp > 5.0) // Không tự chết đói hoàn toàn, chỉ xuống máu thấp
            {
                SetPlayerHealth(playerid, hp - SURVIVAL_DAMAGE_RATE);
                SendMsgF(playerid, COLOR_RED, "* Ban dang bi kiet suc do %s!", 
                    (PlayerHunger[playerid] == 0) ? "DOI BUNG" : "KHAT NUOC");
            }
        }
    }
    return 1;
}

// =============================================================================
// HUD UPDATE
// =============================================================================

static Survival_UpdateHUD(playerid)
{
    if (ptdHunger[playerid] == PlayerText:INVALID_TEXT_DRAW) return;

    new str[32];
    format(str, sizeof(str), "~y~Doi: %d/100", PlayerHunger[playerid]);
    PlayerTextDrawSetString(playerid, ptdHunger[playerid], str);

    format(str, sizeof(str), "~b~Khat: %d/100", PlayerThirst[playerid]);
    PlayerTextDrawSetString(playerid, ptdThirst[playerid], str);
}

// =============================================================================
// API
// =============================================================================

stock Survival_AddHunger(playerid, amount)
{
    PlayerHunger[playerid] += amount;
    if (PlayerHunger[playerid] > MAX_SURVIVAL_STAT) 
        PlayerHunger[playerid] = MAX_SURVIVAL_STAT;
    
    Survival_UpdateHUD(playerid);
    SendMsgF(playerid, COLOR_GREEN, "* Ban da an. (Doi: %d/100)", PlayerHunger[playerid]);
    return 1;
}

stock Survival_AddThirst(playerid, amount)
{
    PlayerThirst[playerid] += amount;
    if (PlayerThirst[playerid] > MAX_SURVIVAL_STAT) 
        PlayerThirst[playerid] = MAX_SURVIVAL_STAT;
    
    Survival_UpdateHUD(playerid);
    SendMsgF(playerid, COLOR_GREEN, "* Ban da uong. (Khat: %d/100)", PlayerThirst[playerid]);
    return 1;
}

stock Survival_GetHunger(playerid)
{
    return PlayerHunger[playerid];
}

stock Survival_GetThirst(playerid)
{
    return PlayerThirst[playerid];
}

stock Survival_SetHunger(playerid, amount)
{
    if (amount < 0) amount = 0;
    if (amount > MAX_SURVIVAL_STAT) amount = MAX_SURVIVAL_STAT;
    PlayerHunger[playerid] = amount;
    Survival_UpdateHUD(playerid);
    return 1;
}

stock Survival_SetThirst(playerid, amount)
{
    if (amount < 0) amount = 0;
    if (amount > MAX_SURVIVAL_STAT) amount = MAX_SURVIVAL_STAT;
    PlayerThirst[playerid] = amount;
    Survival_UpdateHUD(playerid);
    return 1;
}

// =============================================================================
// COMMAND – /useitem [tên vật phẩm]
// =============================================================================

CMD:useitem(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (isnull(params))
        return SendErrorMsg(playerid, "Su dung: /useitem [burger / pizza / nuoc / beer / fish]"), 1;

    if (!Inv_HasItem(playerid, params))
        return SendErrorMsg(playerid, "Ban khong co vat pham nay trong tui do."), 1;

    // Các vật phẩm ăn uống
    if (strcmp(params, "burger", true) == 0 || strcmp(params, "pizza", true) == 0)
    {
        Inv_RemoveItem(playerid, params);
        Survival_AddHunger(playerid, 30);
        ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, false, false, false, false, 0, SYNC_ALL);
    }
    else if (strcmp(params, "nuoc", true) == 0 || strcmp(params, "water", true) == 0)
    {
        Inv_RemoveItem(playerid, params); // Hoặc tên slug chính xác
        Survival_AddThirst(playerid, 40);
        ApplyAnimation(playerid, "VENDING", "VEND_Drink2_P", 4.1, false, false, false, false, 0, SYNC_ALL);
    }
    else if (strcmp(params, "beer", true) == 0)
    {
        Inv_RemoveItem(playerid, params);
        Survival_AddThirst(playerid, 20);
        ApplyAnimation(playerid, "BAR", "dnk_stndM_loop", 4.1, false, false, false, false, 0, SYNC_ALL);
    }
    else if (strcmp(params, "fish_small", true) == 0)
    {
        Inv_RemoveItem(playerid, params);
        Survival_AddHunger(playerid, 15);
        ApplyAnimation(playerid, "FOOD", "EAT_Burger", 4.1, false, false, false, false, 0, SYNC_ALL);
    }
    else
    {
        return SendErrorMsg(playerid, "Vat pham nay khong the an/uong duoc!"), 1;
    }
    
    return 1;
}

ALTCMD:use(useitem);

// Lệnh debug/xem trạng thái
CMD:mystats(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid)) return 1;
    
    SendMsgF(playerid, COLOR_YELLOW, "=== CHI SO SINH TON ===");
    SendMsgF(playerid, COLOR_WHITE, "Đói bụng: %d/100", PlayerHunger[playerid]);
    SendMsgF(playerid, COLOR_WHITE, "Khát nước: %d/100", PlayerThirst[playerid]);
    return 1;
}
