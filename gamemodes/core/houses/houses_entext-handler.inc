#include <YSI_Coding\y_hooks>

stock Player_GetInHouseID(playerid) {
    return GetPVarInt(playerid, "House_Id");
}

stock Player_InHouseEntrance(playerid, houseid) {
    return House_IsValid(houseid) && IsPlayerInDynamicCP(playerid, House_GetEntranceCPHandler(houseid));
}

stock Player_InHouseExit(playerid, houseid) {
    return House_IsValid(houseid) && IsPlayerInDynamicCP(playerid, House_GetExitCPHandler(houseid));
}

stock Player_InHouseWorld(playerid) {
    new
        arr[2];

    Streamer_GetArrayData(STREAMER_TYPE_CP, GetPVarInt(playerid, "House_CheckpointId"), E_STREAMER_EXTRA_ID, arr);
    return House_GetWorld(arr[1]);
}

stock Player_InHouseInterior(playerid) {
    new
        arr[2];

    Streamer_GetArrayData(STREAMER_TYPE_CP, GetPVarInt(playerid, "House_CheckpointId"), E_STREAMER_EXTRA_ID, arr);
    return House_GetInterior(arr[1]);
}

hook OnPlayerConnect(playerid) {
    SetPVarInt(playerid, "House_Id", -1);
    SetPVarInt(playerid, "House_CheckpointId", INVALID_STREAMER_ID);
    return 1;
}

hook OnPlayerEnterDynamicCP(playerid, checkpointid) {
    new
        arr[2];
    Streamer_GetArrayData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_EXTRA_ID, arr);

    if (arr[0] == HOUSES_STREAMER_IDENTIFIER) {
        new
            index = arr[1];
        if (checkpointid == House_GetEntranceCPHandler(index)) {
            new
                caption[32];

            format(caption, sizeof(caption), "~y~~h~Nha #%i", index);
            SetPVarInt(playerid, "House_Id", index);
            SetPVarInt(playerid, "House_CheckpointId", checkpointid);
            Interaction_ShowPrompt(playerid, INTERACTION_TYPE_HOUSE, index, caption, "vao");
        }
        else if (checkpointid == House_GetExitCPHandler(index)) {
            SetPVarInt(playerid, "House_Id", index);
            SetPVarInt(playerid, "House_CheckpointId", checkpointid);
            Interaction_ShowPrompt(playerid, INTERACTION_TYPE_HOUSE, index, "~y~~h~Loi ra", "ra ngoai");
        }
    }
    return 1;
}

hook OnPlayerLeaveDynamicCP(playerid, checkpointid) {
    new
        arr[2];
    Streamer_GetArrayData(STREAMER_TYPE_CP, checkpointid, E_STREAMER_EXTRA_ID, arr);

    if (arr[0] == HOUSES_STREAMER_IDENTIFIER) {
        new
            index = arr[1];

        if (checkpointid == House_GetEntranceCPHandler(index)) {
            Interaction_ClearPrompt(playerid, INTERACTION_TYPE_HOUSE, index);

            // Do not delete data just yet if player is inside the house
            // or entered the house.
            if(Player_IsInsideHouse(playerid)) {
                return 1;
            }

            SetPVarInt(playerid, "House_Id", -1);
            SetPVarInt(playerid, "House_CheckpointId", INVALID_STREAMER_ID);
        }
        else if (checkpointid == House_GetExitCPHandler(index)) {
            Interaction_ClearPrompt(playerid, INTERACTION_TYPE_HOUSE, index);
        }
    }
    return 1;
}
