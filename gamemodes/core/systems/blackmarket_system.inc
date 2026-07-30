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
        "Pistol (Sung Ngan)\t$8,000\n\
Shotgun (Sung Luoc)\t$15,000\n\
AK-47 (Sung Truong)\t$25,000\n\
M4 (Sung Truong)\t$30,000\n\
Dan Sung Ngan (x50)\t$2,500\n\
Dan Truong (x30)\t$2,400\n\
Crack Cocaine (x1)\t$2,000\n\
Co Duoc - Weed (x1)\t$1,500\n\
Heroin (x1)\t$3,500\n\
Bo Thu No C4 (x1)\t$20,000"
    ;

    inline OnBMSelect(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext
        if (!response) return 1;
        BM_ProcessPurchase(playerid, listitem);
        return 1;
    }

    new menuContent[512];
    format(menuContent, sizeof(menuContent), "Vat Pham\tGia\n%s", content);

    Dialog_ShowCallback(playerid, using inline OnBMSelect,
        DIALOG_STYLE_TABLIST_HEADERS,
        "Cho Den",
        menuContent,
        "Mua", "Dong");
    return 1;
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
        return 1;
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
    return 1;
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
            "Crack Cocaine\t(Can 3 Weed + $500)\n\
Heroin\t(Can 5 Weed + $1000)",
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

// =============================================================================
// WEED PLANTING SYSTEM
// =============================================================================

#define MAX_WEED_PLANT_PER_PLAYER	(5)

static
	weedObject[MAX_PLAYERS][MAX_WEED_PLANT_PER_PLAYER],
	weedPercent[MAX_PLAYERS][MAX_WEED_PLANT_PER_PLAYER],
	Text3D:weedLabel[MAX_PLAYERS][MAX_WEED_PLANT_PER_PLAYER],
	Float:weedObjectPosX[MAX_PLAYERS][MAX_WEED_PLANT_PER_PLAYER],
	Float:weedObjectPosY[MAX_PLAYERS][MAX_WEED_PLANT_PER_PLAYER],
	Float:weedObjectPosZ[MAX_PLAYERS][MAX_WEED_PLANT_PER_PLAYER],
	Iterator:weedIndex[MAX_PLAYERS]<MAX_WEED_PLANT_PER_PLAYER>,
	Timer:weedTimer[MAX_PLAYERS][MAX_WEED_PLANT_PER_PLAYER];

hook OnPlayerDisconnect(playerid, reason)
{
	for(new i = 0; i < MAX_WEED_PLANT_PER_PLAYER; i++) {
		Weed_ResetData(playerid, i);
	}
	return 1;
}

stock Weed_Create(playerid, Float:x, Float:y, Float:z)
{
	new slot = Iter_Free(weedIndex[playerid]);

	if (Iter_Count(weedIndex[playerid]) == MAX_WEED_PLANT_PER_PLAYER)
		return SendErrorMsg(playerid, "Ban da trong toi da so cay can sa cho phep!");

	weedPercent[playerid][slot] 	= 0;
	weedObjectPosX[playerid][slot] 	= x;
	weedObjectPosY[playerid][slot] 	= y;
	weedObjectPosZ[playerid][slot] 	= z;

	weedObject[playerid][slot] 		= CreateDynamicObject(19473, x, y, z - 2.2, 0, 0, 0);
	weedLabel[playerid][slot] 		= CreateDynamic3DTextLabelEx("[CAY CAN SA]\n"C_GREY"0% "C_WHITE"tren "C_GREY"100%", COLOR_YELLOW, x, y, z, 100.0, .testlos = 0);

	weedTimer[playerid][slot] 		= repeat OnWeedGrow(playerid, slot);

	Iter_Add(weedIndex[playerid], slot);
	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.0, false , false, false, false, 0, SYNC_ALL);

	SendMsgF(playerid, COLOR_GREEN, "** CAY CAN SA ** Ban da trong can sa - Con co the trong them %i cay.", MAX_WEED_PLANT_PER_PLAYER - Iter_Count(weedIndex[playerid]));
	return slot;
}

stock Weed_ResetData(playerid, slot)
{
	DestroyDynamic3DTextLabel(weedLabel[playerid][slot]);
	DestroyDynamicObject(weedObject[playerid][slot]);

	weedObjectPosX[playerid][slot]		= 0.0;
	weedObjectPosY[playerid][slot]		= 0.0;
	weedObjectPosZ[playerid][slot]		= 0.0;

	weedObject[playerid][slot] 			= INVALID_OBJECT_ID;
	weedLabel[playerid][slot] 			= Text3D:INVALID_3DTEXT_ID;
	weedPercent[playerid][slot] 		= -1;

	Iter_Remove(weedIndex[playerid], slot);
}

stock Weed_GetClosest(playerid)
{
	foreach(new i : weedIndex[playerid]) {
		if(IsPlayerInRangeOfPoint(playerid, 2.0, weedObjectPosX[playerid][i], weedObjectPosY[playerid][i], weedObjectPosZ[playerid][i])) {
			return i;
		}
	}
	return -1;
}

stock Weed_GetPercent(playerid, slot)
{
	if (slot < 0 || slot >= MAX_WEED_PLANT_PER_PLAYER) return -1;
	return weedPercent[playerid][slot];
}

timer OnWeedGrow[30000](playerid, slot)
{
	if(weedPercent[playerid][slot] >= 100){
		UpdateDynamic3DTextLabelText(weedLabel[playerid][slot], COLOR_YELLOW, "[CAY CAN SA]\n"C_WHITE"Da san sang thu hoach!");
		stop weedTimer[playerid][slot];
		return 1;
	}

	new Float:null, Float:z;
	GetDynamicObjectPos(weedObject[playerid][slot], null, null, z);
	weedObjectPosZ[playerid][slot] = z + 0.0666;

	SetDynamicObjectPos(weedObject[playerid][slot],
		weedObjectPosX[playerid][slot],
		weedObjectPosY[playerid][slot],
		weedObjectPosZ[playerid][slot]);

	weedPercent[playerid][slot] += 10;
	new string[64];
	format(string, sizeof(string), "[CAY CAN SA]\n"C_GREY"%d%% "C_WHITE"tren "C_GREY"100%%", weedPercent[playerid][slot]);
	UpdateDynamic3DTextLabelText(weedLabel[playerid][slot], COLOR_YELLOW, string);
	return 1;
}
