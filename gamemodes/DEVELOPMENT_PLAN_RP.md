# Kế Hoạch Phát Triển Gameplay (Phần 2: Tính Năng Tiếp Theo - Roleplay Server)

Tài liệu này nối tiếp danh sách các hạng mục nền tảng (Mục 1 - 12) từ [`TODO_PLAN.md`](file:///c:/Users/LENOVO/Documents/GitHub/GTAHUB/gamemodes/TODO_PLAN.md), quy định các **chức năng tiếp theo** để phát triển gamemode GTAHUB thành một Server Roleplay (RP) hoàn chỉnh.

---

## 🔗 Liên Kết Lộ Trình Lớp Nền Tảng (`TODO_PLAN.md`)

- [x] **1. Audit hệ thống hiện tại**
- [x] **2. Hệ thống interaction dùng chung**
- [x] **3. Stream/load protection quanh entrance**
- [x] **4. Label trang trọng cho điểm quan trọng**
- [ ] **5. Speedometer** *(Đang triển khai)*
- [x] **6. Hệ thống giấy phép** *(Hoàn thành)*
- [ ] **7. Bài kiểm tra lái xe**
- [ ] **8. Điện thoại di động & công cộng cơ bản**
- [ ] **9. Menu tương tác người chơi cơ bản**
- [ ] **10. Perk, VIP và hoạt động grind**
- [ ] **11. Rework job & việc làm thêm**
- [ ] **12. Hệ thống level/XP nghề nghiệp**

---

## 🚀 Danh Sách Việc Cần Làm Tiếp Theo (Kế Hoạch Roleplay)

### 13. Hệ thống Tạo & Quản lý Nhân vật IC (Multi-Character & RP Identity) ✅

- [x] Cho phép 1 tài khoản master sở hữu tối đa 2 nhân vật IC.
- [x] Thiết lập thông tin nhân vật: Họ & Tên IC (`Firstname Lastname`), Tuổi, Giới tính.
- [x] Giao diện Character Creation UI: Tên, Tuổi, Giới tính, Chọn Skin ban đầu (Dialog).
- [x] Giao diện Character Selection UI: Chọn nhân vật khi đăng nhập.
- [x] Spawn tại vị trí lần cuối thoát, lưu stats theo char_id. – `core/account/account_character.inc`
- [x] Lệnh: `/charinfo`, `/mychar`, `/switchchar`, `/deletechar`.
- **File:** `core/account/account_character.inc`, `scriptfiles/characters.sql`

---

### 14. Phân chia Kênh Chat IC / OOC Chuẩn RP ✅

- [x] Chat IC bán kính (Local IC Chat) theo khoảng cách người chơi – `LOCAL_CHAT_DISTANCE (20m)`.
- [x] Thao tác hành động IC: `/me` (mô tả hành động), `/do` (mô tả môi trường/trạng thái).
- [x] Kênh chat phụ IC: Nói thầm (`/low`, `/w`), Hét lớn (`/s`, `/shout`).
- [x] Kênh chat OOC: `/b` (Local OOC 20m), `/o` (Global OOC có cooldown 20s).
- [x] Bộ đàm Faction IC: Kênh radio riêng (`/r`) và Kênh liên bộ Cảnh sát/Quân đội (`/dep`).
- **File:** `core/chat/chat_rp.inc`

---

### 15. Dynamic Inventory System (Túi đồ Slot & Trọng lượng) ✅

- [x] 12 Slot túi đồ, trọng lượng tối đa 15kg, registry 24 loại vật phẩm. – `core/player/player_inventory.inc`
- [x] Hỗ trợ thao tác: `/inv` (xem), `/drop` (thả), `/giveitem2` (đưa cho người khác).
- [x] Cốp xe (`/trunk`, `/puttrunk`, `/taketrunk`) – `vehicle_trunk` table.
- [x] Kham người cảnh sát (`/inspect`) – chỉ LSPD on duty.
- **File:** `core/player/player_inventory.inc`, `scriptfiles/player_inventory.sql`

---

### 16. Hệ thống Kinh Tế & Ngân Hàng RP (Economy & Banking) ✅

- [x] **Paycheck System**: Trả lương định kỳ mỗi 60 phút (Lương Faction/Job, bonus level, tự động tính thuế). – `core/systems/paycheck_system.inc`
- [x] **Chuyển khoản Ngân hàng (`/transfer`)**: Chuyển tiền giữa các tài khoản ngân hàng người chơi (phí 2%). – `core/cmds/cmds_bank.inc`
- [x] **Xem số dư (`/balance`, `/bal`)**: Kiểm tra tiền mặt & ngân hàng. – `core/cmds/cmds_bank.inc`
- [ ] **Hóa đơn & Tiền phạt IC**: Cảnh sát xuất hóa đơn phạt (tích hợp `/fine` của LSPD).

---

### 17. Nâng cấp Xe Cá Nhân RP (Vehicle System RP) ✅

- [x] **Hệ thống Xăng / Nhiên liệu (Fuel System)**: Tiêu hao theo tốc độ, cảnh báo 15%, đổ xăng tại Gas Station (`/refuel`, `/gas`). – `core/player/vehicle/vehicle_fuel.inc`
- [x] **Động cơ thủ công (`/engine`)**: Bật/Tắt máy (chỉ chủ xe hoặc người có chìa khóa), kiểm tra xăng trước khi khởi động. – `core/player/vehicle/vehicle_keys.inc`
- [x] **Khóa/Mở cửa xe (`/lock`)**: Ngăn người không có chìa khóa vào xe. – `core/player/vehicle/vehicle_keys.inc`
- [x] **Chia sẻ chìa khóa phụ (`/givekey`, `/takekey`)**: Giao/Thu chìa khóa phụ (tối đa 4 người). – `core/player/vehicle/vehicle_keys.inc`
- [ ] Đăng ký biển số IC tại City Hall.
- [ ] Hỏng hóc chi tiết (nổ lốp, hỏng động cơ, cạn ắc quy).

---

### 18. Faction Hợp Pháp (LSPD, EMS/LSFD, City Hall) ✅

- [x] **LSPD On/Off Duty (`/duty`)**: Mặc đồng phục, cấp trang bị theo rank, thông báo bộ đàm. – `core/systems/lspd_system.inc`
- [x] **MDC – Mobile Data Computer (`/mdc`)**: Tra cứu hồ sơ nhân thân: tên, level, wanted, giấy phép. – `core/systems/lspd_system.inc`
- [x] **Phiếu phạt IC (`/fine`)**: Cảnh sát phạt tiền người chơi có lý do, trừ tiền mặt/ngân hàng. – `core/systems/lspd_system.inc`
- [x] **Thăng cấp LSPD (`/setrank`)**: Admin cấp rank cảnh sát (level 4+). – `core/systems/lspd_system.inc`
- [ ] EMS/LSFD: On-duty, Cứu thương, Băng bó, CPR, Cáng bệnh nhân. ✅ `core/systems/ems_system.inc`
- [x] **City Hall RP**: Đăng ký biển số xe IC (`/platereg`), Cấp GPDKD (`/bizlicense`), Xem giấy phép (`/mybiz`). – `core/systems/cityhall_rp.inc`

---

### 19. Faction Bất Hợp Pháp (Gangs & Chợ Đen) ✅

- [x] **Chiếm Đánh Địa Bàn (Turf System)**: 10 turf tại LS, timer chiếm 60s, cần ≥2 thành viên. – `core/systems/turf_system.inc`
- [x] **GangZone hiển thị**: Màu gang trên minimap theo chủ sở hữu, nhấp nháy khi bị tấn công. – `core/systems/turf_system.inc`
- [x] **Thu thuế địa bàn**: Mỗi 30 phút, gang có turf nhận tiền thuế chia đều thành viên online. – `core/systems/turf_system.inc`
- [x] **Lệnh `/turfinfo`, `/attackturf`**: Xem thông tin và khởi phát tấn công turf. – `core/systems/turf_system.inc`
- [x] Chế Tạo & Buôn Lậu: Crafting ma túy (Crack, Heroin), Chợ đen mua vũ khí. ✅ `core/systems/blackmarket_system.inc`

---

### 20. Smartphone / Điện Thoại RP ✅

- [x] **Số điện thoại 7 chữ số duy nhất**: Auto-generate khi đăng ký, load khi login. – `core/systems/phone_system.inc`
- [x] **Gửi SMS (`/sms [so] [tin]`)**: Gửi tin nhắn tới số điện thoại player đang online. – `core/systems/phone_system.inc`
- [x] **Gọi điện thoại (`/call [so]`)**: Dialog chấp nhận/từ chối, `/hangup` cúp máy. – `core/systems/phone_system.inc`
- [x] **Gọi khẩn cấp (`/911`)**: Thông báo tới tất cả LSPD on duty, đặt checkpoint dẫn đường. – `core/systems/phone_system.inc`
- [x] **Danh bạ (`/addcontact`, `/myphone`)**: Lưu tên & số điện thoại. – `core/systems/phone_system.inc`
- [x] **Giao diện Smartphone TextDraw UI**: Phông chữ hiện số điện thoại, trạng thái cuộc gọi (Ringing/On-Call/Idle), đếm thời gian gọi. – `core/user-interface/ui_phone.inc`

---

### 21. Server-side Anti-Cheat & Công Cụ Admin RP ✅

- [x] **Hệ thống Ticket Support (`/report`, `/reports`, `/closereport`)**: Player gửi báo cáo, Admin tiếp nhận và đóng. – `core/cmds/cmds_admin_rp.inc`
- [x] **Warn System (`/warn`, `/warns`)**: 3 cảnh cáo tự động ban 24h. – `core/cmds/cmds_admin_rp.inc`
- [x] **Admin Duty Toggle (`/aduty`)**: Bật/Tắt Admin Duty OOC mode. – `core/cmds/cmds_admin_rp.inc`
- [x] **Anti-Cheat Server-side**: Health, Money, Weapon, Teleport, Speedhack detection. Vi phạm tich lũy -> Kick -> Ban. – `core/systems/anticheat_rp.inc`

---

## 🎯 Milestone Tiếp Theo

- [x] **Milestone 7 (Phần A)**: Chat IC/OOC RP (`/me`, `/do`, `/low`, `/s`, `/r`, `/b`, `/o`) hoàn thành.
- [x] **Milestone 8 (Phần A)**: Paycheck, `/transfer`, Nhiên liệu xe, Khóa/Động cơ xe hoàn thành.
- [x] **Milestone 9 (Phần A)**: LSPD Duty + MDC, Turf System, Phone System cơ bản hoàn thành.
- [x] **Milestone 10 (Phần A)**: Ticket Support, Warn System, Admin Duty hoàn thành.
- [x] **Milestone 11 (Batch 2)**: Multi-Character, Dynamic Inventory, EMS System, Black Market & Crafting – Hoàn thành!

---

## 📋 Tổng Hợp Cuối

**Đã hoàn thành:** Mục 13, 14, 15, 16, 17, 18 (A+EMS), 19 (A+BM), 20, 21.
**Còn lại:** City Hall, Biển số xe, Smartphone TextDraw HUD, Anti-cheat full.
