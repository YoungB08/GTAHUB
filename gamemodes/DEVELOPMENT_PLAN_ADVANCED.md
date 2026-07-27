# Kế Hoạch Phát Triển Gameplay (Phần 4: Advanced Systems & QoL)

Tài liệu này vạch ra các hệ thống nâng cao (Advanced Systems) và cải thiện chất lượng trải nghiệm (Quality of Life) cho giai đoạn tiếp theo của server. Các tính năng này tập trung vào tính chân thực (realism), nền kinh tế tự do và sự liên kết chặt chẽ giữa các Roleplay Faction.

---

## 🎯 Danh Sách Chức Năng Nâng Cao (Mục 28 - 33)

### 28. Cửa Hàng Bán Xe & Quản Lý Phương Tiện Nâng Cao (Dealership & Vehicle Management) ✅
- [x] **Tích hợp UI Dealership**: Liên kết `ui_dealership.inc` với logic backend để người chơi có thể mua xe bằng tiền IC.
- [x] **Bảo hiểm xe & Phí gửi xe**: Xe bị nổ → Impound Lot, trả phí 5% giá xe để lấy lại (`/getcar`).
- [x] **Hệ thống hư hỏng (Vehicle Damage System)**: Xe va chạm mạnh có tỷ lệ hỏng động cơ (không thể khởi động), yêu cầu gọi Mechanic (Thợ sửa xe).
- **File:** `core/player/vehicle/vehicle_dealership.inc`, `core/player/vehicle/vehicle_damage.inc`, `core/player/vehicle/vehicle_insurance.inc`

### 29. Hệ Thống Tử Vong & Vết Thương (Death & Injury System) ✅
- [x] **Trạng thái "Bị Thương Nặng" (Downed/Brutally Wounded)**: Khi HP = 0, thay vì chết và hồi sinh tại viện ngay lập tức, người chơi ngã xuống đất với animation đau đớn.
- [x] **Thời gian đếm ngược**: Chờ 5 phút để EMS đến cứu (tích hợp với lệnh `/revive` của `ems_system.inc`).
- [x] **Chấp nhận tử vong (Accept Death)**: Lệnh `/acceptdeath` (dùng được sau 60 giây ngất), hồi sinh tại bệnh viện, trừ 10% tiền mặt mang theo.
- **File:** `core/player/player_death.inc`

### 30. Kho Đồ Chung Cho Faction & Gang (Faction / Gang Safes) ✅
- [x] **Tủ chứa đồ chung**: LSPD có Locker; Gang có Két sắt — hardcoded vị trí tại DB.
- [x] **Tích hợp Inventory System**: Dùng chung inventory system, hỗ trợ `/safe`, `/putsafe [item]`, `/takesafe [item]`.
- [x] **Quyền truy cập (Permissions)**: Yêu cầu Rank 2+ (LSPD) mới được mở Locker.
- **File:** `core/systems/faction_safe.inc`

### 31. Doanh Nghiệp Do Người Chơi Sở Hữu (Player-Owned Businesses) ✅
- [x] **Mua bán doanh nghiệp**: Người chơi có GPDKD có thể mua tiệm 24/7 vô chủ (`/buybiz`).
- [x] **Quản lý kho hàng (Stock Management)**: Chủ tiệm nhập hàng qua `/bizpanel`, hàng giảm khi khách mua. Hỗ trợ lệnh `/buy` cho khách.
- [ ] **Định giá (Pricing)**: Chủ tiệm tự quyết định giá bán. *(Pending — giá hiện tại cố định)*
- [x] **Thu lợi nhuận**: Tiền khách mua chảy vào Business Safe, chủ rút bằng `/bizpanel -> Rút tiền`.
- **File:** `core/systems/business_economy.inc`

### 32. Hệ Thống Đói Khát & Thể Trạng (Hunger & Thirst) ✅
- [ ] **Chỉ số sinh tồn (HUD TextDraw)**: Hiện tại dùng lệnh `/mystats` thay HUD. *(Pending — TextDraw trực quan)*
- [x] **Giảm theo thời gian**: Đói trừ 2/phút, Khát trừ 3/phút. Khi về 0 → mất 2 HP/phút.
- [x] **Tiêu thụ vật phẩm**: `/useitem [burger/pizza/nuoc/beer/fish_small]` (hoặc `/use`) hồi phục chỉ số tương ứng.
- **File:** `core/player/player_survival.inc`

### 33. Cải Thiện Trải Nghiệm & Phím Tắt (QoL Hotkeys) ✅
- [x] **Phím tắt tương tác nhanh**:
  - Nhấn `H` (KEY_CTRL_BACK): Khóa / Mở khóa xe nhanh.
  - Nhấn `N` (KEY_NO): Mở túi đồ (Inventory).
  - Nhấn `Y` tại cửa: Vào / Ra nhà *(hệ thống interaction sẵn có)*.
- [ ] **Cảnh sát - Phím Còi Hú**: Phím tắt bật/tắt đèn ưu tiên nhanh cho LSPD/EMS. *(Pending)*
- **File:** `core/player/player_hotkeys.inc`

---

## 📅 Lộ Trình Triển Khai (Roadmap)
Sau khi dọn dẹp các tính năng cuối cùng của Giai đoạn 3 (Job Skills & Interaction Menu), server sẽ chuyển sang thực hiện Giai đoạn 4 với thứ tự ưu tiên:
1. **Death & Injury System** (Hoàn thiện quy trình Roleplay y tế).
2. **Hunger & Thirst** (Tăng giá trị cho vật phẩm Food/Water trong túi đồ).
3. **Player-Owned Businesses** (Đẩy mạnh kinh tế nội bộ).
4. **Advanced Vehicle & Faction Safes** (Độ sâu quản lý tổ chức & phương tiện).
