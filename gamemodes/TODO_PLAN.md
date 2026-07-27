# Kế Hoạch Phát Triển Gameplay

## Mục Tiêu

Tài liệu này gom các hạng mục cần làm cho gamemode, ưu tiên xây dựng hệ thống nền tảng dùng chung trước rồi mới triển khai các tính năng gameplay mở rộng.

## Danh Sách Việc Cần Làm

### 1. Audit hệ thống hiện tại

- [x] Kiểm tra code hiện có cho entrance, business, house, ATM, pickup/marker, label, dialog/textdraw và vehicle HUD.
- [x] Xác định plugin streamer/object/map đang dùng.
- [ ] Lập danh sách toàn bộ entrance, ATM, business, house, phone booth và job point cần gắn label/tương tác.
- [ ] Kiểm tra cấu trúc lưu player data/database hiện tại để chuẩn bị thêm license, phone, XP nghề.

### 2. Hệ thống interaction dùng chung

- [x] Tạo module interaction chung cho entrance, house, business, ATM, object, phone booth và job point.
- [x] Gắn module interaction vào entrance, house, business và ATM.
- [ ] Gắn module interaction vào object, phone booth và job point.
- [x] Khi người chơi đến gần entrance, house, business hoặc ATM, hiển thị tên điểm và hướng dẫn thao tác.
- [x] Hỗ trợ prompt dạng: `Nhấn Y để vào / N để hủy`.
- [x] Cho phép interaction đã gắn gọi action riêng: vào cửa, ra cửa, vào nhà, ra nhà và mở ATM.
- [ ] Gắn action riêng cho gọi điện thoại, nhận job và object interaction.
- [x] Ưu tiên cấu hình bằng data/config để hạn chế hard-code từng điểm.

### 3. Stream/load an toàn quanh entrance

- [ ] Khi người chơi đến gần entrance, preload object/map/interior liên quan.
- [x] Khi teleport vào interior/custom map, set interior và virtual world đúng.
- [x] Freeze người chơi trong thời gian ngắn nếu cần.
- [x] Đợi object/map load ổn định rồi mới unfreeze.
- [x] Mục tiêu chính: tránh lỗi rơi khỏi map khi bản đồ được chỉnh sửa hoặc tạo mới.

### 4. Label trang trọng cho điểm quan trọng

- [x] Chuẩn hóa format label cho lối vào, nhà, doanh nghiệp và ATM.
- [ ] Chuẩn hóa format label cho job point và phone booth.
- [x] Hiển thị tên địa điểm/doanh nghiệp phía trên marker/object cho entrance, house và ATM.
- [x] Hiển thị loại điểm đã gắn: `Lối vào`, `Nhà ở`, `Doanh nghiệp`, `ATM`.
- [ ] Hiển thị loại điểm còn lại: `Công việc`, `Phone booth`.
- [x] Hiển thị trạng thái cơ bản cho entrance, house và ATM.
- [ ] Hiển thị trạng thái yêu cầu giấy phép sau khi có license system.
- [x] Dùng màu sắc thống nhất, dễ đọc và không gây rối màn hình cho các điểm đã gắn.

### 5. Speedometer

- [ ] Hiển thị speedometer khi người chơi lái xe.
- [ ] Hỗ trợ xe hơi, xe máy, máy bay và thuyền nếu cần.
- [ ] Hiển thị tốc độ, máu xe, nhiên liệu nếu server có fuel, trạng thái động cơ/đèn nếu có.
- [ ] Tự ẩn khi người chơi rời khỏi phương tiện.

### 6. Hệ thống giấy phép

- [ ] Thêm dữ liệu giấy phép vào player data/database.
- [ ] Hỗ trợ bằng lái xe thường.
- [ ] Hỗ trợ bằng lái xe tải/bus nếu cần.
- [ ] Hỗ trợ bằng lái máy bay nếu cần.
- [ ] Hỗ trợ giấy phép súng.
- [ ] Hỗ trợ giấy phép nghề nghiệp đặc biệt.
- [ ] Tích hợp kiểm tra giấy phép vào job, mua/vác súng và các interaction yêu cầu điều kiện.

### 7. Bài kiểm tra lái xe

- [ ] Tạo điểm đăng ký thi bằng lái.
- [ ] Tạo phần thi lý thuyết bằng dialog/UI.
- [ ] Tạo phần thi thực hành bằng checkpoint.
- [ ] Trừ điểm khi va chạm, đi sai đường, xuống xe hoặc quá thời gian.
- [ ] Cấp bằng lái khi người chơi đạt yêu cầu.
- [ ] Cho phép mở rộng sau này cho xe tải, bus hoặc máy bay.

### 8. Điện thoại di động và điện thoại công cộng

- [ ] Tạo dữ liệu số điện thoại cho người chơi.
- [ ] Tạo điện thoại di động để nhận thông báo, nhắn tin và gọi người chơi khác.
- [ ] Tạo danh bạ cơ bản.
- [ ] Tạo điện thoại công cộng dạng interaction point.
- [ ] Điện thoại công cộng yêu cầu nhập số để gọi.
- [ ] Có thể tính phí cuộc gọi nếu economy cần.

### 9. Menu tương tác người chơi

- [ ] Tạo menu/dialog tương tác giữa người chơi.
- [ ] Hỗ trợ xem thông tin nhân vật.
- [ ] Hỗ trợ xem/thẩm tra giấy phép.
- [ ] Hỗ trợ giao dịch tiền hoặc vật phẩm nếu hệ thống inventory đã có.
- [ ] Hỗ trợ mời job/faction/party nếu server có các hệ thống này.
- [ ] Kết nối với stats và player data hiện tại.

### 10. Perk, VIP và hoạt động grind

- [ ] Tạo hệ thống perk có điều kiện mở khóa.
- [ ] Tạo VIP system có thời hạn và quyền lợi rõ ràng.
- [ ] Tránh quyền lợi VIP phá economy hoặc pay-to-win quá mạnh.
- [ ] Thêm poker hoặc minigame kiếm tiền.
- [ ] Thêm các bonus theo thời gian, job streak hoặc event nếu cần.

### 11. Rework job và việc làm thêm

- [ ] Tăng độ sâu cho các job hiện tại.
- [ ] Rework job nông trại cam thành nhiều bước: lấy dụng cụ, thu hoạch, vận chuyển, bán hàng.
- [ ] Thêm yếu tố thời gian, công sức, chất lượng hoặc giới hạn stamina nếu phù hợp.
- [ ] Thêm job mới như delivery, trucker, taxi, mechanic, miner, fisher, warehouse và pilot.
- [ ] Làm reward scale theo độ khó và yêu cầu của từng job.

### 12. Hệ thống level/XP nghề nghiệp

- [ ] Mỗi nghề có level và XP riêng.
- [ ] Job reward tăng theo level nhưng cần cân bằng economy.
- [ ] Mở khóa nhiệm vụ, phương tiện hoặc khu vực theo level nghề.
- [ ] Một số job yêu cầu bằng lái, giấy phép nghề, level tối thiểu hoặc reputation.
- [ ] Lưu XP/level vào database/player data.

## Thứ Tự Ưu Tiên

1. Interaction system và label system.
2. Stream/load protection cho entrance.
3. License system.
4. Driving test.
5. Speedometer.
6. Phone/mobile/public phone.
7. Player interaction menu.
8. Job XP/level system.
9. Rework job hiện tại.
10. Perk/VIP/poker và các hệ thống grind nâng cao.

## Milestone

- [x] Milestone 1: Người chơi đến entrance/ATM/business thấy label, prompt và bấm Y/N hoạt động.
- [x] Milestone 2: Vào interior/custom map không bị rơi map.
- [ ] Milestone 3: License và driving test hoạt động.
- [ ] Milestone 4: Speedometer và phone cơ bản hoạt động.
- [ ] Milestone 5: Menu tương tác và job XP hoạt động.
- [ ] Milestone 6: Job nâng cao, VIP/perk và poker hoạt động.

---

## 📌 Các Tính Năng Nâng Cấp RP Tiếp Theo

- **Phần 2 (Roleplay Core Systems)**: Multi-char, Chat IC/OOC, Dynamic Inventory, Faction LSPD/EMS/Gang, Smartphone UI, Fuel System...  
  👉 [`DEVELOPMENT_PLAN_RP.md`](file:///c:/Users/LENOVO/Documents/GitHub/GTAHUB/gamemodes/DEVELOPMENT_PLAN_RP.md)

- **Phần 3 (Expansion & Systems Depth)**: Driving School, House Storage, Job Skill Levels, Player Interaction Menu, Fishing System, VIP Perks...  
  👉 [`DEVELOPMENT_PLAN_EXPANSION.md`](file:///c:/Users/LENOVO/Documents/GitHub/GTAHUB/gamemodes/DEVELOPMENT_PLAN_EXPANSION.md)

