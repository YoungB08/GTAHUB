/*
    player_inventory.inc
    ---------------------
    Hệ thống Túi Đồ RP dựa trên Slot & Trọng lượng.

    Thiết kế:
    - Mỗi player có MAX_INV_SLOTS (12) slot trong túi.
    - Tổng trọng lượng tối đa: MAX_INV_WEIGHT (15.0 kg).
    - Vật phẩm được định nghĩa trong ItemDefinitions[].
    - Hỗ trợ: Nhặt (pickup), Thả (drop), Dùng (use), Đưa (give), Xem (inspect).
    - Kho cốp xe (Vehicle Trunk) dùng bảng vehicle_trunk.
    - Lệnh:
        /inv           – Mở túi đồ
        /drop [item] [số] – Thả vật phẩm ra đất
        /give [id] [item] [số] – Đưa vật phẩm cho người khác
        /trunk         – Mở/đóng cốp xe
        /puttrunk [item] [số] – Cất đồ vào cốp
        /taketrunk [item] [số] – Lấy đồ từ cốp
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define MAX_INV_SLOTS       (12)
#define MAX_INV_WEIGHT      (15.0)  // kg
#define MAX_TRUNK_SLOTS     (20)
#define INV_GIVE_RADIUS     (5.0)   // Bán kính để đưa đồ
#define INV_TRUNK_RADIUS    (4.0)   // Bán kính mở cốp xe

// =============================================================================
// ITEM DEFINITIONS (Registry trung tâm)
// =============================================================================

#define MAX_ITEM_TYPES  (30)

enum E_ITEM_DEF {
    E_ITEM_SLUG[32],       // Key định danh (slug)
    E_ITEM_LABEL[32],      // Tên hiển thị
    Float:E_ITEM_WEIGHT,   // Trọng lượng / 1 đơn vị (kg)
    bool:E_ITEM_ILLEGAL,   // Có phải đồ bất hợp pháp
    E_ITEM_MAX_STACK       // Số lượng tối đa / stack
}

static const ItemDefs[MAX_ITEM_TYPES][E_ITEM_DEF] = {
    // Vật phẩm thường
    { "bandage",      "Bang Bong",          0.2,  false, 10 },
    { "food",         "Do An",              0.5,  false, 10 },
    { "water",        "Nuoc Uong",          0.5,  false, 10 },
    { "painkillers",  "Thuoc Giam Dau",     0.1,  false, 5  },
    { "lockpick",     "Chia Khoa Pha",      0.1,  false, 3  },
    { "phone",        "Dien Thoai",         0.2,  false, 1  },
    { "wallet",       "Vi Tien",            0.1,  false, 1  },
    { "rope",         "Day Thua",           0.5,  false, 2  },
    { "cuffs",        "Coi Tay",            0.5,  false, 1  },
    // Bất hợp pháp
    { "crack",        "Crack Cocaine",      0.2,  true,  10 },
    { "weed",         "Co Duoc",            0.3,  true,  10 },
    { "heroin",       "Heroin",             0.1,  true,  5  },
    { "c4",           "Bo Thu No C4",       1.0,  true,  2  },
    { "ammo_pistol",  "Dan Sung Ngan",      0.3,  false, 50 },
    { "ammo_rifle",   "Dan Truong",         0.5,  false, 30 },
    // Đồ nghề
    { "toolbox",      "Hop Do Nghe",        2.0,  false, 1  },
    { "medkit",       "Bo So Cuu",          1.5,  false, 1  },
    { "binoculars",   "Ong Nhom",           0.5,  false, 1  },
    { "radio",        "May Bo Dam",         0.3,  false, 1  },
    { "jerry_can",    "Can Xang Du Phong",  3.0,  false, 1  },
    // Nông nghiệp
    { "oranges",      "Qua Cam",            0.2,  false, 20 },
    { "bottles",      "Chai Thuy Tinh",     0.5,  false, 10 },
    { "orange_juice", "Nuoc Cam",           0.5,  false, 10 },
    { "fruit_picker", "Rac Hai Qua",        0.5,  false, 1  },
    // Đồ ăn/thức uống bán tại cửa hàng 24/7 (business_economy.inc, player_survival.inc)
    { "burger",       "Hamburger",          0.5,  false, 10 },
    { "pizza",        "Pizza",              0.6,  false, 10 },
    { "nuoc",         "Chai Nuoc Suoi",     0.5,  false, 10 },
    { "beer",         "Bia",                0.5,  false, 10 },
    // Câu cá (fishing_system.inc)
    { "fish_small",   "Ca Trich (Nho)",     0.3,  false, 20 },
    { "rod",          "Can Cau Ca",         1.0,  false, 1  }
};

// =============================================================================
// DATA
// =============================================================================

enum E_INV_SLOT {
    E_SLOT_NAME[32],
    E_SLOT_LABEL[32],
    E_SLOT_QTY,
    Float:E_SLOT_WEIGHT,
    bool:E_SLOT_ILLEGAL
}

static PlayerInv[MAX_PLAYERS][MAX_INV_SLOTS][E_INV_SLOT];
static PlayerInvSlots[MAX_PLAYERS];   // Số slot đang dùng

static TrunkInv[MAX_PLAYERS][MAX_TRUNK_SLOTS][E_INV_SLOT];
static TrunkSlots[MAX_PLAYERS];   // Slot cốp xe đang mở
#pragma unused TrunkSlots
static TrunkVehicleID[MAX_PLAYERS]; // Xe đang mở cốp (-1 nếu không)

// Statements
static
    Statement:stmt_loadInv,
    Statement:stmt_insertInv,
    Statement:stmt_removeInv,
    Statement:stmt_updateInvQty,
    Statement:stmt_clearInv;
#pragma unused stmt_removeInv
#pragma unused stmt_updateInvQty

// =============================================================================
// HOOKS – MYSQL
// =============================================================================

hook OnMySQLConnected()
{
    static const loadQ[] = "\
    SELECT item_name, item_label, quantity, weight, is_illegal \
    FROM player_inventory WHERE u_id = ?";
    stmt_loadInv = MySQL_PrepareStatement(MySQL_GetHandle(), loadQ);

    static const insertQ[] = "\
    INSERT INTO player_inventory (u_id, item_name, item_label, quantity, weight, is_illegal) \
    VALUES (?, ?, ?, ?, ?, ?)";
    stmt_insertInv = MySQL_PrepareStatement(MySQL_GetHandle(), insertQ);

    static const removeQ[] = "\
    DELETE FROM player_inventory WHERE u_id=? AND item_name=? LIMIT 1";
    stmt_removeInv = MySQL_PrepareStatement(MySQL_GetHandle(), removeQ);

    static const updateQ[] = "\
    UPDATE player_inventory SET quantity=? WHERE u_id=? AND item_name=?";
    stmt_updateInvQty = MySQL_PrepareStatement(MySQL_GetHandle(), updateQ);

    static const clearQ[] = "DELETE FROM player_inventory WHERE u_id=?";
    stmt_clearInv = MySQL_PrepareStatement(MySQL_GetHandle(), clearQ);

    return 1;
}

// =============================================================================
// HOOKS – PLAYER
// =============================================================================

hook OnPlayerConnect(playerid)
{
    Inv_ResetPlayer(playerid);
    TrunkVehicleID[playerid] = -1;
    return 1;
}

hook OnPlayerLogin(playerid)
{
    Inv_Load(playerid);
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    Inv_Save(playerid);
    return 1;
}

hook OnPlayerAutoSave(playerid)
{
    Inv_Save(playerid);
    return 1;
}

// =============================================================================
// INTERNAL – RESET
// =============================================================================

static Inv_ResetPlayer(playerid)
{
    PlayerInvSlots[playerid] = 0;
    for (new i = 0; i < MAX_INV_SLOTS; i++)
    {
        PlayerInv[playerid][i][E_SLOT_NAME][0] = '\0';
        PlayerInv[playerid][i][E_SLOT_LABEL][0] = '\0';
        PlayerInv[playerid][i][E_SLOT_QTY] = 0;
        PlayerInv[playerid][i][E_SLOT_WEIGHT] = 0.0;
        PlayerInv[playerid][i][E_SLOT_ILLEGAL] = false;
    }
}

// =============================================================================
// INTERNAL – LOAD
// =============================================================================

static Inv_Load(playerid)
{
    inline OnInvLoad()
    {
        new name[32], label[32], qty, illegal;
        new Float:weight;
        MySQL_BindResultString(stmt_loadInv, 0, name, 32);
        MySQL_BindResultString(stmt_loadInv, 1, label, 32);
        MySQL_BindResultInt(stmt_loadInv,   2, qty);
        MySQL_BindResultFloat(stmt_loadInv, 3, weight);
        MySQL_BindResultInt(stmt_loadInv,   4, illegal);

        new idx = 0;
        while (MySQL_Statement_FetchRow(stmt_loadInv) && idx < MAX_INV_SLOTS)
        {
            strcopy(PlayerInv[playerid][idx][E_SLOT_NAME],  name, 32);
            strcopy(PlayerInv[playerid][idx][E_SLOT_LABEL], label, 32);
            PlayerInv[playerid][idx][E_SLOT_QTY]     = qty;
            PlayerInv[playerid][idx][E_SLOT_WEIGHT]  = weight;
            PlayerInv[playerid][idx][E_SLOT_ILLEGAL] = bool:illegal;
            idx++;

            MySQL_BindResultString(stmt_loadInv, 0, name, 32);
            MySQL_BindResultString(stmt_loadInv, 1, label, 32);
            MySQL_BindResultInt(stmt_loadInv,   2, qty);
            MySQL_BindResultFloat(stmt_loadInv, 3, weight);
            MySQL_BindResultInt(stmt_loadInv,   4, illegal);
        }
        PlayerInvSlots[playerid] = idx;
    }

    MySQL_BindInt(stmt_loadInv, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_loadInv, using inline OnInvLoad);
}

// =============================================================================
// INTERNAL – SAVE (clear + re-insert)
// =============================================================================

static Inv_Save(playerid)
{
    new uid = Player_GetAccountID(playerid);
    if (uid <= 0) return;

    // Clear cũ
    MySQL_BindInt(stmt_clearInv, 0, uid);
    MySQL_ExecuteThreaded(stmt_clearInv);

    // Insert lại
    for (new i = 0; i < PlayerInvSlots[playerid]; i++)
    {
        if (PlayerInv[playerid][i][E_SLOT_QTY] <= 0) continue;
        MySQL_BindInt(stmt_insertInv, 0, uid);
        MySQL_BindString(stmt_insertInv, 1, PlayerInv[playerid][i][E_SLOT_NAME], 32);
        MySQL_BindString(stmt_insertInv, 2, PlayerInv[playerid][i][E_SLOT_LABEL], 32);
        MySQL_BindInt(stmt_insertInv, 3, PlayerInv[playerid][i][E_SLOT_QTY]);
        MySQL_BindFloat(stmt_insertInv, 4, PlayerInv[playerid][i][E_SLOT_WEIGHT]);
        MySQL_BindInt(stmt_insertInv, 5, _:PlayerInv[playerid][i][E_SLOT_ILLEGAL]);
        MySQL_ExecuteThreaded(stmt_insertInv);
    }
}

// =============================================================================
// HELPERS – Item lookup / weight / slot finding
// =============================================================================

static GetItemDefIdx(const slug[])
{
    for (new i = 0; i < MAX_ITEM_TYPES; i++)
    {
        if (!strcmp(ItemDefs[i][E_ITEM_SLUG], slug, true))
            return i;
    }
    return -1;
}

static Float:Inv_GetTotalWeight(playerid)
{
    new Float:total = 0.0;
    for (new i = 0; i < MAX_INV_SLOTS; i++)
    {
        if (PlayerInv[playerid][i][E_SLOT_QTY] <= 0) continue;
        total += PlayerInv[playerid][i][E_SLOT_WEIGHT] * float(PlayerInv[playerid][i][E_SLOT_QTY]);
    }
    return total;
}

static Inv_FindSlot(playerid, const slug[])
{
    for (new i = 0; i < MAX_INV_SLOTS; i++)
    {
        if (!strcmp(PlayerInv[playerid][i][E_SLOT_NAME], slug, true))
            return i;
    }
    return -1;
}

static Inv_FindEmptySlot(playerid)
{
    for (new i = 0; i < MAX_INV_SLOTS; i++)
    {
        if (PlayerInv[playerid][i][E_SLOT_QTY] <= 0)
            return i;
    }
    return -1;
}

// =============================================================================
// PUBLIC API – Add / Remove item
// =============================================================================

stock bool:Inv_AddItem(playerid, const slug[], qty = 1)
{
    new defIdx = GetItemDefIdx(slug);
    if (defIdx == -1) return false;

    // Kiểm tra trọng lượng
    new Float:addWeight = ItemDefs[defIdx][E_ITEM_WEIGHT] * float(qty);
    if (Inv_GetTotalWeight(playerid) + addWeight > MAX_INV_WEIGHT)
    {
        SendErrorMsgF(playerid, "Tui do qua nang! Toi da %.1f kg.", MAX_INV_WEIGHT);
        return false;
    }

    // Tìm slot có slug này (stack)
    new slotIdx = Inv_FindSlot(playerid, slug);
    if (slotIdx != -1)
    {
        // Stack vào slot hiện có nếu chưa đạt max
        if (PlayerInv[playerid][slotIdx][E_SLOT_QTY] + qty <= ItemDefs[defIdx][E_ITEM_MAX_STACK])
        {
            PlayerInv[playerid][slotIdx][E_SLOT_QTY] += qty;
            return true;
        }
    }

    // Slot mới
    slotIdx = Inv_FindEmptySlot(playerid);
    if (slotIdx == -1)
    {
        SendErrorMsg(playerid, "Tui do da day! Hay bot do truoc.");
        return false;
    }

    strcopy(PlayerInv[playerid][slotIdx][E_SLOT_NAME],  ItemDefs[defIdx][E_ITEM_SLUG],  32);
    strcopy(PlayerInv[playerid][slotIdx][E_SLOT_LABEL], ItemDefs[defIdx][E_ITEM_LABEL], 32);
    PlayerInv[playerid][slotIdx][E_SLOT_QTY]     = qty;
    PlayerInv[playerid][slotIdx][E_SLOT_WEIGHT]  = ItemDefs[defIdx][E_ITEM_WEIGHT];
    PlayerInv[playerid][slotIdx][E_SLOT_ILLEGAL] = ItemDefs[defIdx][E_ITEM_ILLEGAL];

    if (slotIdx + 1 > PlayerInvSlots[playerid])
        PlayerInvSlots[playerid] = slotIdx + 1;

    return true;
}

stock bool:Inv_RemoveItem(playerid, const slug[], qty = 1)
{
    new slotIdx = Inv_FindSlot(playerid, slug);
    if (slotIdx == -1) return false;

    if (PlayerInv[playerid][slotIdx][E_SLOT_QTY] < qty) return false;

    PlayerInv[playerid][slotIdx][E_SLOT_QTY] -= qty;
    if (PlayerInv[playerid][slotIdx][E_SLOT_QTY] <= 0)
    {
        PlayerInv[playerid][slotIdx][E_SLOT_NAME][0] = '\0';
        PlayerInv[playerid][slotIdx][E_SLOT_LABEL][0] = '\0';
        PlayerInv[playerid][slotIdx][E_SLOT_QTY] = 0;
        PlayerInv[playerid][slotIdx][E_SLOT_WEIGHT] = 0.0;
        PlayerInv[playerid][slotIdx][E_SLOT_ILLEGAL] = false;
    }
    return true;
}

stock Inv_GetItemCount(playerid, const slug[])
{
    new slotIdx = Inv_FindSlot(playerid, slug);
    if (slotIdx == -1) return 0;
    return PlayerInv[playerid][slotIdx][E_SLOT_QTY];
}

stock bool:Inv_HasItem(playerid, const slug[], qty = 1)
    return Inv_GetItemCount(playerid, slug) >= qty;

// =============================================================================
// COMMAND – /inv – Hiện túi đồ dạng List
// =============================================================================

CMD:inv(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new content[1024], line[128];
    new Float:totalW = Inv_GetTotalWeight(playerid);
    new itemCount = 0;

    for (new i = 0; i < MAX_INV_SLOTS; i++)
    {
        if (PlayerInv[playerid][i][E_SLOT_QTY] <= 0) continue;
        format(line, sizeof(line), "%s  x%d\t%.1f kg%s\n",
            PlayerInv[playerid][i][E_SLOT_LABEL],
            PlayerInv[playerid][i][E_SLOT_QTY],
            PlayerInv[playerid][i][E_SLOT_WEIGHT] * float(PlayerInv[playerid][i][E_SLOT_QTY]),
            PlayerInv[playerid][i][E_SLOT_ILLEGAL] ? " [!]" : ""
        );
        strcat(content, line, sizeof(content));
        itemCount++;
    }

    if (itemCount == 0)
        content = "{AFAFAF}Tui do trong rong.\n\nHay nhat do len de co vat pham.";

    new header[64];
    format(header, sizeof(header), "Tui Do  (%.1f / %.1f kg)", totalW, MAX_INV_WEIGHT);

    Dialog_Show(playerid, DIALOG_STYLE_LIST, header, content, "Dong", "");
    return 1;
}

// Alias
ALTCMD:inventory(inv);

// =============================================================================
// COMMAND – /drop [item] [so] – Thả đồ xuống đất
// =============================================================================

CMD:drop(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new slug[32];
    new qty = 1;

    if (sscanf(params, "s[32]I(1)i", slug, qty))
        return SendErrorMsg(playerid, "Su dung: /drop [ten vat pham] [so luong]"), 1;

    if (!Inv_HasItem(playerid, slug, qty))
        return SendErrorMsgF(playerid, "Ban khong co du '%s' (can %d).", slug, qty), 1;

    Inv_RemoveItem(playerid, slug, qty);

    new label[32];
    new defIdx = GetItemDefIdx(slug);
    if (defIdx != -1)
        strcopy(label, ItemDefs[defIdx][E_ITEM_LABEL], 32);
    else
        strcopy(label, slug, 32);

    new msg[96];
    format(msg, sizeof(msg), "* %s tha %s x%d xuong dat.", ReturnPlayerName(playerid), label, qty);

    // Gửi local message
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);
    foreach (new i : Player)
    {
        if (IsPlayerInRangeOfPoint(i, 20.0, px, py, pz))
            SendClientMessage(i, 0xC2A2DAFF, msg);
    }
    return 1;
}

// =============================================================================
// COMMAND – /give [playerid] [item] [so] – Đưa đồ cho người khác
// =============================================================================

CMD:giveitem2(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new targetid, slug[32], qty;
    if (sscanf(params, "us[32]i", targetid, slug, qty))
        return SendErrorMsg(playerid, "Su dung: /giveitem2 [playerid] [ten vat pham] [so luong]"), 1;

    if (!IsPlayerConnected(targetid) || targetid == playerid)
        return SendErrorMsg(playerid, "Nguoi choi khong hop le."), 1;

    if (!Player_IsNearPlayer(playerid, targetid, INV_GIVE_RADIUS))
        return SendErrorMsg(playerid, "Nguoi nay qua xa! Hay lai gan hon (5m)."), 1;

    if (!Inv_HasItem(playerid, slug, qty))
        return SendErrorMsgF(playerid, "Ban khong du '%s' de dua.", slug), 1;

    if (!Inv_AddItem(targetid, slug, qty))
        return SendErrorMsg(playerid, "Tui do nguoi nhan qua nang hoac day!"), 1;

    Inv_RemoveItem(playerid, slug, qty);

    new defIdx = GetItemDefIdx(slug);
    new label[32];
    if (defIdx != -1) strcopy(label, ItemDefs[defIdx][E_ITEM_LABEL], 32);
    else strcopy(label, slug, 32);

    SendMsgF(playerid, COLOR_GREEN, "Da dua %s x%d cho %s.", label, qty, ReturnPlayerName(targetid));
    SendMsgF(targetid, COLOR_GREEN, "%s da dua cho ban %s x%d.", ReturnPlayerName(playerid), label, qty);
    return 1;
}

// =============================================================================
// COMMAND – /trunk – Mở cốp xe gần nhất
// =============================================================================

CMD:trunk(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    // Tìm xe cá nhân gần nhất mà mình sở hữu hoặc có chìa khóa
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    new nearVeh = -1;
    new Float:minD = INV_TRUNK_RADIUS;

    for (new vehicleid = 1; vehicleid < MAX_VEHICLES; vehicleid++)
    {
        if (!IsValidVehicle(vehicleid) || !Vehicle_IsOwner(playerid, vehicleid)) continue;
        new Float:vx, Float:vy, Float:vz;
        GetVehiclePos(vehicleid, vx, vy, vz);
        new Float:d = Distance(px, py, pz, vx, vy, vz);
        if (d < minD) { minD = d; nearVeh = vehicleid; }
    }

    if (nearVeh == -1)
        return SendErrorMsg(playerid, "Khong tim thay xe ca nhan cua ban trong pham vi 4m."), 1;

    // Hiển thị nội dung cốp (từ TrunkInv nếu đang mở, hoặc cần load DB)
    TrunkVehicleID[playerid] = nearVeh;
    Inv_ShowTrunkDialog(playerid);
    return 1;
}

static Inv_ShowTrunkDialog(playerid)
{
    new content[512], line[96];
    new itemCount = 0;

    for (new i = 0; i < MAX_TRUNK_SLOTS; i++)
    {
        if (TrunkInv[playerid][i][E_SLOT_QTY] <= 0) continue;
        format(line, sizeof(line), "%s  x%d\n", TrunkInv[playerid][i][E_SLOT_LABEL], TrunkInv[playerid][i][E_SLOT_QTY]);
        strcat(content, line, sizeof(content));
        itemCount++;
    }

    if (itemCount == 0)
        content = "{AFAFAF}Cop xe trong rong.";

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX,
        "Cop Xe",
        content, "Dong", "");
}

// =============================================================================
// COMMAND – /puttrunk [item] [so]
// =============================================================================

CMD:puttrunk(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (TrunkVehicleID[playerid] == -1)
        return SendErrorMsg(playerid, "Hay mo cop xe truoc bang /trunk."), 1;

    new slug[32], qty;
    if (sscanf(params, "s[32]I(1)i", slug, qty))
        return SendErrorMsg(playerid, "Su dung: /puttrunk [ten vat pham] [so luong]"), 1;

    if (!Inv_HasItem(playerid, slug, qty))
        return SendErrorMsgF(playerid, "Ban khong co du '%s'.", slug), 1;

    // Tìm slot trong trunk
    new emptySlot = -1;
    for (new i = 0; i < MAX_TRUNK_SLOTS; i++)
    {
        if (!strcmp(TrunkInv[playerid][i][E_SLOT_NAME], slug, true))
        {
            TrunkInv[playerid][i][E_SLOT_QTY] += qty;
            Inv_RemoveItem(playerid, slug, qty);
            SendMsgF(playerid, COLOR_GREEN, "Da cat %s x%d vao cop xe.", slug, qty);
            return 1;
        }
        if (TrunkInv[playerid][i][E_SLOT_QTY] <= 0 && emptySlot == -1)
            emptySlot = i;
    }

    if (emptySlot == -1)
        return SendErrorMsg(playerid, "Cop xe da day!"), 1;

    new defIdx = GetItemDefIdx(slug);
    new label[32];
    if (defIdx != -1) strcopy(label, ItemDefs[defIdx][E_ITEM_LABEL], 32);
    else strcopy(label, slug, 32);

    strcopy(TrunkInv[playerid][emptySlot][E_SLOT_NAME], slug, 32);
    strcopy(TrunkInv[playerid][emptySlot][E_SLOT_LABEL], label, 32);
    TrunkInv[playerid][emptySlot][E_SLOT_QTY] = qty;
    Inv_RemoveItem(playerid, slug, qty);

    SendMsgF(playerid, COLOR_GREEN, "Da cat %s x%d vao cop xe.", label, qty);
    return 1;
}

// =============================================================================
// COMMAND – /taketrunk [item] [so]
// =============================================================================

CMD:taketrunk(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (TrunkVehicleID[playerid] == -1)
        return SendErrorMsg(playerid, "Hay mo cop xe truoc bang /trunk."), 1;

    new slug[32], qty;
    if (sscanf(params, "s[32]I(1)i", slug, qty))
        return SendErrorMsg(playerid, "Su dung: /taketrunk [ten vat pham] [so luong]"), 1;

    for (new i = 0; i < MAX_TRUNK_SLOTS; i++)
    {
        if (strcmp(TrunkInv[playerid][i][E_SLOT_NAME], slug, true)) continue;
        if (TrunkInv[playerid][i][E_SLOT_QTY] < qty)
        {
            SendErrorMsgF(playerid, "Trong cop chi con %d cai '%s'.", TrunkInv[playerid][i][E_SLOT_QTY], slug);
            return 1;
        }

        if (!Inv_AddItem(playerid, slug, qty))
        {
            SendErrorMsg(playerid, "Tui do cua ban qua nang hoac day!");
            return 1;
        }

        TrunkInv[playerid][i][E_SLOT_QTY] -= qty;
        if (TrunkInv[playerid][i][E_SLOT_QTY] <= 0)
        {
            TrunkInv[playerid][i][E_SLOT_NAME][0] = '\0';
            TrunkInv[playerid][i][E_SLOT_LABEL][0] = '\0';
            TrunkInv[playerid][i][E_SLOT_QTY] = 0;
            TrunkInv[playerid][i][E_SLOT_WEIGHT] = 0.0;
            TrunkInv[playerid][i][E_SLOT_ILLEGAL] = false;
        }

        SendMsgF(playerid, COLOR_GREEN, "Da lay %s x%d tu cop xe.", slug, qty);
        return 1;
    }

    SendErrorMsgF(playerid, "Khong tim thay '%s' trong cop xe.", slug);
    return 1;
}

// =============================================================================
// COMMAND – /inspect [playerid] – Nhìn túi đồ người khác (Police only)
// =============================================================================

CMD:inspect(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (Player_GetClass(playerid) != TEAM_POLICE && Admin_GetPlayerLevel(playerid) < 1)
        return SendErrorMsg(playerid, "Chi Canh Sat moi duoc kham nguoi."), 1;

    if (!LSPD_IsOnDuty(playerid))
        return SendErrorMsg(playerid, "Ban phai on duty de kham nguoi."), 1;

    new targetid;
    if (sscanf(params, "u", targetid))
        return SendErrorMsg(playerid, "Su dung: /inspect [playerid]"), 1;

    if (!IsPlayerConnected(targetid))
        return SendErrorMsg(playerid, "Nguoi choi khong ton tai."), 1;

    if (!Player_IsNearPlayer(playerid, targetid, 5.0))
        return SendErrorMsg(playerid, "Nguoi nay qua xa! Hay den gan hon."), 1;

    new content[512], line[96];
    new bool:hasIllegal = false;
    new itemCount = 0;

    for (new i = 0; i < MAX_INV_SLOTS; i++)
    {
        if (PlayerInv[targetid][i][E_SLOT_QTY] <= 0) continue;
        format(line, sizeof(line), "%s  x%d%s\n",
            PlayerInv[targetid][i][E_SLOT_LABEL],
            PlayerInv[targetid][i][E_SLOT_QTY],
            PlayerInv[targetid][i][E_SLOT_ILLEGAL] ? " {E85454}[BAT HOP PHAP]{FFFFFF}" : "");
        strcat(content, line, sizeof(content));
        if (PlayerInv[targetid][i][E_SLOT_ILLEGAL]) hasIllegal = true;
        itemCount++;
    }

    if (itemCount == 0) content = "Nguoi nay khong mang do gi.";

    // Thông báo cho người bị khám
    SendMsgF(targetid, COLOR_ORANGE,
        "[CANH SAT] Si quan %s dang kham nguoi ban.", ReturnPlayerName(playerid));

    if (hasIllegal)
    {
        SendMsgF(playerid, COLOR_RED,
            "[CANH SAT] Phat hien do BAT HOP PHAP trong nguoi %s!", ReturnPlayerName(targetid));
    }

    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX,
        "Ket Qua Kham Nguoi",
        content, "Dong", "");
    return 1;
}
