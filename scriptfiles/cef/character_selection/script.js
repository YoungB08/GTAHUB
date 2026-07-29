// CEF Character Selection Logic cho GTAHUB

// Dữ liệu nhân vật thực tế (Tải động từ Server Pawn qua CEF)
let characters = [];

let selectedIndex = 0;

// Các phần tử DOM
const charListContainer = document.getElementById("char-list");
const previewName = document.getElementById("preview-name");
const previewFaction = document.getElementById("preview-faction");
const previewImage = document.getElementById("preview-image");
const statLevel = document.getElementById("stat-level");
const statCash = document.getElementById("stat-cash");
const statBank = document.getElementById("stat-bank");
const statPhone = document.getElementById("stat-phone");
const statVehicles = document.getElementById("stat-vehicles");
const previewSkinId = document.getElementById("preview-skin-id");
const enterCityBtn = document.getElementById("enter-city-btn");
const popupContainer = document.getElementById("popup-container");

// Khởi chạy
init();

function init() {
    renderCharList();
    updatePreview();

    if (enterCityBtn) {
        enterCityBtn.addEventListener("click", enterCity);
    }

    // Đóng UI khi bấm Escape
    document.addEventListener("keydown", (e) => {
        if (e.key === "Escape") {
            // Nếu có popup đang hiện thì đóng popup trước
            const popup = document.querySelector(".popup-overlay");
            if (popup) {
                popup.remove();
            } else {
                closeCEF();
            }
        }
    });
}

// Render danh sách nhân vật
function renderCharList() {
    if (!charListContainer) return;
    charListContainer.innerHTML = "";

    characters.forEach((char, index) => {
        const isSelected = (index === selectedIndex);
        const card = document.createElement("div");
        card.className = `p-4 border rounded-2xl cursor-pointer relative transition-all duration-200 hover:scale-[1.02] flex justify-between items-center ${
            isSelected 
            ? "bg-gradient-to-r from-red-950/60 to-slate-900/80 border-red-500 shadow-[0_0_15px_rgba(239,68,68,0.2)]" 
            : "bg-slate-900/40 border-white/5 hover:border-white/20"
        }`;

        card.innerHTML = `
            <div class="flex-1">
                ${isSelected ? '<span class="absolute top-2 right-3 text-[8px] font-bold px-1.5 py-0.5 rounded bg-red-600 text-white">ĐANG CHỌN</span>' : ''}
                <h3 class="text-sm font-black text-white">${char.name}</h3>
                <p class="text-[10px] ${isSelected ? 'text-red-400' : 'text-slate-400'} font-medium mt-0.5">Cấp độ: ${char.level} • Tiền: $${char.cash.toLocaleString()}</p>
                <div class="mt-2 text-[9px] text-slate-500 flex gap-3">
                    <span>SĐT: ${char.phone}</span>
                </div>
            </div>
            
            <!-- Nút xóa nhân vật -->
            <button class="delete-btn ml-3 w-7 h-7 rounded-xl bg-red-600/10 hover:bg-red-600 border border-red-500/30 hover:border-red-500 text-red-400 hover:text-white flex items-center justify-center transition-all z-20" title="Xóa nhân vật">
                <svg class="w-3.5 h-3.5" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M19 7l-.867 12.142A2 2 0 0116.138 21H7.862a2 2 0 01-1.995-1.858L5 7m5 4v6m4-6v6m1-10V4a1 1 0 00-1-1h-4a1 1 0 00-1 1v3M4 7h16"/></svg>
            </button>
        `;

        // Click để chọn nhân vật
        card.addEventListener("click", (e) => {
            // Nếu click trúng nút xóa thì không đổi chọn nhân vật
            if (e.target.closest(".delete-btn")) return;
            
            selectedIndex = index;
            renderCharList();
            updatePreview();
        });

        // Double click để vào thành phố
        card.addEventListener("dblclick", (e) => {
            if (e.target.closest(".delete-btn")) return;
            enterCity();
        });

        // Sự kiện xóa nhân vật
        const deleteBtn = card.querySelector(".delete-btn");
        if (deleteBtn) {
            deleteBtn.addEventListener("click", () => {
                triggerDeleteCharacter(char.name, index);
            });
        }

        charListContainer.appendChild(card);
    });

    // Ô trống tạo nhân vật mới
    const createCard = document.createElement("div");
    createCard.className = "p-4 bg-slate-950/30 border border-dashed border-white/10 hover:border-red-500/50 rounded-2xl flex items-center justify-center gap-2 text-slate-400 hover:text-red-400 transition-all cursor-pointer";
    createCard.innerHTML = `
        <svg class="w-4 h-4" fill="none" stroke="currentColor" viewBox="0 0 24 24"><path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 6v6m0 0v6m0-6h6m-6 0H6"/></svg>
        <span class="text-xs font-bold">Tạo Nhân Vật Mới</span>
    `;
    createCard.addEventListener("click", () => {
        emitCEF("GTAHUB:OpenCreateCharacter");
        window.location.href = "../character_creation/index.html";
    });
    charListContainer.appendChild(createCard);
}

// Cập nhật khung xem trước chi tiết
function updatePreview() {
    const char = characters[selectedIndex];
    if (!char) {
        // Nếu không có nhân vật nào
        if (previewName) previewName.innerText = "Chưa chọn nhân vật";
        if (previewFaction) previewFaction.innerText = "-";
        if (previewImage) previewImage.src = "http://gtahub.kntech.co/skins/0.png";
        if (statLevel) statLevel.innerText = "-";
        if (statCash) statCash.innerText = "-";
        if (statBank) statBank.innerText = "-";
        if (statPhone) statPhone.innerText = "-";
        if (statVehicles) statVehicles.innerHTML = "";
        if (previewSkinId) previewSkinId.innerText = "SKIN #0";
        return;
    }

    if (previewName) previewName.innerText = char.name;
    if (previewFaction) previewFaction.innerText = `Tổ chức: ${char.faction}`;
    if (previewImage) previewImage.src = `http://gtahub.kntech.co/skins/${char.skin}.png`;
    if (statLevel) statLevel.innerText = char.level;
    if (statCash) statCash.innerText = `$${char.cash.toLocaleString()}`;
    if (statBank) statBank.innerText = `$${char.bank.toLocaleString()}`;
    if (statPhone) statPhone.innerText = char.phone;
    
    if (statVehicles) {
        statVehicles.innerHTML = "";
        if (char.vehicles.length === 0) {
            statVehicles.innerHTML = `<span class="text-slate-500 italic block">Không có phương tiện</span>`;
        } else {
            char.vehicles.forEach(vehicle => {
                const item = document.createElement("div");
                item.className = "flex items-center gap-1.5 py-0.5 text-slate-300 font-semibold";
                item.innerHTML = `🚗 <span>${vehicle}</span>`;
                statVehicles.appendChild(item);
            });
        }
    }

    if (previewSkinId) previewSkinId.innerText = `SKIN #${char.skin}`;
}

// Bắt đầu vào thành phố
function enterCity() {
    const char = characters[selectedIndex];
    if (char) {
        emitCEF("GTAHUB:SelectCharacter", char.name);
    }
}

// Gọi Popup xác nhận xóa nhân vật
function triggerDeleteCharacter(charName, index) {
    showPopup({
        title: "Xác nhận xóa nhân vật",
        message: `Hành động này không thể hoàn tác! Vui lòng nhập tên nhân vật <strong>${charName}</strong> dưới đây để xác nhận xóa:`,
        inputPlaceholder: charName,
        inputType: "text",
        confirmText: "Xóa Ngay",
        cancelText: "Hủy Bỏ",
        onConfirm: (inputValue) => {
            if (inputValue.trim() === charName) {
                // Thực hiện xóa
                emitCEF("GTAHUB:DeleteCharacter", charName);
                characters.splice(index, 1);
                
                // Cập nhật lại chỉ số đang chọn
                if (selectedIndex >= characters.length) {
                    selectedIndex = Math.max(0, characters.length - 1);
                }
                
                renderCharList();
                updatePreview();
                
                showPopup({
                    title: "Thành công",
                    message: `Đã xóa nhân vật ${charName} thành công.`,
                    confirmText: "Đóng",
                    cancelText: ""
                });
            } else {
                showPopup({
                    title: "Lỗi xác nhận",
                    message: "Tên xác nhận nhập vào không khớp! Hủy bỏ xóa.",
                    confirmText: "Thử lại",
                    cancelText: ""
                });
            }
        }
    });
}

// ================= POPUP SYSTEM (Hệ thống hộp thoại động) =================
function showPopup({ title, message, inputPlaceholder = "", inputType = "text", confirmText = "Đồng ý", cancelText = "Hủy", onConfirm = null }) {
    if (!popupContainer) return;

    const overlay = document.createElement("div");
    overlay.className = "popup-overlay fixed inset-0 bg-black/80 backdrop-blur-sm flex items-center justify-center z-50 transition-opacity duration-300";
    
    const showInput = (onConfirm !== null && inputPlaceholder !== "");

    overlay.innerHTML = `
        <div class="popup-box glass-panel w-[400px] rounded-3xl p-6 border border-white/10 shadow-[0_0_50px_rgba(0,0,0,0.8)] scale-95 transition-transform duration-300">
            <h4 class="text-sm font-black text-white uppercase tracking-wider mb-2 text-center border-b border-white/5 pb-2">${title}</h4>
            <p class="text-xs text-slate-300 leading-relaxed mb-4 text-center">${message}</p>
            
            ${showInput ? `
                <input type="${inputType}" id="popup-input" placeholder="${inputPlaceholder}" class="w-full bg-slate-900 border border-white/10 rounded-xl px-4 py-2.5 text-xs text-white placeholder-slate-600 focus:outline-none focus:border-red-500 mb-4 transition-colors">
            ` : ''}
            
            <div class="flex gap-2">
                ${cancelText ? `<button id="popup-cancel" class="w-1/2 py-2 bg-slate-800 hover:bg-slate-700 text-slate-300 font-bold text-xs rounded-xl transition-all uppercase tracking-wider">${cancelText}</button>` : ''}
                <button id="popup-confirm" class="${cancelText ? 'w-1/2' : 'w-full'} py-2 bg-gradient-to-r from-red-600 to-rose-700 hover:from-red-500 hover:to-rose-600 text-white font-bold text-xs rounded-xl shadow-[0_0_15px_rgba(239,68,68,0.2)] transition-all uppercase tracking-wider">${confirmText}</button>
            </div>
        </div>
    `;

    popupContainer.appendChild(overlay);

    // Hiệu ứng mở popup
    setTimeout(() => {
        overlay.classList.add("opacity-100");
        const box = overlay.querySelector(".popup-box");
        if (box) box.classList.remove("scale-95");
    }, 10);

    const closePopup = () => {
        overlay.remove();
    };

    const confirmBtn = overlay.querySelector("#popup-confirm");
    const cancelBtn = overlay.querySelector("#popup-cancel");
    const inputElement = overlay.querySelector("#popup-input");

    if (confirmBtn) {
        confirmBtn.addEventListener("click", () => {
            if (onConfirm) {
                const val = inputElement ? inputElement.value : "";
                onConfirm(val);
            }
            closePopup();
        });
    }

    if (cancelBtn) {
        cancelBtn.addEventListener("click", closePopup);
    }

    if (inputElement) {
        inputElement.focus();
        inputElement.addEventListener("keydown", (e) => {
            if (e.key === "Enter") {
                confirmBtn.click();
            }
        });
    }
}

// Phát sự kiện CEF
function emitCEF(eventName, ...args) {
    if (window.cef) {
        cef.emit(eventName, ...args);
    } else {
        console.log(`[Test Mode] Phát sự kiện: ${eventName}`, args);
    }
}

// Đăng ký nhận danh sách nhân vật thực tế từ Server Pawn
if (window.cef) {
    cef.on("GTAHUB:CharacterList", (data) => {
        try {
            const list = (typeof data === "string") ? JSON.parse(data) : data;
            if (Array.isArray(list)) {
                characters = list;
                selectedIndex = 0;
                renderCharList();
                updatePreview();
            }
        } catch (e) {
            console.error("Lỗi parse CharacterList:", e);
        }
    });
}

// Đóng UI
function closeCEF() {
    emitCEF("GTAHUB:CloseUI");
    if (!window.cef) {
        console.log("[Test Mode] Đã đóng UI thành công");
    }
}
