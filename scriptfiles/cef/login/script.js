// CEF Login System cho GTAHUB

// --- DOM Elements ---
const usernameInput = document.getElementById("username");
const passwordInput = document.getElementById("password");
const rememberMeInput = document.getElementById("remember-me");
const loginBtn = document.getElementById("login-btn");
const forgotPasswordLink = document.getElementById("forgot-password");
const toRegisterBtn = document.getElementById("to-register-btn");
const panelTitle = document.getElementById("panel-title");
const statusMsg = document.getElementById("status-msg");

// --- Khoi chay ---
init();

function init() {
    emitCEF("GTAHUB:LoginReady");

    // Quen mat khau
    if (forgotPasswordLink) {
        forgotPasswordLink.addEventListener("click", (e) => {
            e.preventDefault();
            emitCEF("GTAHUB:ForgotPassword");
        });
    }

    // Chuyen sang dang ky
    if (toRegisterBtn) {
        toRegisterBtn.addEventListener("click", (e) => {
            emitCEF("GTAHUB:ToRegisterPage");
        });
    }

    // Bind su kien nut bam chinh
    if (loginBtn) loginBtn.addEventListener("click", submitLogin);

    // Enter Key Navigation
    bindEnterKey(usernameInput, passwordInput);
    bindEnterKey(passwordInput, null, submitLogin);
}

// Ham ho tro phim Enter de chuyen doi o nhap lieu
function bindEnterKey(currentInput, nextInput, submitCallback = null) {
    if (!currentInput) return;
    currentInput.addEventListener("keydown", (e) => {
        if (e.key === "Enter") {
            if (nextInput) {
                nextInput.focus();
            } else if (submitCallback) {
                submitCallback();
            }
        }
    });
}

// ================= HANH DONG: DANG NHAP =================
function submitLogin() {
    const username = usernameInput ? usernameInput.value.trim() : "";
    const password = passwordInput ? passwordInput.value : "";
    const rememberMe = rememberMeInput ? rememberMeInput.checked : false;

    if (!username) {
        showStatus("Vui long nhap ten tai khoan hoac email!", "error");
        if (usernameInput) usernameInput.focus();
        return;
    }
    if (!password) {
        showStatus("Vui long nhap mat khau!", "error");
        if (passwordInput) passwordInput.focus();
        return;
    }

    showStatus("Dang xac thuc tai khoan dang nhap...", "info");
    setLoadingState(loginBtn, true, "Dang Nhap Ngay");

    emitCEF("GTAHUB:SubmitLogin", username, password, rememberMe);

    // Fallback Offline Test
    if (!window.cef) {
        setTimeout(() => {
            if (username === "admin" && password === "123456") {
                handleLoginResponse(true, "Dang nhap thanh cong! Dang tai ban do...");
            } else {
                handleLoginResponse(false, "Sai tai khoan hoac mat khau (Thu: admin/123456)");
            }
        }, 1200);
    }
}

// --- Phan hoi tu Server ---
window.handleLoginResponse = function(success, message) {
    setLoadingState(loginBtn, false, "Dang Nhap Ngay");
    if (success) {
        showStatus(message || "Dang nhap thanh cong! Dang chuyen trang...", "success");
        if (loginBtn) loginBtn.innerText = "Thanh Cong!";
    } else {
        showStatus(message || "Dang nhap that bai!", "error");
    }
};

// Dang ky cac trinh lang nghe su kien tu SA:MP CEF
if (window.cef) {
    cef.on("GTAHUB:LoginResponse", (success, message) => {
        window.handleLoginResponse(success, message);
    });
    cef.on("GTAHUB:SetUsername", (name) => {
        if (usernameInput) {
            usernameInput.value = name;
            usernameInput.readOnly = true;
        }
        if (passwordInput) {
            setTimeout(() => {
                passwordInput.focus();
            }, 100);
        }
    });
    cef.on("GTAHUB:ToLoginPage", () => {
        window.location.href = "index.html";
    });
}

// --- Ham Tien Ich ---
function emitCEF(eventName, ...args) {
    if (window.cef) {
        cef.emit(eventName, ...args);
    } else {
        console.log(`[Test Mode] Phat su kien: ${eventName}`, args);
    }
}

function closeCEF() {
    emitCEF("GTAHUB:CloseUI");
    if (!window.cef) {
        console.log("[Test Mode] Da dong UI thanh cong");
    }
}

function showStatus(text, type) {
    if (!statusMsg) return;
    statusMsg.innerText = text;
    statusMsg.className = "status-msg " + (type || "info");
}

function hideStatus() {
    if (statusMsg) {
        statusMsg.classList.add("hidden");
    }
}

function setLoadingState(btn, isLoading, defaultText) {
    if (!btn) return;
    btn.disabled = isLoading;
    btn.innerText = isLoading ? "Dang Xu Ly..." : defaultText;
}
