/*
    player_hotkeys.inc
    ------------------
    Hệ thống Phím Tắt Tương Tác Nhanh (QoL Hotkeys).

    Tính năng:
    - Nhấn H (KEY_CTRL_BACK): Khóa / Mở khóa xe gần nhất.
    - Nhấn Y (KEY_YES): Mở cửa/vào nhà (đã có từ tương tác chung, có thể mở rộng).
    - Nhấn N (KEY_NO): Mở túi đồ (Inventory).
*/

#include <YSI_Coding\y_hooks>

// =============================================================================
// HOOKS
// =============================================================================

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    // Bỏ qua nếu người chơi đang nhập liệu dialog hoặc chat
    if (GetPVarInt(playerid, "DialogOpened") == 1) return 1;

    // Nhấn N (KEY_NO) -> Mở Inventory
    if (PRESSED(KEY_NO))
    {
        // Gọi lệnh /inv hoặc /inventory (từ player_inventory.inc)
        new emptyInvParams[1];
        cmd_inv(playerid, emptyInvParams);
        return 1;
    }

    // Nhấn H (KEY_CTRL_BACK) -> Khóa / Mở khóa xe khi đang đi bộ
    if (PRESSED(KEY_CTRL_BACK) && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
    {
        new emptyLockParams[1];
        cmd_lock(playerid, emptyLockParams);
        return 1;
    }

    // Nhấn còi hú (KEY_SUBMISSION = phím 2) khi lái xe LSPD / EMS
    if (PRESSED(KEY_SUBMISSION) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
    {
        new class = Player_GetClass(playerid);
        if (class == TEAM_POLICE || class == TEAM_MEDIC)
        {
            new vehicleid = GetPlayerVehicleID(playerid);
            new engine, lights, alarm, doors, bonnet, boot, objective;
            GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
            // Toggle còi hú
            if (alarm == VEHICLE_PARAMS_ON)
            {
                SetVehicleParamsEx(vehicleid, engine, lights, VEHICLE_PARAMS_OFF, doors, bonnet, boot, objective);
                SendMsgF(playerid, COLOR_ORANGE, "[Xe] Da tat coi hu + den uu tien.");
            }
            else
            {
                SetVehicleParamsEx(vehicleid, engine, lights, VEHICLE_PARAMS_ON, doors, bonnet, boot, objective);
                SendMsgF(playerid, COLOR_RED, "[Xe] Da bat coi hu + den uu tien!");
            }
        }
    }

    // Nhấn Y (KEY_YES) -> Đã được xử lý trong interaction system, không cần ghi đè
    // Nhưng nếu tương lai có action riêng lẻ, sẽ add vào đây.

    return 1;
}
