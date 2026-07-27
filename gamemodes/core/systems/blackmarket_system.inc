/*
    blackmarket_system.inc
    ----------------------
    Hệ thống Chợ Đen & Chế Tạo (Black Market & Crafting) cho Server RP.

    Tính năng:
    - Black Market NPC locations bí mật (random mỗi restart).
    - Mua vũ khí, đạn, đồ bất hợp pháp tại chợ đen.
    - Crafting: Chế tạo ma túy từ nguyên liệu.
    - /buybm [item] [so]  – Mua đồ chợ đen bằng tiền mặt.
    - /craftdrug [type]   – Chế tạo ma túy (cần nguyên liệu trong túi).
    - /bmlocate           – Tìm vị trí chợ đen (chỉ dân thường gang).
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define BM_LOCATE_RADIUS    (8.0)   // Bán kính để tương tác với NPC

// Giá chợ đen ($ mỗi đơn vị)
#define BM_PRICE_PISTOL         (8000)
#define BM_PRICE_SHOTGUN        (15000)
#define BM_PRICE_AK47           (25000)
#define BM_PRICE_M4             (30000)
#define BM_PRICE_AMMO_PISTOL    (50)
#define BM_PRICE_AMMO_RIFLE     (80)
#define BM_PRICE_CRACK          (2000)
#define BM_PRICE_WEED           (1500)
#define BM_PRICE_HEROIN         (3500)
#define BM_PRICE_C4             (20000)

// =============================================================================
// BLACK MARKET LOCATIONS (Random mỗi restart)
// =============================================================================

static const Float:BMLocations[][3] = {
    {1215.6, -2093.3, 17.2},    // LS – Garage ngầm
    {448.2,  -1721.7, 17.3},    // LS – Kho bỏ hoang
    {-133.5, -1212.5, 2.3},     // LS – Bãi
    {2397.5, 2505.9,  10.8},    // Bone County
    {-1639.4,-473.6,  14.5},    // Flint County
    {2183.4, -2399.1, 13.5},    // Ocean Docks
    {-2243.5,-399.2,  36.1}     // SF countryside
};

static
    Float:BMPosX,
    Float:BMPosY,
    Float:BMPosZ,
    BMPickup; // Pickup ID

// Cooldown chế tạo ma túy (5 phút)
static CraftCooldown[MAX_PLAYERS];
#define CRAFT_COOLDOWN (300)

// =============================================================================
// HOOK – OnGameModeInit: Đặt chợ đen random
// =============================================================================

hook OnGameModeInit()
{
    BM_PlaceRandom();
    return 1;
}

static BM_PlaceRandom()
{
    new idx = random(sizeof(BMLocations));
    BMPosX = BMLocations[idx][0];
    BMPosY = BMLocations[idx][1];
    BMPosZ = BMLocations[idx][2];

    // Tạo pickup icon (biểu tượng túi tiền)
    if (BMPickup) DestroyPickup(BMPickup);
    BMPickup = CreatePickup(1212, 1, BMPosX, BMPosY, BMPosZ, -1);

    Logger_Log("[blackmarket_system.inc] Black market placed at random location.");
}

// =============================================================================
// HOOK – OnPlayerPickUpPickup
// =============================================================================

hook OnPlayerPickUpPickup(playerid, pickupid)
{
    if (pickupid != BMPickup) return 1;

    if (!Player_IsLoggedIn(playerid)) return 1;
    if (Player_GetClass(playerid) != TEAM_CIVILIAN) return 1;

    BM_ShowMenu(playerid);
    return 1;
}

// =============================================================================
// MENU CHỢ ĐEN
// =============================================================================

static BM_ShowMenu(playerid)
{
    static const content[] =
        "Pistol (Sung Ngan)\t$8,000\n"
        "Shotgun (Sung Luoc)\t$15,000\n"
        "AK-47 (Sung Truong)\t$25,000\n"
        "M4 (Sung Truong)\t$30,000\n"
        "Dan Sung Ngan (x50)\t$2,500\n"
        "Dan Truong (x30)\t$2,400\n"
        "Crack Cocaine (x1)\t$2,000\n"
        "Co Duoc - Weed (x1)\t$1,500\n"
        "Heroin (x1)\t$3,500\n"
        "Bo Thu No C4 (x1)\t$20,000"
    ;

    inline OnBMSelect(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext
        if (!response) return;
        BM_ProcessPurchase(playerid, listitem);
    }

    Dialog_ShowCallback(playerid, using inline OnBMSelect,
        DIALOG_STYLE_TABLIST_HEADERS,
        "Cho Den",
        "Vat Pham\tGia\n" ... content,
        "Mua", "Dong");
}

static BM_ProcessPurchase(playerid, itemIdx)
{
    static const prices[] = {
        BM_PRICE_PISTOL, BM_PRICE_SHOTGUN, BM_PRICE_AK47, BM_PRICE_M4,
        BM_PRICE_AMMO_PISTOL * 50, BM_PRICE_AMMO_RIFLE * 30,
        BM_PRICE_CRACK, BM_PRICE_WEED, BM_PRICE_HEROIN, BM_PRICE_C4
    };

    new price = prices[itemIdx];
    if (GetPlayerMoney(playerid) < price)
    {
        SendErrorMsgF(playerid, "Khong du tien! Can $%d.", price);
        return;
    }

    GivePlayerMoney(playerid, -price);

    switch (itemIdx)
    {
        case 0: GivePlayerWeapon(playerid, 22, 50);    // Pistol
        case 1: GivePlayerWeapon(playerid, 25, 30);    // Shotgun
        case 2: GivePlayerWeapon(playerid, 30, 30);    // AK-47
        case 3: GivePlayerWeapon(playerid, 31, 30);    // M4
        case 4: GivePlayerWeapon(playerid, 22, 50);    // Ammo pistol
        case 5: GivePlayerWeapon(playerid, 30, 30);    // Ammo rifle
        case 6: Inv_AddItem(playerid, "crack");
        case 7: Inv_AddItem(playerid, "weed");
        case 8: Inv_AddItem(playerid, "heroin");
        case 9: Inv_AddItem(playerid, "c4");
    }

    static const itemNames[][] = {
        "Pistol", "Shotgun", "AK-47", "M4",
        "Dan Sung Ngan x50", "Dan Truong x30",
        "Crack Cocaine", "Co Duoc", "Heroin", "C4"
    };

    SendMsgF(playerid, COLOR_GREEN, "[CHO DEN] Da mua %s (-$%d).", itemNames[itemIdx], price);
}

// =============================================================================
// COMMAND – /bmlocate – Tìm chợ đen (hint direction)
// =============================================================================

CMD:bmlocate(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (Player_GetClass(playerid) != TEAM_CIVILIAN)
        return SendErrorMsg(playerid, "Lenh nay chi danh cho dan thuong."), 1;

    // Gang member mới dùng được (hoặc là civilian có join gang)
    new gangid = Gang_GetPlayerGang(playerid);
    if (gangid == -1)
        return SendErrorMsg(playerid, "Ban can gia nhap bang nhom truoc de biet vi tri cho den."), 1;

    // Đặt checkpoint dẫn đến chợ đen
    SetPlayerCheckpoint(playerid, BMPosX, BMPosY, BMPosZ, 5.0);
    SendMsgF(playerid, COLOR_ORANGE, "[CHO DEN] Checkpoint da duoc dat. Di theo ban do (checkpoint tren minimap).");
    return 1;
}

// =============================================================================
// COMMAND – /craftdrug [type] – Chế tạo ma túy
// =============================================================================

CMD:craftdrug(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (Player_GetClass(playerid) != TEAM_CIVILIAN)
        return SendErrorMsg(playerid, "Chi dan thuong moi duoc craft ma tuy."), 1;

    if (isnull(params))
    {
        // Show menu
        Dialog_Show(playerid, DIALOG_STYLE_LIST,
            "Che Tao Ma Tuy",
            "Crack Cocaine\t(Can 3 Weed + $500)\n"
            "Heroin\t(Can 5 Weed + $1000)",
            "Che tao", "Dong");
        return 1;
    }

    // Cooldown
    if (gettime() < CraftCooldown[playerid])
    {
        new rem = CraftCooldown[playerid] - gettime();
        return SendErrorMsgF(playerid, "Hay doi %d giay truoc khi craft tiep.", rem), 1;
    }

    new type[16];
    sscanf(params, "s[16]", type);

    if (!strcmp(type, "crack", true))
    {
        if (!Inv_HasItem(playerid, "weed", 3))
            return SendErrorMsg(playerid, "Can 3 Co Duoc (weed) trong tui de craft Crack!"), 1;
        if (GetPlayerMoney(playerid) < 500)
            return SendErrorMsg(playerid, "Can $500 de mua nguyen lieu hoa chat!"), 1;

        Inv_RemoveItem(playerid, "weed", 3);
        GivePlayerMoney(playerid, -500);
        Inv_AddItem(playerid, "crack");
        CraftCooldown[playerid] = gettime() + CRAFT_COOLDOWN;
        SendMsgF(playerid, COLOR_GREEN, "[CRAFT] Da che tao Crack Cocaine x1 thanh cong.");
    }
    else if (!strcmp(type, "heroin", true))
    {
        if (!Inv_HasItem(playerid, "weed", 5))
            return SendErrorMsg(playerid, "Can 5 Co Duoc (weed) de craft Heroin!"), 1;
        if (GetPlayerMoney(playerid) < 1000)
            return SendErrorMsg(playerid, "Can $1000 de mua nguyen lieu!"), 1;

        Inv_RemoveItem(playerid, "weed", 5);
        GivePlayerMoney(playerid, -1000);
        Inv_AddItem(playerid, "heroin");
        CraftCooldown[playerid] = gettime() + CRAFT_COOLDOWN;
        SendMsgF(playerid, COLOR_GREEN, "[CRAFT] Da che tao Heroin x1 thanh cong.");
    }
    else
    {
        SendErrorMsg(playerid, "Loai ma tuy khong hop le. Su dung: crack | heroin");
    }

    return 1;
}
