/*
    fishing_system.inc
    ------------------
    Hệ thống Câu Cá RP (RP Fishing System & Market).

    Tính năng:
    - Khu vực câu cá: Santa Maria Beach, Palomino Creek, Ocean Docks.
    - Cần câu & Mồi câu: Mua tại cửa hàng 24/7.
    - Lệnh /fish: Bắt đầu câu cá (chờ 5-8 giây cá cắn câu).
    - Phân loại cá:
      * Cá Trích / Cá Bống (Phổ thông) -> $200 - $500
      * Cá Ngừ (Hiếm) -> $1,500 - $3,000
      * Cá Mập Nhỏ (Cực hiếm) -> $8,000 - $15,000
    - Lệnh /sellfish: Bán toàn bộ cá đánh bắt tại Bãi Cá / Chợ.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define FISHING_WAIT_TIME_MS    (6000)      // Thời gian chờ cá cắn câu (6 giây)

// Các vị trí câu cá hợp lệ (bán kính 30m)
static const Float:FishingSpots[][3] = {
    {382.6,   -2067.8, 4.3},   // Santa Maria Beach Pier
    {2306.4,  -2422.5, 3.2},   // Ocean Docks Pier
    {2292.5,  -73.4,   1.5}    // Palomino Creek Lake
};

// =============================================================================
// DATA
// =============================================================================

static bool:IsPlayerFishing[MAX_PLAYERS];

enum E_FISH_TYPE {
    E_FISH_SLUG[32],
    E_FISH_NAME[32],
    E_FISH_PRICE_MIN,
    E_FISH_PRICE_MAX,
    E_FISH_CHANCE       // % tỷ lệ bắt được (tổng = 100)
}

static const FishTypes[][E_FISH_TYPE] = {
    { "fish_small",  "Ca Trich (Nho)",      200,   400,   60 },  // 60%
    { "fish_tuna",   "Ca Ngu (Vừa)",       1500,  3000,  30 },  // 30%
    { "fish_shark",  "Ca Map Nho (Hiếm)",  8000, 15000,  10 }   // 10%
};

// =============================================================================
// HOOKS
// =============================================================================

hook OnPlayerConnect(playerid)
{
    IsPlayerFishing[playerid] = false;
    return 1;
}

// =============================================================================
// HELPER – Kiểm tra người chơi ở gần khu vực câu cá
// =============================================================================

static bool:IsNearFishingSpot(playerid)
{
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    for (new i = 0; i < sizeof(FishingSpots); i++)
    {
        if (Distance(px, py, pz, FishingSpots[i][0], FishingSpots[i][1], FishingSpots[i][2]) <= 30.0)
            return true;
    }
    return false;
}

// =============================================================================
// COMMAND – /fish – Bắt đầu câu cá
// =============================================================================

CMD:fish(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (IsPlayerFishing[playerid])
        return SendErrorMsg(playerid, "Ban dang cau ca roi! Hay kien nhan cho ca can cau."), 1;

    if (!IsNearFishingSpot(playerid))
        return SendErrorMsg(playerid, "Ban phai den khu vuc bô bien hoac cau cang de cau ca."), 1;

    if (IsPlayerInAnyVehicle(playerid))
        return SendErrorMsg(playerid, "Ban khong thể cau ca khi dang ngoi tren xe."), 1;

    IsPlayerFishing[playerid] = true;

    // Apply animation câu cá
    ApplyAnimation(playerid, "SWORD", "sword_4", 4.1, true, false, false, false, 0, SYNC_ALL);

    SendMsgF(playerid, COLOR_GREEN, "[CAU CA] Ban da tha can cau xuong nuoc... Vui long doi ca can cau!");

    defer OnFishBite(playerid);
    return 1;
}

// =============================================================================
// TIMER – Cá cắn câu
// =============================================================================

timer OnFishBite[FISHING_WAIT_TIME_MS](playerid)
{
    if (!IsPlayerConnected(playerid) || !IsPlayerFishing[playerid]) return;

    IsPlayerFishing[playerid] = false;
    ClearAnimations(playerid);

    // Tính ngẫu nhiên loại cá dựa trên chance
    new roll = random(100); // 0-99
    new fishIdx = 0;

    if (roll < 60)
        fishIdx = 0; // Cá Trích (60%)
    else if (roll < 90)
        fishIdx = 1; // Cá Ngừ (30%)
    else
        fishIdx = 2; // Cá Mập (10%)

    // Thêm cá vào túi đồ
    if (Inv_AddItem(playerid, FishTypes[fishIdx][E_FISH_SLUG]))
    {
        new priceEst = RandomRange(FishTypes[fishIdx][E_FISH_PRICE_MIN], FishTypes[fishIdx][E_FISH_PRICE_MAX]);

        SendMsgF(playerid, COLOR_GREEN,
            "====================================================\n"
            "[CAU CA] BẮT ĐƯỢC CÁ! Ban da cau duoc: %s!\n"
            "Gia tri uoc tính: $%d. Dùng /sellfish tai cho de ban.\n"
            "====================================================",
            FishTypes[fishIdx][E_FISH_NAME], priceEst);
    }
    else
    {
        SendErrorMsg(playerid, "[CAU CA] Tuoi do da day! Khong the chieu them ca.");
    }
}

// =============================================================================
// COMMAND – /sellfish – Bán toàn bộ cá trong túi
// =============================================================================

CMD:sellfish(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new totalMoney = 0;
    new totalFish  = 0;

    for (new i = 0; i < sizeof(FishTypes); i++)
    {
        new count = Inv_GetItemCount(playerid, FishTypes[i][E_FISH_SLUG]);
        if (count <= 0) continue;

        for (new c = 0; c < count; c++)
        {
            new price = RandomRange(FishTypes[i][E_FISH_PRICE_MIN], FishTypes[i][E_FISH_PRICE_MAX]);
            totalMoney += price;
            totalFish++;
        }

        Inv_RemoveItem(playerid, FishTypes[i][E_FISH_SLUG], count);
    }

    if (totalFish == 0)
        return SendErrorMsg(playerid, "Ban khong co ca nao trong tui do de ban."), 1;

    GivePlayerMoney(playerid, totalMoney);

    SendMsgF(playerid, COLOR_GREEN,
        "[CHO CA] Da ban x%d con ca! Ban nhan duoc: +$%d.", totalFish, totalMoney);
    return 1;
}
