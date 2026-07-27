#include <vehicle_impl>
#include <vehicle_fuel> // he thong nhien lieu
#include <vehicle_keys> // he thong chia khoa / khoa cua / dong co
#include <vehicle_dealership> // Muc 28: Dealership
#include <vehicle_damage> // Muc 28: Damage System


#if SETUP_TABLE
    #include <vehicle_setup-table>
#endif


#include <YSI_Coding\y_hooks>
hook OnPlayerDisconnect(playerid, reason) {
    Vehicle_Destroy(playerid);
    return 1;
}

hook OnPlayerLogin(playerid) {
    Vehicle_Load(playerid);
    return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate) {
    if(newstate == PLAYER_STATE_DRIVER) {
        if(Vehicle_IsOwner(playerid)) {
            SendMsgF(playerid, COLOR_SAMPBLUE, "Chao mung tro lai xe ca nhan cua ban.");
            return 1;
        }

        foreach(new i : PersonalVehicle) {
            if(Vehicle_GetHandler(i) == GetPlayerVehicleID(playerid)) {
                SendMsgF(playerid, COLOR_SAMPBLUE, "Xe nay thuoc so huu cua "C_GREY"%s", Vehicle_GetOwnerName(i));
                break;
            }
        }
    }
    return 1;
}
