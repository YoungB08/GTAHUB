#include <YSI_Coding\y_hooks>

#define MAX_CUSTOM_WEAPON_DEFS          (64)
#define MAX_PLAYER_CUSTOM_WEAPONS       (16)

static
    Iterator:CustomWeaponDefIndex<MAX_CUSTOM_WEAPON_DEFS>,
    cwpDefName[MAX_CUSTOM_WEAPON_DEFS][64],
    cwpDefBaseWeaponId[MAX_CUSTOM_WEAPON_DEFS],
    Float:cwpDefDamage[MAX_CUSTOM_WEAPON_DEFS],
    cwpDefTier[MAX_CUSTOM_WEAPON_DEFS][32],
    cwpDefMaxAmmo[MAX_CUSTOM_WEAPON_DEFS],
    cwpDefPrice[MAX_CUSTOM_WEAPON_DEFS],
    cwpDefDesc[MAX_CUSTOM_WEAPON_DEFS][128],
    bool:cwpDefEnabled[MAX_CUSTOM_WEAPON_DEFS],

    playerCwpSqlId[MAX_PLAYERS][MAX_PLAYER_CUSTOM_WEAPONS],
    playerCwpDefId[MAX_PLAYERS][MAX_PLAYER_CUSTOM_WEAPONS],
    playerCwpAmmo[MAX_PLAYERS][MAX_PLAYER_CUSTOM_WEAPONS],
    Float:playerCwpDurability[MAX_PLAYERS][MAX_PLAYER_CUSTOM_WEAPONS],
    bool:playerCwpEquipped[MAX_PLAYERS][MAX_PLAYER_CUSTOM_WEAPONS],
    playerCwpCount[MAX_PLAYERS];

stock CustomWeapon_IsValidDef(defId) {
    return (defId > 0 && defId < MAX_CUSTOM_WEAPON_DEFS && Iter_Contains(CustomWeaponDefIndex, defId));
}

stock CustomWeapon_GetTierColor(const tier[], output[], len = 16) {
    output[0] = '\0';
    if(!strcmp(tier, "Mythic", true)) strcat(output, "{FF00FF}", len);
    else if(!strcmp(tier, "Legendary", true)) strcat(output, "{FFA500}", len);
    else if(!strcmp(tier, "Epic", true)) strcat(output, "{A335EE}", len);
    else if(!strcmp(tier, "Rare", true)) strcat(output, "{0070DD}", len);
    else strcat(output, "{1EFF00}", len);
    return 1;
}

stock CustomWeapon_LoadBlueprints() {
    inline OnBlueprintsLoaded() {
        new rows = cache_num_rows();
        if(!rows) return 1;

        new id, enabledInt;
        Iter_Clear(CustomWeaponDefIndex);

        for(new i = 0; i < rows; i++) {
            cache_get_value_name_int(i, "id", id);
            if(id <= 0 || id >= MAX_CUSTOM_WEAPON_DEFS) continue;

            Iter_Add(CustomWeaponDefIndex, id);
            cache_get_value_name(i, "name", cwpDefName[id], 64);
            cache_get_value_name_int(i, "base_weapon_id", cwpDefBaseWeaponId[id]);
            cache_get_value_name_float(i, "damage_override", cwpDefDamage[id]);
            cache_get_value_name(i, "tier", cwpDefTier[id], 32);
            cache_get_value_name_int(i, "max_ammo", cwpDefMaxAmmo[id]);
            cache_get_value_name_int(i, "price", cwpDefPrice[id]);
            cache_get_value_name(i, "description", cwpDefDesc[id], 128);
            cache_get_value_name_int(i, "enabled", enabledInt);
            cwpDefEnabled[id] = (enabledInt == 1);
        }
        printf("[Custom Weapon System] Loaded %d weapon blueprints from MySQL.", rows);
    }

    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnBlueprintsLoaded,
        "SELECT id, name, base_weapon_id, damage_override, tier, max_ammo, price, description, enabled FROM custom_weapons WHERE enabled = 1 ORDER BY id ASC");
    return 1;
}

stock CustomWeapon_LoadPlayerWeapons(playerid) {
    playerCwpCount[playerid] = 0;
    for(new i = 0; i < MAX_PLAYER_CUSTOM_WEAPONS; i++) {
        playerCwpSqlId[playerid][i] = 0;
        playerCwpDefId[playerid][i] = 0;
        playerCwpAmmo[playerid][i] = 0;
        playerCwpDurability[playerid][i] = 100.0;
        playerCwpEquipped[playerid][i] = false;
    }

    new uid = Account_GetUID(playerid);
    if(uid <= 0) return 0;

    new query[256];
    mysql_format(MySQL_GetHandle(), query, sizeof(query),
        "SELECT id, weapon_def_id, ammo, durability, is_equipped FROM player_custom_weapons WHERE u_id = %d LIMIT %d",
        uid, MAX_PLAYER_CUSTOM_WEAPONS);

    inline OnPlayerWeaponsLoaded() {
        new rows = cache_num_rows();
        if(!rows) return 1;

        new sqlId, defId, ammoVal, eqInt, Float:durVal;
        for(new i = 0; i < rows; i++) {
            cache_get_value_name_int(i, "id", sqlId);
            cache_get_value_name_int(i, "weapon_def_id", defId);
            cache_get_value_name_int(i, "ammo", ammoVal);
            cache_get_value_name_float(i, "durability", durVal);
            cache_get_value_name_int(i, "is_equipped", eqInt);

            if(CustomWeapon_IsValidDef(defId)) {
                playerCwpSqlId[playerid][i] = sqlId;
                playerCwpDefId[playerid][i] = defId;
                playerCwpAmmo[playerid][i] = ammoVal;
                playerCwpDurability[playerid][i] = durVal;
                playerCwpEquipped[playerid][i] = (eqInt == 1);
                playerCwpCount[playerid]++;

                if(playerCwpEquipped[playerid][i]) {
                    GivePlayerWeapon(playerid, cwpDefBaseWeaponId[defId], ammoVal);
                }
            }
        }
    }

    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnPlayerWeaponsLoaded, query);
    return 1;
}

stock CustomWeapon_GetEquippedSlot(playerid, baseWeaponId) {
    for(new i = 0; i < playerCwpCount[playerid]; i++) {
        if(playerCwpEquipped[playerid][i]) {
            new defId = playerCwpDefId[playerid][i];
            if(CustomWeapon_IsValidDef(defId) && cwpDefBaseWeaponId[defId] == baseWeaponId) {
                return i;
            }
        }
    }
    return -1;
}

stock CustomWeapon_Equip(playerid, slot) {
    if(slot < 0 || slot >= playerCwpCount[playerid]) return 0;

    new defId = playerCwpDefId[playerid][slot];
    if(!CustomWeapon_IsValidDef(defId)) return 0;

    new baseWep = cwpDefBaseWeaponId[defId];

    // Unequip any other weapon in the same base slot
    for(new i = 0; i < playerCwpCount[playerid]; i++) {
        if(i != slot && playerCwpEquipped[playerid][i]) {
            new otherDef = playerCwpDefId[playerid][i];
            if(CustomWeapon_IsValidDef(otherDef) && cwpDefBaseWeaponId[otherDef] == baseWep) {
                playerCwpEquipped[playerid][i] = false;
                new q[128];
                mysql_format(MySQL_GetHandle(), q, sizeof(q), "UPDATE player_custom_weapons SET is_equipped = 0 WHERE id = %d", playerCwpSqlId[playerid][i]);
                mysql_tquery(MySQL_GetHandle(), q);
            }
        }
    }

    playerCwpEquipped[playerid][slot] = true;
    GivePlayerWeapon(playerid, baseWep, playerCwpAmmo[playerid][slot]);

    new query[128];
    mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE player_custom_weapons SET is_equipped = 1 WHERE id = %d", playerCwpSqlId[playerid][slot]);
    mysql_tquery(MySQL_GetHandle(), query);

    new tColor[16];
    CustomWeapon_GetTierColor(cwpDefTier[defId], tColor);

    SendMsgF(playerid, COLOR_GREEN, "[SUNG CUSTOM] Da trang bi: %s%s"C_WHITE" (Sat thuong: %.1f HP).",
        tColor, cwpDefName[defId], cwpDefDamage[defId]);
    return 1;
}

stock CustomWeapon_Unequip(playerid, slot) {
    if(slot < 0 || slot >= playerCwpCount[playerid]) return 0;

    new defId = playerCwpDefId[playerid][slot];
    if(!CustomWeapon_IsValidDef(defId)) return 0;

    playerCwpEquipped[playerid][slot] = false;
    new baseWep = cwpDefBaseWeaponId[defId];

    // Save ammo before reset
    new currentAmmo = GetPlayerAmmo(playerid);
    if(currentAmmo > 0) playerCwpAmmo[playerid][slot] = currentAmmo;

    // Reset base weapon ammo
    SetPlayerAmmo(playerid, baseWep, 0);

    new query[128];
    mysql_format(MySQL_GetHandle(), query, sizeof(query),
        "UPDATE player_custom_weapons SET is_equipped = 0, ammo = %d WHERE id = %d",
        playerCwpAmmo[playerid][slot], playerCwpSqlId[playerid][slot]);
    mysql_tquery(MySQL_GetHandle(), query);

    SendMsgF(playerid, COLOR_YELLOW, "[SUNG CUSTOM] Da thao trang bi: %s.", cwpDefName[defId]);
    return 1;
}

stock CustomWeapon_GiveNewToPlayer(targetid, defId, ammo) {
    if(!CustomWeapon_IsValidDef(defId)) return 0;
    if(playerCwpCount[targetid] >= MAX_PLAYER_CUSTOM_WEAPONS) return 0;

    new uid = Account_GetUID(targetid);
    if(uid <= 0) return 0;

    new query[256];
    mysql_format(MySQL_GetHandle(), query, sizeof(query),
        "INSERT INTO player_custom_weapons (u_id, weapon_def_id, ammo, durability, is_equipped) VALUES (%d, %d, %d, 100.0, 0)",
        uid, defId, ammo);

    inline OnWeaponGiven() {
        new sqlId = cache_insert_id();
        if(sqlId > 0 && playerCwpCount[targetid] < MAX_PLAYER_CUSTOM_WEAPONS) {
            new slot = playerCwpCount[targetid];
            playerCwpSqlId[targetid][slot] = sqlId;
            playerCwpDefId[targetid][slot] = defId;
            playerCwpAmmo[targetid][slot] = ammo;
            playerCwpDurability[targetid][slot] = 100.0;
            playerCwpEquipped[targetid][slot] = false;
            playerCwpCount[targetid]++;

            new tColor[16];
            CustomWeapon_GetTierColor(cwpDefTier[defId], tColor);

            SendMsgF(targetid, COLOR_GREEN,
                "[SUNG CUSTOM] Ban nhan duoc khau sung Custom: %s%s "C_WHITE"(%d vien dan)!",
                tColor, cwpDefName[defId], ammo);
        }
    }

    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnWeaponGiven, query);
    return 1;
}

stock CustomWeapon_ShowInventory(playerid) {
    if(!playerCwpCount[playerid]) {
        return SendErrorMsg(playerid, "Ban khong co khau sung Custom nao trong kho.");
    }

    new list[2048] = "Sungs Custom\tPham Cap\tDan\tDo Ben\tTrang Thai\n";
    new item[180], tColor[16];

    for(new i = 0; i < playerCwpCount[playerid]; i++) {
        new defId = playerCwpDefId[playerid][i];
        if(!CustomWeapon_IsValidDef(defId)) continue;

        CustomWeapon_GetTierColor(cwpDefTier[defId], tColor);

        format(item, sizeof(item), "%s%s\t%s%s\t"C_WHITE"%d\t%.1f%%\t%s\n",
            tColor, cwpDefName[defId],
            tColor, cwpDefTier[defId],
            playerCwpAmmo[playerid][i],
            playerCwpDurability[playerid][i],
            playerCwpEquipped[playerid][i] ? ("{00FF00}[DA TRANG BI]") : ("{AFAFAF}[CHUA TRANG BI]"));
        strcat(list, item, sizeof(list));
    }

    inline OnInventorySelect(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused pid, dialogid, inputtext
        if(!response) return 1;

        if(listitem >= 0 && listitem < playerCwpCount[playerid]) {
            new slot = listitem;
            if(playerCwpEquipped[playerid][slot]) {
                CustomWeapon_Unequip(playerid, slot);
            } else {
                CustomWeapon_Equip(playerid, slot);
            }
        }
        return 1;
    }

    Dialog_ShowCallback(playerid, using inline OnInventorySelect,
        DIALOG_STYLE_TABLIST_HEADERS, "Kho Sungs Custom Cua Ban", list, "Trang bi/Thao", "Dong");
    return 1;
}

stock CustomWeapon_CreateBlueprint(const name[], baseWep, Float:damage, const tier[], maxAmmo, price, const desc[], creatorid) {
    new query[512];
    mysql_format(MySQL_GetHandle(), query, sizeof(query),
        "INSERT INTO custom_weapons (name, base_weapon_id, damage_override, tier, max_ammo, price, description) \
         VALUES ('%e', %d, %f, '%e', %d, %d, '%e')",
        name, baseWep, damage, tier, maxAmmo, price, desc);

    inline OnBlueprintCreated() {
        new defId = cache_insert_id();
        if(defId > 0 && defId < MAX_CUSTOM_WEAPON_DEFS) {
            Iter_Add(CustomWeaponDefIndex, defId);
            strcpy(cwpDefName[defId], name, 64);
            cwpDefBaseWeaponId[defId] = baseWep;
            cwpDefDamage[defId] = damage;
            strcpy(cwpDefTier[defId], tier, 32);
            cwpDefMaxAmmo[defId] = maxAmmo;
            cwpDefPrice[defId] = price;
            strcpy(cwpDefDesc[defId], desc, 128);
            cwpDefEnabled[defId] = true;

            SendServerMsgF(creatorid, "Da tao Blueprint Custom Weapon ID %d (%s) thanh cong!", defId, name);
        }
    }

    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnBlueprintCreated, query);
    return 1;
}

hook OnPlayerGiveDamage(playerid, damagedid, Float:amount, weaponid, bodypart) {
    #pragma unused bodypart
    if(IsPlayerConnected(damagedid) && weaponid > 0) {
        new slot = CustomWeapon_GetEquippedSlot(playerid, weaponid);
        if(slot != -1) {
            new defId = playerCwpDefId[playerid][slot];
            if(CustomWeapon_IsValidDef(defId)) {
                new Float:customDamage = cwpDefDamage[defId];

                // Reduce extra health from target according to custom damage override
                new Float:targetHp;
                GetPlayerHealth(damagedid, targetHp);
                new Float:newHp = targetHp - (customDamage - amount);
                if(newHp < 0.0) newHp = 0.0;
                SetPlayerHealth(damagedid, newHp);

                // Reduce durability by 0.05 per shot hit
                playerCwpDurability[playerid][slot] -= 0.05;
                if(playerCwpDurability[playerid][slot] < 0.0) playerCwpDurability[playerid][slot] = 0.0;

                new tColor[16];
                CustomWeapon_GetTierColor(cwpDefTier[defId], tColor);

                SendMsgF(playerid, COLOR_ORANGE,
                    "[%s%s"C_ORANGE"] Hit %p (%.1f HP)! Do ben: %.1f%%",
                    tColor, cwpDefName[defId],
                    damagedid, customDamage, playerCwpDurability[playerid][slot]);
            }
        }
    }
    return 1;
}

hook OnMySQLConnected() {
    mysql_query_file(MySQL_GetHandle(), "custom_weapons.sql", false);
    CustomWeapon_LoadBlueprints();
    return 1;
}

hook OnPlayerConnect(playerid) {
    playerCwpCount[playerid] = 0;
    return 1;
}

hook OnPlayerSpawn(playerid) {
    CustomWeapon_LoadPlayerWeapons(playerid);
    return 1;
}
