#include <YSI_Coding\y_hooks>

#define SPEEDOMETER_UPDATE_MS       (250)
#define SPEEDOMETER_KMH_MULTIPLIER  (180.0)

static
    PlayerText:SpeedometerBox[MAX_PLAYERS],
    PlayerText:SpeedometerText[MAX_PLAYERS],
    bool:SpeedometerVisible[MAX_PLAYERS];

static stock bool:Speedometer_IsAircraftModel(modelid)
{
    switch(modelid) {
        case 417, 425, 447, 460, 469, 476, 487, 488, 497, 511, 512, 513, 519, 520, 548, 553, 563, 577, 592, 593: {
            return true;
        }
    }
    return false;
}

static stock bool:Speedometer_IsBoatModel(modelid)
{
    switch(modelid) {
        case 430, 446, 452, 453, 454, 472, 473, 484, 493, 595: {
            return true;
        }
    }
    return false;
}

static stock Speedometer_GetVehicleType(vehicleid, output[], len)
{
    new
        modelid = GetVehicleModel(vehicleid);

    if(Speedometer_IsAircraftModel(modelid)) {
        strcpy(output, "MAY BAY", len);
    }
    else if(Speedometer_IsBoatModel(modelid)) {
        strcpy(output, "THUYEN", len);
    }
    else {
        strcpy(output, "XE", len);
    }
    return 1;
}

static stock Speedometer_GetVehicleSpeed(vehicleid)
{
    new
        Float:x,
        Float:y,
        Float:z;

    GetVehicleVelocity(vehicleid, x, y, z);
    return floatround(floatsqroot((x * x) + (y * y) + (z * z)) * SPEEDOMETER_KMH_MULTIPLIER);
}

static stock Speedometer_Create(playerid)
{
    SpeedometerBox[playerid] = CreatePlayerTextDraw(playerid, 500.000000, 338.000000, "_");
    PlayerTextDrawFont(playerid, SpeedometerBox[playerid], TEXT_DRAW_FONT_1);
    PlayerTextDrawLetterSize(playerid, SpeedometerBox[playerid], 0.000000, 7.000000);
    PlayerTextDrawTextSize(playerid, SpeedometerBox[playerid], 625.000000, 0.000000);
    PlayerTextDrawUseBox(playerid, SpeedometerBox[playerid], true);
    PlayerTextDrawBoxColour(playerid, SpeedometerBox[playerid], 0x00000088);
    PlayerTextDrawBackgroundColor(playerid, SpeedometerBox[playerid], 255);
    PlayerTextDrawSetOutline(playerid, SpeedometerBox[playerid], 0);
    PlayerTextDrawSetShadow(playerid, SpeedometerBox[playerid], 0);
    PlayerTextDrawSetProportional(playerid, SpeedometerBox[playerid], true);
    PlayerTextDrawSetSelectable(playerid, SpeedometerBox[playerid], false);

    SpeedometerText[playerid] = CreatePlayerTextDraw(playerid, 508.000000, 344.000000, "TOC DO");
    PlayerTextDrawFont(playerid, SpeedometerText[playerid], TEXT_DRAW_FONT_1);
    PlayerTextDrawLetterSize(playerid, SpeedometerText[playerid], 0.220000, 1.000000);
    PlayerTextDrawTextSize(playerid, SpeedometerText[playerid], 620.000000, 0.000000);
    PlayerTextDrawAlignment(playerid, SpeedometerText[playerid], TEXT_DRAW_ALIGN_LEFT);
    PlayerTextDrawColor(playerid, SpeedometerText[playerid], COLOR_WHITE);
    PlayerTextDrawBackgroundColor(playerid, SpeedometerText[playerid], 255);
    PlayerTextDrawSetOutline(playerid, SpeedometerText[playerid], 1);
    PlayerTextDrawSetShadow(playerid, SpeedometerText[playerid], 0);
    PlayerTextDrawSetProportional(playerid, SpeedometerText[playerid], true);
    PlayerTextDrawSetSelectable(playerid, SpeedometerText[playerid], false);

    SpeedometerVisible[playerid] = false;
    return 1;
}

static stock Speedometer_Destroy(playerid)
{
    if(SpeedometerBox[playerid] != INVALID_PLAYER_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, SpeedometerBox[playerid]);
        SpeedometerBox[playerid] = INVALID_PLAYER_TEXT_DRAW;
    }

    if(SpeedometerText[playerid] != INVALID_PLAYER_TEXT_DRAW) {
        PlayerTextDrawDestroy(playerid, SpeedometerText[playerid]);
        SpeedometerText[playerid] = INVALID_PLAYER_TEXT_DRAW;
    }

    SpeedometerVisible[playerid] = false;
    return 1;
}

static stock Speedometer_Show(playerid)
{
    if(!SpeedometerVisible[playerid]) {
        PlayerTextDrawShow(playerid, SpeedometerBox[playerid]);
        PlayerTextDrawShow(playerid, SpeedometerText[playerid]);
        SpeedometerVisible[playerid] = true;
    }
    return 1;
}

static stock Speedometer_Hide(playerid)
{
    if(SpeedometerVisible[playerid]) {
        PlayerTextDrawHide(playerid, SpeedometerBox[playerid]);
        PlayerTextDrawHide(playerid, SpeedometerText[playerid]);
        SpeedometerVisible[playerid] = false;
    }
    return 1;
}

static stock Speedometer_Update(playerid)
{
    if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
        return Speedometer_Hide(playerid);
    }

    new
        vehicleid = GetPlayerVehicleID(playerid);

    if(vehicleid == INVALID_VEHICLE_ID) {
        return Speedometer_Hide(playerid);
    }

    new
        engine,
        lights,
        alarm,
        doors,
        bonnet,
        boot,
        objective,
        speed = Speedometer_GetVehicleSpeed(vehicleid),
        type[8],
        string[128],
        Float:health;

    GetVehicleHealth(vehicleid, health);
    GetVehicleParamsEx(vehicleid, engine, lights, alarm, doors, bonnet, boot, objective);
    Speedometer_GetVehicleType(vehicleid, type, sizeof(type));

    format(
        string,
        sizeof(string),
        "~y~~h~%s~n~~w~%03d KM/H~n~Tinh trang: ~g~%.0f~n~~w~Nhien lieu: ~r~N/A~n~~w~May:%s  Den:%s",
        type,
        speed,
        health,
        engine ? ("BAT") : ("TAT"),
        lights ? ("BAT") : ("TAT")
    );

    PlayerTextDrawSetString(playerid, SpeedometerText[playerid], string);
    return Speedometer_Show(playerid);
}

hook OnPlayerConnect(playerid)
{
    SpeedometerBox[playerid] = INVALID_PLAYER_TEXT_DRAW;
    SpeedometerText[playerid] = INVALID_PLAYER_TEXT_DRAW;
    Speedometer_Create(playerid);
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    #pragma unused reason
    Speedometer_Destroy(playerid);
    return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    #pragma unused oldstate

    if(newstate == PLAYER_STATE_DRIVER) {
        Speedometer_Update(playerid);
    }
    else {
        Speedometer_Hide(playerid);
    }
    return 1;
}

static ptask Speedometer_UpdateTask[SPEEDOMETER_UPDATE_MS](playerid)
{
    Speedometer_Update(playerid);
}
