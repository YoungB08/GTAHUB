// CEF Activation System cho GTAHUB

// --- DOM Elements ---
const verifyCodeInput = document.getElementById("verify-code");
const verifyBtn = document.getElementById("verify-btn");
const resendCodeBtn = document.getElementById("resend-code-btn");
const cancelVerifyBtn = document.getElementById("cancel-verify-btn");
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

    // Bind sự kiện nút bấm chính
    if (verifyBtn) verifyBtn.addEventListener("click", submitVerify);
    if (resendCodeBtn) resendCodeBtn.addEventListener("click", resendCode);
    if (cancelVerifyBtn) {
        cancelVerifyBtn.addEventListener("click", () => {
            emitCEF("GTAHUB:ToLoginPage");
        });
    }

    // Enter Key Navigation
    bindEnterKey(verifyCodeInput, null, submitVerify);
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

// ================= HÀNH ĐỘNG: XÁC THỰC EMAIL (OTP) =================
function submitVerify() {
    const code = verifyCodeInput ? verifyCodeInput.value.trim() : "";

    if (!code) {
        showStatus("Vui lòng điền mã OTP xác thực!", "error");
        if (verifyCodeInput) verifyCodeInput.focus();
        return;
    }

    if (code.length !== 6 || isNaN(code)) {
        showStatus("Mã OTP phải là một dãy số gồm đúng 6 chữ số!", "error");
        if (verifyCodeInput) verifyCodeInput.focus();
        return;
    }

    showStatus("Đang gửi mã OTP xác thực email lên server...", "info");
    setLoadingState(verifyBtn, true, "Xác Thực Tài Khoản");

    emitCEF("GTAHUB:VerifyGmail", code);

    // Fallback Offline Test
    if (!window.cef) {
        setTimeout(() => {
            if (code === "123456") {
                handleVerificationResponse(true, "Xác thực thành công! Tài khoản của bạn đã được kích hoạt.");
            } else {
                handleVerificationResponse(false, "Mã xác thực OTP không chính xác hoặc đã hết hạn! (OTP test: 123456)");
            }
        }, 1200);
    }
}

// Gửi lại mã OTP
function resendCode() {
    showStatus("Đang yêu cầu gửi lại mã OTP vào Gmail...", "info");
    emitCEF("GTAHUB:ResendVerificationCode");
    
    setTimeout(() => {
        showStatus("Mã OTP mới đã được gửi lại thành công vào Gmail của bạn!", "success");
    }, 1000);
}

// --- Phản hồi từ Server ---
window.handleVerificationResponse = function(success, message) {
    setLoadingState(verifyBtn, false, "Xác Thực Tài Khoản");
    if (success) {
        showStatus(message || "Kích hoạt tài khoản thành công! Đang chuyển tiếp...", "success");
        if (verifyBtn) verifyBtn.innerText = "Kích Hoạt Thành Công!";
        setTimeout(() => {
            emitCEF("GTAHUB:ToLoginPage");
        }, 1500);
    } else {
        showStatus(message || "Mã OTP không hợp lệ hoặc đã hết hiệu lực!", "error");
    }
};

// Đăng ký các trình lắng nghe sự kiện từ SA:MP CEF
if (window.cef) {
    cef.on("GTAHUB:VerificationResponse", (success, message) => {
        window.handleVerificationResponse(success, message);
    });
    cef.on("GTAHUB:ToActivePage", () => {
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
