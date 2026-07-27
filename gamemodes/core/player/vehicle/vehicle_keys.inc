/*
    vehicle_keys.inc
    ----------------
    Hệ thống Chìa khóa, Động cơ & Khóa cửa xe cá nhân.

    Tính năng:
    - /engine  – Bật/Tắt động cơ xe (chỉ chủ xe hoặc người được giao chìa khóa)
    - /lock    – Khóa/Mở cửa xe (chỉ chủ xe)
    - /givekey [playerid] – Giao chìa khóa phụ cho người chơi khác
    - /takekey [playerid] – Thu lại chìa khóa phụ

    Người có chìa khóa phụ được phép vào xe và bật động cơ.
    Người không có chìa khóa bị khóa cửa không vào được.
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// CONSTANTS
// =============================================================================

#define MAX_VEHICLE_KEYS    (4)     // Tối đa bao nhiêu người được giao chìa khóa phụ
#define KEY_GIVE_RADIUS     (5.0)   // Bán kính để giao chìa khóa

// =============================================================================
// DATA STRUCTURES
// =============================================================================

// Chìa khóa phụ: lưu danh sách playerid được phép dùng xe
// Indexed theo PersonalVehicle iterator index (không phải vehicleid)
static VehicleKeyHolders[MAX_OWNED_VEHICLES][MAX_VEHICLE_KEYS];
static bool:VehicleLocked[MAX_OWNED_VEHICLES];   // true = đang khoá cửa

// =============================================================================
// INITIALIZATION
// =============================================================================

hook OnPlayerConnect(playerid)
{
    // Reset key holders của vehicle index liên quan nếu cần
    return 1;
}

// Reset khi xe bị huỷ (player disconnect)
hook OnPlayerDisconnect(playerid, reason)
{
    // Thu hồi chìa khóa phụ của player khi họ thoát
    foreach (new i : PersonalVehicle)
    {
        for (new k = 0; k < MAX_VEHICLE_KEYS; k++)
        {
            if (VehicleKeyHolders[i][k] == playerid)
                VehicleKeyHolders[i][k] = INVALID_PLAYER_ID;
        }
    }
    return 1;
}

// Init key holders khi vehicle được load
static InitVehicleKeys(vehIndex)
{
    VehicleLocked[vehIndex] = false;
    for (new k = 0; k < MAX_VEHICLE_KEYS; k++)
        VehicleKeyHolders[vehIndex][k] = INVALID_PLAYER_ID;
}

// =============================================================================
// HELPERS
// =============================================================================

// Lấy vehicle index từ vehicleid (SAMP handler)
static GetPersonalVehicleIndex(vehicleid)
{
    foreach (new i : PersonalVehicle)
    {
        if (Vehicle_GetHandler(i) == vehicleid)
            return i;
    }
    return -1;
}

// Kiểm tra player có quyền dùng xe không (chủ hoặc có key)
static bool:Player_CanUseVehicle(playerid, vehIndex)
{
    // Chủ xe
    if (Vehicle_IsOwner(playerid, Vehicle_GetHandler(vehIndex)))
        return true;

    // Người được giao chìa khóa
    for (new k = 0; k < MAX_VEHICLE_KEYS; k++)
    {
        if (VehicleKeyHolders[vehIndex][k] == playerid)
            return true;
    }
    return false;
}

// =============================================================================
// HOOK – Ngăn vào xe khi bị khoá và không có chìa khóa
// =============================================================================

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    if (newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER)
    {
        new vehicleid = GetPlayerVehicleID(playerid);
        new vehIndex  = GetPersonalVehicleIndex(vehicleid);

        if (vehIndex == -1) return 1; // Không phải xe cá nhân

        if (VehicleLocked[vehIndex] && !Player_CanUseVehicle(playerid, vehIndex))
        {
            // Đá ra khỏi xe
            RemovePlayerFromVehicle(playerid);
            SendErrorMsg(playerid, "Xe nay dang khoa cua! Ban can co chia khoa de vao.");
        }
    }
    return 1;
}

// =============================================================================
// /engine – Bật / Tắt động cơ
// =============================================================================

CMD:engine(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
        return SendErrorMsg(playerid, "Ban phai ngoi o ghe lai xe."), 1;

    new vehicleid = GetPlayerVehicleID(playerid);
    new vehIndex  = GetPersonalVehicleIndex(vehicleid);

    if (vehIndex == -1)
        return SendErrorMsg(playerid, "Day khong phai xe ca nhan."), 1;

    if (!Player_CanUseVehicle(playerid, vehIndex))
        return SendErrorMsg(playerid, "Ban khong co chia khoa xe nay!"), 1;

    // Kiểm tra xăng
    if (Vehicle_GetFuel(vehicleid) <= 0.0)
        return SendErrorMsg(playerid, "Xe het xang khong the khoi dong! Hay do xang truoc."), 1;

    new engines, lights, alarm, doors, bonnet, boot, objective;
    GetVehicleParamsEx(vehicleid, engines, lights, alarm, doors, bonnet, boot, objective);

    if (engines == VEHICLE_PARAMS_ON)
    {
        SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_OFF, lights, alarm, doors, bonnet, boot, objective);
        SendMsgF(playerid, COLOR_ORANGE, "[XE] Da tat dong co.");
    }
    else
    {
        SetVehicleParamsEx(vehicleid, VEHICLE_PARAMS_ON, lights, alarm, doors, bonnet, boot, objective);
        SendMsgF(playerid, COLOR_GREEN, "[XE] Da bat dong co.");
    }
    return 1;
}

// =============================================================================
// /lock – Khóa / Mở cửa xe
// =============================================================================

CMD:lock(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    // Tìm xe cá nhân gần nhất
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    new vehIndex = -1;
    new Float:minDist = 8.0;

    foreach (new i : PersonalVehicle)
    {
        // Chỉ chủ xe mới được khóa/mở cửa
        if (!Vehicle_IsOwner(playerid, Vehicle_GetHandler(i))) continue;

        new Float:vx, Float:vy, Float:vz;
        GetVehiclePos(Vehicle_GetHandler(i), vx, vy, vz);
        new Float:d = Distance(px, py, pz, vx, vy, vz);

        if (d < minDist)
        {
            minDist = d;
            vehIndex = i;
        }
    }

    if (vehIndex == -1)
        return SendErrorMsg(playerid, "Khong tim thay xe ca nhan cua ban gan day (8m)."), 1;

    VehicleLocked[vehIndex] = !VehicleLocked[vehIndex];

    if (VehicleLocked[vehIndex])
    {
        // Khoá vật lý cửa xe
        SetVehicleParamsForAll(Vehicle_GetHandler(vehIndex), 0, 1);
        SendMsgF(playerid, COLOR_ORANGE, "[XE] Da KHOA cua xe. Nguoi khac khong the vao.");
    }
    else
    {
        SetVehicleParamsForAll(Vehicle_GetHandler(vehIndex), 0, 0);
        SendMsgF(playerid, COLOR_GREEN, "[XE] Da MO KHOA cua xe.");
    }
    return 1;
}

// =============================================================================
// /givekey [playerid] – Giao chìa khóa phụ
// =============================================================================

CMD:givekey(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new targetid;
    if (sscanf(params, "u", targetid))
        return SendErrorMsg(playerid, "Su dung: /givekey [playerid]"), 1;

    if (!IsPlayerConnected(targetid) || targetid == playerid)
        return SendErrorMsg(playerid, "Nguoi choi khong hop le."), 1;

    if (!Player_IsNearPlayer(playerid, targetid, KEY_GIVE_RADIUS))
        return SendErrorMsg(playerid, "Nguoi choi do qua xa. Hay den gan ho hon."), 1;

    // Tìm xe cá nhân gần nhất mà mình là chủ
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    new vehIndex = -1;
    new Float:minDist = 8.0;

    foreach (new i : PersonalVehicle)
    {
        if (!Vehicle_IsOwner(playerid, Vehicle_GetHandler(i))) continue;
        new Float:vx, Float:vy, Float:vz;
        GetVehiclePos(Vehicle_GetHandler(i), vx, vy, vz);
        new Float:d = Distance(px, py, pz, vx, vy, vz);
        if (d < minDist) { minDist = d; vehIndex = i; }
    }

    if (vehIndex == -1)
        return SendErrorMsg(playerid, "Khong tim thay xe ca nhan cua ban gan day."), 1;

    // Kiểm tra target đã có chìa khoá chưa
    for (new k = 0; k < MAX_VEHICLE_KEYS; k++)
    {
        if (VehicleKeyHolders[vehIndex][k] == targetid)
            return SendErrorMsg(playerid, "Nguoi choi nay da co chia khoa xe nay roi."), 1;
    }

    // Thêm vào slot trống
    for (new k = 0; k < MAX_VEHICLE_KEYS; k++)
    {
        if (VehicleKeyHolders[vehIndex][k] == INVALID_PLAYER_ID)
        {
            VehicleKeyHolders[vehIndex][k] = targetid;

            SendMsgF(playerid, COLOR_GREEN, "Da giao chia khoa xe cho %s (ID: %d).",
                ReturnPlayerName(targetid), targetid);
            SendMsgF(targetid, COLOR_GREEN, "%s da giao cho ban chia khoa xe ca nhan cua ho.",
                ReturnPlayerName(playerid));
            return 1;
        }
    }

    SendErrorMsg(playerid, "Xe nay da dat gioi han so luong chia khoa phu (" #MAX_VEHICLE_KEYS ").");
    return 1;
}

// =============================================================================
// /takekey [playerid] – Thu lại chìa khóa
// =============================================================================

CMD:takekey(playerid, params[])
{
    if (!Player_IsLoggedIn(playerid))
        return SendErrorMsg(playerid, "Ban phai dang nhap truoc."), 1;

    new targetid;
    if (sscanf(params, "u", targetid))
        return SendErrorMsg(playerid, "Su dung: /takekey [playerid]"), 1;

    if (!IsPlayerConnected(targetid) || targetid == playerid)
        return SendErrorMsg(playerid, "Nguoi choi khong hop le."), 1;

    // Tìm xe cá nhân gần nhất mà mình là chủ
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    new vehIndex = -1;
    new Float:minDist = 8.0;

    foreach (new i : PersonalVehicle)
    {
        if (!Vehicle_IsOwner(playerid, Vehicle_GetHandler(i))) continue;
        new Float:vx, Float:vy, Float:vz;
        GetVehiclePos(Vehicle_GetHandler(i), vx, vy, vz);
        new Float:d = Distance(px, py, pz, vx, vy, vz);
        if (d < minDist) { minDist = d; vehIndex = i; }
    }

    if (vehIndex == -1)
        return SendErrorMsg(playerid, "Khong tim thay xe ca nhan cua ban gan day."), 1;

    for (new k = 0; k < MAX_VEHICLE_KEYS; k++)
    {
        if (VehicleKeyHolders[vehIndex][k] == targetid)
        {
            VehicleKeyHolders[vehIndex][k] = INVALID_PLAYER_ID;
            SendMsgF(playerid, COLOR_ORANGE, "Da thu hoi chia khoa tu %s.", ReturnPlayerName(targetid));
            if (IsPlayerConnected(targetid))
                SendMsgF(targetid, COLOR_ORANGE, "%s da thu hoi chia khoa xe cua ban.", ReturnPlayerName(playerid));
            return 1;
        }
    }

    SendErrorMsg(playerid, "Nguoi choi nay khong co chia khoa xe cua ban.");
    return 1;
}
