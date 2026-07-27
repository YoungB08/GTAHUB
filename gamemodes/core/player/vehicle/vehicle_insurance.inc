/*
    vehicle_insurance.inc
    ----------------------
    Hệ thống Bảo hiểm xe & Bãi giữ xe (Vehicle Insurance & Impound Lot).

    Mô tả:
    - Khi xe cá nhân bị nổ, hệ thống đánh dấu xe là "Impounded" trong DB.
    - Xe được phục hồi tại Impound Lot sau khi chủ xe trả phí bảo hiểm.
    - Phí bảo hiểm = 5% giá trị xe (lấy từ Showroom) hoặc tối thiểu $1,000.
    - Lệnh /getcar: Trả phí và lấy xe ra.
    - Vị trí Impound Lot: Los Santos (1413.62, -1728.80, 13.55).
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define INSURANCE_RATE          (0.05)  // 5% giá xe
#define INSURANCE_MIN           (1000)  // Tối thiểu $1,000
#define IMPOUND_SPAWN_X         (1413.62)
#define IMPOUND_SPAWN_Y         (-1728.80)
#define IMPOUND_SPAWN_Z         (13.55)
#define IMPOUND_SPAWN_A         (90.0)

// =============================================================================
// HOOKS
// =============================================================================

hook OnVehicleDeath(vehicleid, killerid)
{
    // Chỉ xử lý xe cá nhân (có chủ sở hữu trong DB)
    new vehIndex = GetPersonalVehicleIndex(vehicleid);
    if (vehIndex == -1) return 1; // Không phải xe cá nhân

    // Đánh dấu xe bị Impound trong DB (cột `impounded` = 1)
    new vehDBId = GetPersonalVehicleDBId(vehicleid);
    if (vehDBId <= 0) return 1;

    mysql_tquery(MySQL_GetHandle(),
        "UPDATE player_vehicles SET impounded=1, pos_x=NULL, pos_y=NULL, pos_z=NULL WHERE id=" #vehDBId,
        "", "");

    // Tìm chủ xe để thông báo
    foreach (new playerid : Player)
    {
        if (Player_IsLoggedIn(playerid) && GetPersonalVehicleOwnerUID(vehicleid) == Account_GetUID(playerid))
        {
            // Tính phí
            new model = GetVehicleModel(vehicleid);
            new fee = Impound_GetFee(model);
            SendMsgF(playerid, COLOR_RED, "[BAO HIEM] Xe cua ban da bi pha huy va dua vao bai giu xe!");
            SendMsgF(playerid, COLOR_RED, "[BAO HIEM] Di den Bai Giu Xe va dung /getcar de tra phi $%d.", fee);
        }
    }
    return 1;
}

// =============================================================================
// HELPERS
// =============================================================================

// Tính phí dựa vào model xe (lookup giá từ ShopVehicles nếu có, không thì dùng mặc định)
static Impound_GetFee(model)
{
    // Bảng giá tham chiếu sơ bộ (giá xe tương ứng trong dealership)
    // Đây là fallback — lý tưởng sẽ join với bảng DB sau
    switch (model)
    {
        case 411, 541: return 25000;    // Infernus, Bullet → 5% = 25k
        case 560:      return 6000;     // Sultan → 120k * 5%
        case 400:      return 2250;     // Landstalker
        case 522:      return 4250;     // NRG-500
        default:       return INSURANCE_MIN;
    }
}

// =============================================================================
// COMMANDS
// =============================================================================

CMD:getcar(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid)) return 1;

    // Kiểm tra có xe bị Impound không
    new uid = Account_GetUID(playerid);
    
    inline OnCheckImpound()
    {
        if (!MySQL_Statement_FetchRow(MySQL_GetLastStatement()))
        {
            SendErrorMsg(playerid, "Ban khong co xe nao dang bi giu tai Bai Giu Xe.");
            return;
        }

        // Lấy thông tin xe
        new vehId, model, fee;
        MySQL_BindResultInt(MySQL_GetLastStatement(), 0, vehId);
        MySQL_BindResultInt(MySQL_GetLastStatement(), 1, model);

        fee = Impound_GetFee(model);

        if (GetPlayerMoney(playerid) < fee)
        {
            SendErrorMsgF(playerid, "Ban khong du $%d de trang trai phi bao hiem.", fee);
            return;
        }

        GivePlayerMoney(playerid, -fee);

        // Xóa flag impounded, set vị trí mới là Impound Lot
        new query[256];
        mysql_format(MySQL_GetHandle(), query, sizeof(query),
            "UPDATE player_vehicles SET impounded=0, pos_x=%f, pos_y=%f, pos_z=%f, pos_a=%f WHERE id=%d",
            IMPOUND_SPAWN_X, IMPOUND_SPAWN_Y, IMPOUND_SPAWN_Z, IMPOUND_SPAWN_A, vehId);
        mysql_tquery(MySQL_GetHandle(), query, "", "");

        // Spawn xe tạm thời
        new newVeh = CreateVehicle(model, IMPOUND_SPAWN_X, IMPOUND_SPAWN_Y, IMPOUND_SPAWN_Z, IMPOUND_SPAWN_A, 1, 1, -1);
        SetVehicleHealth(newVeh, 1000.0);
        
        SendMsgF(playerid, COLOR_GREEN, "[BAO HIEM] Da trang trai phi $%d. Xe cua ban dang o Bai Giu Xe (Impound Lot)!", fee);
        SendMsgF(playerid, COLOR_YELLOW, "Vi tri: /gps impound | Toa do: %.1f, %.1f", IMPOUND_SPAWN_X, IMPOUND_SPAWN_Y);
    }

    new query[256];
    mysql_format(MySQL_GetHandle(), query, sizeof(query),
        "SELECT id, model_id FROM player_vehicles WHERE u_id=%d AND impounded=1 LIMIT 1", uid);
    mysql_tquery_inline(MySQL_GetHandle(), query, using inline OnCheckImpound, "");

    return 1;
}
