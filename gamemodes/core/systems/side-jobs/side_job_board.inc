#include <YSI_Coding\y_hooks>

#define SIDE_JOB_BOARD_INTERACTION_ID       (9100)
#define SIDE_JOB_TIMEOUT                    (420)
#define SIDE_JOB_NONE                       (-1)

enum E_SIDE_JOB_DATA {
    E_SIDE_JOB_NAME[32],
    E_SIDE_JOB_PROGRESS_ID,
    E_SIDE_JOB_MIN_LEVEL,
    E_SIDE_JOB_LICENSE,
    E_SIDE_JOB_REWARD,
    E_SIDE_JOB_MODEL,
    Float:E_SIDE_JOB_VEH_X,
    Float:E_SIDE_JOB_VEH_Y,
    Float:E_SIDE_JOB_VEH_Z,
    Float:E_SIDE_JOB_VEH_A
}

static const SideJobData[][E_SIDE_JOB_DATA] = {
    {"Delivery", JOB_PROGRESS_DELIVERY, 1, LICENSE_DRIVING, 3200, 448, 2115.5190, -1788.4954, 13.5547, 0.0},
    {"Taxi", JOB_PROGRESS_TAXI, 1, LICENSE_DRIVING, 3600, 420, 1486.7281, -1737.4581, 13.2268, 89.0},
    {"Warehouse", JOB_PROGRESS_WAREHOUSE, 1, LICENSE_NONE, 2800, 0, 0.0, 0.0, 0.0, 0.0},
    {"Miner", JOB_PROGRESS_MINER, 2, LICENSE_JOB, 5200, 0, 0.0, 0.0, 0.0, 0.0},
    {"Fisher", JOB_PROGRESS_FISHER, 2, LICENSE_JOB, 4800, 0, 0.0, 0.0, 0.0, 0.0},
    {"Mechanic", JOB_PROGRESS_MECHANIC, 2, LICENSE_TRUCK, 4600, 525, 1648.6710, -1836.0947, 13.3828, 0.0},
    {"Pilot", JOB_PROGRESS_PILOT, 3, LICENSE_PILOT, 7500, 593, 1964.6954, -2493.8677, 13.5469, 90.0}
};

static const Float:SideJobDeliveryRoute[][3] = {
    {1936.5782, -1777.1932, 13.3828},
    {1831.8195, -1682.2674, 13.3828},
    {1535.8329, -1673.7499, 13.3828}
};

static const Float:SideJobWarehouseRoute[][3] = {
    {1476.4772, -1756.2811, 13.5469},
    {1468.2174, -1764.1883, 13.5469},
    {1486.9517, -1767.2218, 13.5469}
};

static const Float:SideJobTaxiRoute[][3] = {
    {1535.8329, -1673.7499, 13.3828},
    {1821.5656, -1682.2516, 13.3828},
    {590.8960, -1231.8680, 17.7482}
};

static const Float:SideJobMechanicRoute[][3] = {
    {1652.3129, -1836.1707, 13.3828},
    {1810.2656, -1901.4414, 13.3828},
    {1936.5782, -1777.1932, 13.3828}
};

static const Float:SideJobMinerRoute[][3] = {
    {587.1121, 871.9628, -42.4973},
    {596.5112, 867.7714, -42.9609},
    {606.1168, 876.2537, -43.5798},
    {592.3000, 884.2000, -42.9000}
};

static const Float:SideJobFisherRoute[][3] = {
    {387.1484, -1751.5078, 7.9453},
    {432.2422, -1747.8828, 8.3281},
    {477.7733, -1746.4531, 8.5000}
};

static const Float:SideJobPilotRoute[][3] = {
    {1810.6481, -2493.4907, 13.5469},
    {1665.7125, -2492.2354, 13.5469},
    {1545.4658, -2492.2234, 13.5469},
    {1964.6954, -2493.8677, 13.5469}
};

static
    SideJobBoardArea,
    bool:SideJobActive[MAX_PLAYERS],
    SideJobID[MAX_PLAYERS],
    SideJobStep[MAX_PLAYERS],
    SideJobVehicle[MAX_PLAYERS],
    SideJobDeadline[MAX_PLAYERS];

static bool:SideJob_IsActive(playerid)
{
    return SideJobActive[playerid];
}

static SideJob_GetRouteSize(sidejobid)
{
    switch(SideJobData[sidejobid][E_SIDE_JOB_PROGRESS_ID]) {
        case JOB_PROGRESS_DELIVERY: return sizeof(SideJobDeliveryRoute);
        case JOB_PROGRESS_TAXI: return sizeof(SideJobTaxiRoute);
        case JOB_PROGRESS_WAREHOUSE: return sizeof(SideJobWarehouseRoute);
        case JOB_PROGRESS_MINER: return sizeof(SideJobMinerRoute);
        case JOB_PROGRESS_FISHER: return sizeof(SideJobFisherRoute);
        case JOB_PROGRESS_MECHANIC: return sizeof(SideJobMechanicRoute);
        case JOB_PROGRESS_PILOT: return sizeof(SideJobPilotRoute);
    }
    return 0;
}

static SideJob_GetRoutePoint(sidejobid, step, &Float:x, &Float:y, &Float:z)
{
    switch(SideJobData[sidejobid][E_SIDE_JOB_PROGRESS_ID]) {
        case JOB_PROGRESS_DELIVERY: {
            if(step >= sizeof(SideJobDeliveryRoute)) return 0;
            x = SideJobDeliveryRoute[step][0];
            y = SideJobDeliveryRoute[step][1];
            z = SideJobDeliveryRoute[step][2];
            return 1;
        }
        case JOB_PROGRESS_WAREHOUSE: {
            if(step >= sizeof(SideJobWarehouseRoute)) return 0;
            x = SideJobWarehouseRoute[step][0];
            y = SideJobWarehouseRoute[step][1];
            z = SideJobWarehouseRoute[step][2];
            return 1;
        }
        case JOB_PROGRESS_TAXI: {
            if(step >= sizeof(SideJobTaxiRoute)) return 0;
            x = SideJobTaxiRoute[step][0];
            y = SideJobTaxiRoute[step][1];
            z = SideJobTaxiRoute[step][2];
            return 1;
        }
        case JOB_PROGRESS_MINER: {
            if(step >= sizeof(SideJobMinerRoute)) return 0;
            x = SideJobMinerRoute[step][0];
            y = SideJobMinerRoute[step][1];
            z = SideJobMinerRoute[step][2];
            return 1;
        }
        case JOB_PROGRESS_FISHER: {
            if(step >= sizeof(SideJobFisherRoute)) return 0;
            x = SideJobFisherRoute[step][0];
            y = SideJobFisherRoute[step][1];
            z = SideJobFisherRoute[step][2];
            return 1;
        }
        case JOB_PROGRESS_MECHANIC: {
            if(step >= sizeof(SideJobMechanicRoute)) return 0;
            x = SideJobMechanicRoute[step][0];
            y = SideJobMechanicRoute[step][1];
            z = SideJobMechanicRoute[step][2];
            return 1;
        }
        case JOB_PROGRESS_PILOT: {
            if(step >= sizeof(SideJobPilotRoute)) return 0;
            x = SideJobPilotRoute[step][0];
            y = SideJobPilotRoute[step][1];
            z = SideJobPilotRoute[step][2];
            return 1;
        }
    }
    return 0;
}

static SideJob_SetCheckpoint(playerid)
{
    new
        sidejobid = SideJobID[playerid],
        step = SideJobStep[playerid],
        Float:x,
        Float:y,
        Float:z;

    if(!SideJob_GetRoutePoint(sidejobid, step, x, y, z)) {
        return 0;
    }

    SetPlayerRaceCheckpoint(playerid, CP_TYPE_GROUND_FINISH, x, y, z, x, y, z, 4.0);
    SendServerMsgF(playerid, "%s: checkpoint %d/%d.", SideJobData[sidejobid][E_SIDE_JOB_NAME], step + 1, SideJob_GetRouteSize(sidejobid));
    return 1;
}

static SideJob_End(playerid, bool:completed, const reason[] = "")
{
    if(!SideJob_IsActive(playerid)) {
        return 0;
    }

    new
        sidejobid = SideJobID[playerid],
        progressid = SideJobData[sidejobid][E_SIDE_JOB_PROGRESS_ID],
        vehicleid = SideJobVehicle[playerid];

    SideJobActive[playerid] = false;
    SideJobID[playerid] = SIDE_JOB_NONE;
    SideJobStep[playerid] = 0;
    SideJobVehicle[playerid] = INVALID_VEHICLE_ID;
    SideJobDeadline[playerid] = 0;
    DisablePlayerRaceCheckpoint(playerid);

    if(IsValidVehicle(vehicleid)) {
        DestroyVehicle(vehicleid);
    }

    if(completed) {
        new
            reward = JobProgress_CalculateReward(playerid, progressid, SideJobData[sidejobid][E_SIDE_JOB_REWARD]),
            xp = max(35, reward / 80);

        GivePlayerMoney(playerid, reward);
        JobProgress_AddXP(playerid, progressid, xp);
        SendServerMsgF(playerid, "Hoan thanh %s. Thuong: %m | XP nghe: %d.", SideJobData[sidejobid][E_SIDE_JOB_NAME], reward, xp);
    }
    else if(!isnull(reason)) {
        SendErrorMsgF(playerid, "%s that bai: %s", SideJobData[sidejobid][E_SIDE_JOB_NAME], reason);
    }
    return 1;
}

static SideJob_Start(playerid, sidejobid)
{
    if(SideJob_IsActive(playerid)) {
        return SendErrorMsg(playerid, "Ban dang lam mot viec phu khac.");
    }

    if(!JobProgress_PlayerMeets(playerid, SideJobData[sidejobid][E_SIDE_JOB_PROGRESS_ID], SideJobData[sidejobid][E_SIDE_JOB_MIN_LEVEL], SideJobData[sidejobid][E_SIDE_JOB_LICENSE])) {
        return 1;
    }

    new vehicleid = INVALID_VEHICLE_ID;

    if(SideJobData[sidejobid][E_SIDE_JOB_MODEL] != 0) {
        vehicleid = CreateVehicle(
            SideJobData[sidejobid][E_SIDE_JOB_MODEL],
            SideJobData[sidejobid][E_SIDE_JOB_VEH_X],
            SideJobData[sidejobid][E_SIDE_JOB_VEH_Y],
            SideJobData[sidejobid][E_SIDE_JOB_VEH_Z],
            SideJobData[sidejobid][E_SIDE_JOB_VEH_A],
            1,
            1,
            -1,
            false
        );
        PutPlayerInVehicle(playerid, vehicleid, 0);
    }

    SideJobActive[playerid] = true;
    SideJobID[playerid] = sidejobid;
    SideJobStep[playerid] = 0;
    SideJobVehicle[playerid] = vehicleid;
    SideJobDeadline[playerid] = gettime() + SIDE_JOB_TIMEOUT;
    SideJob_SetCheckpoint(playerid);

    SendServerMsgF(playerid, "Ban da bat dau %s. Hoan thanh tat ca checkpoint truoc khi het gio.", SideJobData[sidejobid][E_SIDE_JOB_NAME]);
    return 1;
}

static SideJob_ShowBoard(playerid)
{
    inline const _response(response, listitem, string:inputtext[]) {
        #pragma unused inputtext

        if(!response) {
            return 1;
        }

        if(listitem < 0 || listitem >= sizeof(SideJobData)) {
            return 1;
        }

        SideJob_Start(playerid, listitem);
        return 1;
    }

    new list[768], line[96], licenseName[32];
    list[0] = '\0';
    strcat(list, "Viec\tYeu cau\tThuong\n", sizeof(list));

    for(new i; i < sizeof(SideJobData); i++) {
        if(SideJobData[i][E_SIDE_JOB_LICENSE] == LICENSE_NONE) {
            format(licenseName, sizeof(licenseName), "Level %d", SideJobData[i][E_SIDE_JOB_MIN_LEVEL]);
        }
        else {
            License_GetName(SideJobData[i][E_SIDE_JOB_LICENSE], licenseName, sizeof(licenseName));
        }

        format(line, sizeof(line), "%s\t%s\t"C_GREEN"%m\n", SideJobData[i][E_SIDE_JOB_NAME], licenseName, SideJobData[i][E_SIDE_JOB_REWARD]);
        strcat(list, line, sizeof(list));
    }

    Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_TABLIST_HEADERS, "Bang viec lam", list, "Nhan", "Dong");
    return 1;
}

hook OnGameModeInit()
{
    new label[160];
    Interaction_BuildPointLabel(label, sizeof(label), "Cong viec", "Bang viec lam", "Co level, XP va giay phep", "nhan viec");

    SideJobBoardArea = CreateDynamicCircle(1479.3500, -1749.1000, 3.0, 0, 0);
    CreateDynamic3DTextLabel(
        label,
        COLOR_ORANGE,
        1479.3500,
        -1749.1000,
        14.5469,
        INTERACTION_LABEL_DISTANCE,
        .streamdistance = INTERACTION_LABEL_DISTANCE
    );
    return 1;
}

hook OnPlayerConnect(playerid)
{
    SideJobActive[playerid] = false;
    SideJobID[playerid] = SIDE_JOB_NONE;
    SideJobStep[playerid] = 0;
    SideJobVehicle[playerid] = INVALID_VEHICLE_ID;
    SideJobDeadline[playerid] = 0;
    return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    #pragma unused reason

    if(SideJob_IsActive(playerid)) {
        SideJob_End(playerid, false);
    }
    return 1;
}

hook OnPlayerEnterDynArea(playerid, areaid)
{
    if(areaid == SideJobBoardArea) {
        Interaction_ShowPrompt(playerid, INTERACTION_TYPE_JOB, SIDE_JOB_BOARD_INTERACTION_ID, "~y~~h~Bang viec lam", "nhan viec");
    }
    return 1;
}

hook OnPlayerLeaveDynArea(playerid, areaid)
{
    if(areaid == SideJobBoardArea) {
        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_JOB, SIDE_JOB_BOARD_INTERACTION_ID);
    }
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if(PRESSED(KEY_NO) && Interaction_IsActive(playerid, INTERACTION_TYPE_JOB, SIDE_JOB_BOARD_INTERACTION_ID)) {
        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_JOB, SIDE_JOB_BOARD_INTERACTION_ID);
        return 1;
    }

    if(PRESSED(KEY_YES) && Interaction_IsActive(playerid, INTERACTION_TYPE_JOB, SIDE_JOB_BOARD_INTERACTION_ID)) {
        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_JOB, SIDE_JOB_BOARD_INTERACTION_ID);
        SideJob_ShowBoard(playerid);
    }
    return 1;
}

hook OnPlayerEnterRaceCP(playerid)
{
    if(!SideJob_IsActive(playerid)) {
        return 1;
    }

    SideJobStep[playerid]++;

    if(SideJobStep[playerid] >= SideJob_GetRouteSize(SideJobID[playerid])) {
        return SideJob_End(playerid, true);
    }

    SideJob_SetCheckpoint(playerid);
    return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    #pragma unused oldstate

    if(SideJob_IsActive(playerid) && SideJobVehicle[playerid] != INVALID_VEHICLE_ID && newstate == PLAYER_STATE_ONFOOT) {
        SideJob_End(playerid, false, "Ban da roi khoi phuong tien.");
    }
    return 1;
}

static ptask SideJob_Watcher[1000](playerid)
{
    if(!SideJob_IsActive(playerid)) {
        return;
    }

    if(gettime() > SideJobDeadline[playerid]) {
        SideJob_End(playerid, false, "Qua thoi gian.");
        return;
    }

    if(SideJobVehicle[playerid] != INVALID_VEHICLE_ID && GetPlayerVehicleID(playerid) != SideJobVehicle[playerid]) {
        SideJob_End(playerid, false, "Khong con dung phuong tien cong viec.");
    }
}

CMD:sidejobs(playerid, params[])
{
    #pragma unused params
    SideJob_ShowBoard(playerid);
    return 1;
}
