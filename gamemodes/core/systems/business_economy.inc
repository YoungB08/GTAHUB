/*
    business_economy.inc
    --------------------
    Hệ thống Doanh nghiệp Người chơi sở hữu (Player-Owned Businesses).

    Mô tả:
    - Quản lý các doanh nghiệp (24/7, Tiệm Súng).
    - Tạo bảng `businesses` lưu: owner_uid, name, type, price, safe_money, stock_level, pos_x, pos_y, pos_z, vw, int.
    - Người chơi có Giấy Phép Kinh Doanh (GPDKD) có thể mua lại doanh nghiệp vô chủ (/buybiz).
    - Chủ tiệm quản lý hàng tồn kho (/bizstock) và giá bán (/bizprice), thu tiền lời (/bizwithdraw).
    - Khi khách mua hàng ở điểm giao dịch, sẽ bị trừ Stock và cộng tiền vào Safe.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define MAX_BUSINESSES          (50)
#define BIZ_TYPE_247            (1)
#define BIZ_TYPE_AMMUNATION     (2)

// =============================================================================
// DATA
// =============================================================================

enum E_BIZ_DATA
{
    bizID,
    bizOwnerUID,
    bizName[64],
    bizType,
    bizPrice,
    bizSafeMoney,
    bizStock,
    Float:bizX,
    Float:bizY,
    Float:bizZ,
    bizVW,
    bizInt,
    
    // Runtime
    Text3D:bizLabel,
    bizPickup
};

static BusinessData[MAX_BUSINESSES][E_BIZ_DATA];
static TotalBusinesses = 0;

static Statement:stmt_loadBiz;
static Statement:stmt_updateBiz;
static Statement:stmt_updateBizSafeStock;

// =============================================================================
// HOOKS
// =============================================================================

hook OnMySQLConnected()
{
    // Tạo bảng nếu chưa có
    mysql_tquery(MySQL_GetHandle(),
        "CREATE TABLE IF NOT EXISTS businesses (\
            id INT(11) AUTO_INCREMENT PRIMARY KEY,\
            owner_uid INT(11) DEFAULT 0,\
            name VARCHAR(64) DEFAULT 'Doanh nghiep vo chu',\
            type INT(11) DEFAULT 1,\
            price INT(11) DEFAULT 50000,\
            safe_money INT(11) DEFAULT 0,\
            stock_level INT(11) DEFAULT 1000,\
            pos_x FLOAT, pos_y FLOAT, pos_z FLOAT,\
            vw INT(11) DEFAULT 0, interior INT(11) DEFAULT 0\
        )", "", "");

    stmt_loadBiz = MySQL_PrepareStatement(MySQL_GetHandle(), "SELECT id, owner_uid, name, type, price, safe_money, stock_level, pos_x, pos_y, pos_z, vw, interior FROM businesses");
    
    stmt_updateBiz = MySQL_PrepareStatement(MySQL_GetHandle(), 
        "UPDATE businesses SET owner_uid=?, name=?, price=? WHERE id=?");
        
    stmt_updateBizSafeStock = MySQL_PrepareStatement(MySQL_GetHandle(),
        "UPDATE businesses SET safe_money=?, stock_level=? WHERE id=?");

    // Đợi tạo bảng xong rồi load
    defer LoadBusinessesDelay();
    return 1;
}

timer LoadBusinessesDelay[1000]()
{
    Biz_LoadAll();
}

// =============================================================================
// CORE FUNCTIONS
// =============================================================================

static Biz_LoadAll()
{
    inline OnBizLoad()
    {
        TotalBusinesses = 0;
        while (MySQL_Statement_FetchRow(stmt_loadBiz))
        {
            if (TotalBusinesses >= MAX_BUSINESSES) break;

            MySQL_BindResultInt(stmt_loadBiz, 0, BusinessData[TotalBusinesses][bizID]);
            MySQL_BindResultInt(stmt_loadBiz, 1, BusinessData[TotalBusinesses][bizOwnerUID]);
            MySQL_BindResultString(stmt_loadBiz, 2, BusinessData[TotalBusinesses][bizName], 64);
            MySQL_BindResultInt(stmt_loadBiz, 3, BusinessData[TotalBusinesses][bizType]);
            MySQL_BindResultInt(stmt_loadBiz, 4, BusinessData[TotalBusinesses][bizPrice]);
            MySQL_BindResultInt(stmt_loadBiz, 5, BusinessData[TotalBusinesses][bizSafeMoney]);
            MySQL_BindResultInt(stmt_loadBiz, 6, BusinessData[TotalBusinesses][bizStock]);
            MySQL_BindResultFloat(stmt_loadBiz, 7, BusinessData[TotalBusinesses][bizX]);
            MySQL_BindResultFloat(stmt_loadBiz, 8, BusinessData[TotalBusinesses][bizY]);
            MySQL_BindResultFloat(stmt_loadBiz, 9, BusinessData[TotalBusinesses][bizZ]);
            MySQL_BindResultInt(stmt_loadBiz, 10, BusinessData[TotalBusinesses][bizVW]);
            MySQL_BindResultInt(stmt_loadBiz, 11, BusinessData[TotalBusinesses][bizInt]);

            Biz_RefreshVisuals(TotalBusinesses);
            TotalBusinesses++;
        }
        Logger_Log("[business_economy.inc] Loaded %d player-owned businesses.", TotalBusinesses);
    }
    MySQL_ExecuteParallel_Inline(stmt_loadBiz, using inline OnBizLoad);
}

static Biz_RefreshVisuals(index)
{
    if (IsValidDynamic3DTextLabel(BusinessData[index][bizLabel]))
        DestroyDynamic3DTextLabel(BusinessData[index][bizLabel]);
        
    if (IsValidDynamicPickup(BusinessData[index][bizPickup]))
        DestroyDynamicPickup(BusinessData[index][bizPickup]);

    new label[256];
    if (BusinessData[index][bizOwnerUID] == 0)
    {
        format(label, sizeof(label), "[DOANH NGHIEP VO CHU]\n{FFFFFF}%s\nGia: {33AA33}$%d\n{FFFFFF}Dung /buybiz de mua", 
            BusinessData[index][bizName], BusinessData[index][bizPrice]);
    }
    else
    {
        format(label, sizeof(label), "[DOANH NGHIEP]\n{FFFFFF}%s\n{FFAA00}Kho hang: %d units\n{FFFFFF}Dung /buy [ten mon] de mua", 
            BusinessData[index][bizName], BusinessData[index][bizStock]);
    }

    BusinessData[index][bizLabel] = CreateDynamic3DTextLabel(label, 0x00FF00FF, 
        BusinessData[index][bizX], BusinessData[index][bizY], BusinessData[index][bizZ] + 0.5, 
        15.0, INVALID_PLAYER_ID, INVALID_VEHICLE_ID, 0, BusinessData[index][bizVW], BusinessData[index][bizInt]);
        
    BusinessData[index][bizPickup] = CreateDynamicPickup(1274, 1, 
        BusinessData[index][bizX], BusinessData[index][bizY], BusinessData[index][bizZ], 
        BusinessData[index][bizVW], BusinessData[index][bizInt]);
}

static Biz_GetNearest(playerid, Float:radius = 3.0)
{
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);
    
    for (new i = 0; i < TotalBusinesses; i++)
    {
        if (GetPlayerVirtualWorld(playerid) == BusinessData[i][bizVW] && 
            GetPlayerInterior(playerid) == BusinessData[i][bizInt])
        {
            if (Distance(px, py, pz, BusinessData[i][bizX], BusinessData[i][bizY], BusinessData[i][bizZ]) <= radius)
                return i;
        }
    }
    return -1;
}

// =============================================================================
// COMMANDS - ADMIN
// =============================================================================

ACMD:[4]createbiz(playerid, params[])
{
    new type, price, name[64];
    if (sscanf(params, "iis[64]", type, price, name))
        return SendErrorMsg(playerid, "Su dung: /createbiz [Type: 1=247, 2=Ammo] [Price] [Name]"), 1;
        
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);
    new vw = GetPlayerVirtualWorld(playerid);
    new intId = GetPlayerInterior(playerid);

    new query[256];
    mysql_format(MySQL_GetHandle(), query, sizeof(query), 
        "INSERT INTO businesses (type, price, name, pos_x, pos_y, pos_z, vw, interior) VALUES (%d, %d, '%e', %f, %f, %f, %d, %d)",
        type, price, name, px, py, pz, vw, intId);
        
    mysql_tquery(MySQL_GetHandle(), query, "", "");
    
    SendMsgF(playerid, COLOR_GREEN, "[ADMIN] Da tao doanh nghiep thanh cong. Reloading...");
    Biz_LoadAll();
    return 1;
}

// =============================================================================
// COMMANDS - PLAYER
// =============================================================================

CMD:buybiz(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid)) return 1;

    // Yêu cầu GPDKD (Giấy phép kinh doanh) - Từ City Hall
    if (!Player_HasBizLicense(playerid))
        return SendErrorMsg(playerid, "Ban can Giay Phep Kinh Doanh (mua tai City Hall) de so huu doanh nghiep."), 1;

    new id = Biz_GetNearest(playerid);
    if (id == -1)
        return SendErrorMsg(playerid, "Ban khong dung gan mot doanh nghiep nao."), 1;

    if (BusinessData[id][bizOwnerUID] != 0)
        return SendErrorMsg(playerid, "Doanh nghiep nay da co chu."), 1;

    new price = BusinessData[id][bizPrice];
    if (GetPlayerMoney(playerid) < price)
        return SendErrorMsgF(playerid, "Ban khong du $%d de mua doanh nghiep nay.", price), 1;

    GivePlayerMoney(playerid, -price);
    
    // Gán chủ sở hữu (UID)
    BusinessData[id][bizOwnerUID] = Account_GetUID(playerid);
    
    // Save
    MySQL_BindInt(stmt_updateBiz, 0, BusinessData[id][bizOwnerUID]);
    MySQL_BindString(stmt_updateBiz, 1, BusinessData[id][bizName]);
    MySQL_BindInt(stmt_updateBiz, 2, BusinessData[id][bizPrice]);
    MySQL_BindInt(stmt_updateBiz, 3, BusinessData[id][bizID]);
    MySQL_ExecuteThreaded(stmt_updateBiz);
    
    Biz_RefreshVisuals(id);
    
    SendMsgF(playerid, COLOR_GREEN, "[DOANH NGHIEP] Ban da mua thanh cong %s voi gia $%d!", BusinessData[id][bizName], price);
    return 1;
}

CMD:bizpanel(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid)) return 1;
    
    new id = Biz_GetNearest(playerid, 10.0);
    if (id == -1) return SendErrorMsg(playerid, "Khong tim thay doanh nghiep ban quan ly gan day."), 1;
    
    if (BusinessData[id][bizOwnerUID] != Account_GetUID(playerid))
        return SendErrorMsg(playerid, "Ban khong phai la chu doanh nghiep nay."), 1;

    new content[512];
    format(content, sizeof(content), 
        "{FFFFFF}Ten Doanh Nghiep: {FFAA00}%s\n\
        {FFFFFF}Tien trong ket (Safe): {33AA33}$%d\n\
        {FFFFFF}Kho hang (Stock): {00AAFF}%d units\n\n\
        1. Rut tien tu ket\n2. Nhap hang (Stock)\n3. Dat gia ban hang\n4. Doi ten cua hang",
        BusinessData[id][bizName], BusinessData[id][bizSafeMoney], BusinessData[id][bizStock]);

    inline OnBizPanel(response, listitem, string:inputtext[])
    {
        #pragma unused inputtext
        if (!response) return;

        if (listitem == 0) // Rut tien
        {
            if (BusinessData[id][bizSafeMoney] <= 0)
            {
                SendErrorMsg(playerid, "Ket sat dang trong.");
                return;
            }
            GivePlayerMoney(playerid, BusinessData[id][bizSafeMoney]);
            SendMsgF(playerid, COLOR_GREEN, "[DOANH NGHIEP] Da rut $%d tu ket sat.", BusinessData[id][bizSafeMoney]);
            BusinessData[id][bizSafeMoney] = 0;
        }
        else if (listitem == 1) // Nhap hang
        {
            // Gia dinh 1 unit = $10
            new cost = 500; // 50 units
            if (GetPlayerMoney(playerid) < cost)
            {
                SendErrorMsg(playerid, "Ban can $500 de nhap 50 don vi hang.");
                return;
            }
            GivePlayerMoney(playerid, -cost);
            BusinessData[id][bizStock] += 50;
            SendMsgF(playerid, COLOR_GREEN, "[DOANH NGHIEP] Da nhap 50 don vi hang hoa, phi $500.");
        }
        else if (listitem == 2) // Dat gia ban hang
        {
            inline OnSetPrice(res, li, string:priceStr[])
            {
                #pragma unused li
                if (!res) return;
                new newPrice = strval(priceStr);
                if (newPrice < 1 || newPrice > 99999)
                {
                    SendErrorMsg(playerid, "Gia phai tu $1 den $99,999.");
                    return;
                }
                BusinessData[id][bizPrice] = newPrice;
                MySQL_BindInt(stmt_updateBiz, 0, BusinessData[id][bizOwnerUID]);
                MySQL_BindString(stmt_updateBiz, 1, BusinessData[id][bizName]);
                MySQL_BindInt(stmt_updateBiz, 2, newPrice);
                MySQL_BindInt(stmt_updateBiz, 3, BusinessData[id][bizID]);
                MySQL_ExecuteThreaded(stmt_updateBiz);
                Biz_RefreshVisuals(id);
                SendMsgF(playerid, COLOR_GREEN, "[DOANH NGHIEP] Da cap nhat gia ban thanh $%d.", newPrice);
            }
            Dialog_ShowCallback(playerid, using inline OnSetPrice, DIALOG_STYLE_INPUT, "Dat Gia Ban", "Nhap gia ban moi cho cua hang ($1 - $99,999):", "Xac Nhan", "Huy");
            return;
        }
        else if (listitem == 3) // Doi ten cua hang
        {
            inline OnRename(res, li, string:newName[])
            {
                #pragma unused li
                if (!res) return;
                if (strlen(newName) < 3 || strlen(newName) > 63)
                {
                    SendErrorMsg(playerid, "Ten phai tu 3 den 63 ky tu.");
                    return;
                }
                format(BusinessData[id][bizName], 64, "%s", newName);
                MySQL_BindInt(stmt_updateBiz, 0, BusinessData[id][bizOwnerUID]);
                MySQL_BindString(stmt_updateBiz, 1, BusinessData[id][bizName]);
                MySQL_BindInt(stmt_updateBiz, 2, BusinessData[id][bizPrice]);
                MySQL_BindInt(stmt_updateBiz, 3, BusinessData[id][bizID]);
                MySQL_ExecuteThreaded(stmt_updateBiz);
                Biz_RefreshVisuals(id);
                SendMsgF(playerid, COLOR_GREEN, "[DOANH NGHIEP] Da doi ten cua hang thanh: %s.", BusinessData[id][bizName]);
            }
            Dialog_ShowCallback(playerid, using inline OnRename, DIALOG_STYLE_INPUT, "Doi Ten Cua Hang", "Nhap ten moi cho doanh nghiep:", "Xac Nhan", "Huy");
            return;
        }

        // Save safe & stock
        MySQL_BindInt(stmt_updateBizSafeStock, 0, BusinessData[id][bizSafeMoney]);
        MySQL_BindInt(stmt_updateBizSafeStock, 1, BusinessData[id][bizStock]);
        MySQL_BindInt(stmt_updateBizSafeStock, 2, BusinessData[id][bizID]);
        MySQL_ExecuteThreaded(stmt_updateBizSafeStock);
        
        Biz_RefreshVisuals(id);
    }
    
    Dialog_ShowCallback(playerid, using inline OnBizPanel, DIALOG_STYLE_LIST, "Quan Ly Doanh Nghiep", content, "Chon", "Thoat");
    return 1;
}

CMD:buy(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid)) return 1;
    
    new id = Biz_GetNearest(playerid);
    if (id == -1) return SendErrorMsg(playerid, "Ban phai o diem giao dich doanh nghiep de mua hang."), 1;
    
    if (BusinessData[id][bizStock] <= 0)
        return SendErrorMsg(playerid, "Cua hang nay hien da het hang (Out of stock)."), 1;

    // Giả lập menu mua hàng 24/7
    if (BusinessData[id][bizType] == BIZ_TYPE_247)
    {
        inline OnBuy247(response, listitem, string:inputtext[])
        {
            #pragma unused inputtext
            if (!response) return;

            new itemSlug[32], price, stockDec;
            if (listitem == 0)      { format(itemSlug, sizeof(itemSlug), "burger"); price = 15; stockDec = 1; }
            else if (listitem == 1) { format(itemSlug, sizeof(itemSlug), "nuoc"); price = 10; stockDec = 1; }
            else if (listitem == 2) { format(itemSlug, sizeof(itemSlug), "medkit"); price = 150; stockDec = 5; }
            else if (listitem == 3) { format(itemSlug, sizeof(itemSlug), "rod"); price = 50; stockDec = 2; }
            
            if (BusinessData[id][bizStock] < stockDec)
            {
                SendErrorMsg(playerid, "Cua hang khong du hang cho san pham nay.");
                return;
            }

            if (GetPlayerMoney(playerid) < price)
            {
                SendErrorMsgF(playerid, "Ban khong du $%d de mua.", price);
                return;
            }

            if (Inv_AddItem(playerid, itemSlug))
            {
                GivePlayerMoney(playerid, -price);
                BusinessData[id][bizStock] -= stockDec;
                BusinessData[id][bizSafeMoney] += price;
                
                MySQL_BindInt(stmt_updateBizSafeStock, 0, BusinessData[id][bizSafeMoney]);
                MySQL_BindInt(stmt_updateBizSafeStock, 1, BusinessData[id][bizStock]);
                MySQL_BindInt(stmt_updateBizSafeStock, 2, BusinessData[id][bizID]);
                MySQL_ExecuteThreaded(stmt_updateBizSafeStock);

                Biz_RefreshVisuals(id);
                SendMsgF(playerid, COLOR_GREEN, "[24/7] Da mua thanh cong %s voi gia $%d.", itemSlug, price);
            }
            else
            {
                SendErrorMsg(playerid, "Tui do cua ban da day!");
            }
        }
        
        Dialog_ShowCallback(playerid, using inline OnBuy247, DIALOG_STYLE_LIST, "Cua Hang 24/7",
            "Burger ($15)\nNuoc uong ($10)\nBo So Cuu ($150)\nCan Cau Ca ($50)", "Mua", "Huy");
    }
    // Ammo cũng làm tương tự (sẽ trừ stock nhiều hơn)
    else if (BusinessData[id][bizType] == BIZ_TYPE_AMMUNATION)
    {
        SendMsgF(playerid, COLOR_YELLOW, "Chuc nang mua vu khi chua tich hop vao inventory. Vui long doi ban cap nhat sau.");
    }
    
    return 1;
}
