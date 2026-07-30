/*
    vehicle_fuel.inc
    ----------------
    Hệ thống nhiên liệu (Fuel System) cho phương tiện cá nhân.

    Tính năng:
    - Tiêu hao nhiên liệu khi xe đang chạy (tốc độ càng cao càng tốn nhiều).
    - Xe hết xăng không thể khởi động.
    - Đổ xăng tại điểm Gas Station bằng lệnh /refuel hoặc mua Jerry Can.
    - Lưu mức xăng vào DB khi đỗ xe (Vehicle_Park) hoặc AutoSave.

    Phụ thuộc:
    - vehicle_impl.inc – VehicleUID[], VehicleHandler[]
    - player.inc       – OnPlayerSecondUpdate, OnPlayerStateChange
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define FUEL_MAX                    (100.0)     // Đầy bình
#define FUEL_CONSUME_PER_SECOND     (0.02)      // Tiêu hao cơ bản / giây khi đứng yên
#define FUEL_CONSUME_SPEED_FACTOR   (0.001)     // Thêm fuel tiêu hao theo tốc độ
#define FUEL_WARN_THRESHOLD         (15.0)      // % cảnh báo gần hết xăng
#define FUEL_WARN_INTERVAL          (30)        // Cảnh báo mỗi N giây

// Giá đổ xăng (VND/lần)
#define FUEL_PRICE_PER_UNIT         (500)       // 500$ / 1 đơn vị fuel

// Gas station locations (model nearest check)
// Refuel trong phạm vi 10m của pump
#define FUEL_REFUEL_RADIUS          (10.0)

// =============================================================================
// STATIC VARIABLES
// =============================================================================

static
    Statement:stmt_loadFuel,
    Statement:stmt_saveFuel,
    Statement:stmt_insertFuel;

// Fuel theo vehicleid (SAMP vehicle handler)
static Float:VehicleFuel[MAX_VEHICLES];
// Timer cảnh báo
static FuelWarnTimer[MAX_PLAYERS];
// Đang đổ xăng
static bool:IsRefueling[MAX_PLAYERS];

// =============================================================================
// GAS STATION LOCATIONS (LS)
// =============================================================================

static const Float:GasStations[][3] = {
    {2206.4, -179.5,  28.8},   // Unity Station
    {-73.0,  -1210.8, 2.3},    // Verdant Meadows
    {614.4,  -1.5,    16.7},   // Blueberry
    {-2413.7, 973.1,  45.2},   // Dillimore
    {-1.4,   2035.3,  59.9},   // Ganton (LS)
    {1913.8, -1769.3, 13.4},   // El Corona
    {-2243.5,-399.2,  36.1},   // Flint County
    {1029.7, -1026.7, 32.1},   // Martin Bridge
    {2092.8, 2377.8,  10.8},   // Palomino Creek
    {-1606.0,-673.5,  14.5}    // Fisher's Lagoon
};

// =============================================================================
// HOOKS – MYSQL SETUP
// =============================================================================

hook OnMySQLConnected()
{
    stmt_insertFuel = MySQL_PrepareStatement(MySQL_GetHandle(),
        "INSERT INTO vehicle_fuel (vehicle_id, fuel) VALUES (?, ?) ON DUPLICATE KEY UPDATE fuel = VALUES(fuel)");

    static const loadQ[] = "SELECT fuel FROM vehicle_fuel WHERE vehicle_id = ?";
    stmt_loadFuel = MySQL_PrepareStatement(MySQL_GetHandle(), loadQ);

    static const saveQ[] = "UPDATE vehicle_fuel SET fuel = ? WHERE vehicle_id = ?";
    stmt_saveFuel = MySQL_PrepareStatement(MySQL_GetHandle(), saveQ);

    return 1;
}

// =============================================================================
// PUBLIC API – LOAD / SAVE
// =============================================================================

stock Fuel_Load(vehicleID, vehicleHandler)
{
    inline OnFuelLoad()
    {
        new Float:fuel;
        MySQL_BindResultFloat(stmt_loadFuel, 0, fuel);

        if (MySQL_Statement_FetchRow(stmt_loadFuel))
            VehicleFuel[vehicleHandler] = fuel;
        else
            VehicleFuel[vehicleHandler] = FUEL_MAX; // Xe mới – đầy bình
    }

    MySQL_BindInt(stmt_loadFuel, 0, vehicleID);
    MySQL_ExecuteParallel_Inline(stmt_loadFuel, using inline OnFuelLoad);
}

stock Fuel_Save(vehicleID, vehicleHandler)
{
    MySQL_BindFloat(stmt_saveFuel, 0, VehicleFuel[vehicleHandler]);
    MySQL_BindInt(stmt_saveFuel, 1, vehicleID);
    MySQL_ExecuteThreaded(stmt_saveFuel);
}

stock Fuel_Insert(vehicleID, vehicleHandler)
{
    MySQL_BindInt(stmt_insertFuel, 0, vehicleID);
    MySQL_BindFloat(stmt_insertFuel, 1, FUEL_MAX);
    MySQL_ExecuteThreaded(stmt_insertFuel);
    VehicleFuel[vehicleHandler] = FUEL_MAX;
}

// =============================================================================
// GETTERS / SETTERS
// =============================================================================

stock Float:Vehicle_GetFuel(vehicleid)
    return VehicleFuel[vehicleid];

stock Vehicle_SetFuel(vehicleid, Float:amount)
{
    if (amount > FUEL_MAX) amount = FUEL_MAX;
    if (amount < 0.0) amount = 0.0;
    VehicleFuel[vehicleid] = amount;
}

stock Vehicle_AddFuel(vehicleid, Float:amount)
    Vehicle_SetFuel(vehicleid, VehicleFuel[vehicleid] + amount);

// =============================================================================
// HOOK – TIÊU HAO NHIÊN LIỆU (mỗi giây)
// =============================================================================

hook OnPlayerSecondUpdate(playerid)
{
    // Chỉ xử lý khi đang lái xe
    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return 1;

    new vehicleid = GetPlayerVehicleID(playerid);
    if (!vehicleid || vehicleid == INVALID_VEHICLE_ID) return 1;

    // Nếu đã hết xăng
    if (VehicleFuel[vehicleid] <= 0.0)
    {
        VehicleFuel[vehicleid] = 0.0;

        // Tắt động cơ
        new engines, lights, alarm, doors, bonnet, boot, objective;
        GetVehicleParamsEx(vehicleid, engines, lights, alarm, doors, bonnet, boot, objective);
        if (engines == VEHICLE_PARAMS_ON)
        {
            SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_OFF, lights, alarm, doors, bonnet, boot, objective);
            SendErrorMsg(playerid, "Xe da het xang! Hay den tram xang gan nhat de do xang.");
        }
        return 1;
    }

    // Tinh tieu hao theo toc do
    new Float:speed = GetVehicleSpeed(vehicleid);

    new Float:consume = FUEL_CONSUME_PER_SECOND + (speed * FUEL_CONSUME_SPEED_FACTOR);
    VehicleFuel[vehicleid] -= consume;
    if (VehicleFuel[vehicleid] < 0.0) VehicleFuel[vehicleid] = 0.0;

    // Cảnh báo gần hết xăng (mỗi FUEL_WARN_INTERVAL giây)
    if (VehicleFuel[vehicleid] <= FUEL_WARN_THRESHOLD)
    {
        FuelWarnTimer[playerid]++;
        if (FuelWarnTimer[playerid] >= FUEL_WARN_INTERVAL)
        {
            FuelWarnTimer[playerid] = 0;
            SendMsgF(playerid, COLOR_ORANGE, "[XANG] Chi con %.1f%% xang. Hay do xang som!", VehicleFuel[vehicleid]);
        }
    }
    else
    {
        FuelWarnTimer[playerid] = 0;
    }

    return 1;
}

// =============================================================================
// HOOK – RESET KHI KẾT NỐI
// =============================================================================

hook OnPlayerConnect(playerid)
{
    FuelWarnTimer[playerid] = 0;
    IsRefueling[playerid] = false;
    return 1;
}

// =============================================================================
// HELPER – Kiểm tra gần Gas Station
// =============================================================================

static bool:IsNearGasStation(playerid)
{
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    for (new i = 0; i < sizeof(GasStations); i++)
    {
        if (IsPlayerInRangeOfPoint(playerid, FUEL_REFUEL_RADIUS, GasStations[i][0], GasStations[i][1], GasStations[i][2]))
            return true;
    }
    return false;
}

// =============================================================================
// COMMAND – /refuel
// =============================================================================

CMD:refuel(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER && GetPlayerState(playerid) != PLAYER_STATE_ONFOOT)
        return SendErrorMsg(playerid, "Ban can o trong xe hoac gan xe de do xang."), 1;

    if (!IsNearGasStation(playerid))
        return SendErrorMsg(playerid, "Ban khong o gan tram xang! Tim bien hieu tram xang tren ban do."), 1;

    if (IsRefueling[playerid])
        return SendErrorMsg(playerid, "Ban dang do xang roi!"), 1;

    new vehicleid;

    if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
        vehicleid = GetPlayerVehicleID(playerid);
    else
    {
        // Tìm xe gần nhất
        new Float:px, Float:py, Float:pz;
        GetPlayerPos(playerid, px, py, pz);
        new Float:minDist = 5.0;
        foreach (new i : PersonalVehicle)
        {
            new Float:vx2, Float:vy2, Float:vz2;
            GetVehiclePos(Vehicle_GetHandler(i), vx2, vy2, vz2);
            new Float:d = Distance(px, py, pz, vx2, vy2, vz2);
            if (d < minDist)
            {
                minDist = d;
                vehicleid = Vehicle_GetHandler(i);
            }
        }
        if (!vehicleid)
            return SendErrorMsg(playerid, "Khong tim thay xe ca nhan gan do."), 1;
    }

    new Float:currentFuel = VehicleFuel[vehicleid];
    new Float:needed      = FUEL_MAX - currentFuel;

    if (needed <= 0.1)
        return SendMsgF(playerid, COLOR_GREEN, "Binh xang da day (%.1f%%).", currentFuel), 1;

    new cost = floatround(needed) * FUEL_PRICE_PER_UNIT;

    if (GetPlayerMoney(playerid) < cost)
        return SendErrorMsgF(playerid, "Ban khong du tien! Can $%d de do %.1f don vi xang.", cost, needed), 1;

    IsRefueling[playerid] = true;
    GivePlayerMoney(playerid, -cost);
    Vehicle_SetFuel(vehicleid, FUEL_MAX);
    IsRefueling[playerid] = false;

    SendMsgF(playerid, COLOR_GREEN, "Da do day xang! Chi phi: $%d.", cost);
    return 1;
}

// Alias /gas
ALTCMD:gas(refuel);
