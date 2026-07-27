static
	PlayerC4Object[MAX_PLAYERS],
	bool:PlayerC4Planted[MAX_PLAYERS];

CMD:plantc4(playerid, params[])
{
    if (Player_GetJob(playerid) != TYPE_TERRORIST) {
        return SendErrorMsg(playerid, "Chi terrorist moi dung duoc lenh nay.");
    }

	if (Player_GetC4(playerid) <= 0)
		return SendErrorMsg(playerid, "Ban khong co C4 de dat bom.");

	if (PlayerC4Planted[playerid])
		return SendErrorMsg(playerid, "Ban da dat C4 roi, dung /detonate.");
	new
		Float:x,
		Float:y,
		Float:z;

	GetPlayerPos(playerid, x, y, z);

	PlayerC4Object[playerid] = CreateDynamicObject(1252, x, y, z-1, 90, 0, 0);

	ApplyAnimation(playerid, "BOMBER", "BOM_Plant", 4.1, false, true, true, false, 5, SYNC_ALL);
	PlayerC4Planted[playerid] = true;
	return true;
}

CMD:detonate(playerid, params[])
{
	if (!PlayerC4Planted[playerid])
		return SendErrorMsg(playerid, "Ban chua dat C4 nao de kich no.");

	new
		Float:x,
		Float:y,
		Float:z;
	GetDynamicObjectPos(PlayerC4Object[playerid], x, y, z);

	// Create UI
	CreateExplosion(x, y, z, 0, 5.0);
	Player_GiveWantedLevel(playerid, 4);
	// Call Function
	CallLocalFunction("OnExplosiveDetonate", "ifff", playerid, x, y, z);
	// Rese Data
	DestroyDynamicObject(PlayerC4Object[playerid]);
	PlayerC4Object[playerid] = INVALID_OBJECT_ID;
	PlayerC4Planted[playerid] = false;
	return true;
}
