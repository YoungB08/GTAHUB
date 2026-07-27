# Kế Hoạch Phát Triển Gameplay (Phần 4: Advanced Systems & QoL)

Tài liệu này vạch ra các hệ thống nâng cao (Advanced Systems) và cải thiện chất lượng trải nghiệm (Quality of Life) cho giai đoạn tiếp theo của server. Các tính năng này tập trung vào tính chân thực (realism), nền kinh tế tự do và sự liên kết chặt chẽ giữa các Roleplay Faction.

---

## 🎯 Danh Sách Chức Năng Nâng Cao (Mục 28 - 33)

### 28. Cửa Hàng Bán Xe & Quản Lý Phương Tiện Nâng Cao (Dealership & Vehicle Management)
- [ ] **Tích hợp UI Dealership**: Liên kết `ui_dealership.inc` với logic backend để người chơi có thể mua xe bằng tiền IC.
- [ ] **Bảo hiểm xe & Phí gửi xe**: Xe bị nổ sẽ mất phí bảo hiểm để lấy lại tại bãi đỗ xe (Impound Lot).
- [ ] **Hệ thống hư hỏng (Vehicle Damage System)**: Xe va chạm mạnh có tỷ lệ hỏng động cơ (không thể khởi động), yêu cầu gọi Mechanic (Thợ sửa xe).
- **File dự kiến:** `core/player/vehicle/vehicle_dealership.inc`, `core/player/vehicle/vehicle_damage.inc`

### 29. Hệ Thống Tử Vong & Vết Thương (Death & Injury System)
- [ ] **Trạng thái "Bị Thương Nặng" (Downed/Brutally Wounded)**: Khi HP = 0, thay vì chết và hồi sinh tại viện ngay lập tức, người chơi ngã xuống đất với animation đau đớn.
- [ ] **Thời gian đếm ngược**: Chờ 5-10 phút để EMS đến cứu (lệnh `/revive` của hệ thống EMS đã làm).
- [ ] **Chấp nhận tử vong (Accept Death)**: Có thể chọn `/acceptdeath` để chết hẳn, hồi sinh tại bệnh viện, mất một lượng tiền viện phí và một số vật phẩm.
- **File dự kiến:** `core/player/player_death.inc`

### 30. Kho Đồ Chung Cho Faction & Gang (Faction / Gang Safes)
- [ ] **Tủ chứa đồ chung**: LSPD có Locker chứa súng đạn; Gang có Két sắt chứa ma túy, tiền bẩn.
- [ ] **Tích hợp Inventory System**: Sử dụng chung core slot/weight inventory system, hỗ trợ `/putsafe` và `/takesafe`.
- [ ] **Quyền truy cập (Permissions)**: Chỉ các rank cao trong Faction/Gang mới có quyền quản lý hoặc giới hạn số lượng lấy.
- **File dự kiến:** `core/systems/faction_safe.inc`

### 31. Doanh Nghiệp Do Người Chơi Sở Hữu (Player-Owned Businesses)
- [ ] **Mua bán doanh nghiệp**: Người chơi có Giấy Phép Kinh Doanh (GPDKD) có thể mua các cửa hàng (24/7, Tiệm Súng, Trạm Xăng).
- [ ] **Quản lý kho hàng (Stock Management)**: Chủ tiệm phải mua hàng (nhập kho) để bán. Hết hàng -> cửa hàng đóng cửa.
- [ ] **Định giá (Pricing)**: Chủ tiệm tự quyết định giá bán các mặt hàng (VD: Nước từ $10 đến $50).
- [ ] **Thu lợi nhuận**: Tiền khách mua chảy vào quỹ doanh nghiệp (Business Safe), chủ tiệm có thể rút tiền.
- **File dự kiến:** `core/systems/business_economy.inc`

### 32. Hệ Thống Đói Khát & Thể Trạng (Hunger & Thirst)
- [ ] **Chỉ số sinh tồn**: Thêm thanh Thể lực (Đói) và Nước (Khát) trên HUD hoặc TextDraw.
- [ ] **Giảm theo thời gian**: Chỉ số tụt dần mỗi phút. Khi Đói/Khát về 0, người chơi bắt đầu mất HP.
- [ ] **Tiêu thụ vật phẩm**: Tích hợp Inventory (`/use food`, `/use water`) để hồi phục các chỉ số này. Cần mua thức ăn tại 24/7, nhà hàng, hoặc câu cá.
- **File dự kiến:** `core/player/player_survival.inc`

### 33. Cải Thiện Trải Nghiệm & Phím Tắt (QoL Hotkeys)
- [ ] **Phím tắt tương tác nhanh**:
  - Nhấn `H`: Khóa / Mở khóa xe.
  - Nhấn `Y` tại cửa: Vào / Ra nhà.
  - Nhấn `N`: Mở túi đồ (Inventory).
- [ ] **Cảnh sát - Phím Còi Hú**: Phím tắt bật/tắt đèn ưu tiên nhanh cho LSPD/EMS.
- **File dự kiến:** `core/player/player_hotkeys.inc`

---

## 📅 Lộ Trình Triển Khai (Roadmap)
Sau khi dọn dẹp các tính năng cuối cùng của Giai đoạn 3 (Job Skills & Interaction Menu), server sẽ chuyển sang thực hiện Giai đoạn 4 với thứ tự ưu tiên:
1. **Death & Injury System** (Hoàn thiện quy trình Roleplay y tế).
2. **Hunger & Thirst** (Tăng giá trị cho vật phẩm Food/Water trong túi đồ).
3. **Player-Owned Businesses** (Đẩy mạnh kinh tế nội bộ).
4. **Advanced Vehicle & Faction Safes** (Độ sâu quản lý tổ chức & phương tiện).
