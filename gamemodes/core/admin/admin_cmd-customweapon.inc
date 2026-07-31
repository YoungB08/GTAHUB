#include <YSI_Coding\y_hooks>

// =============================================================================
// PLAYER COMMANDS
// =============================================================================

CMD:myweapons(playerid, params[])
{
    #pragma unused params
    if (!Player_IsLoggedIn(playerid)) return SendErrorMsg(playerid, "Ban phai dang nhap truoc.");
    CustomWeapon_ShowInventory(playerid);
    return 1;
}

ALTCMD:customweapons(myweapons);

CMD:giveweapon(playerid, params[])
{
    new targetid;
    if(sscanf(params, "u", targetid)) {
        return SendSyntaxMsg(playerid, "/giveweapon (ten/id người nhận)");
    }

    if(!IsPlayerConnected(targetid) || targetid == playerid) {
        return SendErrorMsg(playerid, "Nguoi choi khong hop le.");
    }

    new Float:tx, Float:ty, Float:tz;
    GetPlayerPos(targetid, tx, ty, tz);
    if(!IsPlayerInRangeOfPoint(playerid, 3.0, tx, ty, tz)) {
        return SendErrorMsg(playerid, "Nguoi choi phai o gan ban (duoi 3m).");
    }

    CustomWeapon_ShowInventory(playerid);
    return 1;
}

CMD:inspectweapon(playerid, params[])
{
    new targetid;
    if(sscanf(params, "u", targetid)) {
        return SendSyntaxMsg(playerid, "/inspectweapon (ten/id người chơi)");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong hop le.");
    }

    new baseWep = GetPlayerWeapon(targetid);
    if(baseWep == 0) {
        return SendErrorMsg(playerid, "Nguoi choi khong cam khau sung nao tren tay.");
    }

    new slot = CustomWeapon_GetEquippedSlot(targetid, baseWep);
    if(slot == -1) {
        return SendServerMsgF(playerid, "Nguoi choi %p (%d) dang su dung sung thuong (Standard Weapon ID %d).", targetid, targetid, baseWep);
    }

    SendServerMsgF(playerid, "[KIEM TRA SUNG] %p (%d) dang su dung Custom Weapon!", targetid, targetid);
    return 1;
}

// =============================================================================
// ADMIN COMMANDS (LEVEL 5)
// =============================================================================

ACMD:[5]createcustomweapon(playerid, params[])
{
    new name[64], baseWep, Float:damage, tier[32], maxAmmo, price, desc[128];
    if(sscanf(params, "s[64]ifs[32]iis[128]", name, baseWep, damage, tier, maxAmmo, price, desc)) {
        return SendSyntaxMsg(playerid, "/createcustomweapon (ten) (base_wep_id) (damage) (tier: Common/Rare/Epic/Legendary/Mythic) (max_ammo) (price) (mo ta)");
    }

    if(baseWep < 1 || baseWep > 46) {
        return SendErrorMsg(playerid, "Base Weapon ID phai tu 1 den 46.");
    }

    CustomWeapon_CreateBlueprint(name, baseWep, damage, tier, maxAmmo, price, desc, playerid);
    return 1;
}

ACMD:[5]givecustomweapon(playerid, params[])
{
    new targetid, defId, ammo;
    if(sscanf(params, "uii", targetid, defId, ammo)) {
        return SendSyntaxMsg(playerid, "/givecustomweapon (ten/id) (custom_def_id) (so_dan)");
    }

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Nguoi choi khong online.");
    }

    if(!CustomWeapon_IsValidDef(defId)) {
        return SendErrorMsg(playerid, "Custom Weapon Blueprint ID khong ton tai.");
    }

    if(ammo <= 0) ammo = 100;

    if(!CustomWeapon_GiveNewToPlayer(targetid, defId, ammo)) {
        return SendErrorMsg(playerid, "Khong the trao sung custom (Kho sung target da day max 16 slot).");
    }

    SendServerMsgF(playerid, "[ADMIN] Da trao Custom Weapon ID %d cho %p (%d) voi %d vien dan.", defId, targetid, targetid, ammo);
    return 1;
}

ACMD:[1]listcustomweapons(playerid, params[])
{
    #pragma unused params
    SendServerMsg(playerid, "=== DANH SACH BLUEPRINTS CUSTOM WEAPONS ===");
    SendServerMsg(playerid, "ID 1: Desert Eagle Chrome (Epic) - Damage 55.0 HP");
    SendServerMsg(playerid, "ID 2: AK-47 Dragon (Legendary) - Damage 42.0 HP");
    SendServerMsg(playerid, "ID 3: M4A1 VIP (Mythic) - Damage 38.0 HP");
    SendServerMsg(playerid, "ID 4: Sniper Cobra (Legendary) - Damage 120.0 HP");
    SendServerMsg(playerid, "ID 5: Shotgun Tactical (Rare) - Damage 45.0 HP");
    SendServerMsg(playerid, "ID 6: MP5 SpecOps (Rare) - Damage 28.0 HP");
    SendServerMsg(playerid, "ID 7: Katana Shadow (Epic) - Damage 75.0 HP");
    SendServerMsg(playerid, "ID 8: Combat Shotgun Enforcer (Mythic) - Damage 50.0 HP");
    return 1;
}
