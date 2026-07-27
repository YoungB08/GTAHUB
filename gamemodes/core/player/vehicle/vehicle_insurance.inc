/*
    vehicle_insurance.inc
    ----------------------
    Hệ thống Bảo hiểm xe & Bãi giữ xe (Vehicle Insurance & Impound Lot).

    Mô tả:
    - Khi xe cá nhân bị nổ, hệ thống đánh dấu xe là "Impounded" trong DB.
    - Xe được phục hồi tại Impound Lot sau khi chủ xe trả phí bảo hiểm.
    - Phí bảo hiểm = 5% giá trị xe hoặc tối thiểu $1,000.
    - Lệnh /getcar: Trả phí và lấy xe ra.
    - Vị trí Impound Lot: Los Santos (1413.62, -1728.80, 13.55).

    API dựa trên vehicle_impl.inc:
    - PersonalVehicle (Iterator)
    - VehicleOwnerID[], VehicleUID[], VehicleModelID[], VehicleHandler[]
    - Vehicle_IsOwner(playerid, vehicleid)
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
    // Tìm trong PersonalVehicle Iterator xem xe này có phải xe cá nhân không
    new ownerUID = Vehicle_GetOwnerUID(vehicleid);
    new vehDBId  = Vehicle_GetDatabaseID(vehicleid);
    new model    = GetVehicleModel(vehicleid);

    if (ownerUID <= 0 || vehDBId <= 0) return 1;

    // Đánh dấu Impounded trong DB
    new query[200];
    mysql_format(MySQL_GetHandle(), query, sizeof(query),
        "UPDATE player_vehicles SET impounded=1 WHERE vehicle_id=%d AND u_id=%d",
        vehDBId, ownerUID);
    mysql_tquery(MySQL_GetHandle(), query, "", "");

    // Thông báo chủ xe nếu đang online
    new fee = Impound_GetFee(model);
    foreach (new playerid : Player)
    {
        if (!Player_IsLoggedIn(playerid)) continue;
        if (Player_GetAccountID(playerid) != ownerUID) continue;

        SendMsgF(playerid, COLOR_RED, "[BAO HIEM] Xe cua ban da bi pha huy va dua vao Bai Giu Xe!");
        SendMsgF(playerid, COLOR_ORANGE, "[BAO HIEM] Den Bai Giu Xe va dung /getcar de tra phi $%d.", fee);
        break;
    }
    return 1;
}

// =============================================================================
// HELPERS
// =============================================================================

static Impound_GetFee(model)
{
    switch (model)
    {
        case 411, 541: return 25000;  // Infernus, Bullet
        case 560:      return 6000;   // Sultan
        case 400:      return 2250;   // Landstalker
        case 522:      return 4250;   // NRG-500
        default:       return INSURANCE_MIN;
    }
}

// =============================================================================
// COMMANDS
// =============================================================================

CMD:getcar(playerid, params[])
{
    #pragma unused params
    if (!Player_IsLoggedIn(playerid)) return 1;

    new uid = Player_GetAccountID(playerid);

    inline OnCheckImpound()
    {
        if (!cache_num_rows())
        {
            SendErrorMsg(playerid, "Ban khong co xe nao dang bi giu tai Bai Giu Xe.");
            return 1;
        }

        new vehId = -1, model = -1;
        cache_get_value_name_int(0, "vehicle_id", vehId);
        cache_get_value_name_int(0, "model_id", model);

        new fee = Impound_GetFee(model);
        if (GetPlayerMoney(playerid) < fee)
        {
            SendErrorMsgF(playerid, "Ban khong du $%d de trang trai phi bao hiem.", fee);
            return 1;
        }

        GivePlayerMoney(playerid, -fee);

        // Xóa flag impounded và cập nhật vị trí về Impound Lot
        new updateQuery[256];
        mysql_format(MySQL_GetHandle(), updateQuery, sizeof(updateQuery),
            "UPDATE player_vehicles SET impounded=0, x=%f, y=%f, z=%f, rot=%f WHERE vehicle_id=%d",
            IMPOUND_SPAWN_X, IMPOUND_SPAWN_Y, IMPOUND_SPAWN_Z, IMPOUND_SPAWN_A, vehId);
        mysql_tquery(MySQL_GetHandle(), updateQuery, "", "");

        // Spawn xe tạm thời tại Impound Lot
        new newVeh = CreateVehicle(model, IMPOUND_SPAWN_X, IMPOUND_SPAWN_Y, IMPOUND_SPAWN_Z, IMPOUND_SPAWN_A, 1, 1, -1);
        SetVehicleHealth(newVeh, 1000.0);

        SendMsgF(playerid, COLOR_GREEN, "[BAO HIEM] Da tra phi $%d. Xe cua ban o Bai Giu Xe (%.1f, %.1f).", fee, IMPOUND_SPAWN_X, IMPOUND_SPAWN_Y);
        return 1;
    }

    new selectQuery[256];
    mysql_format(MySQL_GetHandle(), selectQuery, sizeof(selectQuery),
        "SELECT vehicle_id, model_id FROM player_vehicles WHERE u_id=%d AND impounded=1 LIMIT 1", uid);
    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnCheckImpound, selectQuery);

    return 1;
}
