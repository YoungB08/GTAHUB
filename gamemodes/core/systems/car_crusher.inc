#include <YSI_Coding\y_hooks>

static
    Float:MOVE_SPEED = 0.05,
    WAIT_DURATION = 1; // minutes

static
	ContainerDoors[2],
	ContainerCheckpoint,
	Text3D: StatusLabel,
	CrusherStage,
    CrusherCooldown[MAX_PLAYERS],
	CrusherVehicleID = INVALID_VEHICLE_ID;

static const WantedModels[] = {
    470,
    468,
    409,
    533,
    534,
    402,
    405,
    411,
    483,
    445,
    535,
    496,
    580,
    475,
    521,
    415,
    489,
    439,
    514,
    480,
    536,
    463,
    500,
    477,
    587,
    429,
    506,
    508,
    579,
    424
};

static _IsVehicleWanted(vehicleid) {
	new
        model = GetVehicleModel(vehicleid);
	for(new i; i < sizeof(WantedModels); ++i) {
	    if(WantedModels[i] == model) {
            return 1;
        }
	}
	return 0;
}

hook OnGameModeInit() {
	CreateDynamicObject(19321, 2098.69946, -1988.17786, 14.01000, 0.00000, 0.00000, 136.00000); // container object
	ContainerDoors[0] = CreateDynamicObject(3062, 2102.26611, -1986.72510, 13.94830, 0.00000, 0.00000, 0.00000);
	ContainerDoors[1] = CreateDynamicObject(3062, 2100.02051, -1984.51746, 13.94830, 0.00000, 0.00000, -260.5230);
	StatusLabel = CreateDynamic3DTextLabel("Trang thai may ep xe:\n"C_GREEN"Kha dung", COLOR_WHITE, 2102.6682, -1983.9194, 13.5463, 25.0, .testlos = 1);
	ContainerCheckpoint = CreateDynamicCP(2098.5313, -1988.2269, 12.7, 2.0);
	return 1;
}

hook OnPlayerConnect(playerid) {
    RemoveBuildingForPlayer(playerid, 3723, 2100.7031, -1989.3984, 16.8672, 0.25);
	RemoveBuildingForPlayer(playerid, 3722, 2100.7031, -1989.3984, 16.8672, 0.25);

    CrusherCooldown[playerid] = 0;
	return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate) {
    if(newstate == PLAYER_STATE_DRIVER) {
        new
            vehicleid = GetPlayerVehicleID(playerid);

        if(!_IsVehicleWanted(vehicleid)) {
            return 1;
        }

        MessageBox_Show(
            playerid,
            TYPE_MSGBOX,
            "~y~~h~Goi y!",
            "Ban co the ban xe nay tai may ep xe o ~y~Willowfield ~w~de doi lay ~g~tien~w~!"
        );
    }
    return 1;
}

hook OnPlayerEnterDynamicCP(playerid, checkpointid) {
	if(checkpointid == ContainerCheckpoint) {
        if((gettime() - CrusherCooldown[playerid]) < 60 * WAIT_DURATION) {
            return SendErrorMsg(playerid, "Vui long cho truoc khi dung lai.");
        }

        if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
            return SendErrorMsg(playerid, "Ban khong o trong xe.");
        }

        if(CrusherStage > 0) {
            return SendErrorMsg(playerid, "May ep xe dang duoc su dung.");
        }

        if(!_IsVehicleWanted(GetPlayerVehicleID(playerid))) {
            return SendErrorMsg(playerid, "May ep khong nhan mau xe nay!");
        }

        CrusherStage = 1;
        CrusherVehicleID = GetPlayerVehicleID(playerid);

        SetPlayerPos(playerid, 2102.6682, -1983.9194, 13.5463);
		SetPlayerFacingAngle(playerid, 136.0);

		CrusherCooldown[playerid] = gettime();

        new
            reward = random(5000) + 3000;

	    GameTextForPlayer(playerid, "~n~~n~~g~~h~~h~Cam on da giao dich!", 4000, 3);

	    GivePlayerMoney(playerid, reward);

        UpdateDynamic3DTextLabelText(StatusLabel, COLOR_WHITE, "Trang thai may ep xe:\n"C_RED"Khong kha dung");
	    MoveDynamicObject(ContainerDoors[0], 2102.26611 + 0.025, -1986.72510, 13.94830, MOVE_SPEED, 0.00000, 0.00000, 136.00000);
	    MoveDynamicObject(ContainerDoors[1], 2100.02051 + 0.025, -1984.51746, 13.94830, MOVE_SPEED, 0.00000, 0.00000, -43.5523);
  	}
	return 1;
}

hook OnDynamicObjectMoved(objectid) {
	if(objectid == ContainerDoors[0]) {
		switch(CrusherStage) {
		    case 1: {
		        CrusherStage = 2;

                defer OpenDoors();

                SetVehicleToRespawn(CrusherVehicleID);
		        CrusherVehicleID = INVALID_VEHICLE_ID;
			}
		    case 2: {
				CrusherStage = 0;
			    UpdateDynamic3DTextLabelText(StatusLabel, COLOR_WHITE, "Trang thai may ep xe:\n"C_GREEN"Kha dung");
			}
		}
	}
	return 1;
}

timer OpenDoors[1500]() {
    MoveDynamicObject(ContainerDoors[0], 2102.26611 - 0.025, -1986.72510, 13.94830, MOVE_SPEED, 0.00000, 0.00000, 0.00000);
    MoveDynamicObject(ContainerDoors[1], 2100.02051 - 0.025, -1984.51746, 13.94830, MOVE_SPEED, 0.00000, 0.00000, -260.5230);
	return 1;
}
