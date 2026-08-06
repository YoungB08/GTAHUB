"use strict";

const MAX_MESSAGES = 10;
const MAX_HISTORY = 50;
const chatFeed = document.getElementById("chat-feed");
const chatForm = document.getElementById("chat-form");
const chatInput = document.getElementById("chat-input");
const chatCount = document.getElementById("chat-count");
const inputHistory = [];
let historyIndex = 0;
let historyDraft = "";
let renderConfirmed = false;

function rgbaToCss(value) {
    const color = Number(value) >>> 0;
    const red = (color >>> 24) & 255;
    const green = (color >>> 16) & 255;
    const blue = (color >>> 8) & 255;
    const alpha = (color & 255) / 255;
    return `rgba(${red}, ${green}, ${blue}, ${alpha.toFixed(3)})`;
}

function appendColorText(container, message, baseColor) {
    const expression = /\{([0-9a-fA-F]{6})\}/g;
    let cursor = 0;
    let match;
    let activeColor = baseColor;

    while ((match = expression.exec(message)) !== null) {
        if (match.index > cursor) {
            const part = document.createElement("span");
            part.textContent = message.slice(cursor, match.index);
            part.style.color = activeColor;
            container.appendChild(part);
        }
        activeColor = `#${match[1]}`;
        cursor = expression.lastIndex;
    }

    if (cursor < message.length) {
        const part = document.createElement("span");
        part.textContent = message.slice(cursor);
        part.style.color = activeColor;
        container.appendChild(part);
    }
}

function addMessage(color, rawMessage) {
    const message = String(rawMessage || "")
        .replace(/~[a-zA-Z]~/g, "")
        .replace(/\s+/g, " ")
        .trim();
    if (!message) return;

    const colorCss = rgbaToCss(color);
    const row = document.createElement("div");
    row.className = "chat-message";
    row.style.setProperty("--message-color", colorCss);

    const time = document.createElement("time");
    time.className = "chat-time";
    time.textContent = new Date().toLocaleTimeString("vi-VN", {
        hour: "2-digit",
        minute: "2-digit",
        hour12: false
    });

    const text = document.createElement("div");
    text.className = "chat-text";
    appendColorText(text, message, colorCss);

    row.append(time, text);
    chatFeed.appendChild(row);

    while (chatFeed.children.length > MAX_MESSAGES) {
        chatFeed.firstElementChild.remove();
    }

    chatFeed.scrollTop = chatFeed.scrollHeight;

    Array.from(chatFeed.children).forEach((item, index, items) => {
        item.classList.toggle("is-old", index < items.length - 6);
    });

    if (!renderConfirmed) {
        renderConfirmed = true;
        cef.emit("GTAHUB:ChatRendered");
    }
}

function safeDecode(encoded) {
    try {
        return decodeURIComponent(encoded);
    } catch {
        return encoded.replace(/%([0-9A-F]{2})/gi, (match, hex) => {
            const code = parseInt(hex, 16);
            return String.fromCharCode(code);
        });
    }
}

function addMessagePayload(rawPayload) {
    const payload = String(rawPayload || "");
    const separator = payload.indexOf("|");
    if (separator === -1) return;

    const color = Number(payload.slice(0, separator));
    addMessage(color, safeDecode(payload.slice(separator + 1)));
}

function setInputOpen(open, initialValue = "") {
    chatForm.classList.toggle("is-hidden", !open);
    if (open) {
        chatInput.value = String(initialValue || "");
        historyIndex = inputHistory.length;
        historyDraft = "";
        updateCount();
        requestAnimationFrame(() => chatInput.focus());
    } else {
        chatInput.blur();
        chatInput.value = "";
        updateCount();
    }
}

function updateCount() {
    chatCount.textContent = `${chatInput.value.length}/144`;
}

function rememberInput(message) {
    if (inputHistory[inputHistory.length - 1] !== message) {
        inputHistory.push(message);
        if (inputHistory.length > MAX_HISTORY) inputHistory.shift();
    }
    historyIndex = inputHistory.length;
    historyDraft = "";
}

function showHistoryEntry(index) {
    chatInput.value = index < inputHistory.length ? inputHistory[index] : historyDraft;
    updateCount();
    requestAnimationFrame(() => {
        const end = chatInput.value.length;
        chatInput.setSelectionRange(end, end);
    });
}

function encodeForPawn(value) {
    try {
        return encodeURIComponent(value);
    } catch {
        return encodeURIComponent(value.replace(/[\uD800-\uDFFF]/g, "\uFFFD"));
    }
}

function submitChat() {
    const message = chatInput.value.trim();
    if (message) {
        rememberInput(message);
        if (/^\/(?:q|quit)$/i.test(message)) {
            cef.emit("GTAHUB:ChatQuit");
        } else {
            cef.emit("GTAHUB:ChatSubmit", encodeForPawn(message));
        }
    } else {
        cef.emit("GTAHUB:ChatClose");
    }
    setInputOpen(false);
}

chatInput.addEventListener("input", updateCount);

chatInput.addEventListener("keydown", (event) => {
    if (event.key === "Enter") {
        event.preventDefault();
        submitChat();
    } else if (event.key === "ArrowUp") {
        event.preventDefault();
        if (!inputHistory.length) return;

        if (historyIndex === inputHistory.length) historyDraft = chatInput.value;
        if (historyIndex > 0) historyIndex--;
        showHistoryEntry(historyIndex);
    } else if (event.key === "ArrowDown") {
        event.preventDefault();
        if (historyIndex >= inputHistory.length) return;

        historyIndex++;
        showHistoryEntry(historyIndex);
    }
});

chatForm.addEventListener("submit", (event) => {
    event.preventDefault();
    submitChat();
});

window.addEventListener("keydown", (event) => {
    if (event.key === "Escape" && !chatForm.classList.contains("is-hidden")) {
        event.preventDefault();
        cef.emit("GTAHUB:ChatClose");
        setInputOpen(false);
    }
});

if (typeof cef !== "undefined" && cef && typeof cef.on === "function") {
    cef.on("GTAHUB:ChatMessage", addMessagePayload);
    cef.on("GTAHUB:ChatInput", setInputOpen);
    cef.emit("GTAHUB:ChatPageReady");
} else {
    if (!new URLSearchParams(window.location.search).has("embed")) {
        document.body.classList.add("browser-preview");
    }
    addMessage(0x53d8a6ff, "GTAHUB: Chào mừng đến với GTAHUB Roleplay.");
    addMessage(0xffffffff, "[12] Đăng_Phát: Mọi người tập trung tại City Hall nhé.");
    addMessage(0xc8a8e9ff, "* Đăng Phát mở cửa xe và quan sát xung quanh.");
    addMessage(0x5dd39eff, "[CẢNH SÁT RADIO] Unit 21: Đã rõ, đang trên đường đến.");
    addMessage(0xffbe55ff, "GTAHUB: Tiền lương sẽ được phát sau 5 phút.");
    setInputOpen(true);
}
