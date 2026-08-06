/*
    Normal
        - SendErrorMsg(playerid, string[]);
        - SendSyntaxMsg(playerid, string[]);
        - SendServerMsg(playerid, string[]);
    Formattted
        - SendErrorMsgF(playerid, string[], {Float,_}...);
        - SendSyntaxMsgF(playerid, string[], {Float,_}...);
        - SendServerMsgF(playerid, string[], {Float,_}...);
*/

#include <YSI_Coding\y_va>

static
    chat_msgOut[CHAT_FORMATTED_BYTES];


stock SendErrorMsg(playerid, const string[]) {
    SendClientMessage(playerid, COLOR_RED, string);
    UI_ShowError(playerid, string);
    return 1;
}

stock SendSyntaxMsg(playerid, const string[]) {
    return SendMsgF(playerid, COLOR_YELLOW, "Cu phap: "C_WHITE"%s", string);
}

stock SendServerMsg(playerid, const string[]) {
    return SendMsgF(playerid, COLOR_GREY, "GTAHUB: "C_WHITE"%s", string);
}

stock SendErrorMsgF(playerid, const fmat[], {Float,_}:...) {
    formatex(chat_msgOut, sizeof(chat_msgOut), fmat, ___(2));
    return SendErrorMsg(playerid, chat_msgOut);
}

stock SendServerMsgF(playerid, const fmat[], {Float,_}:...) {
    formatex(chat_msgOut, sizeof(chat_msgOut), fmat, ___(2));
    return SendServerMsg(playerid, chat_msgOut);
}

stock SendMsgF(playerid, colour, const fmat[], {Float,_}:...) {
    formatex(chat_msgOut, sizeof(chat_msgOut), fmat, ___(3));
    return SendClientMessage(playerid, colour, chat_msgOut);
}

stock SendMsgAllF(colour, const fmat[], {Float,_}:...) {
    formatex(chat_msgOut, sizeof(chat_msgOut), fmat, ___(2));
    return SendClientMessageToAll(colour, chat_msgOut);
}
