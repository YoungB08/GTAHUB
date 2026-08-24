#include <YSI_Coding\y_hooks>

static
    chat_Cooldown[MAX_PLAYERS],
    chat_Count[MAX_PLAYERS],
    String:g_LastPlayerMessage[MAX_PLAYERS];

forward HandlePlayerChat(playerid, const text[]);
public HandlePlayerChat(playerid, const text[])
{
    if (text[0] == '/')
    {
        printf("[CMD Debug] HandlePlayerChat slash input: playerid=%d raw='%s'", playerid, text);

        new tempCmd[CHAT_MAX_UTF8_BYTES];
        format(tempCmd, sizeof(tempCmd), "%s", text);
        return OnPlayerCommandText(playerid, tempCmd);
    }

    if(Mute_PlayerHas(playerid)) {
        SendErrorMsgF(playerid, "Ban khong the chat toan cuc, ban dang bi mute trong ~r~%i ~w~giay!", Mute_GetPlayerDuration(playerid));
        return 0;
    }

    if(!Player_IsLoggedIn(playerid) || !Char_IsActive(playerid)) {
        SendErrorMsg(playerid, "Ban phai dang nhap va chon nhan vat de chat.");
        return 0;
    }

    if(gettime() < chat_Cooldown[playerid]) {
        chat_Count[playerid] ++;

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

    // Dynamic PawnPlus String Duplicate Message Protection
    new String:strCurrent = str_new(processedMessage);
    if (str_valid(g_LastPlayerMessage[playerid]) && str_cmp(g_LastPlayerMessage[playerid], strCurrent) == 0) {
        str_delete(strCurrent);
        SendErrorMsg(playerid, "Khong duoc lap lai cung mot tin nhan.");
        return 0;
    }

    if (str_valid(g_LastPlayerMessage[playerid])) {
        str_delete(g_LastPlayerMessage[playerid]);
    }
    g_LastPlayerMessage[playerid] = strCurrent;

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

    SendLocalMessage(playerid, COLOR_RP_LOCAL_CHAT, msg, LOCAL_CHAT_DISTANCE);

    chat_Cooldown[playerid] = gettime() + 2;
    return 0;
}

hook OnPlayerText(playerid, text[])
{
    printf("[CMD Debug] OnPlayerText entered: playerid=%d raw='%s'", playerid, text);
    return HandlePlayerChat(playerid, text);
}

hook OnPlayerConnect(playerid)
{
    chat_Cooldown[playerid] = 0;
    chat_Count[playerid] = 0;
    if (str_valid(g_LastPlayerMessage[playerid])) {
        str_delete(g_LastPlayerMessage[playerid]);
    }
    g_LastPlayerMessage[playerid] = String:0;
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    if (str_valid(g_LastPlayerMessage[playerid])) {
        str_delete(g_LastPlayerMessage[playerid]);
    }
    g_LastPlayerMessage[playerid] = String:0;
    return 1;
}
