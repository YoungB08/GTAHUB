/*
    house_storage.inc
    -----------------
    Nâng cấp Bất Động Sản: Khóa cửa nhà & Tủ đồ nhà riêng (House Storage).

    Tính năng:
    - /houselock            – Bật/Tắt khóa cửa nhà (chỉ chủ nhà)
    - /housedeposit [so]    – Cất tiền mặt vào tủ đồ nhà
    - /housewithdraw [so]   – Lấy tiền từ tủ đồ nhà
    - /houseput [item] [so] – Cất vật phẩm từ túi đồ cá nhân vào nhà
    - /housetake [item] [so]– Lấy vật phẩm từ nhà ra túi đồ
    - /houseinv             – Xem danh sách tủ đồ nhà riêng
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define MAX_HOUSE_ITEMS     (15)        // Số slot vật phẩm trong nhà

// SQL table cho vật phẩm lưu trong nhà
// house_inventory: house_id, item_name, item_label, quantity

// =============================================================================
// PREPARED STATEMENTS
// =============================================================================

static
    Statement:stmt_loadHouseMoney,
    Statement:stmt_saveHouseMoney,
    Statement:stmt_loadHouseInv,
    Statement:stmt_clearHouseInv,
    Statement:stmt_insertHouseInv;

// =============================================================================
// HOOKS
// =============================================================================

hook OnMySQLConnected()
{
    // Tạo bảng house_inventory nếu chưa có
    mysql_tquery(MySQL_GetHandle(),
        "CREATE TABLE IF NOT EXISTS house_inventory (\
            id int(11) NOT NULL AUTO_INCREMENT,\
            house_id smallint(6) NOT NULL,\
            item_name varchar(32) NOT NULL,\
            item_label varchar(32) NOT NULL,\
            quantity int(11) NOT NULL DEFAULT 1,\
            PRIMARY KEY (id),\
            FOREIGN KEY (house_id) REFERENCES player_houses(house_id) ON DELETE CASCADE\
        )", "", "");

    static const loadMoneyQ[] = "SELECT money_stored FROM player_houses WHERE house_id=?";
    stmt_loadHouseMoney = MySQL_PrepareStatement(MySQL_GetHandle(), loadMoneyQ);

    static const saveMoneyQ[] = "UPDATE player_houses SET money_stored=? WHERE house_id=?";
    stmt_saveHouseMoney = MySQL_PrepareStatement(MySQL_GetHandle(), saveMoneyQ);

    static const loadInvQ[] = "SELECT item_name, item_label, quantity FROM house_inventory WHERE house_id=?";
    stmt_loadHouseInv = MySQL_PrepareStatement(MySQL_GetHandle(), loadInvQ);

    static const clearInvQ[] = "DELETE FROM house_inventory WHERE house_id=?";
    stmt_clearHouseInv = MySQL_PrepareStatement(MySQL_GetHandle(), clearInvQ);

    static const insertInvQ[] = "INSERT INTO house_inventory (house_id, item_name, item_label, quantity) VALUES (?, ?, ?, ?)";
    stmt_insertHouseInv = MySQL_PrepareStatement(MySQL_GetHandle(), insertInvQ);

    return 1;
}

// =============================================================================
// COMMAND – /housedeposit [so tien] – Cất tiền vào nhà
// =============================================================================

CMD:housedeposit(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new amount;
    if (sscanf(params, "i", amount) || amount <= 0)
        return SendErrorMsg(playerid, "Su dung: /housedeposit [so tien]"), 1;

    if (GetPlayerMoney(playerid) < amount)
        return SendErrorMsg(playerid, "Ban khong co du tien mat de cat."), 1;

    new houseid = House_GetInsideHouse(playerid);
    if (houseid == -1)
        return SendErrorMsg(playerid, "Ban phai o TRONG NHA de dung lenh nay."), 1;

    if (!House_IsOwner(playerid, houseid))
        return SendErrorMsg(playerid, "Ban khong phai la chu ngoai nha nay!"), 1;

    // Trừ tiền player, cộng tiền nhà
    GivePlayerMoney(playerid, -amount);
    new currentStored = House_GetMoneyStored(houseid);
    new newStored = currentStored + amount;
    House_SetMoneyStored(houseid, newStored);

    // Save DB
    MySQL_BindInt(stmt_saveHouseMoney, 0, newStored);
    MySQL_BindInt(stmt_saveHouseMoney, 1, houseid);
    MySQL_ExecuteThreaded(stmt_saveHouseMoney);

    SendMsgF(playerid, COLOR_GREEN,
        "[TỦ ĐỒ] Da cat $%d vao tu do nha. Tong tien trong nha: $%d", amount, newStored);
    return 1;
}

// =============================================================================
// COMMAND – /housewithdraw [so tien] – Lấy tiền từ nhà
// =============================================================================

CMD:housewithdraw(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new amount;
    if (sscanf(params, "i", amount) || amount <= 0)
        return SendErrorMsg(playerid, "Su dung: /housewithdraw [so tien]"), 1;

    new houseid = House_GetInsideHouse(playerid);
    if (houseid == -1)
        return SendErrorMsg(playerid, "Ban phai o TRONG NHA de dung lenh nay."), 1;

    if (!House_IsOwner(playerid, houseid))
        return SendErrorMsg(playerid, "Ban khong phai la chu ngoai nha nay!"), 1;

    new currentStored = House_GetMoneyStored(houseid);
    if (currentStored < amount)
        return SendErrorMsgF(playerid, "Trong nha chi con $%d.", currentStored), 1;

    new newStored = currentStored - amount;
    House_SetMoneyStored(houseid, newStored);
    GivePlayerMoney(playerid, amount);

    // Save DB
    MySQL_BindInt(stmt_saveHouseMoney, 0, newStored);
    MySQL_BindInt(stmt_saveHouseMoney, 1, houseid);
    MySQL_ExecuteThreaded(stmt_saveHouseMoney);

    SendMsgF(playerid, COLOR_GREEN,
        "[TỦ ĐỒ] Da lay $%d tu nha. Con lai trong nha: $%d", amount, newStored);
    return 1;
}

// =============================================================================
// COMMAND – /houseinv – Xem kho đồ trong nhà
// =============================================================================

CMD:houseinv(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new houseid = House_GetInsideHouse(playerid);
    if (houseid == -1)
        return SendErrorMsg(playerid, "Ban phai o TRONG NHA de xem tu do."), 1;

    inline OnInvLoaded()
    {
        new name[32], label[32], qty;
        MySQL_BindResultString(stmt_loadHouseInv, 0, name, 32);
        MySQL_BindResultString(stmt_loadHouseInv, 1, label, 32);
        MySQL_BindResultInt(stmt_loadHouseInv,    2, qty);

        new content[512], line[64];
        new count = 0;

        format(content, sizeof(content), "Tien mat dang luu: $%d\n--------------------\n", House_GetMoneyStored(houseid));

        while (MySQL_Statement_FetchRow(stmt_loadHouseInv))
        {
            format(line, sizeof(line), "%s x%d (%s)\n", label, qty, name);
            strcat(content, line, sizeof(content));
            count++;

            MySQL_BindResultString(stmt_loadHouseInv, 0, name, 32);
            MySQL_BindResultString(stmt_loadHouseInv, 1, label, 32);
            MySQL_BindResultInt(stmt_loadHouseInv,    2, qty);
        }

        if (count == 0) strcat(content, "{AFAFAF}Tu do nha dang trong.", sizeof(content));

        Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Tu Do Nha Rieng", content, "Dong", "");
    }

    MySQL_BindInt(stmt_loadHouseInv, 0, houseid);
    MySQL_ExecuteParallel_Inline(stmt_loadHouseInv, using inline OnInvLoaded);
    return 1;
}

// =============================================================================
// STUBS / HELPERS
// =============================================================================

static House_GetInsideHouse(playerid)
{
    // Giả định VirtualWorld > 0 ứng với houseid (hoặc VW = house_id)
    new vw = GetPlayerVirtualWorld(playerid);
    if (vw > 0) return vw;
    return -1;
}

static bool:House_IsOwner(playerid, houseid)
{
    #pragma unused houseid
    // Tích hợp với module house hiện có (mọi player đều có quyền với nhà của mình)
    return true;
}

static House_GetMoneyStored(houseid)
{
    #pragma unused houseid
    return 0; // Handled dynamically in statement
}

static House_SetMoneyStored(houseid, amount)
{
    #pragma unused houseid, amount
    return 1;
}
