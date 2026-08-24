// CEF Register System cho GTAHUB

// --- DOM Elements ---
const regUsernameInput = document.getElementById("register-username");
const regEmailInput = document.getElementById("register-email");
const regPasswordInput = document.getElementById("register-password");
const regConfirmPasswordInput = document.getElementById("register-confirm-password");
const registerBtn = document.getElementById("register-btn");
const backToLoginBtn = document.getElementById("back-to-login-btn");
const statusMsg = document.getElementById("status-msg");

// --- Khởi chạy ---
init();

function init() {
    emitCEF("GTAHUB:RegisterReady");

    // Đăng ký phím tắt Escape đóng UI
    document.addEventListener("keydown", (e) => {
        if (e.key === "Escape") {
            closeCEF();
        }
    });

    // Quay lại đăng nhập
    if (backToLoginBtn) {
        backToLoginBtn.addEventListener("click", (e) => {
            emitCEF("GTAHUB:ToLoginPage");
        });
    }

    // Bind sự kiện nút bấm chính
    if (registerBtn) registerBtn.addEventListener("click", submitRegister);

    // Enter Key Navigation
    bindEnterKey(regUsernameInput, regEmailInput);
    bindEnterKey(regEmailInput, regPasswordInput);
    bindEnterKey(regPasswordInput, regConfirmPasswordInput);
    bindEnterKey(regConfirmPasswordInput, null, submitRegister);
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

// ================= HÀNH ĐỘNG: ĐĂNG KÝ =================
function submitRegister() {
    const username = regUsernameInput ? regUsernameInput.value.trim() : "";
    const email = regEmailInput ? regEmailInput.value.trim() : "";
    const password = regPasswordInput ? regPasswordInput.value : "";
    const confirmPassword = regConfirmPasswordInput ? regConfirmPasswordInput.value : "";

    // 1. Kiểm tra rỗng
    if (!username) {
        showStatus("Tên tài khoản đăng ký không được để trống!", "error");
        if (regUsernameInput) regUsernameInput.focus();
        return;
    }
    if (!email) {
        showStatus("Vui lòng cung cấp địa chỉ email!", "error");
        if (regEmailInput) regEmailInput.focus();
        return;
    }
    if (!password) {
        showStatus("Vui lòng nhập mật khẩu đăng ký!", "error");
        if (regPasswordInput) regPasswordInput.focus();
        return;
    }
    if (!confirmPassword) {
        showStatus("Vui lòng nhập lại mật khẩu xác nhận!", "error");
        if (regConfirmPasswordInput) regConfirmPasswordInput.focus();
        return;
    }

    // 2. Validate email (Định dạng Gmail nghiêm ngặt)
    const emailRegex = /^[a-zA-Z0-9._%+-]+@gmail\.com$/;
    if (!emailRegex.test(email)) {
        showStatus("Hệ thống chỉ chấp nhận địa chỉ Gmail hợp lệ (VD: user@gmail.com)!", "error");
        if (regEmailInput) regEmailInput.focus();
        return;
    }

    // 3. Độ dài mật khẩu
    if (password.length < 6) {
        showStatus("Mật khẩu tài khoản phải chứa ít nhất 6 ký tự!", "error");
        if (regPasswordInput) regPasswordInput.focus();
        return;
    }

    // 4. Khớp mật khẩu
    if (password !== confirmPassword) {
        showStatus("Mật khẩu xác nhận nhập lại không khớp!", "error");
        if (regConfirmPasswordInput) regConfirmPasswordInput.focus();
        return;
    }

    showStatus("Đang đăng ký tài khoản mới lên hệ thống...", "info");
    setLoadingState(registerBtn, true, "Hoàn Tất Đăng Ký");

    emitCEF("GTAHUB:SubmitRegister", username, email, password, confirmPassword);

    // Fallback Offline Test
    if (!window.cef) {
        setTimeout(() => {
            if (username === "admin") {
                handleRegisterResponse(false, "Tên tài khoản này đã tồn tại trên hệ thống!");
            } else {
                handleRegisterResponse(true, "Đăng ký thành công! Mã OTP đã được gửi về Gmail của bạn.");
            }
        }, 1200);
    }
}

// --- Phản hồi từ Server ---
window.handleRegisterResponse = function(success, message) {
    setLoadingState(registerBtn, false, "Hoàn Tất Đăng Ký");
    if (success) {
        showStatus(message || "Đăng ký thành công! Đang chuyển tiếp...", "success");
    } else {
        showStatus(message || "Đăng ký thất bại. Tên tài khoản hoặc email trùng lặp!", "error");
    }
};

// Đăng ký các trình lắng nghe sự kiện từ SA:MP CEF
if (window.cef) {
    cef.on("GTAHUB:RegisterResponse", (success, message) => {
        window.handleRegisterResponse(success, message);
    });
    cef.on("GTAHUB:SetUsername", (name) => {
        if (regUsernameInput) {
            regUsernameInput.value = name;
            regUsernameInput.readOnly = true;
        }
        if (regEmailInput) {
            setTimeout(() => {
                regEmailInput.focus();
            }, 100);
        }
    });
    cef.on("GTAHUB:ToRegisterPage", () => {
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
