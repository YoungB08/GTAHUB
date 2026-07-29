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
const charDobInput = document.getElementById("char-dob");
const charGenderInput = document.getElementById("char-gender");
const charDescInput = document.getElementById("char-desc");
const selectSkinBtn = document.getElementById("select-skin-btn");
const selectedSkinLabel = document.getElementById("selected-skin-label");

const previewImage = document.getElementById("preview-image");
const previewSkinId = document.getElementById("preview-skin-id");

const skinModal = document.getElementById("skin-modal");
const skinSearch = document.getElementById("skin-search");
const skinGrid = document.getElementById("skin-grid");
const closeModalBtn = document.getElementById("close-modal-btn");

const statusMsg = document.getElementById("status-msg");
const createBtn = document.getElementById("create-btn");
const cancelBtn = document.getElementById("cancel-btn");

// Khởi chạy
init();

function init() {
    // Sự kiện thay đổi giới tính
    if (charGenderInput) {
        charGenderInput.addEventListener("change", (e) => {
            setGender(e.target.value);
        });
    }

    // Sự kiện mở/đóng Modal chọn skin
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

// Cập nhật trạng thái giới tính
function setGender(gender) {
    selectedGender = gender;
    // Đặt skin mặc định cho giới tính tương ứng
    selectedSkinId = (gender === "male") ? 0 : 9;
    updatePreview();
}

// Cập nhật hình ảnh xem trước & nhãn hiển thị
function updatePreview() {
    const skinUrl = `http://gtahub.kntech.co/skins/${selectedSkinId}.png`;
    if (previewImage) {
        previewImage.src = skinUrl;
    }
    if (previewSkinId) {
        previewSkinId.innerText = `SKIN #${selectedSkinId}`;
    }
    if (selectedSkinLabel) {
        selectedSkinLabel.innerText = `Đang chọn Skin: #${selectedSkinId}`;
    }
}

// Mở Modal chọn Skin
function openModal() {
    if (skinSearch) skinSearch.value = "";
    renderSkinGrid("");
    
    if (skinModal) {
        skinModal.classList.remove("opacity-0", "pointer-events-none");
        skinModal.classList.add("opacity-100", "pointer-events-auto");
        const box = skinModal.querySelector(".transform");
        if (box) {
            box.classList.remove("scale-95");
            box.classList.add("scale-100");
        }
    }
}

// Đóng Modal chọn Skin
function closeModal() {
    if (skinModal) {
        skinModal.classList.add("opacity-0", "pointer-events-none");
        skinModal.classList.remove("opacity-100", "pointer-events-auto");
        const box = skinModal.querySelector(".transform");
        if (box) {
            box.classList.add("scale-95");
            box.classList.remove("scale-100");
        }
    }
}

// Render lưới skin
function renderSkinGrid(searchTerm = "") {
    if (!skinGrid) return;
    skinGrid.innerHTML = "";

    const skins = SKIN_DATA[selectedGender] || [];
    
    // Lọc theo từ khóa tìm kiếm (nếu có)
    const filteredSkins = skins.filter(id => {
        if (!searchTerm) return true;
        return id.toString().includes(searchTerm);
    });

    if (filteredSkins.length === 0) {
        skinGrid.innerHTML = `<div class="col-span-6 text-center text-xs text-slate-500 py-8">Không tìm thấy skin nào phù hợp</div>`;
        return;
    }

    filteredSkins.forEach(id => {
        const isSelected = (id === selectedSkinId);
        const card = document.createElement("div");
        card.className = `bg-slate-900/50 border rounded-2xl p-2.5 flex flex-col items-center cursor-pointer transition-all duration-200 hover:scale-105 ${
            isSelected 
            ? "border-red-500 bg-red-950/20 shadow-[0_0_15px_rgba(239,68,68,0.2)]" 
            : "border-white/5 hover:border-red-500/40 hover:bg-slate-900/80"
        }`;
        
        card.innerHTML = `
            <img src="http://gtahub.kntech.co/skins/${id}.png" alt="Skin ${id}" class="w-16 h-20 object-contain mb-1.5 drop-shadow-md" loading="lazy">
            <span class="text-[10px] font-mono font-bold ${isSelected ? 'text-red-400' : 'text-slate-400'}">ID: ${id}</span>
        `;

        // Click để chọn skin
        card.addEventListener("click", () => {
            // Loại bỏ highlight ở các thẻ cũ
            const oldSelected = skinGrid.querySelector(".border-red-500");
            if (oldSelected) {
                oldSelected.classList.remove("border-red-500", "bg-red-950/20", "shadow-[0_0_15px_rgba(239,68,68,0.2)]");
                oldSelected.classList.add("border-white/5", "hover:border-red-500/40");
                const span = oldSelected.querySelector("span");
                if (span) span.className = "text-[10px] font-mono font-bold text-slate-400";
            }
            
            // Highlight thẻ mới
            card.classList.remove("border-white/5", "hover:border-red-500/40");
            card.classList.add("border-red-500", "bg-red-950/20", "shadow-[0_0_15px_rgba(239,68,68,0.2)]");
            const span = card.querySelector("span");
            if (span) span.className = "text-[10px] font-mono font-bold text-red-400";

            selectedSkinId = id;
            updatePreview();
        });

        // Double click để chọn skin & đóng modal
        card.addEventListener("dblclick", () => {
            selectedSkinId = id;
            updatePreview();
            closeModal();
        });

        skinGrid.appendChild(card);
    });
}

// Kiểm tra và gửi thông tin nhân vật
function submitCharacter() {
    const name = charNameInput ? charNameInput.value.trim() : "";
    const dobString = charDobInput ? charDobInput.value : "";
    const desc = charDescInput ? charDescInput.value.trim() : "";

    // 1. Kiểm tra Tên nhân vật (Định dạng Roleplay: Firstname_Lastname, chỉ chứa chữ cái tiếng Anh)
    const nameRegex = /^[A-Z][a-zA-Z]+_[A-Z][a-zA-Z]+$/;
    if (!name) {
        showStatus("Vui lòng nhập họ và tên nhân vật!", "error");
        if (charNameInput) charNameInput.focus();
        return;
    }
    if (!nameRegex.test(name)) {
        showStatus("Tên sai định dạng Roleplay! Ví dụ đúng: Michal_Bullbaranek (viết hoa chữ cái đầu và có dấu gạch dưới '_')", "error");
        if (charNameInput) charNameInput.focus();
        return;
    }

    // 2. Kiểm tra Ngày sinh & Tuổi (18 tuổi trở lên)
    if (!dobString) {
        showStatus("Vui lòng nhập ngày tháng năm sinh!", "error");
        if (charDobInput) charDobInput.focus();
        return;
    }
    const dob = new Date(dobString);
    const today = new Date();
    let age = today.getFullYear() - dob.getFullYear();
    const monthDiff = today.getMonth() - dob.getMonth();
    if (monthDiff < 0 || (monthDiff === 0 && today.getDate() < dob.getDate())) {
        age--;
    }
    if (age < 18 || age > 80) {
        showStatus("Tuổi nhân vật phải từ 18 đến 80 tuổi để tham gia thành phố!", "error");
        if (charDobInput) charDobInput.focus();
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

    // Môi trường Test Mode
    if (!window.cef) {
        console.log(`[Test Mode] Gửi tạo nhân vật: Tên=${name}, Ngày sinh=${dobString}, Giới tính=${selectedGender}, Skin=${selectedSkinId}, Mô tả=${desc}`);
        setTimeout(() => {
            if (name === "Michal_Bullbaranek") {
                handleCreateResponse(false, "Tên nhân vật này đã được sử dụng!");
            } else {
                handleCreateResponse(true, "Tạo nhân vật thành công! Đang chuyển vào game...");
            }
        }, 1500);
    }
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
function showStatus(text, type) {
    if (!statusMsg) return;
    statusMsg.innerText = text;
    statusMsg.className = "text-[11px] font-semibold text-center py-2.5 px-3 rounded-xl mb-4 transition-all duration-300 block";

    statusMsg.classList.remove(
        "bg-red-500/10", "border", "border-red-500/20", "text-red-400",
        "bg-emerald-500/10", "border-emerald-500/20", "text-emerald-400",
        "bg-slate-500/10", "border-slate-500/20", "text-slate-400"
    );

    if (type === "error") {
        statusMsg.classList.add("bg-red-500/10", "border", "border-red-500/20", "text-red-400");
    } else if (type === "success") {
        statusMsg.classList.add("bg-emerald-500/10", "border", "border-emerald-500/20", "text-emerald-400");
    } else {
        statusMsg.classList.add("bg-slate-500/10", "border", "border-slate-500/20", "text-slate-400");
    }
}

// Phát tín hiệu CEF
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
    if (!window.cef) {
        console.log("[Test Mode] Đã đóng UI thành công");
    }
}

// Hàm phản hồi từ Server
window.handleCreateResponse = function(success, message) {
    setLoadingState(false);
    if (success) {
        showStatus(message || "Khởi tạo nhân vật thành công!", "success");
        if (createBtn) createBtn.innerText = "Thành Công!";
        setTimeout(() => {
            emitCEF("GTAHUB:ToCharacterSelectionPage");
            window.location.href = "../character_selection/index.html";
        }, 1500);
    } else {
        showStatus(message || "Tạo nhân vật thất bại. Vui lòng kiểm tra lại!", "error");
    }
};

// Lắng nghe phản hồi từ SA:MP CEF
if (window.cef) {
    cef.on("GTAHUB:CreateCharacterResponse", (success, message) => {
        window.handleCreateResponse(success, message);
    });
}
