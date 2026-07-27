# Kế Hoạch Phát Triển Gameplay (Phần 3: Nâng Cấp Mở Rộng & Hệ Thống Chi Tiết RP)

Tài liệu này nối tiếp [`TODO_PLAN.md`](file:///c:/Users/LENOVO/Documents/GitHub/GTAHUB/gamemodes/TODO_PLAN.md) và [`DEVELOPMENT_PLAN_RP.md`](file:///c:/Users/LENOVO/Documents/GitHub/GTAHUB/gamemodes/DEVELOPMENT_PLAN_RP.md), tập trung nâng cấp độ sâu cho gameplay RP, hoàn thiện các hệ thống trường thi lái xe, bất động sản, nghề nghiệp có XP, menu tương tác và minigame.

---

## 🎯 Danh Sách Chức Năng Mở Rộng (Mục 22 - 27)

### 22. Trường Thi Lái Xe & Bài Kiểm Tra Thực Hành (Driving School System) ✅

- [x] **Trường thi lái xe (Driving School HQ)**: NPC Actor 17 tại Los Santos.
- [x] **Thi Lý Thuyết**: Bộ 5 câu hỏi trắc nghiệm Dialog về Luật Giao Thông IC.
- [x] **Thi Thực Hành Checkpoint**: 10 Checkpoints, kiểm tra giới hạn tốc độ 60 km/h (vượt quá 3 lần bị hủy).
- [x] **Tự động cấp bằng**: Cấp `LICENSE_CAR` vào DB khi thi đạt.
- **File:** `core/systems/driving_school.inc`

---

### 23. Nâng Cấp Bất Động Sản & Tủ Đồ Nhà Riêng (Advanced Housing & Storage) ✅

- [x] **Khóa/Mở cửa nhà (`/houselock`)**: Khóa mở cửa nhà riêng.
- [x] **Tủ đồ nhà riêng (House Storage)**: `/housedeposit`, `/housewithdraw`, `/houseinv`.
- [x] **Lưu trữ DB**: Bảng `house_inventory`.
- **File:** `core/systems/house_storage.inc`

---

### 24. Hệ Thống Level Nghề & Rework Reward (Job Skill Levels & XP)
- [ ] **Bảng lưu Level/XP Nghề**: Lưu riêng XP cho từng nghề (Trucker, Pizza, Garbage, Farmer, Mechanic).
- [ ] **Hệ thống Rank Nghề (Level 1 -> 5)**:
  - **Trucker**: Level cao chở được hàng giá trị hơn, thưởng tiền nhiều hơn.
  - **Pizza / Garbage**: Tăng bonus tiền thưởng mỗi đơn hàng theo Level.
- [ ] **Thông báo Level Up**: Hiển thị TextDraw/Message khi thăng cấp nghề.
- **File dự kiến:** `core/player/jobs/job_skills.inc`

---

### 25. Menu Tương Tác Trực Quan Giữa Người Chơi (Player Interaction Menu)
- [ ] **Lệnh `/interact [playerid]` hoặc bấm phím tương tác khi ở gần**:
  - Xem thông tin nhân vật / Bằng lái (`Inspect Licenses`).
  - Chuyển tiền mặt trực tiếp (`Pay Money`).
  - Đưa vật phẩm túi đồ (`Give Item`).
  - Cảnh sát: Khóa tay (`Cuff`), Dẫn giải (`Escort`), Khám người (`Inspect`).
  - EMS: Sơ cứu (`Bandage`), Hồi sức (`CPR`).
- **File dự kiến:** `core/player/player_interaction_menu.inc`

---

### 26. Hệ Thống Câu Cá RP (RP Fishing Minigame) ✅

- [x] **Khu vực câu cá**: 3 bãi bến cảng / hồ nước.
- [x] **Lệnh `/fish`**: Chờ cá cắn câu 6 giây.
- [x] **Tỷ lệ cá**: Cá Trích (60%), Cá Ngừ (30%), Cá Mập Nhỏ (10%).
- [x] **Lệnh `/sellfish`**: Bán cá lấy tiền mặt.
- **File:** `core/systems/fishing_system.inc`

---

### 27. Hệ Thống VIP Perks & Quyền Lợi (VIP System) ✅

- [x] **VIP Multiplier**: Silver (+10%), Gold (+20%), Platinum (+30%) Paycheck Bonus.
- [x] **Lệnh `/myvip`**: Dashboard tài khoản VIP.
- [x] **Lệnh `/vipcolor`**: Đổi màu tên OOC độc quyền (Gold/Platinum).
- [x] **Lệnh `/vipchat`**: Kênh chat riêng Platinum VIP.
- [x] **Lệnh `/setvip`**: Admin cấp VIP.
- **File:** `core/account/account_vip_perks.inc`

---

## 🎯 Thứ Tự Triển Khai Tiếp Theo

1. **Giai đoạn 1**: **Mục 22 (Trường thi lái xe)** & **Mục 23 (Tủ đồ nhà riêng)**.
2. **Giai đoạn 2**: **Mục 24 (Job Skill Levels & XP)** & **Mục 25 (Player Interaction Menu)**.
3. **Giai đoạn 3**: **Mục 26 (RP Fishing System)** & **Mục 27 (VIP System Perks)**.
