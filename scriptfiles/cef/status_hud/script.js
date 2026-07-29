// CEF Status HUD Logic
let hudSettings = {
    bgMode: "dark",
    style: "wave",
    opacity: 0.90,
    brightness: 1.00,
    scale: 1.00,
    weaponScale: 1.00,
    ammoScale: 1.00,
    posX: 32,
    posY: 32,
    isEditMode: false
};

let statusValues = {
    health: 100,
    armor: 75,
    hunger: 100,
    thirst: 50,
    stamina: 100,
    cash: 21571,
    coin: 1622,
    id: 99,
    ping: 35,
    online: 420,
    wanted: 0,           // Mặc định wanted = 0 (Ẩn)
    weaponId: 34,
    weaponAmmo: "29/194"
};

const hudContainer = document.getElementById("status-hud-container");
const customizerPanel = document.getElementById("customizer-panel");
const togglePanelBtn = document.getElementById("toggle-panel-btn");
const closePanelBtn = document.getElementById("close-panel-btn");
const dragHandle = document.getElementById("drag-handle");

const hudCash = document.getElementById("hud-cash");
const hudCoin = document.getElementById("hud-coin");
const hudId = document.getElementById("hud-id");
const hudPing = document.getElementById("hud-ping");
const hudOnline = document.getElementById("hud-online");

const hudWantedBadge = document.getElementById("hud-wanted-badge");
const hudWantedVal = document.getElementById("hud-wanted-val");

const hudWeaponIcon = document.getElementById("hud-weapon-icon");
const hudWeaponAmmo = document.getElementById("hud-weapon-ammo");

const sliderOpacity = document.getElementById("slider-opacity");
const sliderScale = document.getElementById("slider-scale");
const sliderWeaponScale = document.getElementById("slider-weapon-scale");
const sliderAmmoScale = document.getElementById("slider-ammo-scale");
const sliderWantedTest = document.getElementById("slider-wanted-test");

const opacityValText = document.getElementById("opacity-val");
const scaleValText = document.getElementById("scale-val");
const weaponScaleValText = document.getElementById("weapon-scale-val");
const ammoScaleValText = document.getElementById("ammo-scale-val");
const wantedTestValText = document.getElementById("wanted-test-val");

const toggleEditBtn = document.getElementById("toggle-edit-mode");
const editModeText = document.getElementById("edit-mode-text");
const resetHudBtn = document.getElementById("reset-hud-btn");
const saveHudBtn = document.getElementById("save-hud-btn");

const styleBtns = document.querySelectorAll(".style-btn");
const bgBtns = document.querySelectorAll(".bg-btn");

init();

function init() {
    loadSavedSettings();
    applySettings();
    updateStatusValues(statusValues);

    if (togglePanelBtn) togglePanelBtn.addEventListener("click", togglePanel);
    if (closePanelBtn) closePanelBtn.addEventListener("click", hidePanel);

    if (sliderOpacity) sliderOpacity.addEventListener("input", (e) => {
        hudSettings.opacity = parseFloat(e.target.value) / 100;
        opacityValText.innerText = `${e.target.value}%`;
        applySettings();
    });

    if (sliderScale) sliderScale.addEventListener("input", (e) => {
        hudSettings.scale = parseFloat(e.target.value) / 100;
        scaleValText.innerText = `${e.target.value}%`;
        applySettings();
    });

    if (sliderWeaponScale) sliderWeaponScale.addEventListener("input", (e) => {
        hudSettings.weaponScale = parseFloat(e.target.value) / 100;
        if (weaponScaleValText) weaponScaleValText.innerText = `${e.target.value}%`;
        applySettings();
    });

    if (sliderAmmoScale) sliderAmmoScale.addEventListener("input", (e) => {
        hudSettings.ammoScale = parseFloat(e.target.value) / 100;
        if (ammoScaleValText) ammoScaleValText.innerText = `${e.target.value}%`;
        applySettings();
    });

    if (sliderWantedTest) sliderWantedTest.addEventListener("input", (e) => {
        const level = parseInt(e.target.value);
        setWantedLevel(level);
        if (wantedTestValText) {
            wantedTestValText.innerText = level > 0 ? `${level} Sao` : "0 Sao (Ẩn)";
        }
    });

    styleBtns.forEach(btn => {
        btn.addEventListener("click", () => {
            styleBtns.forEach(b => b.classList.remove("active", "border-red-500", "text-white"));
            btn.classList.add("active", "border-red-500", "text-white");
            hudSettings.style = btn.dataset.style;
            applySettings();
        });
    });

    bgBtns.forEach(btn => {
        btn.addEventListener("click", () => {
            bgBtns.forEach(b => b.classList.remove("active", "border-red-500", "text-white"));
            btn.classList.add("active", "border-red-500", "text-white");
            hudSettings.bgMode = btn.dataset.bg;
            applySettings();
        });
    });

    if (toggleEditBtn) toggleEditBtn.addEventListener("click", toggleEditMode);
    if (resetHudBtn) resetHudBtn.addEventListener("click", resetSettings);
    if (saveHudBtn) saveHudBtn.addEventListener("click", saveSettings);

    initDragLogic();
}

// HÀM ĐIỀU KHIỂN WANTED LEVEL
function setWantedLevel(level) {
    statusValues.wanted = parseInt(level) || 0;
    if (hudWantedBadge && hudWantedVal) {
        if (statusValues.wanted > 0) {
            hudWantedVal.innerText = statusValues.wanted.toString();
            hudWantedBadge.classList.remove("hidden");
        } else {
            hudWantedBadge.classList.add("hidden");
        }
    }
}

// HÀM ẨN/HIỆN BADGE WANTED TRỰC TIẾP TỪ CEF
function toggleWantedBadge(visible) {
    if (hudWantedBadge) {
        if (visible) {
            hudWantedBadge.classList.remove("hidden");
        } else {
            hudWantedBadge.classList.add("hidden");
        }
    }
}

function updateStatusValues(stats) {
    statusValues = { ...statusValues, ...stats };

    if (hudCash && statusValues.cash !== undefined) hudCash.innerText = `$${parseInt(statusValues.cash).toLocaleString()}`;
    if (hudCoin && statusValues.coin !== undefined) hudCoin.innerText = parseInt(statusValues.coin).toLocaleString();
    if (hudId && statusValues.id !== undefined) hudId.innerText = statusValues.id.toString();
    if (hudPing && statusValues.ping !== undefined) hudPing.innerText = `${statusValues.ping}ms`;
    if (hudOnline && statusValues.online !== undefined) hudOnline.innerText = statusValues.online.toString();

    if (statusValues.wanted !== undefined) {
        setWantedLevel(statusValues.wanted);
    }

    if (hudWeaponIcon && statusValues.weaponId !== undefined) {
        const wid = parseInt(statusValues.weaponId) || 0;
        if (wid === 0) {
            hudWeaponIcon.src = "http://gtahub.kntech.co/weapons/0.png";
            if (hudWeaponAmmo) hudWeaponAmmo.innerText = "-";
        } else {
            hudWeaponIcon.src = `http://gtahub.kntech.co/weapons/${wid}.png`;
            if (hudWeaponAmmo && statusValues.weaponAmmo !== undefined) {
                hudWeaponAmmo.innerText = statusValues.weaponAmmo.toString();
            }
        }
    }

    const keys = ["health", "armor", "hunger", "thirst", "stamina"];
    keys.forEach(key => {
        const val = Math.max(0, Math.min(100, statusValues[key]));

        const fillEl = document.getElementById(`fill-${key}`);
        if (fillEl) fillEl.style.height = `${val}%`;

        const circleEl = document.getElementById(`circle-${key}`);
        if (circleEl) {
            const maxDash = 125.6;
            const offset = maxDash - (maxDash * val / 100);
            circleEl.style.strokeDashoffset = offset;
        }
    });
}

function applySettings() {
    if (!hudContainer) return;

    const boxes = hudContainer.querySelectorAll(".hud-box");
    boxes.forEach(box => {
        box.className = `hud-box style-${hudSettings.style} color-${box.id.split('-')[1]}`;
    });

    const cards = hudContainer.querySelectorAll(".hud-card");
    cards.forEach(card => {
        card.classList.remove("hud-bg-dark", "hud-bg-red", "hud-bg-none");
        if (hudSettings.bgMode === "red") {
            card.classList.add("hud-bg-red");
        } else if (hudSettings.bgMode === "none") {
            card.classList.add("hud-bg-none");
        } else {
            card.classList.add("hud-bg-dark");
        }
    });

    if (hudWeaponIcon) hudWeaponIcon.style.transform = `scale(${hudSettings.weaponScale || 1.0})`;
    if (hudWeaponAmmo) hudWeaponAmmo.style.transform = `scale(${hudSettings.ammoScale || 1.0})`;

    hudContainer.style.opacity = hudSettings.opacity;
    hudContainer.style.transform = `scale(${hudSettings.scale})`;

    if (hudSettings.isEditMode) {
        hudContainer.classList.add("border-2", "border-dashed", "border-red-500", "p-2", "rounded-3xl", "bg-black/40");
        dragHandle.classList.remove("hidden");
    } else {
        hudContainer.classList.remove("border-2", "border-dashed", "border-red-500", "p-2", "rounded-3xl", "bg-black/40");
        dragHandle.classList.add("hidden");
    }
}

function togglePanel() {
    if (customizerPanel.classList.contains("hidden")) {
        customizerPanel.classList.remove("hidden");
    } else {
        hidePanel();
    }
}

function hidePanel() {
    customizerPanel.classList.add("hidden");
    if (hudSettings.isEditMode) toggleEditMode();
}

function toggleEditMode() {
    hudSettings.isEditMode = !hudSettings.isEditMode;
    if (hudSettings.isEditMode) {
        editModeText.innerText = "Tắt Chế Độ Kéo Di Chuyển";
        toggleEditBtn.classList.add("border-red-500", "bg-red-950/40");
    } else {
        editModeText.innerText = "Bật Khóa/Kéo Di Chuyển Vị Trí";
        toggleEditBtn.classList.remove("border-red-500", "bg-red-950/40");
    }
    applySettings();
}

function initDragLogic() {
    let isDragging = false;
    let startX, startY, initialLeft, initialBottom;

    dragHandle.addEventListener("mousedown", (e) => {
        if (!hudSettings.isEditMode) return;
        isDragging = true;
        startX = e.clientX;
        startY = e.clientY;

        const rect = hudContainer.getBoundingClientRect();
        initialLeft = rect.left;
        initialBottom = window.innerHeight - rect.bottom;

        document.addEventListener("mousemove", onMouseMove);
        document.addEventListener("mouseup", onMouseUp);
    });

    function onMouseMove(e) {
        if (!isDragging) return;
        const dx = e.clientX - startX;
        const dy = e.clientY - startY;

        const newLeft = Math.max(0, Math.min(window.innerWidth - 350, initialLeft + dx));
        const newBottom = Math.max(0, Math.min(window.innerHeight - 300, initialBottom - dy));

        hudContainer.style.left = `${newLeft}px`;
        hudContainer.style.bottom = `${newBottom}px`;

        hudSettings.posX = newLeft;
        hudSettings.posY = newBottom;
    }

    function onMouseUp() {
        isDragging = false;
        document.removeEventListener("mousemove", onMouseMove);
        document.removeEventListener("mouseup", onMouseUp);
    }
}

function saveSettings() {
    localStorage.setItem("GTAHUB_HUD_SETTINGS", JSON.stringify(hudSettings));
    emitCEF("GTAHUB:SaveHUDSettings", JSON.stringify(hudSettings));
    hidePanel();
}

function loadSavedSettings() {
    const saved = localStorage.getItem("GTAHUB_HUD_SETTINGS");
    if (saved) {
        try {
            hudSettings = { ...hudSettings, ...JSON.parse(saved) };
            hudSettings.isEditMode = false;

            if (sliderOpacity) sliderOpacity.value = hudSettings.opacity * 100;
            if (sliderScale) sliderScale.value = hudSettings.scale * 100;
            if (sliderWeaponScale) sliderWeaponScale.value = (hudSettings.weaponScale || 1.0) * 100;
            if (sliderAmmoScale) sliderAmmoScale.value = (hudSettings.ammoScale || 1.0) * 100;

            if (opacityValText) opacityValText.innerText = `${Math.round(hudSettings.opacity * 100)}%`;
            if (scaleValText) scaleValText.innerText = `${Math.round(hudSettings.scale * 100)}%`;
            if (weaponScaleValText) weaponScaleValText.innerText = `${Math.round((hudSettings.weaponScale || 1.0) * 100)}%`;
            if (ammoScaleValText) ammoScaleValText.innerText = `${Math.round((hudSettings.ammoScale || 1.0) * 100)}%`;

            if (hudSettings.posX) hudContainer.style.left = `${hudSettings.posX}px`;
            if (hudSettings.posY) hudContainer.style.bottom = `${hudSettings.posY}px`;

            bgBtns.forEach(btn => {
                if (btn.dataset.bg === hudSettings.bgMode) {
                    btn.classList.add("active", "border-red-500", "text-white");
                } else {
                    btn.classList.remove("active", "border-red-500", "text-white");
                }
            });

        } catch (e) {
            console.error("Lỗi parse cấu hình HUD", e);
        }
    }
}

function resetSettings() {
    hudSettings = {
        bgMode: "dark",
        style: "wave",
        opacity: 0.90,
        brightness: 1.00,
        scale: 1.00,
        weaponScale: 1.00,
        ammoScale: 1.00,
        posX: 32,
        posY: 32,
        isEditMode: false
    };
    localStorage.removeItem("GTAHUB_HUD_SETTINGS");
    applySettings();
    hudContainer.style.left = "32px";
    hudContainer.style.bottom = "32px";
}

// Global Export Các Hàm Gọi Từ CEF Server
window.setWantedLevel = setWantedLevel;
window.toggleWantedBadge = toggleWantedBadge;
window.updateStatusValues = updateStatusValues;

// LẮNG NGHE SỰ KIỆN TỪ CEF (SA:MP / RAGEMP)
if (window.cef) {
    cef.on("GTAHUB:UpdateStatus", (data) => {
        if (typeof data === "object") {
            updateStatusValues(data);
        } else if (typeof data === "string") {
            try { updateStatusValues(JSON.parse(data)); } catch(e){}
        }
    });

    cef.on("GTAHUB:SetWanted", (level) => {
        setWantedLevel(level);
    });

    cef.on("GTAHUB:ToggleWanted", (visible) => {
        toggleWantedBadge(visible);
    });

    cef.on("GTAHUB:SetWeapon", (weaponId, weaponAmmo) => {
        updateStatusValues({ weaponId, weaponAmmo });
    });
}

function emitCEF(eventName, ...args) {
    if (window.cef) {
        cef.emit(eventName, ...args);
    } else if (window.mp) {
        mp.trigger(eventName, ...args);
    } else {
        console.log(`[Test Mode] Emit Event: ${eventName}`, args);
    }
}

// Mock Test Offline trên Trình Duyệt
if (!window.cef && !window.mp) {
    setInterval(() => {
        const mockHealth = Math.floor(Math.random() * 40) + 60;
        const mockArmor = Math.floor(Math.random() * 100);
        const mockHunger = Math.floor(Math.random() * 50) + 50;
        const mockThirst = Math.floor(Math.random() * 50) + 50;
        const mockStamina = Math.floor(Math.random() * 30) + 70;
        const mockCash = Math.floor(Math.random() * 50000);
        const mockCoin = Math.floor(Math.random() * 2000);
        const mockWeaponId = [0, 24, 29, 30, 31, 34, 35][Math.floor(Math.random() * 7)];
        const mockAmmo = mockWeaponId === 0 ? "-" : `${Math.floor(Math.random() * 30)}/${Math.floor(Math.random() * 300)}`;

        updateStatusValues({
            health: mockHealth,
            armor: mockArmor,
            hunger: mockHunger,
            thirst: mockThirst,
            stamina: mockStamina,
            cash: mockCash,
            coin: mockCoin,
            weaponId: mockWeaponId,
            weaponAmmo: mockAmmo
        });
    }, 4000);
}
