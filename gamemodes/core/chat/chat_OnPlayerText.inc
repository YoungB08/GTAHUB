static
    chat_Cooldown[MAX_PLAYERS],
    chat_Count[MAX_PLAYERS],
    chat_LastMessage[MAX_PLAYERS][144];

public OnPlayerText(playerid, text[])
{
    if(Mute_PlayerHas(playerid)) {
        SendErrorMsgF(playerid, "Ban khong the chat toan cuc, ban dang bi mute trong ~r~%i ~w~giay!", Mute_GetPlayerDuration(playerid));
        return 0;
    }

    if(!Player_IsLoggedIn(playerid)) {
        SendErrorMsg(playerid, "Ban can dang nhap truoc khi chat.");
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
        processedMessage[144];

    ExpandMentions(text, processedMessage, sizeof(processedMessage), -1);

    // User can not send the same message twice.
    if (!isnull(chat_LastMessage[playerid]) && !strcmp(chat_LastMessage[playerid], processedMessage, true, 144)) {
        SendErrorMsg(playerid, "Khong duoc lap lai cung mot tin nhan.");
        return 0;
    }

    strcpy(chat_LastMessage[playerid], processedMessage, 144);

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

    // Normal Global Message
    SendMsgAllF(GetPlayerColor(playerid), "[%i] %p "C_WHITE": %s", playerid, playerid, processedMessage);

    chat_Cooldown[playerid] = gettime() + 2;
    return 0;
}
