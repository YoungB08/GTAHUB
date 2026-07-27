# Kế Hoạch Phát Triển Gameplay (Phần 2: Tính Năng Tiếp Theo - Roleplay Server)

Tài liệu này nối tiếp danh sách các hạng mục nền tảng (Mục 1 - 12) từ [`TODO_PLAN.md`](file:///c:/Users/LENOVO/Documents/GitHub/GTAHUB/gamemodes/TODO_PLAN.md), quy định các **chức năng tiếp theo** để phát triển gamemode GTAHUB thành một Server Roleplay (RP) hoàn chỉnh.

---

## 🔗 Liên Kết Lộ Trình Lớp Nền Tảng (`TODO_PLAN.md`)

- [x] **1. Audit hệ thống hiện tại**
- [x] **2. Hệ thống interaction dùng chung**
- [x] **3. Stream/load protection quanh entrance**
- [x] **4. Label trang trọng cho điểm quan trọng**
- [ ] **5. Speedometer** *(Đang triển khai)*
- [ ] **6. Hệ thống giấy phép** *(Đang triển khai)*
- [ ] **7. Bài kiểm tra lái xe**
- [ ] **8. Điện thoại di động & công cộng cơ bản**
- [ ] **9. Menu tương tác người chơi cơ bản**
- [ ] **10. Perk, VIP và hoạt động grind**
- [ ] **11. Rework job & việc làm thêm**
- [ ] **12. Hệ thống level/XP nghề nghiệp**

---

## 🚀 Danh Sách Việc Cần Làm Tiếp Theo (Kế Hoạch Roleplay)

### 13. Hệ thống Tạo & Quản lý Nhân vật IC (Multi-Character & RP Identity)

- [ ] Cho phép 1 tài khoản master sở hữu tối đa 2-3 nhân vật IC.
- [ ] Thiết lập thông tin nhân vật: Họ & Tên IC (`Firstname_Lastname`), Tuổi, Giới tính, Ngày sinh, Tiền sử IC.
- [ ] Giao diện Character Creation UI: Tạo nhân vật mới, chọn trang phục (Skin) ban đầu.
- [ ] Giao diện Character Selection UI: Chọn nhân vật khi đăng nhập.
- [ ] Hệ thống chọn vị trí Spawn RP (Nhà riêng, Lần cuối thoát, Faction HQ, Bệnh viện).

---

### 14. Phân chia Kênh Chat IC / OOC Chuẩn RP

- [ ] Chat IC bán kính (Local IC Chat) theo khoảng cách người chơi.
- [ ] Thao tác hành động IC: `/me` (mô tả hành động), `/do` (mô tả môi trường/trạng thái).
- [ ] Kênh chat phụ IC: Nói thầm (`/low`, `/w`), Hét lớn (`/s`), Loa cầm tay (`/m` - Megaphone).
- [ ] Kênh chat OOC: `/b` (Local OOC), `/o` (Global OOC có cooldown), `/pm` (Tin nhắn OOC cá nhân).
- [ ] Bộ đàm Faction IC: Kênh radio riêng (`/r`) và Kênh liên bộ bộ đàm Cảnh sát/Y tế (`/dep`).

---

### 15. Dynamic Inventory System (Túi đồ Slot & Trọng lượng)

- [ ] Chuyển đổi module `player_items` thành hệ thống Túi đồ theo Slot/Trọng lượng với UI hiện đại.
- [ ] Hỗ trợ các thao tác vật phẩm: Vứt (Drop), Sử dụng (Use), Giao dịch (Give), Xem thông tin (Inspect).
- [ ] Hỗ trợ lưu trữ vật phẩm ngoại cảnh: Cốp xe (Vehicle Trunk), Tủ đồ nhà riêng (House Storage), Kho Faction.

---

### 16. Hệ thống Kinh Tế & Ngân Hàng RP (Economy & Banking)

- [ ] **Paycheck System**: Trả lương định kỳ mỗi giờ chơi (Lương Faction/Job hoặc trợ cấp thất nghiệp, tự động trừ thuế nhà/xe).
- [ ] **Chuyển khoản Ngân hàng (`/transfer`)**: Chuyển tiền giữa các tài khoản ngân hàng người chơi tại ATM/Ngân hàng.
- [ ] **Hóa đơn & Tiền phạt (Invoices & Fines)**: Cảnh sát, Bác sĩ và Doanh nghiệp có thể xuất hóa đơn/tiền phạt cho người chơi thanh toán.

---

### 17. Nâng cấp Xe Cá Nhân RP (Vehicle System RP)

- [ ] **Đăng ký biển số IC**: Mua xe tại Dealership -> Đăng ký biển số chính thức tại City Hall.
- [ ] **Hệ thống Xăng / Nhiên liệu (Fuel System)**: Tiêu hao nhiên liệu theo tốc độ/loại xe, đổ xăng tại Cây xăng hoặc dùng Bình xăng dự phòng (Jerry Can).
- [ ] **Động cơ & Chìa khóa xe**: Bật/Tắt máy thủ công (`/engine` hoặc phím `N`), Khóa/Mở cửa xe (`/lock`), Chia sẻ chìa khóa phụ (`/givekey`).
- [ ] **Hỏng hóc chi tiết**: Hỏng động cơ, nổ lốp, cạn bình ắc quy; yêu cầu Thợ máy (Mechanic) hoặc Bộ dụng cụ sửa xe (Repair Kit).

---

### 18. Faction Hợp Pháp (LSPD, EMS/LSFD, City Hall)

- [ ] **LSPD (Los Santos Police Department)**:
  - On Duty / Off Duty, lấy trang bị theo Rank.
  - Máy tính Cảnh sát (MDC - Mobile Data Computer): Tra cứu tiền án, biển số xe, danh sách truy nã IC.
  - Lập biên bản phạt, tạm giam tại Trụ sở hoặc Nhà tù Bolingbroke.
- [ ] **EMS / LSFD (Cấp Cứu & Cứu Hỏa)**:
  - Băng bó, Hồi sức CPR, Cáng cứu thương, Đưa về Bệnh viện điều trị.
  - Xử lý các trạng thái chấn thương (Bị đạn bắn, tai nạn xe, ngộ độc).
- [ ] **Chính Phủ (City Hall)**: Quản lý ngân sách thành phố, cấp giấy phép kinh doanh, điều chỉnh thuế suất.

---

### 19. Faction Bất Hợp Pháp (Gangs & Chợ Đen)

- [ ] **Băng Đảng (Gangster / Mafia / Cartel)**: Quản lý Rank, Bảng phân quyền Faction, Kho Faction (Tiền + Vật phẩm + Vũ khí).
- [ ] **Chiếm Đánh Địa Bàn (Turf System)**: Phân chia khu vực tại Los Santos, tranh chấp turf và thu thuế địa bàn.
- [ ] **Chế Tạo & Buôn Lậu (Crafting & Black Market)**: Quy trình chế tạo Ma túy nâng cao (Weed/Meth/Crack), Chợ đen mua bán linh kiện súng cấm.

---

### 20. Smartphone UI Nâng Cao (Mobile Phone RP)

- [ ] **Giao diện Smartphone TextDraw**: Hiển thị dạng điện thoại thông minh ở góc màn hình.
- [ ] **Ứng dụng Danh bạ & Gọi/SMS**: Lưu danh bạ, gửi tin nhắn SMS, gọi điện thoại real-time.
- [ ] **Ứng dụng GPS Navigation**: Định vị nhanh các địa điểm công cộng, đồn cảnh sát, bệnh viện, cây xăng, việc làm.
- [ ] **Ứng dụng Dịch Vụ**: Gọi khẩn cấp 911 (Cảnh sát/Cứu thương), Taxi, Thợ máy (Mechanic).

---

### 21. Server-side Anti-Cheat & Công Cụ Admin RP

- [ ] **Hệ thống Ticket Support (`/reports`)**: Người chơi gửi yêu cầu trợ giúp IC/OOC -> Admin tiếp nhận và xử lý.
- [ ] **Admin Spectate Nâng Cao (`/spec`)**: Xem thông số real-time (Tiền, Faction, Ping, FPS, Vũ khí, Xe).
- [ ] **Xử Phạt RP**: `Jail RP` (Giam OOC do vi phạm luật RP), `Warn System` (3 cảnh cáo tự động Ban).
- [ ] **Kích hoạt Server-side Anti-Cheat**: Bật toàn bộ các module Anti-cheat độc lập có sẵn trong repo (Weapon, Money, Teleport, Airbreak, Speedhack, Car Jack).

---

## 🎯 Milestone Tiếp Theo

- [ ] **Milestone 5**: Hệ thống Giấy phép (License), Driving Test & Speedometer hoàn thành.
- [ ] **Milestone 6**: Điện thoại di động (Phone System) & Menu tương tác người chơi (Player Menu) hoạt động.
- [ ] **Milestone 7**: Hệ thống Tạo nhân vật IC (Multi-char) & Chat IC/OOC (`/me`, `/do`, `/r`) hoàn thành.
- [ ] **Milestone 8**: Paycheck, Ngân hàng, Nhiên liệu xe (Fuel System) & Dynamic Inventory hoạt động.
- [ ] **Milestone 9**: LSPD MDC System, EMS Cứu thương & Faction Gang Turf hoạt động.
- [ ] **Milestone 10**: Kích hoạt toàn bộ Anti-cheat server-side & Công cụ Admin RP hoàn chỉnh.
