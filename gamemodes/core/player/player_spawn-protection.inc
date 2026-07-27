#include <YSI_Coding\y_hooks>

static
	BitArray:Player_SpawnProtection<MAX_PLAYERS>;

hook OnPlayerSpawn(playerid) {
    if (CallLocalFunction("Admin_IsPlayerSpectating", "i", playerid)) {
        return 1;
    }
	MessageBox_Show(playerid, TYPE_INFORMATION, "~y~Bao Ve Spawn", "Ban duoc bao ve spawn trong 5 giay.", 5000);
   	defer EndPlayerSpawnProtection(playerid);
   	Bit_Let(Player_SpawnProtection, playerid);
	return 1;
}

timer EndPlayerSpawnProtection[5000](playerid)
{
	MessageBox_Show(playerid, TYPE_INFORMATION, "~y~Bao Ve Spawn", "Bao ve spawn da ket thuc, chuc may man!", 5000);

	CallLocalFunction("OnPlayerSpawnProtectionEnd", "i", playerid);
	Bit_Vet(Player_SpawnProtection, playerid);
    return 1;
}

stock Player_HasSpawnProtect(playerid) {
	return Bit_Get(Player_SpawnProtection, playerid);
}
