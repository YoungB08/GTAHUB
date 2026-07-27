#include <YSI_Coding\y_hooks>

// GPS UI - Simple box above the minimap showing destination name and live distance

static
    PlayerText:gps_BoxTD[MAX_PLAYERS],
    PlayerText:gps_NameTD[MAX_PLAYERS],
    PlayerText:gps_DistTD[MAX_PLAYERS],
    bool:gps_UIVisible[MAX_PLAYERS]
;

hook OnPlayerConnect(playerid) {
    gps_UIVisible[playerid] = false;

    // Background box (semi-transparent dark rectangle above minimap)
    gps_BoxTD[playerid] = CreatePlayerTextDraw(playerid, 24.000000, 300.000000, "_");
    PlayerTextDrawFont(playerid, gps_BoxTD[playerid], TEXT_DRAW_FONT_1);
    PlayerTextDrawLetterSize(playerid, gps_BoxTD[playerid], 0.000000, 3.200000);
    PlayerTextDrawUseBox(playerid, gps_BoxTD[playerid], true);
    PlayerTextDrawBoxColour(playerid, gps_BoxTD[playerid], 0x00000099);
    PlayerTextDrawTextSize(playerid, gps_BoxTD[playerid], 130.000000, 0.000000);
    PlayerTextDrawColor(playerid, gps_BoxTD[playerid], 0);
    PlayerTextDrawBackgroundColor(playerid, gps_BoxTD[playerid], 0);
    PlayerTextDrawSetOutline(playerid, gps_BoxTD[playerid], 0);
    PlayerTextDrawSetShadow(playerid, gps_BoxTD[playerid], 0);
    PlayerTextDrawSetProportional(playerid, gps_BoxTD[playerid], true);

    // Destination name text (e.g. "GPS: Bank")
    gps_NameTD[playerid] = CreatePlayerTextDraw(playerid, 28.000000, 302.000000, "~y~GPS: ~w~---");
    PlayerTextDrawFont(playerid, gps_NameTD[playerid], TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, gps_NameTD[playerid], 0.180000, 0.180000 * 4);
    PlayerTextDrawColor(playerid, gps_NameTD[playerid], -1);
    PlayerTextDrawBackgroundColor(playerid, gps_NameTD[playerid], TD_OUTLINE_COLOUR);
    PlayerTextDrawSetOutline(playerid, gps_NameTD[playerid], 1);
    PlayerTextDrawSetProportional(playerid, gps_NameTD[playerid], true);

    // Distance text (e.g. "142m")
    gps_DistTD[playerid] = CreatePlayerTextDraw(playerid, 28.000000, 317.000000, "~g~0m");
    PlayerTextDrawFont(playerid, gps_DistTD[playerid], TEXT_DRAW_FONT_2);
    PlayerTextDrawLetterSize(playerid, gps_DistTD[playerid], 0.220000, 0.220000 * 4);
    PlayerTextDrawColor(playerid, gps_DistTD[playerid], -1);
    PlayerTextDrawBackgroundColor(playerid, gps_DistTD[playerid], TD_OUTLINE_COLOUR);
    PlayerTextDrawSetOutline(playerid, gps_DistTD[playerid], 1);
    PlayerTextDrawSetProportional(playerid, gps_DistTD[playerid], true);

    return 1;
}

hook OnPlayerDeathEx(playerid, killerid, reason) {
    GPS_UI_Hide(playerid);
    return 1;
}

stock GPS_UI_Show(playerid, const name[]) {
    new string[64];
    format(string, sizeof(string), "~y~GPS: ~w~%s", name);
    PlayerTextDrawSetString(playerid, gps_NameTD[playerid], string);
    PlayerTextDrawSetString(playerid, gps_DistTD[playerid], "~g~Calculating...");

    PlayerTextDrawShow(playerid, gps_BoxTD[playerid]);
    PlayerTextDrawShow(playerid, gps_NameTD[playerid]);
    PlayerTextDrawShow(playerid, gps_DistTD[playerid]);
    gps_UIVisible[playerid] = true;
}

stock GPS_UI_Update(playerid, Float:distance) {
    if(!gps_UIVisible[playerid]) return 0;

    new string[32];
    if(distance >= 1000.0) {
        format(string, sizeof(string), "~g~%.1fkm", distance / 1000.0);
    } else {
        format(string, sizeof(string), "~g~%dm", floatround(distance));
    }
    PlayerTextDrawSetString(playerid, gps_DistTD[playerid], string);
    return 1;
}

stock GPS_UI_Hide(playerid) {
    if(!gps_UIVisible[playerid]) return 0;

    PlayerTextDrawHide(playerid, gps_BoxTD[playerid]);
    PlayerTextDrawHide(playerid, gps_NameTD[playerid]);
    PlayerTextDrawHide(playerid, gps_DistTD[playerid]);
    gps_UIVisible[playerid] = false;
    return 1;
}
