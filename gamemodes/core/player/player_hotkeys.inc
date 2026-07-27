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
        cmd_inv(playerid, "");
        return 1;
    }

    // Nhấn H (KEY_CTRL_BACK) -> Khóa / Mở khóa xe khi đang đi bộ
    if (PRESSED(KEY_CTRL_BACK) && GetPlayerState(playerid) == PLAYER_STATE_ONFOOT)
    {
        cmd_lock(playerid, "");
        return 1;
    }

    // Nhấn Y (KEY_YES) -> Đã được xử lý trong interaction system, không cần ghi đè
    // Nhưng nếu tương lai có action riêng lẻ, sẽ add vào đây.

    return 1;
}
