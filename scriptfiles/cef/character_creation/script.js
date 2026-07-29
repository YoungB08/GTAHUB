// CEF Bridge Script cho Tạo Nhân Vật GTAHUB

// Danh sách ID Skin
const SKIN_DATA = {
    "male": [
        0, 1, 2, 3, 4, 5, 6, 7, 8, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
        32, 33, 34, 35, 36, 37, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 57, 58, 59, 60, 61, 62, 66,
        67, 68, 70, 71, 72, 73, 78, 79, 80, 81, 82, 83, 84, 86, 94, 95, 96, 97, 98, 99, 100, 101, 102,
        103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123,
        124, 125, 126, 127, 128, 132, 133, 134, 135, 136, 137, 142, 143, 144, 146, 147, 149, 153, 154, 155, 156,
        158, 159, 160, 161, 162, 167, 168, 170, 171, 173, 174, 175, 176, 177, 179, 180, 181, 182, 183, 184, 185,
        186, 187, 188, 189, 200, 202, 203, 204, 206, 208, 209, 210, 212, 213, 217, 220, 221, 222, 223, 227, 228,
        229, 230, 234, 235, 236, 239, 240, 241, 242, 247, 248, 249, 250, 252, 253, 254, 255, 258, 259, 260, 261,
        262, 264, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286,
        287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299
    ],
    "female": [
        9, 10, 11, 12, 13, 31, 38, 39, 40, 41, 53, 54, 55, 56, 63, 64, 65, 69, 75, 76, 77, 85, 87, 88,
        89, 90, 91, 92, 93, 129, 130, 131, 138, 139, 140, 141, 145, 148, 150, 151, 152, 157, 169, 172, 178,
        190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 201, 205, 207, 211, 214, 215, 216, 218, 219, 224,
        225, 226, 231, 232, 233, 237, 238, 243, 244, 245, 246, 251, 256, 257, 263, 265, 266, 267
    ]
};

// Trạng thái hiện tại
let selectedGender = "male";
let selectedSkinId = 0;

// Các phần tử DOM
const charNameInput = document.getElementById("char-name");
const dobDayInput = document.getElementById("dob-day");
const dobMonthInput = document.getElementById("dob-month");
const dobYearInput = document.getElementById("dob-year");
const genderMaleBtn = document.getElementById("gender-male-btn");
const genderFemaleBtn = document.getElementById("gender-female-btn");
const charDescInput = document.getElementById("char-desc");
const selectSkinBtn = document.getElementById("select-skin-btn");
const selectedSkinLabel = document.getElementById("selected-skin-label");

const previewImage = document.getElementById("preview-image");
const previewSkinId = document.getElementById("preview-skin-id");

const skinModal = document.getElementById("skin-modal");
const skinSearch = document.getElementById("skin-search");
const skinGrid = document.getElementById("skin-grid");
const closeModalBtn = document.getElementById("close-modal-btn");
const confirmSkinBtn = document.getElementById("confirm-skin-btn");

const statusMsg = document.getElementById("status-msg");
const createBtn = document.getElementById("create-btn");
const cancelBtn = document.getElementById("cancel-btn");

// Khởi chạy
init();

function init() {
    // Sự kiện chọn giới tính bằng Nút bấm
    if (genderMaleBtn) {
        genderMaleBtn.addEventListener("click", () => setGender("male"));
    }
    if (genderFemaleBtn) {
        genderFemaleBtn.addEventListener("click", () => setGender("female"));
    }

    // Mở/Đóng Modal chọn skin
    if (selectSkinBtn) {
        selectSkinBtn.addEventListener("click", openModal);
    }
    if (closeModalBtn) {
        closeModalBtn.addEventListener("click", closeModal);
    }

    // Tìm kiếm skin trong Grid
    if (skinSearch) {
        skinSearch.addEventListener("input", (e) => {
            renderSkinGrid(e.target.value.trim());
        });
    }

    // Nút tạo nhân vật
    if (createBtn) {
        createBtn.addEventListener("click", submitCharacter);
    }

    // Nút Hủy
    if (cancelBtn) {
        cancelBtn.addEventListener("click", () => {
            emitCEF("GTAHUB:ToCharacterSelectionPage");
            window.location.href = "../character_selection/index.html";
        });
    }

    // Đóng UI khi bấm Escape
    document.addEventListener("keydown", (e) => {
        if (e.key === "Escape") {
            if (skinModal && !skinModal.classList.contains("opacity-0")) {
                closeModal();
            } else {
                closeCEF();
            }
        }
    });

    // Cập nhật preview ban đầu
    updatePreview();
}

// Thay đổi giới tính Nam / Nữ
function setGender(gender) {
    selectedGender = gender;

    if (gender === "male") {
        if (genderMaleBtn) {
            genderMaleBtn.className = "py-2.5 px-2 rounded-xl bg-red-600 border border-red-500 text-white font-bold text-xs flex items-center justify-center gap-1 transition-all";
        }
        if (genderFemaleBtn) {
            genderFemaleBtn.className = "py-2.5 px-2 rounded-xl bg-slate-950/80 border border-white/10 text-slate-400 font-bold text-xs flex items-center justify-center gap-1 hover:bg-slate-800 transition-all";
        }
        if (!SKIN_DATA.male.includes(selectedSkinId)) {
            selectedSkinId = SKIN_DATA.male[0];
        }
    } else {
        if (genderFemaleBtn) {
            genderFemaleBtn.className = "py-2.5 px-2 rounded-xl bg-red-600 border border-red-500 text-white font-bold text-xs flex items-center justify-center gap-1 transition-all";
        }
        if (genderMaleBtn) {
            genderMaleBtn.className = "py-2.5 px-2 rounded-xl bg-slate-950/80 border border-white/10 text-slate-400 font-bold text-xs flex items-center justify-center gap-1 hover:bg-slate-800 transition-all";
        }
        if (!SKIN_DATA.female.includes(selectedSkinId)) {
            selectedSkinId = SKIN_DATA.female[0];
        }
    }

    updatePreview();
}

// Cập nhật khung xem trước Skin
function updatePreview() {
    if (previewImage) previewImage.src = `http://gtahub.kntech.co/skins/${selectedSkinId}.png`;
    if (previewSkinId) previewSkinId.innerText = `SKIN #${selectedSkinId}`;
    if (selectedSkinLabel) selectedSkinLabel.innerText = `Đang chọn Skin: #${selectedSkinId}`;
}

// Mở Modal chọn Skin
function openModal() {
    if (!skinModal) return;
    renderSkinGrid();
    skinModal.classList.remove("opacity-0", "pointer-events-none");
    skinModal.firstElementChild.classList.remove("scale-95");
    skinModal.firstElementChild.classList.add("scale-100");
}

// Đóng Modal chọn Skin
function closeModal() {
    if (!skinModal) return;
    skinModal.classList.add("opacity-0", "pointer-events-none");
    skinModal.firstElementChild.classList.remove("scale-100");
    skinModal.firstElementChild.classList.add("scale-95");
}

// Render kho skin trong Modal theo Giới tính đang chọn
function renderSkinGrid(searchQuery = "") {
    if (!skinGrid) return;
    skinGrid.innerHTML = "";

    const availableSkins = SKIN_DATA[selectedGender] || [];
    const filteredSkins = availableSkins.filter(id => {
        if (!searchQuery) return true;
        return id.toString().includes(searchQuery);
    });

    if (filteredSkins.length === 0) {
        skinGrid.innerHTML = `<div class="col-span-6 text-center text-xs text-slate-500 py-8">Không tìm thấy skin phù hợp</div>`;
        return;
    }

    filteredSkins.forEach(skinId => {
        const isSelected = (skinId === selectedSkinId);
        const card = document.createElement("div");
        card.className = `p-2 bg-slate-950/80 border rounded-2xl flex flex-col items-center justify-between cursor-pointer transition-all hover:scale-105 ${
            isSelected ? "border-red-500 bg-red-950/40" : "border-white/5 hover:border-white/20"
        }`;

        card.innerHTML = `
            <img src="http://gtahub.kntech.co/skins/${skinId}.png" alt="Skin ${skinId}" class="h-24 object-contain mb-1">
            <span class="text-[10px] font-mono font-bold ${isSelected ? 'text-red-400' : 'text-slate-400'}">#${skinId}</span>
        `;

        card.addEventListener("click", () => {
            selectedSkinId = skinId;
            updatePreview();
            closeModal();
        });

        skinGrid.appendChild(card);
    });
}

// Kiểm tra và gửi thông tin nhân vật
function submitCharacter() {
    const name = charNameInput ? charNameInput.value.trim() : "";
    const day = dobDayInput ? dobDayInput.value.trim() : "";
    const month = dobMonthInput ? dobMonthInput.value.trim() : "";
    const year = dobYearInput ? dobYearInput.value.trim() : "";
    const desc = charDescInput ? charDescInput.value.trim() : "";

    // 1. Kiểm tra Tên nhân vật (Định dạng Roleplay: Firstname_Lastname)
    const nameRegex = /^[A-Z][a-zA-Z]+_[A-Z][a-zA-Z]+$/;
    if (!name) {
        showStatus("Vui lòng nhập họ và tên nhân vật!", "error");
        if (charNameInput) charNameInput.focus();
        return;
    }
    if (!nameRegex.test(name)) {
        showStatus("Tên sai định dạng Roleplay! Ví dụ đúng: John_Smith (viết hoa chữ cái đầu và có dấu '_')", "error");
        if (charNameInput) charNameInput.focus();
        return;
    }

    // 2. Kiểm tra Ngày sinh & Tuổi (18 tuổi trở lên)
    if (!day || !month || !year) {
        showStatus("Vui lòng nhập đầy đủ Ngày, Tháng, Năm sinh!", "error");
        return;
    }

    const d = parseInt(day), m = parseInt(month), y = parseInt(year);
    if (isNaN(d) || d < 1 || d > 31 || isNaN(m) || m < 1 || m > 12 || isNaN(y) || y < 1950 || y > 2007) {
        showStatus("Ngày tháng năm sinh không hợp lệ! Năm sinh phải từ 1950 đến 2007.", "error");
        return;
    }

    const dobString = `${y}-${m.toString().padStart(2, '0')}-${d.toString().padStart(2, '0')}`;
    const age = 2025 - y;
    if (age < 18 || age > 80) {
        showStatus("Tuổi nhân vật phải từ 18 đến 80 tuổi để tham gia thành phố!", "error");
        return;
    }

    // 3. Kiểm tra Mô tả nhân vật
    if (!desc) {
        showStatus("Vui lòng nhập mô tả ngoại hình hoặc tiểu sử ngắn!", "error");
        if (charDescInput) charDescInput.focus();
        return;
    }
    if (desc.length < 10) {
        showStatus("Mô tả nhân vật quá ngắn! Vui lòng nhập tối thiểu 10 ký tự.", "error");
        if (charDescInput) charDescInput.focus();
        return;
    }

    // Khóa nút tạo
    showStatus("Đang xử lý tạo nhân vật...", "info");
    setLoadingState(true);

    // Gửi sự kiện lên CEF server
    emitCEF("GTAHUB:CreateCharacter", name, dobString, selectedGender, selectedSkinId, desc);
}

// Trạng thái nút Tạo
function setLoadingState(isLoading) {
    if (!createBtn) return;
    if (isLoading) {
        createBtn.disabled = true;
        createBtn.innerText = "Đang Tạo...";
        createBtn.classList.add("opacity-50", "cursor-not-allowed");
    } else {
        createBtn.disabled = false;
        createBtn.innerText = "Hoàn Tất Tạo";
        createBtn.classList.remove("opacity-50", "cursor-not-allowed");
    }
}

// Hiển thị thông báo trạng thái
function showStatus(msg, type = "info") {
    if (!statusMsg) return;
    statusMsg.innerText = msg;
    statusMsg.classList.remove("hidden", "bg-red-500/20", "text-red-300", "border-red-500/30", "bg-emerald-500/20", "text-emerald-300", "border-emerald-500/30", "bg-blue-500/20", "text-blue-300", "border-blue-500/30");

    if (type === "error") {
        statusMsg.classList.add("bg-red-500/20", "text-red-300", "border", "border-red-500/30");
    } else if (type === "success") {
        statusMsg.classList.add("bg-emerald-500/20", "text-emerald-300", "border", "border-emerald-500/30");
    } else {
        statusMsg.classList.add("bg-blue-500/20", "text-blue-300", "border", "border-blue-500/30");
    }
}

// Lắng nghe phản hồi tạo nhân vật từ Server Pawn
if (window.cef) {
    cef.on("GTAHUB:CreateCharacterResponse", (success, message) => {
        setLoadingState(false);
        if (success) {
            showStatus(message || "Tạo nhân vật thành công! Đang chuyển...", "success");
        } else {
            showStatus(message || "Tạo nhân vật thất bại!", "error");
        }
    });
}

// Phát sự kiện CEF
function emitCEF(eventName, ...args) {
    if (window.cef) {
        cef.emit(eventName, ...args);
    } else {
        console.log(`[Test Mode] Phát sự kiện: ${eventName}`, args);
    }
}

// Đóng UI
function closeCEF() {
    emitCEF("GTAHUB:CloseUI");
}
