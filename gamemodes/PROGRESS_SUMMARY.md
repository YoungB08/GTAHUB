# Báo Cáo Tiến Độ Phát Triển Server RP (GTAHUB RP)

**Branch hiện tại:** `feature/roleplay-system-upgrade`  
**Trạng thái Git:** Working tree clean (`origin/feature/roleplay-system-upgrade` - Commit `2d74fef`).

---

## 📊 Tổng Quan Tiến Độ

### ✅ Các Tính Năng Đã Hoàn Thành & Đã Push (Batch 1)

#### 1. Hệ thống Chat IC/OOC (`core/chat/chat_rp.inc`)
- **Chat IC Local**: Tự động chuyển OnPlayerText thành chat cục bộ trong bán kính `20m` (Local Chat).
- **Hành động IC**:
  - `/me <hành động>`: Hiển thị hành động nhân vật (màu tím).
  - `/do <mô tả>`: Mô tả trạng thái/môi trường xung quanh (màu xanh da trời).
- **Kênh chat phụ IC**:
  - `/low` / `/w <tin>`: Nói thầm (bán kính 5m).
  - `/s` / `/shout <tin>`: Hét to (bán kính 50m).
- **Bộ đàm IC**:
  - `/r <tin>`: Kênh radio nội bộ Faction (Cảnh sát, Quân đội, Y tế, Gangster).
  - `/dep <tin>`: Kênh bộ đàm liên bộ (Cảnh sát & Quân đội).
- **Kênh chat OOC**:
  - `/b <tin>`: Local OOC (bán kính 20m).
  - `/o <tin>`: Global OOC toàn server (có cooldown 20s/lần).

#### 2. Nâng Cấp Xe Cá Nhân RP (`vehicle_fuel.inc`, `vehicle_keys.inc`, `vehicle_fuel.sql`)
- **Hệ thống Xăng (Fuel System)**:
  - Tiêu hao nhiên liệu theo vận tốc xe.
  - Tự động cảnh báo khi xăng dưới `15%`.
  - Nạp nhiên liệu bằng `/refuel` hoặc `/gas` tại 10 trạm xăng chính ở Los Santos.
- **Động cơ & Khóa xe**:
  - `/engine`: Bật/Tắt động cơ thủ công (yêu cầu còn xăng, chỉ chủ xe hoặc người có chìa phụ).
  - `/lock`: Khóa/Mở cửa xe (ngăn người lạ vào xe).
  - `/givekey` & `/takekey`: Giao/Thu hồi chìa khóa phụ (tối đa 4 chìa/xe).

#### 3. Kinh Tế & Ngân Hàng RP (`paycheck_system.inc`, `cmds_bank.inc`)
- **Paycheck System**:
  - Trả lương tự động mỗi 60 phút theo Class/Job + bonus theo level.
  - Lệnh `/paycheck`, `/salary` kiểm tra thông tin lương và thời gian đếm ngược.
- **Ngân Hàng RP**:
  - `/transfer [playerid] [số tiền]`: Chuyển khoản ngân hàng giữa người chơi (phí 2%).
  - `/balance`, `/bal`: Tra cứu nhanh tiền mặt & số dư ngân hàng.

#### 4. LSPD Duty & MDC System (`lspd_system.inc`)
- **Chế độ `/duty`**: Bật/tắt trực ban, mặc đồng phục theo Rank, cấp phát trang bị (Dùi đùi, Pistol, Shotgun).
- **Máy tính MDC (`/mdc [playerid]`)**: Tra cứu thông tin nhân thân, mức truy nã, chỉ số Kills/Deaths và các loại Giấy phép (Bằng lái xe, Xe tải, Máy bay, Bằng súng).
- **Phiếu phạt IC (`/fine [playerid] [số tiền] [lý do]`)**: Phạt tiền IC có lý do, tự động trừ tiền mặt/ngân hàng.
- **Thăng cấp (`/setrank`)**: Admin (level 4+) cấp rank Cảnh sát (Cadet -> Captain).

#### 5. Turf War System - Chiếm Đánh Địa Bàn (`turf_system.inc`, `turfs.sql`)
- **10 Địa bàn tại Los Santos**: Ganton, Glen Park, Idlewood, East Beach, Jefferson...
- **GangZone Display**: Hiển thị màu đại diện Gang trên minimap, nhấp nháy khi bị tấn công.
- **Tấn công địa bàn (`/attackturf`)**: Cần tối thiểu 2 thành viên trong turf, đếm ngược 60 giây.
- **Thu thuế tự động**: Mỗi 30 phút tự phân phối tiền thuế thu nhập từ địa bàn chia đều cho các thành viên gang đang online.
- **Xem thông tin (`/turfinfo`)**: Tra cứu chủ sở hữu và trạng thái địa bàn.

#### 6. Smartphone System (`phone_system.inc`, `phone.sql`)
- **Số điện thoại duy nhất**: Tự động tạo số điện thoại 7 chữ số (`09xxxxx`) khi đăng ký tài khoản.
- **Nhắn tin & Cuộc gọi**: `/sms [số] [nội dung]`, `/call [số]` (hiển thị Dialog phản hồi Realtime), `/hangup` (cúp máy).
- **Gọi khẩn cấp `/911 [lý do]`**: Gửi vị trí & tự động bật Checkpoint dẫn đường tới tất cả Cảnh sát đang On-Duty.
- **Danh bạ (`/addcontact`, `/myphone`)**: Quản lý danh bạ cá nhân.

#### 7. Admin RP Tools (`cmds_admin_rp.inc`)
- **Ticket Support System**:
  - `/report [nội dung]`: Người chơi gửi yêu cầu hỗ trợ.
  - `/reports`: Admin xem danh sách ticket đang mở.
  - `/closereport [id]`: Admin đóng ticket sau khi xử lý.
- **Cảnh báo OOC (`/warn [playerid] [lý do]`, `/warns`)**: Tích lũy 3 cảnh cáo tự động Ban 24 giờ.
- **Chế độ Admin Duty (`/aduty`)**: Chuyển đổi trạng thái làm việc OOC (đổi màu tên sang đỏ).

---

## ⌛ Các Mục Còn Lại Cần Làm (Phần B)

- [ ] **Mục 13**: Multi-Character System (Quản lý 2-3 nhân vật IC trên 1 tài khoản).
- [ ] **Mục 15**: Dynamic Inventory System (Túi đồ Slot/Weight, Cốp xe, Tủ đồ nhà riêng).
- [ ] **Mục 18 (B)**: EMS/LSFD System (Băng bó, CPR, Cáng cứu thương, Bệnh viện).
- [ ] **Mục 19 (B)**: Crafting & Black Market (Chế tạo ma túy, Chợ đen súng).
- [ ] **Mục 20 (B)**: Smartphone TextDraw UI (Giao diện hình chiếc điện thoại góc màn hình).
