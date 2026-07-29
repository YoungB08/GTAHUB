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

// --- Khởi chạy ---
init();

function init() {
    // Đăng ký phím tắt Escape đóng UI
    document.addEventListener("keydown", (e) => {
        if (e.key === "Escape") {
            closeCEF();
        }
    });

    // Quên mật khẩu
    if (forgotPasswordLink) {
        forgotPasswordLink.addEventListener("click", (e) => {
            e.preventDefault();
            emitCEF("GTAHUB:ForgotPassword");
        });
    }

    // Chuyển sang đăng ký
    if (toRegisterBtn) {
        toRegisterBtn.addEventListener("click", (e) => {
            emitCEF("GTAHUB:ToRegisterPage");
        });
    }

    // Bind sự kiện nút bấm chính
    if (loginBtn) loginBtn.addEventListener("click", submitLogin);

    // Enter Key Navigation
    bindEnterKey(usernameInput, passwordInput);
    bindEnterKey(passwordInput, null, submitLogin);
}

// Hàm hỗ trợ phím Enter để chuyển đổi ô nhập liệu
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

// ================= HÀNH ĐỘNG: ĐĂNG NHẬP =================
function submitLogin() {
    const username = usernameInput ? usernameInput.value.trim() : "";
    const password = passwordInput ? passwordInput.value : "";
    const rememberMe = rememberMeInput ? rememberMeInput.checked : false;

    if (!username) {
        showStatus("Vui lòng nhập tên tài khoản hoặc email!", "error");
        if (usernameInput) usernameInput.focus();
        return;
    }
    if (!password) {
        showStatus("Vui lòng nhập mật khẩu!", "error");
        if (passwordInput) passwordInput.focus();
        return;
    }

    showStatus("Đang xác thực tài khoản đăng nhập...", "info");
    setLoadingState(loginBtn, true, "Đăng Nhập Ngay");

    emitCEF("GTAHUB:SubmitLogin", username, password, rememberMe);

    // Fallback Offline Test
    if (!window.cef) {
        setTimeout(() => {
            if (username === "admin" && password === "123456") {
                handleLoginResponse(true, "Đăng nhập thành công! Đang tải bản đồ...");
            } else {
                handleLoginResponse(false, "Sai tài khoản hoặc mật khẩu (Thử: admin/123456)");
            }
        }, 1200);
    }
}

// --- Phản hồi từ Server ---
window.handleLoginResponse = function(success, message) {
    setLoadingState(loginBtn, false, "Đăng Nhập Ngay");
    if (success) {
        showStatus(message || "Đăng nhập thành công!", "success");
        if (loginBtn) loginBtn.innerText = "Thành Công!";
        setTimeout(() => {
            emitCEF("GTAHUB:ToCharacterSelectionPage");
            window.location.href = "../character_selection/index.html";
        }, 1500);
    } else {
        showStatus(message || "Đăng nhập thất bại!", "error");
    }
};

// Đăng ký các trình lắng nghe sự kiện từ SA:MP CEF
if (window.cef) {
    cef.on("GTAHUB:LoginResponse", (success, message) => {
        window.handleLoginResponse(success, message);
    });
    cef.on("GTAHUB:ToLoginPage", () => {
        // Hỗ trợ backend bắt chuyển hướng từ client
        window.location.href = "index.html";
    });
}

// --- Hàm Tiện Ích ---
function emitCEF(eventName, ...args) {
    if (window.cef) {
        cef.emit(eventName, ...args);
    } else {
        console.log(`[Test Mode] Phát sự kiện: ${eventName}`, args);
    }
}

function closeCEF() {
    emitCEF("GTAHUB:CloseUI");
    if (!window.cef) {
        console.log("[Test Mode] Đã đóng UI thành công");
    }
}

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

function hideStatus() {
    if (statusMsg) {
        statusMsg.classList.add("hidden");
    }
}

function setLoadingState(btn, isLoading, defaultText) {
    if (!btn) return;
    if (isLoading) {
        btn.disabled = true;
        btn.innerText = "Đang Xử Lý...";
        btn.classList.add("opacity-50", "cursor-not-allowed");
    } else {
        btn.disabled = false;
        btn.innerText = defaultText;
        btn.classList.remove("opacity-50", "cursor-not-allowed");
    }
}
