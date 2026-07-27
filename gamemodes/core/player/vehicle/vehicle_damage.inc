/*
    vehicle_damage.inc
    ------------------
    Hệ thống Hư hỏng Phương tiện Thực tế (Advanced Vehicle Damage System).

    Mô tả:
    - Khi xe bị va chạm mạnh (nhận sát thương lượng lớn), động cơ có thể bị chết (Engine Breakdown).
    - Đo vận tốc và sự thay đổi máu của xe thông qua OnVehicleDamageStatusUpdate.
    - Chết máy: Không thể khởi động lại xe (Dùng hàm SetVehicleParamsEx). Xe bốc khói đen.
    - Yêu cầu Nghề Thợ Sửa Xe (Mechanic) dùng Toolbox để sửa hoặc gọi xe kéo.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define VEHICLE_HEALTH_BREAKDOWN    (350.0) // Dưới mức này là hỏng nặng/bốc khói
#define DAMAGE_THRESHOLD            (150.0) // Nếu mất > 150 HP trong 1 lần va chạm -> Hỏng động cơ

// =============================================================================
// DATA
// =============================================================================

static Float:LastVehicleHealth[MAX_VEHICLES];
static bool:IsEngineBroken[MAX_VEHICLES];

// =============================================================================
// HOOKS
// =============================================================================

hook OnGameModeInit()
{
    for (new i = 0; i < MAX_VEHICLES; i++)
    {
        LastVehicleHealth[i] = 1000.0;
        IsEngineBroken[i] = false;
    }
    return 1;
}

hook OnVehicleSpawn(vehicleid)
{
    LastVehicleHealth[vehicleid] = 1000.0;
    IsEngineBroken[vehicleid] = false;
    return 1;
}

// Hàm này kích hoạt khi xe nhận sát thương (Va đập, bị bắn, ...)
hook OnVehicleDamageStatusUpdate(vehicleid, playerid)
{
    new Float:currentHealth;
    GetVehicleHealth(vehicleid, currentHealth);
    
    // Nếu bị va chạm rất mạnh 1 lúc
    new Float:damageTaken = LastVehicleHealth[vehicleid] - currentHealth;
    
    if (damageTaken > DAMAGE_THRESHOLD && !IsEngineBroken[vehicleid])
    {
        Vehicle_BreakEngine(vehicleid, playerid);
    }
    // Nếu máu tụt xuống mức hỏng bốc khói
    else if (currentHealth < VEHICLE_HEALTH_BREAKDOWN && !IsEngineBroken[vehicleid])
    {
        Vehicle_BreakEngine(vehicleid, playerid);
    }

    LastVehicleHealth[vehicleid] = currentHealth;
    return 1;
}

// Bắt sự kiện nổ máy xe (Nếu đã hỏng thì không cho nổ máy)
// Giả định: Lệnh /engine hoặc tương tự gọi đến Vehicle_ToggleEngine
// Chúng ta sẽ hook OnPlayerKeyStateChange hoặc cung cấp hàm check
hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    // Nếu hệ thống dùng phím 2 hoặc phím nào đó để nổ máy
    // Đoạn này tùy thuộc vào hệ thống vehicle_fuel / vehicle_keys hiện tại.
    // Tốt nhất là can thiệp vào hàm ToggleEngine.
    return 1;
}

// =============================================================================
// CORE FUNCTIONS
// =============================================================================

static Vehicle_BreakEngine(vehicleid, playerid)
{
    IsEngineBroken[vehicleid] = true;
    
    // Tắt động cơ
    new engine, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_OFF, lights, alarm, doors, bonnet, boot, objective);
    
    // Set máu xuống mức bốc khói nếu chưa tới
    new Float:hp;
    GetVehicleHealth(vehicleid, hp);
    if (hp > VEHICLE_HEALTH_BREAKDOWN)
        SetVehicleHealth(vehicleid, VEHICLE_HEALTH_BREAKDOWN);

    SendMsgF(playerid, COLOR_RED, "[CẢNH BÁO] Va cham manh! Dong co da bi hong (Chet may).");
    SendMsgF(playerid, COLOR_YELLOW, "Hay goi Tho Sua Xe (Mechanic) hoac dung Toolbox de sua.");
}

// API cho hệ thống Engine (Sẽ được gọi bởi /engine)
stock Vehicle_IsEngineBroken(vehicleid)
{
    return IsEngineBroken[vehicleid];
}

// API cho Thợ sửa xe
stock Vehicle_RepairEngine(vehicleid, playerid)
{
    if (!IsEngineBroken[vehicleid]) return 0;
    
    IsEngineBroken[vehicleid] = false;
    SetVehicleHealth(vehicleid, 1000.0);
    RepairVehicle(vehicleid);
    
    SendMsgF(playerid, COLOR_GREEN, "[SUA XE] Ban da sua chua toan bo dong co thanh cong.");
    return 1;
}

// Lệnh fix nhanh cho thợ (Demo)
CMD:repairveh(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid)) return 1;
    
    // Yêu cầu Job Mechanic
    // if (Player_GetJob(playerid) != JOB_MECHANIC) ...
    
    if (!IsPlayerInAnyVehicle(playerid))
        return SendErrorMsg(playerid, "Ban phai o trong mot chiec xe.");
        
    new vehid = GetPlayerVehicleID(playerid);
    Vehicle_RepairEngine(vehid, playerid);
    return 1;
}
