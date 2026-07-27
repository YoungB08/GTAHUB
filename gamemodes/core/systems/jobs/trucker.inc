#include <YSI_Coding\y_hooks>

#define MAX_DELIVERY_NAME       (30)

static enum {
    STATE_PICKUP = 0,
    STATE_DELIVER
}

static
    Timer:TruckingPickupTimer[MAX_PLAYERS],
    Timer:TruckingDeliveryTimer[MAX_PLAYERS]
;

static
    bool:IsPlayerTrucking[MAX_PLAYERS],
    TruckingCooldown[MAX_PLAYERS],
    bool:PlayerCanTruck[MAX_PLAYERS],
    PlayerTruckingState[MAX_PLAYERS];

enum E_DELIVERY_DATA
{
    E_STORE[MAX_DELIVERY_NAME],
    Float:E_STORE_X,
    Float:E_STORE_Y,
    Float:E_STORE_Z
}

static
    const DeliverData[][E_DELIVERY_DATA] =
{
    {"Disco", 1821.5656, -1682.2516, 13.3828},
    {"LSPD", 1535.8329, -1673.7499, 13.3828},
    {"LS BANK", 590.8960, -1231.8680, 17.7482},
    {"Ammunation Plaza", 1358.8153, -1278.5531, 13.3517}
};

CMD:trucker(playerid) {
    if(!PlayerCanTruck[playerid]) {
        return SendErrorMsg(playerid, "Ban khong o trong xe tai!");
    }
    if(Player_GetWantedLevel(playerid) != 0) {
        return SendErrorMsg(playerid, "Chi nguoi khong bi truy na moi co the lai xe tai.");
    }
    if(Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return SendErrorMsg(playerid, "Chi dan thuong moi co the lai xe tai.");
    }
    if(!JobProgress_PlayerMeets(playerid, JOB_PROGRESS_TRUCKER, 1, LICENSE_TRUCK)) {
        return 1;
    }
    Trucker_Pickup(playerid);
    return 1;
}

Trucker_Pickup(playerid) {
    if(gettime() < TruckingCooldown[playerid]) {
        return SendErrorMsg(playerid, "Vui long cho truoc khi bat dau nhiem vu xe tai lai!");
    }
    IsPlayerTrucking[playerid] = true;
    PlayerTruckingState[playerid] = STATE_PICKUP;

    DisablePlayerRaceCheckpoint(playerid);
    SetPlayerRaceCheckpoint(playerid, CP_TYPE_GROUND_FINISH, 2174.6182, -2266.6165, 13.3797, 2174.6182, -2266.6165, 13.3797, 3.0);

    UI_TimeLeft(playerid, 300);
    TruckingPickupTimer[playerid] = defer TruckingPickup(playerid);

    SendServerMsg(playerid, "Ban da nhan viec xe tai, hay den lay hang!");
    return 1;
}

Trucker_Begin(playerid) {
    PlayerTruckingState[playerid] = STATE_DELIVER;
    new randloc = random(sizeof(DeliverData));
    new
        Float:x,
        Float:y,
        Float:z,
        storeName[MAX_DELIVERY_NAME];

    x = DeliverData[randloc][E_STORE_X];
    y = DeliverData[randloc][E_STORE_Y];
    z = DeliverData[randloc][E_STORE_Z];
    strcpy(storeName, DeliverData[randloc][E_STORE], sizeof(storeName));

    DisablePlayerRaceCheckpoint(playerid);
    SetPlayerRaceCheckpoint(playerid, CP_TYPE_GROUND_FINISH, x, y, z, x, y, z, 3.0);

	stop TruckingPickupTimer[playerid];
    UI_TimeLeft(playerid, 300);
    TruckingDeliveryTimer[playerid] = defer TruckingDelivery(playerid);

    SendServerMsgF(playerid, "Hay den %s va giao hang.", storeName);
    SendServerMsg(playerid, "Dang lam viec, dung roi khoi xe!");
    return 1;
}

Trucker_End(playerid) {
    IsPlayerTrucking[playerid] = false;
    TruckingCooldown[playerid] = gettime() + 15;

    DisablePlayerRaceCheckpoint(playerid);
    UI_HideTimeLeft(playerid);
    stop TruckingDeliveryTimer[playerid];
    stop TruckingPickupTimer[playerid];
    return 1;
}

hook OnPlayerEnterRaceCP(playerid)
{
    if(IsPlayerTrucking[playerid])
    {
        if(PlayerTruckingState[playerid] == STATE_DELIVER) {
            new randm = JobProgress_CalculateReward(playerid, JOB_PROGRESS_TRUCKER, random(5000) + 4000);
            GivePlayerMoney(playerid, randm);
            JobProgress_AddXP(playerid, JOB_PROGRESS_TRUCKER, max(50, randm / 75));
            Trucker_End(playerid);
            SendServerMsg(playerid, "Ban da hoan thanh nhiem vu xe tai!");
            SendServerMsgF(playerid, "Cua hang da tra cho ban %d!", randm);
            stop TruckingDeliveryTimer[playerid];
        }
        if(PlayerTruckingState[playerid] == STATE_PICKUP) {
            SendServerMsg(playerid, "Nhan vien dang chat hang len xe tai cua ban!");
            UI_TimeLeft(playerid, 10);
            TogglePlayerControllable(playerid, false);
            defer TruckingStandBy(playerid);
        }
    }
    return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate) {
    if(newstate == PLAYER_STATE_ONFOOT) {
        if(IsPlayerTrucking[playerid]) {
            SendServerMsg(playerid, "That bai! Ban da roi khoi xe!");
            Trucker_End(playerid);
        }
        if(PlayerCanTruck[playerid]) {
            PlayerCanTruck[playerid] = false;
        }
    }
    if(newstate == PLAYER_STATE_DRIVER) {
        new pvehid = GetPlayerVehicleID(playerid);
        if(GetVehicleModel(pvehid) == 414) {
            defer TruckerMsg(playerid);
        }
    }
    return 1;
}

timer TruckingPickup[300000](playerid)
{
    SendServerMsg(playerid, "Ban da that bai khi lay hang.");
    Trucker_End(playerid);
}

timer TruckingDelivery[300000](playerid)
{
    SendServerMsg(playerid, "Ban da that bai khi giao hang.");
    Trucker_End(playerid);
}

timer TruckingStandBy[10000](playerid)
{
    Trucker_Begin(playerid);
    TogglePlayerControllable(playerid, true);
}

timer TruckerMsg[1000](playerid)
{
    SendServerMsg(playerid, "Dung /trucker de bat dau nhiem vu xe tai.");
    PlayerCanTruck[playerid] = true;
}
