#include <YSI_Coding\y_hooks>
#include <YSI_Data\y_bit>

static 
    WeaponPrice[MAX_PLAYERS],
    WeaponID[MAX_PLAYERS],
    WeaponAmmo[MAX_PLAYERS],
    WeaponSeller[MAX_PLAYERS],
    BitArray:HasBeenOffered<MAX_PLAYERS>,
    SellWeaponCooldown[MAX_PLAYERS],
    SellArmourcooldown[MAX_PLAYERS];
	

hook OnPlayerConnect(playerid, reason) {
	SellWeaponCooldown[playerid] = 
	SellArmourcooldown[playerid] = 
    WeaponID[playerid] = 
    WeaponPrice[playerid] = 
    WeaponAmmo[playerid] = -1;
    WeaponSeller[playerid] = INVALID_PLAYER_ID;
	Bit_Set(HasBeenOffered, playerid, false);
    return 1;
}

CMD:sellgun(playerid, params[])
{
	
	new 
		weaponid, 
        targetid, 
        weaponprice, 
        weaponammo;

	if (Player_GetJob(playerid) != TYPE_WEAPONDEALER || Player_GetClass(playerid) != TEAM_CIVILIAN) {
        return SendErrorMsg(playerid, "Ban khong duoc dung lenh nay neu khong phai weapon dealer.");
    } 
	if (gettime() - SellWeaponCooldown[playerid] < 40) {
		return SendErrorMsgF(playerid, "Ban vua ban vu khi, vui long cho "C_GREY"%i giay ", 40 - (gettime() - SellWeaponCooldown[playerid]));
    }

	if (sscanf(params, "riii", targetid, weaponid, weaponprice, weaponammo)) {
		return SendSyntaxMsg(playerid, "/sellgun <playerid> <weaponid> <gia> <ammo (toi da 1,000)>");
    }

	if (!IsPlayerConnected(targetid)) {
		return SendErrorMsg(playerid, "Nguoi choi ban muon ban vu khi khong online.");
    }

	if (!Player_IsSpawned(targetid)) {
		return SendErrorMsg(playerid, "Nguoi choi ban muon ban vu khi chua spawn.");
    }

	if (!Player_IsNearPlayer(playerid, targetid, 4.0)) {
		return SendErrorMsg(playerid, "Nguoi choi ban muon ban vu khi khong o gan ban.");
    }

	if (targetid == playerid) {    
		return SendErrorMsg(playerid, "Ban khong the ban sung cho chinh minh.");
    }

	if (weaponprice < 1000 || weaponprice > 8000) {
		return SendErrorMsg(playerid, "Gia vu khi phai tu 1,000 den 8,000.");
    }

	if (!(22 <= weaponid <= 34)) {
		return SendErrorMsg(playerid, "Weapon id khong hop le. Weapon id chi tu 22 - 34.");
    }

	if (!(0 <= weaponammo <= 1000)) {
		return SendErrorMsg(playerid, "Ammo khong hop le. Ammo chi tu 0 - 1000.");
    }

	Bit_Set(HasBeenOffered, targetid, true);
	WeaponID[targetid] = weaponid;
	WeaponPrice[targetid] = weaponprice;
	WeaponAmmo[targetid] = weaponammo;
 	WeaponSeller[targetid] = playerid;

	SendMsgF(playerid, COLOR_ORANGE, "** WEAPON **: "C_WHITE" Ban da rao ban cho %p (%i) mot %W voi gia "C_GREEN"$%i"C_WHITE".", targetid, targetid, weaponid, weaponprice);
	SendMsgF(targetid, COLOR_ORANGE, "** WEAPON **: "C_WHITE" Weapon dealer "C_GREY"%p (%i) "C_WHITE"rao ban cho ban mot %W voi gia "C_GREEN"$%i"C_WHITE", dung /acceptgun de chap nhan.", playerid, playerid, weaponid, weaponprice);
	
	SellWeaponCooldown[playerid] = gettime();
	return true;
}

CMD:acceptgun(playerid, params[])
{
	if (!Bit_Get(HasBeenOffered, playerid)) {
		return SendErrorMsg(playerid, "Chua ai rao ban sung cho ban.");
    }

	if (GetPlayerMoney(playerid) < WeaponPrice[playerid]) {
		return SendErrorMsgF(playerid, "Ban khong co "C_GREEN"$%d "C_WHITE"de mua vu khi.", WeaponPrice[playerid]);
    }

	GivePlayerMoney(playerid, -WeaponPrice[playerid]);
	GivePlayerMoney(WeaponSeller[playerid], WeaponPrice[playerid]);

	GivePlayerWeapon(playerid, WeaponID[playerid], WeaponAmmo[playerid]);

	SendMsgF(WeaponSeller[playerid], COLOR_ORANGE, "** WEAPON **: "C_WHITE"%p (%i) da chap nhan giao dich, ban nhan "C_GREEN"$%i"C_WHITE" khi ban sung.", playerid, playerid, WeaponPrice[playerid]);
	SendMsgF(playerid, COLOR_ORANGE, "** WEAPON **: "C_WHITE"Ban da mua thanh cong mot %W voi gia "C_GREEN"$%i"C_WHITE" tu "C_GREY"%p(%i)", WeaponID[playerid], WeaponPrice[playerid], 
	WeaponSeller[playerid], WeaponSeller[playerid]);

	Bit_Set(HasBeenOffered, playerid, false), 
	WeaponID[playerid] = -1, 
	WeaponPrice[playerid] = -1, 
	WeaponAmmo[playerid] = -1, 
	WeaponSeller[playerid] = INVALID_PLAYER_ID;
	return true;
}

CMD:sellarmour(playerid, params[])
{
	new 
		targetid, armourprice, armouramount; 

	if (Player_GetJob(playerid) != TYPE_WEAPONDEALER || Player_GetClass(playerid) != TEAM_CIVILIAN) {
		return SendErrorMsg(playerid, "Ban khong duoc dung lenh nay neu khong phai weapon dealer.");
    }

	if (gettime() - SellArmourcooldown[playerid] < 40)  {
		return SendErrorMsgF(playerid, "Ban vua ban giap, vui long cho %i giay ", 40 - (gettime() - SellArmourcooldown[playerid]));
    }

	if (sscanf(params, "rii", targetid, armourprice, armouramount)) {
		return SendSyntaxMsg(playerid, "/sellarmour <playerid> <gia> <so luong>");
    }

	if (!IsPlayerConnected(targetid)) {
		return SendErrorMsg(playerid, "Nguoi choi ban muon ban giap khong online.");
    }

	if (!Player_IsSpawned(targetid)) {
		return SendErrorMsg(playerid, "Nguoi choi ban muon ban giap chua spawn.");
    }

	if (!Player_IsNearPlayer(playerid, targetid, 4.0)) {
		return SendErrorMsg(playerid, "Nguoi choi ban muon ban giap khong o gan ban.");
    }

	if (targetid == playerid)  {
		return SendErrorMsg(playerid, "Ban khong the ban giap cho chinh minh.");
    }

	if (!(0 <= armouramount <= 100)) {
		return SendErrorMsg(playerid, "Luong giap khong hop le. Giap chi tu 1 - 100."); 
    }

    new 
        Float:armour = Player_GetArmour(targetid);
	if (armour >= 80.0) {
		return SendErrorMsgF(playerid, "Nguoi choi ban muon ban giap da co du giap (%.1f).", armour);
    }

	Bit_Set(HasBeenOffered, targetid, true);
	WeaponPrice[targetid] = armourprice, 
	WeaponAmmo[targetid] = armouramount, 
	WeaponSeller[targetid] = playerid;

	SendMsgF(playerid, COLOR_ORANGE, "** WEAPON **: "C_WHITE" Ban da rao ban cho "C_GREY"%p (%i) "C_WHITE"giap(%i) voi gia "C_GREEN"$%i"C_WHITE".", targetid, targetid, armouramount, armourprice);
	SendMsgF(targetid, COLOR_ORANGE, "** WEAPON **: "C_WHITE" Weapon dealer "C_GREY"%p (%i) "C_WHITE"rao ban cho ban giap(%i) voi gia "C_GREEN"$%i"C_WHITE", dung /acceptarmour de chap nhan.", playerid, playerid, armouramount, armourprice);
	
	SellArmourcooldown[playerid] = gettime();
	return true;
}

CMD:acceptarmour(playerid, params[])
{
	if (!Bit_Get(HasBeenOffered, playerid)) {
		return SendErrorMsg(playerid, "Chua ai rao ban giap cho ban.");
    }

	if (GetPlayerMoney(playerid) < WeaponPrice[playerid]) {
		return SendErrorMsgF(playerid, "Ban khong co "C_GREEN"$%d "C_WHITE"de mua giap.", WeaponPrice[playerid]);
    }

	GivePlayerMoney(playerid, -WeaponPrice[playerid] );
	GivePlayerMoney(WeaponSeller[playerid], WeaponPrice[playerid]);

	new 
		Float:playerCurrentArmour;

	GetPlayerArmour(playerid, playerCurrentArmour);

	SetPlayerArmour(playerid, playerCurrentArmour + WeaponAmmo[playerid]);
	

	SendMsgF(WeaponSeller[playerid], COLOR_ORANGE, "** WEAPON **: "C_WHITE"%p (%i) da chap nhan giao dich, ban nhan "C_GREEN"$%i"C_WHITE" khi ban giap.", playerid, playerid, WeaponPrice[playerid]);
	SendMsgF(playerid, COLOR_ORANGE, "** WEAPON **: "C_WHITE"Ban da mua thanh cong giap(%i) voi gia "C_GREEN"$%i"C_WHITE" tu "C_GREY"%p(%i)", WeaponAmmo[playerid], WeaponPrice[playerid], 
	WeaponSeller[playerid], WeaponSeller[playerid]);

	Bit_Set(HasBeenOffered, playerid, false), 
	WeaponPrice[playerid] = -1, 
	WeaponAmmo[playerid] = -1, 
	WeaponSeller[playerid] = INVALID_PLAYER_ID;
	return true;
}
