(() => {
  const types = [
    { id: 'car', code: 'CLASS B', title: 'Car License', vi: 'Bằng Lái Ô Tô', text: 'Điều khiển xe dân dụng, sedan và xe thể thao.', duration: '25 câu · 15 phút', fee: '$1,500', icon: '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M19 17h2c.6 0 1-.4 1-1v-3c0-.9-.7-1.7-1.5-1.9C18.7 10.6 16 10 16 10s-1.3-1.4-2.2-2.3c-.5-.4-1.1-.7-1.8-.7H5c-.6 0-1.1.4-1.4.9l-1.5 2.8C1.4 11.3 1 12.1 1 13v3c0 .6.4 1 1 1h2"/><circle cx="7" cy="17" r="2"/><circle cx="17" cy="17" r="2"/></svg>' },
    { id: 'motorcycle', code: 'CLASS A', title: 'Motorcycle License', vi: 'Bằng Lái Xe Máy', text: 'Điều khiển xe máy, mô tô phân khối lớn.', duration: '25 câu · 15 phút', fee: '$800', icon: '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="5.5" cy="17.5" r="3.5"/><circle cx="18.5" cy="17.5" r="3.5"/><path d="M15 6h2l2 4.5M12 17.5V11l-3-4H4"/></svg>' },
    { id: 'truck', code: 'CLASS C', title: 'Truck License', vi: 'Bằng Lái Xe Tải', text: 'Vận chuyển hàng hóa thương mại hạng nặng.', duration: '25 câu · 15 phút', fee: '$3,500', icon: '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="1" y="3" width="15" height="13" rx="2"/><polygon points="16 8 20 8 23 11 23 16 16 16 16 8"/><circle cx="5.5" cy="18.5" r="2.5"/><circle cx="18.5" cy="18.5" r="2.5"/></svg>' },
    { id: 'pilot', code: 'PILOT A1', title: 'Pilot License', vi: 'Bằng Phi Công', text: 'Cất cánh và điều khiển máy bay, trực thăng.', duration: '25 câu · 15 phút', fee: '$12,000', icon: '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M17.8 19.2L16 11l3.5-3.5c.8-.8.8-2 0-2.8-.8-.8-2-.8-2.8 0L13 8.2 4.8 6.4c-.5-.1-.9.1-1.1.5l-1.2 2.3c-.2.4-.1.9.2 1.1L8 14.5l-3 3-2.3-.8c-.4-.1-.8.1-.9.4l-.6 1.1c-.2.4-.1.8.2 1.1l3.5 3.5c.3.3.7.4 1.1.2l1.1-.6c.3-.1.5-.5.4-.9l-.8-2.3 3-3 4.2 5.3c.3.3.7.4 1.1.2l2.3-1.2c.4-.2.6-.6.5-1.1z"/></svg>' },
    { id: 'boat', code: 'MARITIME', title: 'Boat License', vi: 'Bằng Tàu Thuyền', text: 'Vận hành cano, du thuyền và xuồng máy.', duration: '25 câu · 15 phút', fee: '$5,000', icon: '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M2 20a6 6 0 0 0 8 0 6 6 0 0 0 8 0 6 6 0 0 0 4 0M4 16l2-7h12l2 7H4zM12 9V3M9 6h6"/></svg>' },
    { id: 'weapon', code: 'ARMS LIC', title: 'Firearms License', vi: 'Giấy Phép Vũ Khí', text: 'Sở hữu và mang theo súng ngắn cá nhân hợp pháp.', duration: '25 câu · 15 phút', fee: '$15,000', icon: '<svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M6 18h12M12 2v6M8 8h8M4 14h16M7 18v3M17 18v3"/></svg>' }
  ];

/**
 * Trình sinh 100 câu hỏi độc lập & ngẫu nhiên cho từng loại bằng lái GTA V RP
 * Hỗ trợ 6 danh mục: car, motorcycle, truck, pilot, boat, weapon
 */
function generateCategoryQuestions(category) {
  const dataset = {
    car: {
      speedZones: [
        { zone: "khu dân cư Vinewood Hills", speed: "40 km/h" },
        { zone: "nội thành Los Santos", speed: "50 km/h" },
        { zone: "khu vực Legion Square đông người", speed: "30 km/h" },
        { zone: "đại lộ Del Perro", speed: "80 km/h" },
        { zone: "đường cao tốc Los Santos Freeway", speed: "120 km/h" },
        { zone: "khu vực trường học / bệnh viện Pillbox", speed: "25 km/h" },
        { zone: "khu công nghiệp Cypress Flats", speed: "60 km/h" },
        { zone: "đường ven biển Vespucci", speed: "45 km/h" },
        { zone: "hầm chui vượt thành phố", speed: "70 km/h" },
        { zone: "khu vực bãi đỗ xe công cộng", speed: "15 km/h" }
      ],
      scenarios: [
        { sit: "Gặp đèn đỏ tại ngã tư", ans: "Dừng hẳn trước vạch dừng chờ tín hiệu xanh", wrong: ["Tăng tốc vượt qua", "Bấm còi rẽ trái", "Chạy từ từ nếu vắng xe"] },
        { sit: "Chuyển làn đường", ans: "Bật xi nhan, quan sát gương chiếu hậu và điểm mù", wrong: ["Bấm còi liên tục", "Vọt ga chuyển tức thì", "Tắt đèn chiếu sáng"] },
        { sit: "Xe Cảnh sát (LSPD/BCSO) đang hú còi ưu tiên phía sau", ans: "Giảm tốc độ và tấp sát lề phải nhường đường", wrong: ["Đua tốc độ", "Phanh gấp giữa làn", "Bấm còi inh ỏi"] },
        { sit: "Lái xe trong điều kiện trời mưa to", ans: "Bật đèn chiếu gần và giữ khoảng cách an toàn tăng gấp đôi", wrong: ["Bật pha chiếu xa", "Chạy tối đa tốc độ", "Tắt hết đèn"] },
        { sit: "Đi vào bùng binh (vòng xoay)", ans: "Nhường đường cho các xe đang di chuyển bên trong vòng xoay", wrong: ["Cướp làn ưu tiên", "Xe to đi trước", "Bấm còi ép xe khác"] },
        { sit: "Đỗ xe trên đoạn đường có dốc", ans: "Kéo phanh tay và đánh lái hướng bánh xe vào lề đường an toàn", wrong: ["Chỉ về số N", "Tắt máy thả trôi", "Không cần khóa xe"] },
        { sit: "Người đi bộ đang qua đường tại vạch kẻ", ans: "Dừng lại nhường đường an toàn cho người đi bộ", wrong: ["Nhấn ga vọt qua", "Bấm còi đuổi", "Lách lên vỉa hè"] },
        { sit: "Khi xe có nồng độ cồn vượt mức quy định", ans: "Tuyệt đối không điều khiển phương tiện giao thông", wrong: ["Uống nước rồi lái", "Chỉ chạy chậm", "Lái đường vắng"] },
        { sit: "Gặp biển báo tam giác viền đỏ, nền vàng", ans: "Biển báo nguy hiểm, cần chú ý giảm tốc độ quan sát", wrong: ["Biển cấm đỗ", "Khu vực tự do", "Biển báo ưu tiên"] },
        { sit: "Xe phía trước bật đèn khẩn cấp (Hazard)", ans: "Giảm tốc độ, giữ khoảng cách và chỉ vượt khi an toàn", wrong: ["Bám sát đuôi", "Vượt ẩu lấn làn", "Chặn đầu xe"] }
      ]
    },

    motorcycle: {
      speedZones: [
        { zone: "hẻm nhỏ Rancho/Davis", speed: "30 km/h" },
        { zone: "đại lộ Vinewood", speed: "50 km/h" },
        { zone: "khu vực đồi núi Vinewood Hills", speed: "40 km/h" },
        { zone: "cao tốc Senora Freeway", speed: "100 km/h" },
        { zone: "đường bờ biển Palomino", speed: "60 km/h" },
        { zone: "khu dân cư Vespucci", speed: "40 km/h" },
        { zone: "tuyến đường đèo ngoằn ngoèo", speed: "35 km/h" },
        { zone: "khu chợ Mission Row", speed: "20 km/h" },
        { zone: "khu công nghiệp cảng biển", speed: "50 km/h" },
        { zone: "cầu vượt Los Santos", speed: "70 km/h" }
      ],
      scenarios: [
        { sit: "Trang bị bắt buộc khi vận hành mô tô", ans: "Đội mũ bảo hiểm đạt chuẩn cài quai đúng quy cách", wrong: ["Chỉ cần kính mát", "Đeo tai nghe nhạc", "Không cần trang bị"] },
        { sit: "Phanh gấp trên mặt đường trơn trượt", ans: "Sử dụng lực phanh cả hai bánh kết hợp nhấp nhả ga", wrong: ["Bóp chết phanh trước", "Chỉ đạp cứng phanh sau", "Thả hai tay lái"] },
        { sit: "Hành vi bốc đầu (Wheelie) trên đường phố", ans: "Vi phạm nghiêm trọng, bị tịch thu bằng lái và giam xe", wrong: ["Được khuyến khích", "Chỉ bị nhắc nhở", "Không vi phạm"] },
        { sit: "Số lượng người lớn tối đa được chở trên xe máy", ans: "Chở tối đa 1 người ngồi sau", wrong: ["Chở 2 người", "Chở 3 người", "Không giới hạn"] },
        { sit: "Khi xe máy bị nổ lốp đột ngột", ans: "Giữ chặt tay lái, hạ ga từ từ, không phanh gấp", wrong: ["Phanh gấp lập tức", "Nhảy khỏi xe", "Đánh võng xe"] },
        { sit: "Vào cua ở tốc độ cao", ans: "Giảm tốc độ trước khi vào cua và nghiêng người theo góc lái", wrong: ["Bóp phanh cứng trong cua", "Tăng hết ga", "Chống chân xuống đường"] },
        { sit: "Lái xe ban đêm", ans: "Bật đèn chiếu sáng phía trước và đèn định vị phía sau", wrong: ["Tắt đèn để ẩn nấp", "Chỉ bật đèn xi nhan", "Bật đèn cảnh báo liên tục"] },
        { sit: "Khi vượt xe ô tô cỡ lớn", ans: "Chỉ vượt bên trái nơi có tầm nhìn thoáng và không có vạch liền", wrong: ["Lách bên phải sát vỉa hè", "Vượt tại khúc cua gấp", "Bám sát đuôi xe vượt"] },
        { sit: "Khi dừng xe chờ đèn đỏ", ans: "Về số thấp hoặc về N, giữ phanh chân/tay ổn định", wrong: ["Rồ ga liên tục", "Buông cả hai tay", "Bật pha vào mặt người đối diện"] },
        { sit: "Tình trạng mệt mỏi, buồn ngủ", ans: "Dừng xe tại nơi an toàn để nghỉ ngơi", wrong: ["Chạy thật nhanh về nhà", "Vừa lái vừa gọi điện thoại", "Uống rượu giải mỏi"] }
      ]
    },

    truck: {
      weights: [
        { type: "Xe tải van giao hàng nhỏ", limit: "3.5 tấn" },
        { type: "Xe tải thùng chở hàng trung", limit: "8 tấn" },
        { type: "Xe đầu kéo Container lớn", limit: "30 tấn" },
        { type: "Xe bồn chở xăng dầu", limit: "24 tấn" },
        { type: "Xe tải chở vật liệu xây dựng", limit: "15 tấn" },
        { type: "Xe tải lạnh chở thực phẩm", limit: "10 tấn" },
        { type: "Xe cứu hộ Flatbed", limit: "12 tấn" },
        { type: "Xe ben chở cát đá", limit: "18 tấn" },
        { type: "Xe chở gỗ đường dài", limit: "25 tấn" },
        { type: "Xe siêu trường siêu trọng", limit: "Trên 40 tấn" }
      ],
      scenarios: [
        { sit: "Hệ thống phanh khí nén (Air Brake)", ans: "Xả nước bình chứa khí và kiểm tra áp suất đủ tiêu chuẩn trước khi chạy", wrong: ["Khóa van khí", "Không cần kiểm tra", "Bơm nước vào bình"] },
        { sit: "Thời gian lái xe liên tục tối đa cho phép", ans: "Không được lái quá 4 giờ liên tục", wrong: ["Lái 8 giờ liên tục", "Lái 12 giờ liên tục", "Lái không giới hạn"] },
        { sit: "Khi xe đổ dốc đèo dài", ans: "Về số thấp, sử dụng phanh động cơ kết hợp phanh chân", wrong: ["Về số N (Mo) thả trôi", "Tắt khóa điện", "Chỉ rà phanh chân liên tục"] },
        { sit: "Điểm mù nguy hiểm nhất của xe tải", ans: "Ngay sát mũi xe, bên hông phụ và trực diện toàn bộ phía sau đuôi", wrong: ["Chỉ có trên nóc thùng", "Chỉ ở gương chiếu hậu", "Không có điểm mù"] },
        { sit: "Chở hàng hóa nguy hiểm / dễ cháy nổ", ans: "Gắn biển cảnh báo nguy hiểm, trang bị bình cứu hỏa và đi đúng lộ trình", wrong: ["Chạy vào giờ cao điểm", "Không cần biển báo", "Đốt lửa kiểm tra thùng"] },
        { sit: "Quy tắc phân bổ hàng hóa trên thùng", ans: "Phân bổ đều trọng tâm, dằn buộc cố định không để xô lệch", wrong: ["Dồn toàn bộ về phía đuôi", "Chất hàng cao quá nóc", "Xếp nghiêng một bên"] },
        { sit: "Lùi xe tải vào ngõ hẹp hoặc kho bãi", ans: "Cần có người phụ xe đứng ngoài quan sát và xi nhan hướng dẫn", wrong: ["Nhắm mắt lùi nhanh", "Chỉ nhìn gương trái", "Tắt hết đèn lùi"] },
        { sit: "Khi rẽ xe đầu kéo tại ngã 4 hẹp", ans: "Mở rộng bán kính quay vòng và theo dõi sát góc cua của rơ-moóc", wrong: ["Đánh lái gắt sát lề", "Tăng tốc vượt nhanh", "Bấm còi đè làn khác"] },
        { sit: "Khoảng cách an toàn của xe tải nặng", ans: "Khoảng cách phanh dài hơn rất nhiều so với xe con thông thường", wrong: ["Ngắn hơn xe con", "Dừng tức thì khi đạp phanh", "Giống xe máy"] },
        { sit: "Kiểm tra bánh xe trước hành trình", ans: "Kiểm tra áp suất lốp, độ mòn và siết chặt các bu-lông tắc-kê", wrong: ["Chỉ cần rửa sạch mâm", "Để lốp non hơi cho êm", "Bỏ qua kiểm tra"] }
      ]
    },

    pilot: {
      altitudes: [
        { area: "khu dân cư nội đô Los Santos", alt: "1.000 feet AGL" },
        { area: "vùng hoang dã Blaine County", alt: "500 feet AGL" },
        { area: "vùng phụ cận sân bay LSIA", alt: "Theo hướng dẫn đài ATC" },
        { area: "vùng đồi núi Mount Chiliad", alt: "2.000 feet so với đỉnh núi" },
        { area: "khu vực mặt biển mở", alt: "500 feet" },
        { area: "hành lang bay quốc tế", alt: "FL 180 trở lên" },
        { area: "khu vực diễn tập quân sự", alt: "Cấm xâm phạm" },
        { area: "khu vực hải cảng Los Santos", alt: "1.000 feet" },
        { area: "khu vực sân bay Sandy Shores", alt: "800 feet AGL" },
        { area: "khu vực đập chứa nước Land Act Dam", alt: "1.500 feet" }
      ],
      scenarios: [
        { sit: "Vùng trời căn cứ quân sự Fort Zancudo", ans: "Khu vực cấm bay (Prohibited Airspace), nghiêm cấm xâm nhập", wrong: ["Vùng bay biểu diễn", "Khu vực hạ cánh tự do", "Khu vực tập lái phi cơ"] },
        { sit: "Mã Squawk Code 7700 trên Transponder", ans: "Tín hiệu cảnh báo tình trạng khẩn cấp (Mayday / Emergency)", wrong: ["Báo máy bay hạ cánh an toàn", "Báo máy bay hết dầu", "Báo thời tiết đẹp"] },
        { sit: "Mất liên lạc vô tuyến (NORDO) khi đang bay", ans: "Cài đặt Squawk 7600, tuân theo quy tắc bay khẩn cấp", wrong: ["Hạ cánh ngay xuống xa lộ", "Tắt toàn bộ hệ thống bay", "Nhảy dù khẩn cấp"] },
        { sit: "Bị cướp máy bay (Hijack)", ans: "Cài đặt Squawk 7500 và giữ bình tĩnh theo quy trình an ninh", wrong: ["Tắt động cơ", "Bật loa hành khách", "Mở cửa máy bay"] },
        { sit: "Gió giật ngược bất ngờ (Wind Shear) lúc hạ cánh", ans: "Thực hiện hủy hạ cánh và bay lên lại (Go-around)", wrong: ["Ép máy bay đập xuống sàn", "Kéo phanh dừng gấp", "Tắt động cơ thả trôi"] },
        { sit: "Bay trong điều kiện mây mù (IFR)", ans: "Điều khiển hoàn toàn dựa vào bảng đồng hồ và thiết bị dẫn đường", wrong: ["Nhìn bằng mắt thường", "Hạ thật thấp sát đất", "Bay theo cảm tính"] },
        { sit: "Quy trình Pre-flight Inspection", ans: "Bắt buộc kiểm tra kỹ thuật vỏ, cánh, dầu, lái và khí tượng trước chuyến bay", wrong: ["Bỏ qua nếu đang vội", "Chỉ cần lau sạch kính lái", "Khởi động bay luôn"] },
        { sit: "Mất áp suất khoang lái ở độ cao lớn", ans: "Đeo ngay mặt nạ dưỡng khí và hạ độ cao khẩn cấp xuống mức an toàn", wrong: ["Mở cửa sổ thông gió", "Uống nước liên tục", "Tăng độ cao tối đa"] },
        { sit: "Hỏng một động cơ trên máy bay hai động cơ", ans: "Duy trì thăng bằng, báo Mayday tới ATC và chuyển hướng về sân bay gần nhất", wrong: ["Buông cần lái", "Nhảy dù lập tức", "Tắt luôn động cơ còn lại"] },
        { sit: "Ưu tiên quyền đường trên không", ans: "Máy bay đang trong tình trạng khẩn cấp luôn có quyền ưu tiên cao nhất", wrong: ["Máy bay to hơn được ưu tiên", "Máy bay bay nhanh hơn", "Máy bay tư nhân"] }
      ]
    },

    boat: {
      locations: [
        { loc: "Khu vực bến du thuyền Puerto Del Sol", rule: "Tốc độ dưới 5 hải lý/giờ (No Wake Zone)" },
        { loc: "Khu vực bãi tắm biển Vespucci", rule: "Cấm tuyệt đối phương tiện cơ giới tiếp cận sát bờ" },
        { loc: "Vùng nước cởi mở Thái Bình Dương", rule: "Tự do hải trình nhưng giữ an toàn tầm nhìn" },
        { loc: "Vịnh Paleto Bay", rule: "Cảnh giác đá ngầm và duy trì phao cứu hộ" },
        { loc: "Kênh đào Los Santos (LS Canals)", rule: "Đi tốc độ chậm, tránh tạo sóng đánh vỡ bờ" },
        { loc: "Khu vực cảng thương mại Terminal", rule: "Nhường đường tuyệt đối cho tàu hàng siêu trọng tải" },
        { loc: "Khu vực đầm lầy Zancudo", rule: "Lưu ý bãi cạn và rạn rong biển làm kẹt chân vịt" },
        { loc: "Hồ Alamo Sea", rule: "Cảnh giác gió xoáy cục bộ và sương mù" },
        { loc: "Khu vực dàn khoan ngoài khơi", rule: "Giữ khoảng cách an toàn tối thiểu 500 mét" },
        { loc: "Khu bảo tồn biển sinh thái", rule: "Cấm xả rác và nhiên liệu độc hại" }
      ],
      scenarios: [
        { sit: "Hai thuyền đi đối đầu có nguy cơ va chạm", ans: "Cả hai thuyền cùng bẻ lái sang mạn phải (Starboard) để tránh nhau", wrong: ["Cùng bẻ sang mạn trái", "Dừng lại giữa dòng", "Tăng hết tốc độ"] },
        { sit: "Đèn mạn trái (Port Side) của tàu thuyền vào ban đêm", ans: "Có màu Đỏ", wrong: ["Màu Xanh lá cây", "Màu Trắng", "Màu Vàng chớp"] },
        { sit: "Đèn mạn phải (Starboard Side) vào ban đêm", ans: "Có màu Xanh lá cây", wrong: ["Màu Đỏ", "Màu Xanh dương", "Màu Tím"] },
        { sit: "Sự cố người rơi xuống nước (Man Overboard)", ans: "Ném phao cứu sinh, hô hoán, đánh dấu vị trí và quay mũi tàu cứu hộ", wrong: ["Tăng tốc bỏ đi", "Nhảy hết xuống biển", "Tắt toàn bộ đèn"] },
        { sit: "Trang bị an toàn cứu sinh tối thiểu", ans: "Áo phao đạt chuẩn trang bị đủ 100% cho số người trên tàu", wrong: ["Chỉ cần 1 phao tròn", "Cần câu cá", "Đèn pin nhỏ"] },
        { sit: "Khi tàu thuyền gặp giông bão lớn", ans: "Điều khiển mũi tàu hướng nghiêng góc đón sóng, duy trì vận tốc ổn định", wrong: ["Để sóng đánh ngang thân tàu", "Chạy lùi", "Tắt máy trôi dạt"] },
        { sit: "Thả neo đậu tàu ở vùng nước sâu", ans: "Chiều dài dây neo phải gấp 3 đến 5 lần độ sâu mực nước", wrong: ["Bằng đúng độ sâu nước", "Ngắn hơn độ sâu", "Gấp 50 lần"] },
        { sit: "Phao tiêu luồng hàng hải màu đỏ và xanh", ans: "Chỉ dẫn ranh giới luồng chạy an toàn tránh cạn và bãi ngầm", wrong: ["Trang trí cảnh quan", "Nơi neo đậu thuyền tự do", "Điểm câu cá"] },
        { sit: "Cập mạn bến cảng (Docking)", ans: "Tiếp cận góc nghiêng nhẹ ngược dòng chảy hoặc ngược hướng gió để hãm đà", wrong: ["Lao vuông góc thẳng tốc độ cao", "Thả neo từ xa giật lại", "Đâm mạnh vào đệm cao su"] },
        { sit: "Sử dụng bia rượu khi điều khiển tàu thuyền", ans: "Nghiêm cấm tuyệt đối theo luật hàng hải", wrong: ["Được phép nếu uống ít", "Cho phép vào ban ngày", "Không quản lý"] }
      ]
    },

    weapon: {
      rules: [
        { code: "Quy tắc 1 (An toàn súng)", desc: "Luôn coi súng đã nạp đạn và sẵn sàng nổ" },
        { code: "Quy tắc 2 (Hướng nòng súng)", desc: "Không bao giờ chĩa nòng súng vào mục tiêu không muốn tiêu diệt" },
        { code: "Quy tắc 3 (Kỷ luật ngón tay)", desc: "Giữ ngón tay ngoài vành cò súng cho đến khi ngắm bắn" },
        { code: "Quy tắc 4 (Nhận biết mục tiêu)", desc: "Luôn xác định rõ mục tiêu và những gì phía sau mục tiêu" },
        { code: "Quy tắc 5 (Bảo quản vũ khí)", desc: "Cất giữ trong két an toàn, tháo rời đạn khi không sử dụng" },
        { code: "Quy tắc 6 (Vùng cấm nổ súng)", desc: "Cấm nổ súng tại khu vực Greenzone (BV, Sở CS, Tòa án)" },
        { code: "Quy tắc 7 (Kiểm tra của Cảnh sát)", desc: "Giữ tay rời xa vũ khí, đứng yên và thông báo vị trí súng" },
        { code: "Quy tắc 8 (Quyền tự vệ chính đáng)", desc: "Chỉ được nổ súng khi tính mạng bị đe dọa trực tiếp" },
        { code: "Quy tắc 9 (Chuyển nhượng súng)", desc: "Bắt buộc sang tên đăng ký hợp pháp qua cơ quan có thẩm quyền" },
        { code: "Quy tắc 10 (Súng bị kẹt đạn)", desc: "Giữ nòng hướng an toàn 30s, tháo băng đạn rồi kéo khóa nòng" }
      ],
      scenarios: [
        { sit: "Mang súng công khai (Open Carry) không có giấy phép", ans: "Hành vi phạm tội nghiêm trọng, bị tịch thu súng và giam giữ", wrong: ["Được tự do biểu diễn", "Chỉ nhắc nhở", "Hợp pháp nếu súng không có đạn"] },
        { sit: "Khi vào các tòa nhà công quyền / Tòa án", ans: "Bắt buộc gửi vũ khí tại tủ giữ đồ an ninh hoặc bàn giao bảo vệ", wrong: ["Rút súng cầm trên tay", "Được mang tự do", "Chĩa súng chào hỏi"] },
        { sit: "Sử dụng súng khi đang say rượu bia", ans: "Vi phạm pháp luật nghiêm trọng, lập tức bị tước giấy phép vĩnh viễn", wrong: ["Chỉ phạt hành chính", "Được phép nếu chỉ bắn lên trời", "Không bị cấm"] },
        { sit: "Khi súng bị hóc/kẹt đạn tại trường bắn", ans: "Giữ nòng súng hướng về bia, đợi 30 giây, tháo hộp tiếp đạn và xử lý kẹt", wrong: ["Nhìn thẳng vào đầu nòng súng", "Đập súng xuống bàn", "Bóp cò thật mạnh"] },
        { sit: "Giấy phép sử dụng súng cá nhân (CCW)", ans: "Cấp quyền sở hữu và mang theo vũ khí tự vệ hợp pháp theo quy định IC", wrong: ["Quyền tự do bắn cướp", "Quyền miễn trừ pháp luật", "Quyền tấn công cảnh sát"] },
        { sit: "Cất giữ súng trong gia đình", ans: "Khóa súng trong hộp an toàn, bảo quản đạn ở ngăn riêng biệt ngoài tầm tay trẻ em", wrong: ["Để trên mặt bàn phòng khách", "Để dưới gối ngủ", "Ném trong cốp xe mở"] },
        { sit: "Trường hợp nổ súng tự vệ hợp pháp", ans: "Khi đối tượng dùng vũ khí nguy hiểm đe dọa trực tiếp đến tính mạng", wrong: ["Bắn cảnh cáo người cãi nhau với mình", "Bắn người đòi nợ", "Bắn chó sủa to"] },
        { sit: "Giao dịch mua bán súng giữa các công dân", ans: "Phải làm thủ tục đăng ký chuyển quyền sở hữu tại Ammu-Nation hoặc DMV", wrong: ["Giao dịch ngầm ngoài hẻm tối", "Cho mượn súng không giấy tờ", "Bán rẻ không khai báo"] },
        { sit: "Khu vực Greenzone (Khu an toàn công cộng)", ans: "Nghiêm cấm tuyệt đối mọi hành vi rút súng, đe dọa hoặc nổ súng", wrong: ["Được bắn nếu bị khiêu khích", "Cho phép nổ súng ban đêm", "Không có quy định"] },
        { sit: "Bị lực lượng LSPD yêu cầu kiểm tra giấy phép vũ khí", ans: "Hạ hai tay xuống vị trí thấy rõ, làm theo khẩu lệnh của cảnh sát", wrong: ["Rút súng ra khoe với cảnh sát", "Bỏ chạy ngay lập tức", "Bóp cò dọa cảnh sát"] }
      ]
    }
  };

  const currentCat = dataset[category] || dataset.car;
  const questions = [];

  // Tạo chính xác 100 câu hỏi logic với nội dung cụ thể
  for (let i = 0; i < 100; i++) {
    const qNum = i + 1;
    let questionText = "";
    let correctAnswer = "";
    let wrongOptions = [];

    if (category === "car" || category === "motorcycle") {
      if (i < 40) {
        const item = currentCat.speedZones[i % currentCat.speedZones.length];
        const variation = Math.floor(i / 10) + 1;
        questionText = `[Tình huống ${qNum}] Theo quy định giao thông San Andreas, tốc độ tối đa cho phép tại ${item.zone} (Điều chỉnh đợt ${variation}) là bao nhiêu?`;
        correctAnswer = item.speed;
        wrongOptions = ["20 km/h", "90 km/h", "Không giới hạn tốc độ"];
      } else {
        const item = currentCat.scenarios[i % currentCat.scenarios.length];
        const round = Math.floor(i / 10);
        questionText = `[Tình huống ${qNum} - Quy tắc an toàn #${round}] Khi gặp trường hợp: ${item.sit}, người lái xe bắt buộc phải xử lý như thế nào?`;
        correctAnswer = item.ans;
        wrongOptions = [...item.wrong];
      }
    } else if (category === "truck") {
      if (i < 40) {
        const item = currentCat.weights[i % currentCat.weights.length];
        questionText = `[Câu ${qNum}] Giới hạn tải trọng chuyên chở an toàn cho phép đối với loại phương tiện "${item.type}" là bao nhiêu?`;
        correctAnswer = item.limit;
        wrongOptions = ["1 tấn", "50 tấn", "Tự do không giới hạn"];
      } else {
        const item = currentCat.scenarios[i % currentCat.scenarios.length];
        questionText = `[Câu ${qNum}] Quy định an toàn vận tải thương mại đối với tình huống: ${item.sit} là gì?`;
        correctAnswer = item.ans;
        wrongOptions = [...item.wrong];
      }
    } else if (category === "pilot") {
      if (i < 40) {
        const item = currentCat.altitudes[i % currentCat.altitudes.length];
        questionText = `[Hàng không ${qNum}] Độ cao an toàn tối thiểu quy định khi phi cơ bay qua khu vực ${item.area} là bao nhiêu?`;
        correctAnswer = item.alt;
        wrongOptions = ["100 feet", "Sát mặt đất", "Không có quy định"];
      } else {
        const item = currentCat.scenarios[i % currentCat.scenarios.length];
        questionText = `[Hàng không ${qNum}] Quy trình ứng phó kỹ thuật bay tiêu chuẩn đối với: ${item.sit} được thực hiện ra sao?`;
        correctAnswer = item.ans;
        wrongOptions = [...item.wrong];
      }
    } else if (category === "boat") {
      if (i < 40) {
        const item = currentCat.locations[i % currentCat.locations.length];
        questionText = `[Hàng hải ${qNum}] Quy định di chuyển an toàn khi tàu thuyền hoạt động tại ${item.loc} là gì?`;
        correctAnswer = item.rule;
        wrongOptions = ["Chạy hết công suất", "Cấm mọi tàu thuyền", "Được phép diễn xiếc lượn sóng"];
      } else {
        const item = currentCat.scenarios[i % currentCat.scenarios.length];
        questionText = `[Hàng hải ${qNum}] Quy tắc an toàn hàng hải quốc tế áp dụng khi gặp tình huống: ${item.sit} là gì?`;
        correctAnswer = item.ans;
        wrongOptions = [...item.wrong];
      }
    } else if (category === "weapon") {
      if (i < 40) {
        const item = currentCat.rules[i % currentCat.rules.length];
        questionText = `[Vũ khí ${qNum}] Nội dung cốt lõi của quy tắc an toàn vũ khí cá nhân "${item.code}" là gì?`;
        correctAnswer = item.desc;
        wrongOptions = ["Sử dụng tự do nơi công cộng", "Bắn chỉ thiên khi tức giận", "Tự chế súng"];
      } else {
        const item = currentCat.scenarios[i % currentCat.scenarios.length];
        questionText = `[Vũ khí ${qNum}] Quy chuẩn pháp lý và hành xử IC khi gặp tình huống: ${item.sit} là gì?`;
        correctAnswer = item.ans;
        wrongOptions = [...item.wrong];
      }
    }

    // Xáo trộn đáp án ngẫu nhiên
    const answers = [correctAnswer, ...wrongOptions];
    const shuffled = answers
      .map((value) => ({ value, sort: Math.random() }))
      .sort((a, b) => a.sort - b.sort)
      .map((item) => item.value);

    questions.push({
      id: `${category}_${qNum}`,
      q: questionText,
      a: shuffled,
      correct: shuffled.indexOf(correctAnswer)
    });
  }

  return questions;
}
  // Pre-generate 100 questions for all 6 categories
  const questionBanks = {
    car: generateCategoryQuestions('car'),
    motorcycle: generateCategoryQuestions('motorcycle'),
    truck: generateCategoryQuestions('truck'),
    pilot: generateCategoryQuestions('pilot'),
    boat: generateCategoryQuestions('boat'),
    weapon: generateCategoryQuestions('weapon')
  };

  const userLicenses = new Set(['car']);

  let questions = [];
  let state = {
    activeTab: 'hub',
    view: 'select',
    type: 'car',
    question: 0,
    selected: null,
    answers: [],
    seconds: 900,
    timer: null
  };

  const content = document.getElementById('licenseContent');

  function updateHeader(title, subtitle, step) {
    document.getElementById('licenseTitle').textContent = title;
    document.getElementById('licenseSubtitle').textContent = subtitle;
    document.getElementById('questionCounter').textContent = step;
  }

  function cardPreview(typeId = 'car') {
    const selectedType = types.find((t) => t.id === typeId) || types[0];
    return `
      <article class="card-preview">
        <div class="card-top">
          <span>STATE OF SAN ANDREAS · DMV</span>
          <b class="card-emblem">SA</b>
        </div>
        <h3 class="card-name">HUB GTA</h3>
        <div class="card-details">
          <div class="card-detail"><span>CHỦ THẺ</span><strong>HUB GTA</strong></div>
          <div class="card-detail"><span>LOẠI BẰNG</span><strong>${selectedType.code}</strong></div>
          <div class="card-detail"><span>MÃ GIẤY PHÉP</span><strong>SA-LIC-99218</strong></div>
          <div class="card-detail"><span>NGÀY CẤP</span><strong>08/08/2026</strong></div>
        </div>
        <div class="card-bottom">
          <span>LOS SANTOS OFFICIAL DRIVER LICENSE</span>
          <span class="fake-qr" aria-label="Mã QR xác thực"></span>
        </div>
      </article>
    `;
  }

  function renderHub() {
    state.activeTab = 'hub';
    state.view = 'select';
    updateHeader('Đăng ký & Thi Bằng Lái', 'Chọn loại giấy phép để bắt đầu bài thi lý thuyết ngẫu nhiên 25 câu.', 'DMV HUB');

    content.innerHTML = `
      <div class="license-select-grid">
        ${types.map((type) => {
          const isOwned = userLicenses.has(type.id);
          return `
            <button class="license-option" data-license="${type.id}" type="button">
              <div class="option-header">
                <span class="license-option-icon">${type.icon}</span>
                <span class="license-badge">${type.code}</span>
              </div>
              <h2>${type.vi}</h2>
              <p>${type.text}</p>
              <footer>
                <span>${type.duration}</span>
                <strong>${isOwned ? 'Đã sở hữu ✓' : `Lệ phí ${type.fee} →`}</strong>
              </footer>
            </button>
          `;
        }).join('')}
      </div>

      <div class="license-preview-row">
        <div class="preview-desc">
          <span class="micro-label">MẪU THẺ CHÍNH THỨC</span>
          <h2>San Andreas Digital License Card</h2>
          <p>Mỗi đề thi được trích ngẫu nhiên 25 câu hỏi từ ngân hàng 100 câu hỏi chuẩn DMV. Đạt từ 80% (20/25 câu) trở lên để được cấp bằng chính thức.</p>
        </div>
        <div>
          ${cardPreview(state.type)}
        </div>
      </div>
    `;

    content.querySelectorAll('[data-license]').forEach((button) => {
      button.addEventListener('click', () => {
        const typeId = button.dataset.license;
        startTheory(typeId);
      });
    });
  }

  function startTheory(typeId) {
    state.type = typeId;
    const pool = questionBanks[typeId] || questionBanks.car;

    // Pick 25 random questions from the 100 question pool
    const shuffled = [...pool].sort(() => Math.random() - 0.5);
    questions = shuffled.slice(0, 25);

    state.view = 'theory';
    state.question = 0;
    state.answers = new Array(questions.length).fill(null);
    state.seconds = 900; // 15 minutes for 25 questions
    state.selected = null;

    clearInterval(state.timer);
    state.timer = setInterval(() => {
      state.seconds -= 1;
      updateTimer();
      if (state.seconds <= 0) finish();
    }, 1000);

    if (window.emitCEF) window.emitCEF('license.select', typeId);
    renderTheory();
  }

  function updateTimer() {
    const timer = document.getElementById('timerValue');
    if (timer) {
      const min = String(Math.floor(state.seconds / 60)).padStart(2, '0');
      const sec = String(state.seconds % 60).padStart(2, '0');
      timer.textContent = `${min}:${sec}`;
    }
  }

  function renderTheory() {
    const question = questions[state.question];
    const currentType = types.find((t) => t.id === state.type) || types[0];
    state.selected = state.answers[state.question] ?? null;

    updateHeader(
      'Bài thi lý thuyết',
      `${currentType.vi} · Đề 25 câu ngẫu nhiên · Đạt tối thiểu 80% (20/25) để đỗ.`,
      `CÂU ${state.question + 1} / ${questions.length}`
    );

    const progressPct = ((state.question + 1) / questions.length) * 100;

    content.innerHTML = `
      <div class="theory-layout">
        <div class="theory-main">
          <div class="progress-row">
            <span>TIẾN ĐỘ THI</span>
            <div class="progress-track"><span style="width: ${progressPct}%;"></span></div>
            <strong>${state.question + 1}/${questions.length}</strong>
          </div>

          <article class="question-card">
            <span class="micro-label">CÂU HỎI ${String(state.question + 1).padStart(2, '0')} / ${questions.length}</span>
            <h2>${question.q}</h2>
            <div class="answer-grid">
              ${question.a.map((answer, index) => `
                <button class="answer-card ${state.selected === index ? 'is-selected' : ''}" type="button" data-answer="${index}">
                  <span class="answer-letter">${String.fromCharCode(65 + index)}</span>
                  <span>${answer}</span>
                </button>
              `).join('')}
            </div>

            <div class="theory-actions">
              <button class="btn btn-secondary" id="previousQuestion" type="button" ${state.question === 0 ? 'disabled' : ''}>← Câu trước</button>
              <button class="btn btn-primary" id="nextQuestion" type="button">
                ${state.question === questions.length - 1 ? 'Nộp bài' : 'Câu tiếp →'}
              </button>
            </div>
          </article>
        </div>

        <aside class="timer-card">
          <div class="timer-circle">
            <strong id="timerValue">15:00</strong>
          </div>
          <h3>Thời gian còn lại</h3>
          <p>Nhấp vào một đáp án A, B, C hoặc D để chọn câu trả lời.</p>
          <div class="rule-list">
            <div><span>Bằng thi</span><strong>${currentType.code}</strong></div>
            <div><span>Số câu hỏi</span><strong>${questions.length} câu ngẫu nhiên</strong></div>
            <div><span>Điểm đạt</span><strong>80% (20/25 câu)</strong></div>
          </div>
        </aside>
      </div>
    `;

    updateTimer();

    // Bind Answer Card Click Events
    content.querySelectorAll('[data-answer]').forEach((button) => {
      button.addEventListener('click', () => {
        const answerIdx = Number(button.dataset.answer);
        state.selected = answerIdx;
        state.answers[state.question] = answerIdx;
        renderTheory();
      });
    });

    // Previous Question Event
    content.querySelector('#previousQuestion')?.addEventListener('click', () => {
      if (state.question > 0) {
        state.question -= 1;
        state.selected = state.answers[state.question] ?? null;
        renderTheory();
      }
    });

    // Next Question Event
    content.querySelector('#nextQuestion')?.addEventListener('click', () => {
      if (state.selected === null) {
        alert('Vui lòng chọn một đáp án trước khi chuyển sang câu tiếp theo!');
        return;
      }

      if (state.question < questions.length - 1) {
        state.question += 1;
        state.selected = state.answers[state.question] ?? null;
        renderTheory();
      } else {
        finish();
      }
    });
  }

  function finish() {
    clearInterval(state.timer);
    const correctCount = state.answers.reduce((sum, answer, index) => sum + (answer === questions[index].correct ? 1 : 0), 0);
    const passThreshold = Math.ceil(questions.length * 0.8); // 20 / 25
    const isPassed = correctCount >= passThreshold;

    state.view = 'result';
    updateHeader('Kết quả bài thi', 'Hồ sơ kết quả đã được lưu vào hệ thống DMV San Andreas.', 'KẾT QUẢ');

    content.innerHTML = `
      <div class="result-layout">
        <div class="result-message">
          <div class="result-icon ${isPassed ? 'pass' : 'fail'}">${isPassed ? '✓' : '✕'}</div>
          <h2>${isPassed ? 'Chúc mừng, bạn đã đậu!' : 'Chưa đạt yêu cầu'}</h2>
          <p>${isPassed ? `Bạn đã trả lời đúng ${correctCount}/${questions.length} câu (vượt qua mốc ${passThreshold} câu). Bằng lái đã được cấp vào ví cá nhân!` : `Bạn trả lời đúng ${correctCount}/${questions.length} câu (yêu cầu tối thiểu ${passThreshold}/25 câu). Hãy ôn tập và thi lại.`}</p>
          <button class="btn btn-primary" id="resultAction" type="button">
            ${isPassed ? 'Nhận bằng & Xem ví' : 'Thi lại ngay'}
          </button>
        </div>

        <div>
          <div class="score-card">
            <span>ĐIỂM SỐ BÀI THI</span>
            <strong>${correctCount}/${questions.length} CÂU ĐÚNG</strong>
            <div class="score-bar"><span style="width: ${(correctCount / questions.length) * 100}%;"></span></div>
            <div class="score-detail">
              <span>Đúng ${correctCount} câu</span>
              <span>Yêu cầu: ${passThreshold}/25 câu (80%)</span>
            </div>
          </div>
          <div style="margin-top: 18px;">
            ${cardPreview(state.type)}
          </div>
        </div>
      </div>
    `;

    if (window.emitCEF) window.emitCEF('license.finishTheory', correctCount);

    document.getElementById('resultAction').addEventListener('click', () => {
      if (isPassed) {
        userLicenses.add(state.type);
        if (window.emitCEF) window.emitCEF('license.issue', state.type);
        renderWallet();
      } else {
        startTheory(state.type);
      }
    });
  }

  function renderWallet() {
    state.activeTab = 'wallet';
    state.view = 'wallet';
    updateHeader('Ví Bằng Lái Cá Nhân', 'Danh sách các loại giấy phép đã được cấp chính thức.', 'VÍ CÁ NHÂN');

    content.innerHTML = `
      <div class="wallet-layout">
        <div class="wallet-header">
          <h2>Giấy phép sở hữu (HUB GTA)</h2>
          <button class="btn btn-primary btn-sm" id="verifyQrBtn" type="button">
            <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><rect x="3" y="3" width="7" height="7"/><rect x="14" y="3" width="7" height="7"/><rect x="14" y="14" width="7" height="7"/><rect x="3" y="14" width="7" height="7"/></svg>
            Xác thực QR (PD Verify)
          </button>
        </div>

        <div class="wallet-grid">
          ${types.map((type) => {
            const isOwned = userLicenses.has(type.id);
            return `
              <div class="wallet-card-item ${isOwned ? 'owned' : ''}">
                <div class="wallet-item-top">
                  <span class="wallet-icon">${type.icon}</span>
                  <span class="badge ${isOwned ? 'badge-success' : 'badge-warning'}">
                    ${isOwned ? 'ĐÃ CẤP' : 'CHƯA CÓ'}
                  </span>
                </div>
                <h3>${type.vi}</h3>
                <p>${type.text}</p>
                <div style="margin-top: auto; padding-top: 10px;">
                  ${isOwned 
                    ? `<span class="text-success" style="font-size:10px; font-weight:700;">Hạn dùng: 08/08/2027</span>`
                    : `<button class="btn btn-secondary btn-sm btn-block" data-register="${type.id}" type="button">Đăng ký thi ngay</button>`}
                </div>
              </div>
            `;
          }).join('')}
        </div>
      </div>
    `;

    document.querySelectorAll('[data-tab]').forEach((btn) => {
      btn.classList.toggle('is-active', btn.dataset.tab === 'wallet');
    });

    document.getElementById('verifyQrBtn')?.addEventListener('click', () => {
      document.getElementById('qrModal').classList.remove('hidden');
    });

    content.querySelectorAll('[data-register]').forEach((btn) => {
      btn.addEventListener('click', () => startTheory(btn.dataset.register));
    });
  }

  // Bind Header Tab Clicks
  document.querySelectorAll('[data-tab]').forEach((tabBtn) => {
    tabBtn.addEventListener('click', () => {
      const tab = tabBtn.dataset.tab;
      document.querySelectorAll('[data-tab]').forEach((b) => b.classList.remove('is-active'));
      tabBtn.classList.add('is-active');
      if (tab === 'hub') renderHub();
      else if (tab === 'wallet') renderWallet();
    });
  });

  document.getElementById('closeLicenses')?.addEventListener('click', () => {
    if (window.emitCEF) window.emitCEF('license.close');
  });

  document.getElementById('qrCloseBtn')?.addEventListener('click', () => {
    document.getElementById('qrModal').classList.add('hidden');
  });

  document.addEventListener('keydown', (event) => {
    if (event.code === 'Escape') {
      const qrModal = document.getElementById('qrModal');
      if (qrModal && !qrModal.classList.contains('hidden')) {
        qrModal.classList.add('hidden');
      } else if (window.emitCEF) {
        window.emitCEF('license.close');
      }
    }
  });

  if (window.onCEF) {
    window.onCEF('license.open', (type) => type ? startTheory(type) : renderHub());
  }

  // Initial View
  renderHub();
})();
