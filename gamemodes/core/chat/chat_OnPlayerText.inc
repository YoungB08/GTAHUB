#include <YSI_Coding\y_hooks>

static
    chat_Cooldown[MAX_PLAYERS],
    chat_Count[MAX_PLAYERS],
    chat_LastMessage[MAX_PLAYERS][CHAT_MAX_UTF8_BYTES];

forward CEF_HandlePlayerChat(playerid, const text[]);
public CEF_HandlePlayerChat(playerid, const text[])
{
    printf("[CEF Chat Debug] CEF_HandlePlayerChat start: playerid=%d text='%s'", playerid, text);

    if(Mute_PlayerHas(playerid)) {
        printf("[CEF Chat Debug] Player %d is MUTED", playerid);
        SendErrorMsgF(playerid, "Ban khong the chat toan cuc, ban dang bi mute trong ~r~%i ~w~giay!", Mute_GetPlayerDuration(playerid));
        return 0;
    }

    if(!Player_IsLoggedIn(playerid) || !Char_IsActive(playerid)) {
        printf("[CEF Chat Debug] Player %d NOT logged in or NOT active (loggedIn=%d, active=%d)",
            playerid, Player_IsLoggedIn(playerid), Char_IsActive(playerid));
        SendErrorMsg(playerid, "Ban phai dang nhap va chon nhan vat de chat.");
        return 0;
    }

    if(gettime() < chat_Cooldown[playerid]) {
        chat_Count[playerid] ++;
        printf("[CEF Chat Debug] Player %d cooldown active (count=%d)", playerid, chat_Count[playerid]);

        if(3 <= chat_Count[playerid] < 10) {
            SendServerMsg(playerid, "Dung spam chat, neu khong ban se bi mute!");
            return 0;
        }
        if(chat_Count[playerid] >= 10) {
            chat_Count[playerid] = 0;
            Mute_ToggleForPlayer(playerid, true, 60);
            SendServerMsgF(playerid, "Ban da bi mute "C_GREY"%i "C_WHITE"giay vi spam chat.", Mute_GetPlayerDuration(playerid));
            return 0;
        }
    }
    else {
        if(chat_Count[playerid] > 0) {
            chat_Count[playerid] --;
        }
    }

    new
        processedMessage[CHAT_MAX_UTF8_BYTES];

    ExpandMentions(text, processedMessage, sizeof(processedMessage), -1);

    // User can not send the same message twice.
    if (!isnull(chat_LastMessage[playerid]) && !strcmp(chat_LastMessage[playerid], processedMessage, true, CHAT_MAX_UTF8_BYTES)) {
        printf("[CEF Chat Debug] Player %d duplicate message blocked: '%s'", playerid, processedMessage);
        SendErrorMsg(playerid, "Khong duoc lap lai cung mot tin nhan.");
        return 0;
    }

    strcpy(chat_LastMessage[playerid], processedMessage, CHAT_MAX_UTF8_BYTES);

    // Police Radio
    if(processedMessage[0] == '!') {
        if(!Player_GetClass(playerid)) {
            return 0;
        }

        foreach(new i : Player) {
            if(Player_GetClass(i) != TEAM_POLICE) {
                continue;
            }

            SendMsgF(i, COLOR_BLUE, "(Radio) si quan %p: "C_WHITE"%s", playerid, processedMessage[1]);
        }
        return 0;
    }

    // Admin Chat
    if(processedMessage[0] == '#') {
        if(Admin_GetPlayerLevel(playerid) == 0) {
            return 0;
        }

        foreach(new i : Player) {
            if(Admin_GetPlayerLevel(i) <= 0) {
                continue;
            }

            SendMsgF(i, COLOR_PINK, "(Admin) %p(%i): "C_WHITE"%s", playerid, playerid, processedMessage[1]);
        }
        return 0;
    }

    // Normal Local IC Chat (20m radius)
    new msg[CHAT_FORMATTED_BYTES];
    formatex(msg, sizeof(msg), "[%d] %p: %s", playerid, playerid, processedMessage);
    printf("[CEF Chat Debug] Sending local message for player %d: '%s'", playerid, msg);

    SendLocalMessage(playerid, COLOR_RP_LOCAL_CHAT, msg, LOCAL_CHAT_DISTANCE);

    chat_Cooldown[playerid] = gettime() + 2;
    return 1;
}

public OnPlayerText(playerid, text[])
{
    return CEF_HandlePlayerChat(playerid, text);
}

hook OnPlayerConnect(playerid)
{
    chat_Cooldown[playerid] = 0;
    chat_Count[playerid] = 0;
    chat_LastMessage[playerid][0] = '\0';
    return 1;
}
