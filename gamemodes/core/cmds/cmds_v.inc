#include <YSI_Coding\y_hooks>

CMD:v(playerid, params[])
{
	if(isnull(params))
		return SendSyntaxMsg(playerid, "/v [tuy chon: park | lock]");

	if(!strcmp(params, "park"))
	{
		if(!Vehicle_IsOwner(playerid))
			return SendErrorMsg(playerid, "Ban phai ngoi trong xe ca nhan cua minh.");

		Vehicle_Park(playerid);
		SendServerMsg(playerid, "Da dau xe, vi tri da duoc luu!");
	}
	if(!strcmp(params, "lock")) {
		new
			vehicleid = GetNearestVehicle(playerid);

		if(vehicleid == INVALID_VEHICLE_ID) {
			return SendErrorMsg(playerid, "Khong co xe nao gan day.");
		}

		if(!Vehicle_IsOwner(playerid, vehicleid))
			return SendErrorMsg(playerid, "Ban phai dung gan xe ca nhan cua minh.");

		new
    		engine, lights, alarm, doors, bonnet, boot, objective;
		foreach(new i : PersonalVehicle) {
			if(vehicleid != Vehicle_GetHandler(i)) {
				continue;
			}

			GetVehicleParamsEx(Vehicle_GetHandler(i), engine, lights, alarm, doors, bonnet, boot, objective);
			SetVehicleParamsEx(Vehicle_GetHandler(i), engine, lights, alarm, (doors) ? (0) : (1), bonnet, boot, objective);
			break;
		}
		SendMsgF(playerid, COLOR_SAMPBLUE, "Xe da %s", (doors) ? ("khoa") : ("mo khoa"));
	}

	return 1;
}
