# Kế Hoạch Tối Ưu Hóa Toàn Bộ Hệ Thống (All System Optimization Roadmap)

## Context & Objectives
Mục tiêu là tối ưu hóa toàn bộ hệ thống dự án GTAHUB bằng cách sử dụng các tính năng tiên tiến của **PawnPlus** (`String:`, `List:`, `Map:`, `Task:` Async) kết hợp với các nguyên tắc Clean Code / DRY / Dynamic Memory Allocation. Điều này giúp giảm >80% bộ nhớ RAM tĩnh (Static RAM), loại bỏ nguy cơ tràn mảng, và nâng cao hiệu năng xử lý của server (FPS & Tick rate).

---

## Roadmap & Execution Checklist

- [ ] **Sub-task 1: Tối ưu Hệ thống Chat IC/OOC (`core/chat/` & `core/cmds/`)**
  - Tích hợp PawnPlus `String:` loại bỏ mảng tĩnh `chat_LastMessage` (768B/player).
  - Tích hợp `List:` / `Map:` cho Lịch sử PM đa tuyến (`cmds_pm.inc`).
  - Tối ưu bộ lọc Mention `@user` và Chat Global `/r` không block thread.

- [ ] **Sub-task 2: Tối ưu Hệ thống Vật phẩm & Kho đồ Người chơi (`core/player/`)**
  - Refactor `player_inventory.inc` và `player_items.inc` từ mảng 2 chiều tĩnh sang PawnPlus `List:` / `Map:`.
  - Tối ưu bộ đệm phím tắt `player_hotkeys.inc` và kỹ năng nhân vật.

- [ ] **Sub-task 3: Tối ưu Quản lý Phương tiện & Cốp xe (`core/player/vehicle/`)**
  - Chuyển đổi lưu trữ phụ tùng, cốp xe (`vehicle_trunk`), tình trạng hư hỏng sang PawnPlus `Map:`.
  - Tối ưu hóa các vòng lặp kiểm tra khoảng cách phương tiện.

- [ ] **Sub-task 4: Tối ưu Hệ thống Kinh tế, Điện thoại & Async Logging (`core/systems/`)**
  - Áp dụng PawnPlus `Task:` Async cho hệ thống Paycheck (`paycheck_system.inc`), Tin nhắn điện thoại (`phone_system.inc`), và cửa hàng (`stores_system.inc`).
  - Ghi nhật ký giao dịch và log hệ thống bất đồng bộ không gây sụt giảm FPS server.

---

## Chi tiết Sub-task 1: Chat System Optimization (Thực hiện đầu tiên)

### Thay đổi dự kiến:
1. **[chat_OnPlayerText.inc](file:///d:/GTAHUB/gamemodes/core/chat/chat_OnPlayerText.inc)**:
   * Thay thế `chat_LastMessage[MAX_PLAYERS][CHAT_MAX_UTF8_BYTES]` bằng `String:g_LastPlayerMessage[MAX_PLAYERS]`.
   * Sử dụng `str_clone` / `str_val` để kiểm tra duplicate message cực nhanh và dọn dẹp khi ngắt kết nối.
2. **[cmds_pm.inc](file:///d:/GTAHUB/gamemodes/core/cmds/cmds_pm.inc)**:
   * Chuyển `PlayerLastPM` thành `List:g_PMHistory[playerid]`.
   * Lệnh `/re` phản hồi người gần nhất, lệnh `/replylist` hiển thị danh sách người vừa PM.
3. **[chat_rp.inc](file:///d:/GTAHUB/gamemodes/core/chat/chat_rp.inc)**:
   * Chuẩn hóa `CMD:r` (Chat Global) và định dạng tin nhắn bằng PawnPlus dynamic string.

---

## Verification Plan
1. **Automated Build**: Chạy `sampctl build` sau mỗi Sub-task để đảm bảo 0 lỗi compilation.
2. **Runtime Verification**: Kiểm tra việc giải phóng bộ nhớ (garbage collection) và khả năng xử lý chuỗi UTF-8 tiếng Việt.
