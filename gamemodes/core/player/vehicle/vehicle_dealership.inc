/*
    vehicle_dealership.inc
    ----------------------
    Hệ thống Showroom mua xe (Vehicle Dealership Backend).

    Mô tả:
    - Nối với giao diện `ui_dealership.inc`.
    - Khi người chơi click "Mua" (BTN_PURCHASE), kiểm tra tiền, trừ tiền.
    - Lưu xe vào CSDL `player_vehicles`.
    - Spawn xe ra điểm nhận xe gần Showroom.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

// Vị trí giao xe sau khi mua
#define DEALERSHIP_SPAWN_X      (2135.0)
#define DEALERSHIP_SPAWN_Y      (-1145.0)
#define DEALERSHIP_SPAWN_Z      (25.0)
#define DEALERSHIP_SPAWN_A      (180.0)

// =============================================================================
// DATA
// =============================================================================

static DealershipCurrentIndex[MAX_PLAYERS];

enum E_SHOP_VEHICLE {
    eVehModel,
    eVehName[32],
    eVehPrice
}

// Danh sách xe bán trong Showroom
static const ShopVehicles[][E_SHOP_VEHICLE] = {
    {426, "Premier", 25000},
    {400, "Landstalker", 45000},
    {560, "Sultan", 120000},
    {411, "Infernus", 500000},
    {541, "Bullet", 450000},
    {522, "NRG-500", 85000},
    {468, "Sanchez", 15000}
};

// =============================================================================
// HOOKS
// =============================================================================

hook OnPlayerConnect(playerid)
{
    DealershipCurrentIndex[playerid] = 0;
    return 1;
}

// =============================================================================
// COMMANDS
// =============================================================================

CMD:buyvehicle(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid)) return 1;

    // TODO: Kiểm tra vị trí đứng (phải ở Showroom Wang Cars hoặc Grotti)
    
    DealershipCurrentIndex[playerid] = 0;
    UpdateDealershipUI(playerid);
    
    // Gọi hàm show UI từ ui_dealership.inc
    // (Giả sử UI_ShowDealershipTextdraw() có tồn tại hoặc tương tự)
    CallLocalFunction("UI_ShowDealershipTextDraw", "i", playerid);
    SelectTextDraw(playerid, COLOR_RED);
    
    return 1;
}

// =============================================================================
// INTERNAL LOGIC
// =============================================================================

static UpdateDealershipUI(playerid)
{
    new idx = DealershipCurrentIndex[playerid];
    new model = ShopVehicles[idx][eVehModel];
    new price = ShopVehicles[idx][eVehPrice];
    
    CallLocalFunction("UI_SetDealershipVehicleModel", "ii", playerid, model);
    CallLocalFunction("UI_SetDealershipVehicleHeader", "ii", playerid, model);
    CallLocalFunction("UI_SetDealershipVehiclePrice", "ii", playerid, price);
}

// =============================================================================
// CALLBACK TỪ UI_DEALERSHIP
// =============================================================================

#define BTN_LEFT        1
#define BTN_RIGHT       2
#define BTN_CLOSE       3
#define BTN_PURCHASE    4

forward OnPlayerClickDealershipTextdraw(playerid, type);
public OnPlayerClickDealershipTextdraw(playerid, type)
{
    if (type == BTN_CLOSE)
    {
        CancelSelectTextDraw(playerid);
        CallLocalFunction("UI_HideDealershipTextDraw", "i", playerid);
        return 1;
    }
    
    if (type == BTN_LEFT)
    {
        DealershipCurrentIndex[playerid]--;
        if (DealershipCurrentIndex[playerid] < 0)
            DealershipCurrentIndex[playerid] = sizeof(ShopVehicles) - 1;
        UpdateDealershipUI(playerid);
        return 1;
    }
    
    if (type == BTN_RIGHT)
    {
        DealershipCurrentIndex[playerid]++;
        if (DealershipCurrentIndex[playerid] >= sizeof(ShopVehicles))
            DealershipCurrentIndex[playerid] = 0;
        UpdateDealershipUI(playerid);
        return 1;
    }
    
    if (type == BTN_PURCHASE)
    {
        new idx = DealershipCurrentIndex[playerid];
        new price = ShopVehicles[idx][eVehPrice];
        new model = ShopVehicles[idx][eVehModel];
        
        if (GetPlayerMoney(playerid) < price)
        {
            SendErrorMsg(playerid, "Ban khong du tien de mua chiec xe nay!");
            return 1;
        }
        
        GivePlayerMoney(playerid, -price);
        
        CancelSelectTextDraw(playerid);
        CallLocalFunction("UI_HideDealershipTextDraw", "i", playerid);
        
        // TODO: Chèn vào DB player_vehicles
        // Giả sử có hàm Vehicle_CreatePlayerVehicle(u_id, model, x, y, z, a, col1, col2)
        new u_id = Account_GetUID(playerid);
        new query[512];
        mysql_format(MySQL_GetHandle(), query, sizeof(query), 
            "INSERT INTO player_vehicles (u_id, model_id, pos_x, pos_y, pos_z, pos_a, color_1, color_2) VALUES (%d, %d, %f, %f, %f, %f, 1, 1)",
            u_id, model, DEALERSHIP_SPAWN_X, DEALERSHIP_SPAWN_Y, DEALERSHIP_SPAWN_Z, DEALERSHIP_SPAWN_A);
            
        mysql_tquery(MySQL_GetHandle(), query, "", "");
        
        // Sinh xe tạm thời cho player lái về
        new vehid = CreateVehicle(model, DEALERSHIP_SPAWN_X, DEALERSHIP_SPAWN_Y, DEALERSHIP_SPAWN_Z, DEALERSHIP_SPAWN_A, 1, 1, -1);
        PutPlayerInVehicle(playerid, vehid, 0);
        
        SendMsgF(playerid, COLOR_GREEN, "[SHOWROOM] Chuc mung! Ban da mua thanh cong %s voi gia $%d.", ShopVehicles[idx][eVehName], price);
        SendMsgF(playerid, COLOR_YELLOW, "Luu y: Chiec xe da duoc luu vao tai khoan. Hay lai xe an toan!");
        return 1;
    }

    return 1;
}
