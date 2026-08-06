// =============================================================================
// GTAHUB custom CEF chat
// =============================================================================

#include <YSI_Coding\y_hooks>

#define CEF_CHAT_VK_T          (0x54)
#define CEF_CHAT_VK_F6         (0x75)
#define CEF_CHAT_VK_DIVIDE     (0x6F)
#define CEF_CHAT_VK_SLASH      (0xBF)

forward OnCEF_ChatSubmit(playerid, browserid, const message[]);
forward OnCEF_ChatQuit(playerid, browserid);
forward OnCEF_ChatClose(playerid, browserid);
forward OnCEF_ChatPageReady(playerid, browserid);
forward OnCEF_ChatRendered(playerid, browserid);
forward CEF_ChatFinalizeBrowser(playerid);
forward CEF_ChatExitPlayer(playerid);
forward CEF_HandlePlayerChat(playerid, const text[]);

static bool:gCEFChatRequested[MAX_PLAYERS];
static bool:gCEFChatCreated[MAX_PLAYERS];
static bool:gCEFChatOpen[MAX_PLAYERS];
static bool:gCEFChatPageReady[MAX_PLAYERS];
static bool:gCEFChatRendered[MAX_PLAYERS];
static gCEFChatPayload[CHAT_FORMATTED_BYTES * 3 + 24];
static gCEFChatInput[CHAT_MAX_UTF8_BYTES];

stock CEF_ChatInit()
{
    CEF_RegisterEvent("GTAHUB:ChatSubmit", "OnCEF_ChatSubmit", Argument_String);
    CEF_RegisterEvent("GTAHUB:ChatQuit", "OnCEF_ChatQuit");
    CEF_RegisterEvent("GTAHUB:ChatClose", "OnCEF_ChatClose");
    CEF_RegisterEvent("GTAHUB:ChatPageReady", "OnCEF_ChatPageReady");
    CEF_RegisterEvent("GTAHUB:ChatRendered", "OnCEF_ChatRendered");
    return 1;
}

stock CEF_ChatOnConnect(playerid)
{
    gCEFChatRequested[playerid] = false;
    gCEFChatCreated[playerid] = false;
    gCEFChatOpen[playerid] = false;
    gCEFChatPageReady[playerid] = false;
    gCEFChatRendered[playerid] = false;
    return 1;
}

stock CEF_ChatOnDisconnect(playerid)
{
    gCEFChatRequested[playerid] = false;
    gCEFChatCreated[playerid] = false;
    gCEFChatOpen[playerid] = false;
    gCEFChatPageReady[playerid] = false;
    gCEFChatRendered[playerid] = false;
    return 1;
}

stock bool:CEF_ChatCanReceive(playerid)
{
    return IsPlayerConnected(playerid)
        && CEF_IsPlayerCEFReady(playerid)
        && gCEFChatCreated[playerid];
}

static CEF_ChatEncodeOutput(const input[], output[], outputSize)
{
    static const hex[] = "0123456789ABCDEF";
    new readIndex;
    new writeIndex;

    while (input[readIndex] != '\0' && writeIndex + 3 < outputSize)
    {
        new value = input[readIndex++] & 0xFF;
        output[writeIndex++] = '%';
        output[writeIndex++] = hex[(value >> 4) & 0x0F];
        output[writeIndex++] = hex[value & 0x0F];
    }
    output[writeIndex] = '\0';
    return writeIndex;
}

stock CEF_ChatPushMessage(playerid, colour, const message[])
{
    printf("[CEF Chat Debug] CEF_ChatPushMessage playerid=%d colour=0x%08X text='%s'", playerid, colour, message);
    if (!CEF_ChatCanReceive(playerid))
    {
        printf("[CEF Chat Debug] CEF_ChatPushMessage BLOCKED: CEF_ChatCanReceive is false!");
        return 0;
    }

    format(gCEFChatPayload, sizeof(gCEFChatPayload), "%d|", colour);
    new offset = strlen(gCEFChatPayload);
    CEF_ChatEncodeOutput(message, gCEFChatPayload[offset], sizeof(gCEFChatPayload) - offset);
    printf("[CEF Chat Debug] CEF_EmitEvent GTAHUB:ChatMessage payload='%s'", gCEFChatPayload);
    CEF_EmitEvent(playerid, CEF_BROWSER_CUSTOM_CHAT, "GTAHUB:ChatMessage", CEF_STR(gCEFChatPayload));
    return 1;
}

stock CEF_ShowCustomChat(playerid)
{
    if (!IsPlayerConnected(playerid) || !CEF_IsPlayerCEFReady(playerid) || !Char_IsActive(playerid))
        return 0;

    CEF_ToggleChatInput(playerid, false);
    CEF_SetKeyCapture(playerid, true);
    CEF_EnableKey(playerid, CEF_CHAT_VK_T, true);
    CEF_EnableKey(playerid, CEF_CHAT_VK_F6, true);
    CEF_EnableKey(playerid, CEF_CHAT_VK_DIVIDE, true);
    CEF_EnableKey(playerid, CEF_CHAT_VK_SLASH, true);

    if (!gCEFChatCreated[playerid])
    {
        gCEFChatRequested[playerid] = true;
        CEF_TrackBrowserCreated(playerid, CEF_BROWSER_CUSTOM_CHAT, false);
        CEF_CreateBrowser(playerid, CEF_BROWSER_CUSTOM_CHAT, CEF_URL_CUSTOM_CHAT, false, false);
    }
    return 1;
}

stock CEF_HideCustomChat(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;

    if (CEF_IsPlayerCEFReady(playerid))
    {
        CEF_EnableKey(playerid, CEF_CHAT_VK_T, false);
        CEF_EnableKey(playerid, CEF_CHAT_VK_F6, false);
        CEF_EnableKey(playerid, CEF_CHAT_VK_DIVIDE, false);
        CEF_EnableKey(playerid, CEF_CHAT_VK_SLASH, false);
        CEF_SetKeyCapture(playerid, false);
        CEF_FocusBrowser(playerid, CEF_BROWSER_CUSTOM_CHAT, false);
        CEF_DestroyBrowser(playerid, CEF_BROWSER_CUSTOM_CHAT);
    }

    gCEFChatRequested[playerid] = false;
    gCEFChatCreated[playerid] = false;
    gCEFChatOpen[playerid] = false;
    gCEFChatPageReady[playerid] = false;
    gCEFChatRendered[playerid] = false;
    return 1;
}

stock CEF_ChatSetOpen(playerid, bool:open, const initialValue[] = "")
{
    gCEFChatOpen[playerid] = open;
    if (IsPlayerConnected(playerid) && CEF_IsPlayerCEFReady(playerid))
    {
        CEF_FocusBrowser(playerid, CEF_BROWSER_CUSTOM_CHAT, open);
        CEF_EmitEvent(playerid, CEF_BROWSER_CUSTOM_CHAT, "GTAHUB:ChatInput",
            CEF_BOOL(open), CEF_STR(initialValue));
    }
    return 1;
}

stock CEF_ChatOnBrowserCreated(playerid, browserid, bool:success)
{
    if (browserid != CEF_BROWSER_CUSTOM_CHAT) return 0;

    gCEFChatCreated[playerid] = success;
    if (!success)
    {
        gCEFChatRequested[playerid] = false;
        CEF_ToggleChatInput(playerid, true);
        return 0;
    }

    SetTimerEx("CEF_ChatFinalizeBrowser", 200, false, "i", playerid);
    return 1;
}

public OnCEF_ChatPageReady(playerid, browserid)
{
    if (browserid != CEF_BROWSER_CUSTOM_CHAT || !gCEFChatRequested[playerid]) return 0;

    gCEFChatPageReady[playerid] = true;
    printf("[CEF Chat] Page ready: playerid=%d", playerid);
    if (gCEFChatCreated[playerid])
        SetTimerEx("CEF_ChatFinalizeBrowser", 50, false, "i", playerid);
    return 1;
}

public OnCEF_ChatRendered(playerid, browserid)
{
    if (browserid != CEF_BROWSER_CUSTOM_CHAT) return 0;
    gCEFChatRendered[playerid] = true;
    printf("[CEF Chat] Render confirmed: playerid=%d", playerid);
    return 1;
}

public CEF_ChatFinalizeBrowser(playerid)
{
    if (!IsPlayerConnected(playerid) || !CEF_IsPlayerCEFReady(playerid) || !gCEFChatCreated[playerid])
        return 0;

    gCEFChatPageReady[playerid] = true;
    CEF_ClearNativeChatFallback(playerid);
    CEF_ToggleChatInput(playerid, false);
    CEF_ChatPushMessage(playerid, 0x53D8A6FF,
        "GTAHUB: Chat tiếng Việt đã sẵn sàng. Nhấn T hoặc F6 để nhập tin nhắn.");
    printf("[CEF Chat] Ready: playerid=%d", playerid);
    return 1;
}

hook OnCefPressKey(playerid, key, scancode, modifiers, bool:down, bool:isrepeat)
{
    #pragma unused scancode, modifiers
    if (!down || isrepeat || !IsPlayerConnected(playerid) || !CEF_IsPlayerCEFReady(playerid) || !Char_IsActive(playerid))
        return 1;

    if (key == CEF_CHAT_VK_T || key == CEF_CHAT_VK_F6)
    {
        if (gCEFChatOpen[playerid])
        {
            printf("[CEF Chat] Toggle close key: playerid=%d key=%d", playerid, key);
            CEF_ChatSetOpen(playerid, false);
        }
        else
        {
            printf("[CEF Chat] Open key: playerid=%d key=%d", playerid, key);
            CEF_ChatSetOpen(playerid, true);
        }
    }
    else if (key == CEF_CHAT_VK_DIVIDE || key == CEF_CHAT_VK_SLASH)
    {
        if (!gCEFChatOpen[playerid])
        {
            printf("[CEF Chat] Open command key: playerid=%d key=%d", playerid, key);
            CEF_ChatSetOpen(playerid, true, "/");
        }
    }
    return 1;
}

static CEF_ChatHexValue(character)
{
    if ('0' <= character <= '9') return character - '0';
    if ('A' <= character <= 'F') return character - 'A' + 10;
    if ('a' <= character <= 'f') return character - 'a' + 10;
    return -1;
}

static CEF_ChatDecodeInput(const encoded[], output[], outputSize)
{
    new readIndex;
    new writeIndex;
    new encodedLength = strlen(encoded);

    while (readIndex < encodedLength && writeIndex < outputSize - 1)
    {
        if (encoded[readIndex] == '%' && readIndex + 2 < encodedLength)
        {
            new high = CEF_ChatHexValue(encoded[readIndex + 1]);
            new low = CEF_ChatHexValue(encoded[readIndex + 2]);
            if (high != -1 && low != -1)
            {
                output[writeIndex++] = (high << 4) | low;
                readIndex += 3;
                continue;
            }
        }

        output[writeIndex++] = encoded[readIndex++];
    }

    output[writeIndex] = '\0';
    return writeIndex;
}

public OnCEF_ChatSubmit(playerid, browserid, const message[])
{
    #pragma unused browserid
    CEF_ChatSetOpen(playerid, false);

    if (!IsPlayerConnected(playerid) || isnull(message)) return 0;

    CEF_ChatDecodeInput(message, gCEFChatInput, sizeof(gCEFChatInput));
    printf("[CEF Chat] Submit: playerid=%d bytes=%d command=%d",
        playerid, strlen(gCEFChatInput), gCEFChatInput[0] == '/');

    if (gCEFChatInput[0] == '/')
    {
        OnPlayerCommandText(playerid, gCEFChatInput);
    }
    else
    {
        CEF_HandlePlayerChat(playerid, gCEFChatInput);
    }
    return 1;
}

public OnCEF_ChatQuit(playerid, browserid)
{
    #pragma unused browserid
    CEF_ChatSetOpen(playerid, false);

    if (!IsPlayerConnected(playerid)) return 0;
    SetTimerEx("CEF_ChatExitPlayer", 100, false, "i", playerid);
    return 1;
}

public CEF_ChatExitPlayer(playerid)
{
    if (!IsPlayerConnected(playerid)) return 0;
    CEF_ExitGame(playerid);
    return 1;
}

public OnCEF_ChatClose(playerid, browserid)
{
    #pragma unused browserid
    return CEF_ChatSetOpen(playerid, false);
}

CMD:chatpreview(playerid, params[])
{
    #pragma unused params
    if (!CEF_ChatCanReceive(playerid))
        return CEF_SendClientMessage(playerid, 0xE75C5CFF, "CEF chat chua san sang."), 1;

    CEF_ChatPushMessage(playerid, 0x53D8A6FF, "GTAHUB: Chào mừng đến với GTAHUB Roleplay.");
    CEF_ChatPushMessage(playerid, 0xFFFFFFFF, "[12] Đăng_Phát: Mọi người tập trung tại City Hall nhé.");
    CEF_ChatPushMessage(playerid, 0xC8A8E9FF, "* Đăng Phát mở cửa xe và quan sát xung quanh.");
    CEF_ChatPushMessage(playerid, 0x5DD39EFF, "[CẢNH SÁT RADIO] Unit 21: Đã rõ, đang trên đường đến.");
    CEF_ChatPushMessage(playerid, 0xFFBE55FF, "GTAHUB: Tiền lương sẽ được phát sau 5 phút.");
    return 1;
}

CMD:chatutf8(playerid, params[])
{
    #pragma unused params
    if (!CEF_ChatCanReceive(playerid))
        return CEF_SendClientMessage(playerid, 0xE75C5CFF, "CEF chat chưa sẵn sàng."), 1;

    CEF_ChatPushMessage(playerid, 0x53D8A6FF,
        "Kiểm tra UTF-8: Tiếng Việt hiển thị đầy đủ, không lỗi dấu hỏi.");
    return 1;
}
