// Definitions
#define MAX_ENTRANCES               (128 * 2)
#define MAX_STORE_NAME              (30)

#define ENTRANCE_UP                 (true)
#define ENTRANCE_DOWN               (false)
#define ENTRANCE_AREA_RADIUS        (1.5)
#define ENTRANCE_EXIT_AREA_RADIUS   (3.0)
#define ENTRANCE_TELEPORT_OFFSET    (2.0)
#define ENTRANCE_DYNAMIC_WORLD_BASE (1000)
#define ENTRANCE_REUSE_BLOCK_MS     (3500)

// Declarations
static
    // Animation
    bool:EnExObjectState[MAX_ENTRANCES],
    EnExObject[MAX_ENTRANCES],
    Text3D:EnExLabel[MAX_ENTRANCES],
    EnExMapIcon[MAX_ENTRANCES],
    // Area
    EnExArea[MAX_ENTRANCES],
    // Entrance
    entranceStoreName[MAX_ENTRANCES][MAX_STORE_NAME],
    Float: entranceX[MAX_ENTRANCES],
    Float: entranceY[MAX_ENTRANCES],
    Float: entranceZ[MAX_ENTRANCES],
    Float: entranceAng[MAX_ENTRANCES],
    entranceVirtualID[MAX_ENTRANCES],
    entranceInteriorID[MAX_ENTRANCES],
    bool:entranceIsExitPoint[MAX_ENTRANCES],
    // Exit
    Float: exitX[MAX_ENTRANCES],
    Float: exitY[MAX_ENTRANCES],
    Float: exitZ[MAX_ENTRANCES],
    Float: exitAng[MAX_ENTRANCES],
    exitVirtualID[MAX_ENTRANCES],
    exitInteriorID[MAX_ENTRANCES],
    entranceLinkedID[MAX_ENTRANCES],
    entranceDatabaseID[MAX_ENTRANCES],
    entranceMapIcon[MAX_ENTRANCES],
    // Other
    Iterator: entranceID<MAX_ENTRANCES>,
    playerEnExCoolDown[MAX_PLAYERS],
    getShopName[MAX_PLAYERS][MAX_STORE_NAME],
    entranceFreezePlayer[MAX_ENTRANCES],
    PlayerInteriorExitID[MAX_PLAYERS],
    PlayerBlockedEntranceID[MAX_PLAYERS],
    PlayerBlockedEntranceUntil[MAX_PLAYERS],
    PlayerAdminEntranceListID[MAX_PLAYERS][MAX_ENTRANCES],
    PlayerAdminEntranceListCount[MAX_PLAYERS];

stock Entrance_UpdateLabel(id) {
    if(!Iter_Contains(entranceID, id)) {
        return 0;
    }

    new
        label[180],
        labelName[MAX_STORE_NAME],
        pointType[16],
        status[24],
        action[16],
        labelColor;

    if(entranceIsExitPoint[id]) {
        strcpy(pointType, "LOI RA", sizeof(pointType));
        strcpy(status, "San sang", sizeof(status));
        strcpy(action, "ra ngoai", sizeof(action));
        labelColor = COLOR_GREEN;
    }
    else {
        strcpy(pointType, "LOI VAO", sizeof(pointType));
        strcpy(status, "Dang mo", sizeof(status));
        strcpy(action, "vao", sizeof(action));
        labelColor = COLOR_ORANGE;
    }

    if(isnull(entranceStoreName[id])) {
        strcpy(labelName, pointType, sizeof(labelName));
    }
    else {
        strcpy(labelName, entranceStoreName[id], sizeof(labelName));
    }

    Interaction_BuildPointLabel(label, sizeof(label), pointType, labelName, status, action);

    if(EnExLabel[id] == INVALID_3DTEXT_ID) {
        EnExLabel[id] = CreateDynamic3DTextLabel(
            label,
            labelColor,
            entranceX[id],
            entranceY[id],
            entranceZ[id] + 1.2,
            INTERACTION_LABEL_DISTANCE,
            .worldid = entranceVirtualID[id],
            .interiorid = entranceInteriorID[id],
            .streamdistance = INTERACTION_LABEL_DISTANCE
        );
    }
    else {
        UpdateDynamic3DTextLabelText(EnExLabel[id], labelColor, label);
    }
    return 1;
}

// Attach a map icon to assigned entranceid.
stock Entrance_SetIcon(id, iconid) {

    if(!Iter_Contains(entranceID, id)) {
        return 0;
    }

    entranceMapIcon[id] = iconid;

    if(entranceIsExitPoint[id] || iconid <= 0) {
        return 1;
    }

    if(EnExMapIcon[id] != INVALID_STREAMER_ID) {
        DestroyDynamicMapIcon(EnExMapIcon[id]);
    }

    EnExMapIcon[id] = CreateDynamicMapIcon(entranceX[id], entranceY[id], entranceZ[id], iconid, -1, .style = MAPICON_GLOBAL);
    return 1;
}

// Attach entrance name to assigned entranceid.
stock Entrance_SetName(id, const name[]) {

    if(!Iter_Contains(entranceID, id)){
        return 0;
    }

    if(isnull(name)) {
        printf("ERROR: Entrance Name is null.");
        return 0;
    }

    strcpy(entranceStoreName[id], name, MAX_STORE_NAME);
    Entrance_UpdateLabel(id);
    return 1;
}

// Returns the entrance name that player is inside.
stock Entrance_PlayerIn(playerid) {
    return (!isnull(getShopName[playerid])) ? (getShopName[playerid]) : ("None");
}

stock Entrance_EnableFreeze(id, bool:status) {
    if(!Iter_Contains(entranceID, id)) {
        return 0;
    }

    entranceFreezePlayer[id] = status;
    return 1;
}
stock Entrance_FreeWorld() {
    return Iter_Free(entranceID);
}

//Create an entrance with exit on specificied coordinates.
stock Entrance_Create(
    Float:x,
    Float:y,
    Float:z,
    Float:ang,
    world,
    interior,
    Float:eX,
    Float:eY,
    Float:eZ,
    Float:eAng,
    eWorld = 0,
    eInterior = 0,
    Float:streamdistance = 200.0,
    Float:drawdistance = 200.0,
    bool:isExitPoint = false) {
    new
        idx = Iter_Alloc(entranceID),
        arr[2],
        Float:areaRadius = ENTRANCE_AREA_RADIUS;

    if(idx == INTERACTION_INVALID_ID) {
        return INTERACTION_INVALID_ID;
    }

    EnExLabel[idx] = INVALID_3DTEXT_ID;
    EnExMapIcon[idx] = INVALID_STREAMER_ID;

    arr[0] = ENEX_STREAMER_IDENTIFIER;
    arr[1] = idx;

    // Object
    EnExObject[idx] = CreateDynamicObject(19198, x, y, z, 0.0, 0.0, 0.0, world, interior, -1,streamdistance, drawdistance);

    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, EnExObject[idx], E_STREAMER_EXTRA_ID, arr, sizeof(arr));

    MoveDynamicObject(EnExObject[idx], x, y, (z + 1.0), 1.0);

    if(isExitPoint) {
        areaRadius = ENTRANCE_EXIT_AREA_RADIUS;
    }

    // Area
    EnExArea[idx] = CreateDynamicCircle(x, y, areaRadius, world, interior);

    Streamer_SetArrayData(STREAMER_TYPE_AREA, EnExArea[idx], E_STREAMER_EXTRA_ID, arr, sizeof(arr));

    // Setting data
    EnExObjectState[idx]            = ENTRANCE_UP;

    entranceX[idx]                  = x;
    entranceY[idx]                  = y;
    entranceZ[idx]                  = z;
    entranceAng[idx]                = ang;
    entranceVirtualID[idx]          = world;
    entranceInteriorID[idx]         = interior;
    entranceIsExitPoint[idx]        = isExitPoint;

    exitX[idx]                      = eX;
    exitY[idx]                      = eY;
    exitZ[idx]                      = eZ;
    exitAng[idx]                    = eAng;
    exitVirtualID[idx]              = eWorld;
    exitInteriorID[idx]             = eInterior;
    entranceLinkedID[idx]           = INTERACTION_INVALID_ID;
    entranceDatabaseID[idx]         = 0;
    entranceMapIcon[idx]            = 0;
    Entrance_UpdateLabel(idx);
    return idx;
}

stock Entrance_LinkPair(entranceid, exitid) {
    if(!Iter_Contains(entranceID, entranceid) || !Iter_Contains(entranceID, exitid)) {
        return 0;
    }

    entranceLinkedID[entranceid] = exitid;
    entranceLinkedID[exitid] = entranceid;
    return 1;
}

stock Entrance_GetLinkedID(id) {
    if(!Iter_Contains(entranceID, id)) {
        return INTERACTION_INVALID_ID;
    }
    return entranceLinkedID[id];
}

stock Entrance_GetEntryID(id) {
    if(!Iter_Contains(entranceID, id)) {
        return INTERACTION_INVALID_ID;
    }

    if(entranceIsExitPoint[id] && Iter_Contains(entranceID, entranceLinkedID[id])) {
        return entranceLinkedID[id];
    }
    return id;
}

stock Entrance_GetExitID(id) {
    if(!Iter_Contains(entranceID, id)) {
        return INTERACTION_INVALID_ID;
    }

    if(entranceIsExitPoint[id]) {
        return id;
    }

    if(Iter_Contains(entranceID, entranceLinkedID[id])) {
        return entranceLinkedID[id];
    }
    return INTERACTION_INVALID_ID;
}

stock Entrance_AllocateVirtualWorld() {
    new world = ENTRANCE_DYNAMIC_WORLD_BASE;

    foreach(new idx : entranceID) {
        if(entranceVirtualID[idx] >= world) {
            world = entranceVirtualID[idx] + 1;
        }

        if(exitVirtualID[idx] >= world) {
            world = exitVirtualID[idx] + 1;
        }
    }
    return world;
}

stock Entrance_CreatePair(
    const name[],
    iconid,
    bool:freeze,
    Float:entryX,
    Float:entryY,
    Float:entryZ,
    Float:entryA,
    entryWorld,
    entryInterior,
    Float:targetX,
    Float:targetY,
    Float:targetZ,
    Float:targetA,
    targetWorld,
    targetInterior,
    databaseid = 0
) {
    new entryid = Entrance_Create(
        entryX,
        entryY,
        entryZ,
        entryA,
        entryWorld,
        entryInterior,
        targetX,
        targetY,
        targetZ,
        targetA,
        targetWorld,
        targetInterior
    );

    if(!Iter_Contains(entranceID, entryid)) {
        return INTERACTION_INVALID_ID;
    }

    new exitid = Entrance_Create(
        targetX,
        targetY,
        targetZ,
        targetA,
        targetWorld,
        targetInterior,
        entryX,
        entryY,
        entryZ,
        entryA,
        entryWorld,
        entryInterior,
        .isExitPoint = true
    );

    if(!Iter_Contains(entranceID, exitid)) {
        return entryid;
    }

    Entrance_LinkPair(entryid, exitid);
    Entrance_SetName(entryid, name);
    Entrance_SetName(exitid, name);
    Entrance_EnableFreeze(entryid, freeze);
    Entrance_EnableFreeze(exitid, freeze);
    Entrance_SetIcon(entryid, iconid);

    entranceDatabaseID[entryid] = databaseid;
    entranceDatabaseID[exitid] = databaseid;
    entranceMapIcon[entryid] = iconid;
    entranceMapIcon[exitid] = iconid;
    return entryid;
}

stock Entrance_SavePairToDatabase(id) {
    new
        entryid = Entrance_GetEntryID(id),
        exitid = Entrance_GetExitID(id);

    if(!Iter_Contains(entranceID, entryid) || !Iter_Contains(entranceID, exitid)) {
        return 0;
    }

    new
        query[1024],
        dbid = entranceDatabaseID[entryid];

    if(dbid > 0) {
        mysql_format(
            MySQL_GetHandle(),
            query,
            sizeof(query),
            "\
                UPDATE entrances SET \
                    name = '%e', map_icon = %i, freeze_player = %i, \
                    entry_x = %f, entry_y = %f, entry_z = %f, entry_a = %f, entry_world = %i, entry_interior = %i, \
                    exit_x = %f, exit_y = %f, exit_z = %f, exit_a = %f, exit_world = %i, exit_interior = %i, enabled = 1 \
                WHERE entrance_id = %i",
            entranceStoreName[entryid],
            entranceMapIcon[entryid],
            entranceFreezePlayer[entryid] ? 1 : 0,
            entranceX[entryid],
            entranceY[entryid],
            entranceZ[entryid],
            entranceAng[entryid],
            entranceVirtualID[entryid],
            entranceInteriorID[entryid],
            entranceX[exitid],
            entranceY[exitid],
            entranceZ[exitid],
            entranceAng[exitid],
            entranceVirtualID[exitid],
            entranceInteriorID[exitid],
            dbid
        );
        mysql_tquery(MySQL_GetHandle(), query);
        return 1;
    }

    inline const OnEntranceInserted()
    {
        new insertedid = cache_insert_id();

        if(insertedid > 0) {
            entranceDatabaseID[entryid] = insertedid;
            entranceDatabaseID[exitid] = insertedid;
        }
    }

    mysql_format(
        MySQL_GetHandle(),
        query,
        sizeof(query),
        "\
            INSERT INTO entrances \
                (name, map_icon, freeze_player, entry_x, entry_y, entry_z, entry_a, entry_world, entry_interior, \
                exit_x, exit_y, exit_z, exit_a, exit_world, exit_interior) \
            VALUES \
                ('%e', %i, %i, %f, %f, %f, %f, %i, %i, %f, %f, %f, %f, %i, %i)",
        entranceStoreName[entryid],
        entranceMapIcon[entryid],
        entranceFreezePlayer[entryid] ? 1 : 0,
        entranceX[entryid],
        entranceY[entryid],
        entranceZ[entryid],
        entranceAng[entryid],
        entranceVirtualID[entryid],
        entranceInteriorID[entryid],
        entranceX[exitid],
        entranceY[exitid],
        entranceZ[exitid],
        entranceAng[exitid],
        entranceVirtualID[exitid],
        entranceInteriorID[exitid]
    );
    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnEntranceInserted, query);
    return 1;
}

static Entrance_DestroyPoint(id) {
    if(!Iter_Contains(entranceID, id)) {
        return 0;
    }

    if(EnExObject[id] != INVALID_STREAMER_ID) {
        DestroyDynamicObject(EnExObject[id]);
    }

    if(EnExArea[id] != INVALID_STREAMER_ID) {
        DestroyDynamicArea(EnExArea[id]);
    }

    if(EnExLabel[id] != INVALID_3DTEXT_ID) {
        DestroyDynamic3DTextLabel(EnExLabel[id]);
    }

    if(EnExMapIcon[id] != INVALID_STREAMER_ID) {
        DestroyDynamicMapIcon(EnExMapIcon[id]);
    }

    EnExObject[id] = INVALID_STREAMER_ID;
    EnExArea[id] = INVALID_STREAMER_ID;
    EnExLabel[id] = INVALID_3DTEXT_ID;
    EnExMapIcon[id] = INVALID_STREAMER_ID;
    entranceStoreName[id][0] = EOS;
    entranceLinkedID[id] = INTERACTION_INVALID_ID;
    entranceDatabaseID[id] = 0;
    entranceMapIcon[id] = 0;
    entranceFreezePlayer[id] = false;
    Iter_Remove(entranceID, id);
    return 1;
}

static Entrance_RebuildPoint(id) {
    if(!Iter_Contains(entranceID, id)) {
        return 0;
    }

    new
        arr[2],
        Float:areaRadius = entranceIsExitPoint[id] ? ENTRANCE_EXIT_AREA_RADIUS : ENTRANCE_AREA_RADIUS;

    if(EnExObject[id] != INVALID_STREAMER_ID) {
        DestroyDynamicObject(EnExObject[id]);
    }

    if(EnExArea[id] != INVALID_STREAMER_ID) {
        DestroyDynamicArea(EnExArea[id]);
    }

    if(EnExLabel[id] != INVALID_3DTEXT_ID) {
        DestroyDynamic3DTextLabel(EnExLabel[id]);
    }

    if(EnExMapIcon[id] != INVALID_STREAMER_ID) {
        DestroyDynamicMapIcon(EnExMapIcon[id]);
    }

    arr[0] = ENEX_STREAMER_IDENTIFIER;
    arr[1] = id;
    EnExLabel[id] = INVALID_3DTEXT_ID;
    EnExMapIcon[id] = INVALID_STREAMER_ID;
    EnExObject[id] = CreateDynamicObject(19198, entranceX[id], entranceY[id], entranceZ[id], 0.0, 0.0, 0.0, entranceVirtualID[id], entranceInteriorID[id], -1, 200.0, 200.0);
    Streamer_SetArrayData(STREAMER_TYPE_OBJECT, EnExObject[id], E_STREAMER_EXTRA_ID, arr, sizeof(arr));
    MoveDynamicObject(EnExObject[id], entranceX[id], entranceY[id], entranceZ[id] + 1.0, 1.0);

    EnExArea[id] = CreateDynamicCircle(entranceX[id], entranceY[id], areaRadius, entranceVirtualID[id], entranceInteriorID[id]);
    Streamer_SetArrayData(STREAMER_TYPE_AREA, EnExArea[id], E_STREAMER_EXTRA_ID, arr, sizeof(arr));
    Entrance_UpdateLabel(id);

    if(!entranceIsExitPoint[id] && entranceMapIcon[id] > 0) {
        Entrance_SetIcon(id, entranceMapIcon[id]);
    }
    return 1;
}

stock Entrance_DeletePair(id) {
    new
        entryid = Entrance_GetEntryID(id),
        exitid = Entrance_GetExitID(id),
        dbid;

    if(!Iter_Contains(entranceID, entryid)) {
        return 0;
    }

    dbid = entranceDatabaseID[entryid];

    if(dbid > 0) {
        new query[96];
        mysql_format(MySQL_GetHandle(), query, sizeof(query), "DELETE FROM entrances WHERE entrance_id = %i", dbid);
        mysql_tquery(MySQL_GetHandle(), query);
    }

    if(Iter_Contains(entranceID, exitid)) {
        Entrance_DestroyPoint(exitid);
    }

    Entrance_DestroyPoint(entryid);
    return 1;
}

stock Entrance_SetPairName(id, const name[]) {
    new
        entryid = Entrance_GetEntryID(id),
        exitid = Entrance_GetExitID(id);

    if(!Iter_Contains(entranceID, entryid)) {
        return 0;
    }

    Entrance_SetName(entryid, name);

    if(Iter_Contains(entranceID, exitid)) {
        Entrance_SetName(exitid, name);
    }

    Entrance_SavePairToDatabase(entryid);
    return 1;
}

stock Entrance_SetEntryPosition(id, Float:x, Float:y, Float:z, Float:a, world, interior) {
    new
        entryid = Entrance_GetEntryID(id),
        exitid = Entrance_GetExitID(id);

    if(!Iter_Contains(entranceID, entryid)) {
        return 0;
    }

    entranceX[entryid] = x;
    entranceY[entryid] = y;
    entranceZ[entryid] = z;
    entranceAng[entryid] = a;
    entranceVirtualID[entryid] = world;
    entranceInteriorID[entryid] = interior;

    if(Iter_Contains(entranceID, exitid)) {
        exitX[exitid] = x;
        exitY[exitid] = y;
        exitZ[exitid] = z;
        exitAng[exitid] = a;
        exitVirtualID[exitid] = world;
        exitInteriorID[exitid] = interior;
    }

    Entrance_RebuildPoint(entryid);
    Entrance_SavePairToDatabase(entryid);
    return 1;
}

stock Entrance_SetExitPosition(id, Float:x, Float:y, Float:z, Float:a, world, interior) {
    new
        entryid = Entrance_GetEntryID(id),
        exitid = Entrance_GetExitID(id);

    if(!Iter_Contains(entranceID, entryid) || !Iter_Contains(entranceID, exitid)) {
        return 0;
    }

    entranceX[exitid] = x;
    entranceY[exitid] = y;
    entranceZ[exitid] = z;
    entranceAng[exitid] = a;
    entranceVirtualID[exitid] = world;
    entranceInteriorID[exitid] = interior;

    exitX[entryid] = x;
    exitY[entryid] = y;
    exitZ[entryid] = z;
    exitAng[entryid] = a;
    exitVirtualID[entryid] = world;
    exitInteriorID[entryid] = interior;

    Entrance_RebuildPoint(exitid);
    Entrance_SavePairToDatabase(entryid);
    return 1;
}

stock Entrance_TeleportAdmin(playerid, id, bool:toExit = false) {
    new
        targetid = toExit ? Entrance_GetExitID(id) : Entrance_GetEntryID(id);

    if(!Iter_Contains(entranceID, targetid)) {
        return 0;
    }

    Interaction_LoadSafeTeleport(
        playerid,
        entranceX[targetid],
        entranceY[targetid],
        entranceZ[targetid] + 0.5,
        entranceAng[targetid],
        entranceVirtualID[targetid],
        entranceInteriorID[targetid],
        false,
        500
    );
    return 1;
}

stock Entrance_BuildAdminList(playerid, output[], len) {
    output[0] = EOS;
    PlayerAdminEntranceListCount[playerid] = 0;

    strcat(output, "ID\tSQL\tTen\tLoai\n", len);

    foreach(new idx : entranceID) {
        if(entranceIsExitPoint[idx]) {
            continue;
        }

        new
            line[96],
            dbid = entranceDatabaseID[idx];

        format(
            line,
            sizeof(line),
            "%i\t%i\t%s\t%s\n",
            idx,
            dbid,
            isnull(entranceStoreName[idx]) ? ("LOI VAO") : entranceStoreName[idx],
            dbid > 0 ? ("SQL") : ("Static")
        );
        strcat(output, line, len);
        PlayerAdminEntranceListID[playerid][PlayerAdminEntranceListCount[playerid]] = idx;
        PlayerAdminEntranceListCount[playerid] ++;
    }
    return PlayerAdminEntranceListCount[playerid];
}

stock Entrance_GetAdminListSelection(playerid, listitem) {
    if(!(0 <= listitem < PlayerAdminEntranceListCount[playerid])) {
        return INTERACTION_INVALID_ID;
    }
    return PlayerAdminEntranceListID[playerid][listitem];
}

stock Entrance_LoadPersisted() {
    inline const OnEntranceLoad()
    {
        new
            rows = cache_num_rows(),
            dbid,
            iconid,
            freezeInt,
            entryWorld,
            entryInterior,
            targetWorld,
            targetInterior,
            name[MAX_STORE_NAME],
            Float:entryX,
            Float:entryY,
            Float:entryZ,
            Float:entryA,
            Float:targetX,
            Float:targetY,
            Float:targetZ,
            Float:targetA;

        for(new i; i < rows; i++) {
            cache_get_value_name_int(i, "entrance_id", dbid);
            cache_get_value_name(i, "name", name, sizeof(name));
            cache_get_value_name_int(i, "map_icon", iconid);
            cache_get_value_name_int(i, "freeze_player", freezeInt);
            cache_get_value_name_float(i, "entry_x", entryX);
            cache_get_value_name_float(i, "entry_y", entryY);
            cache_get_value_name_float(i, "entry_z", entryZ);
            cache_get_value_name_float(i, "entry_a", entryA);
            cache_get_value_name_int(i, "entry_world", entryWorld);
            cache_get_value_name_int(i, "entry_interior", entryInterior);
            cache_get_value_name_float(i, "exit_x", targetX);
            cache_get_value_name_float(i, "exit_y", targetY);
            cache_get_value_name_float(i, "exit_z", targetZ);
            cache_get_value_name_float(i, "exit_a", targetA);
            cache_get_value_name_int(i, "exit_world", targetWorld);
            cache_get_value_name_int(i, "exit_interior", targetInterior);

            Entrance_CreatePair(
                name,
                iconid,
                freezeInt ? true : false,
                entryX,
                entryY,
                entryZ,
                entryA,
                entryWorld,
                entryInterior,
                targetX,
                targetY,
                targetZ,
                targetA,
                targetWorld,
                targetInterior,
                dbid
            );
        }
    }

    MySQL_TQueryInline(
        MySQL_GetHandle(),
        using inline OnEntranceLoad,
        "\
            SELECT \
                entrance_id, name, map_icon, freeze_player, \
                entry_x, entry_y, entry_z, entry_a, entry_world, entry_interior, \
                exit_x, exit_y, exit_z, exit_a, exit_world, exit_interior \
            FROM entrances \
            WHERE enabled = 1 \
            ORDER BY entrance_id ASC"
    );
    return 1;
}

hook OnMySQLConnected()
{
    Entrance_LoadPersisted();
    return 1;
}

stock Entrance_IsPlayerAllowed(playerid) {
    return (gettime() > playerEnExCoolDown[playerid]);
}

// Assign unix timestamp of player last entered CP
stock Entrance_UpdatePlayerTick(playerid, seconds = 2) {
    return playerEnExCoolDown[playerid] = (gettime() + seconds);
}

stock Entrance_PreloadTarget(playerid, idx) {
    if(!Iter_Contains(entranceID, idx)) {
        return 0;
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:a;

    Entrance_GetTargetPoint(idx, x, y, z, a);
    Streamer_UpdateEx(playerid, x, y, z, exitVirtualID[idx], exitInteriorID[idx], -1, 0, 0);
    return 1;
}

stock Entrance_GetTargetPoint(idx, &Float:x, &Float:y, &Float:z, &Float:a) {
    x = exitX[idx];
    y = exitY[idx];
    z = exitZ[idx];
    a = exitAng[idx];

    x += ENTRANCE_TELEPORT_OFFSET * floatsin(-a, degrees);
    y += ENTRANCE_TELEPORT_OFFSET * floatcos(-a, degrees);
    return 1;
}

stock Entrance_ClearReuseBlock(playerid)
{
    PlayerBlockedEntranceID[playerid] = INTERACTION_INVALID_ID;
    PlayerBlockedEntranceUntil[playerid] = 0;
    return 1;
}

stock Entrance_BlockReuse(playerid, idx, duration = ENTRANCE_REUSE_BLOCK_MS)
{
    if(!Iter_Contains(entranceID, idx)) {
        return 0;
    }

    PlayerBlockedEntranceID[playerid] = idx;
    PlayerBlockedEntranceUntil[playerid] = GetTickCount() + duration;
    return 1;
}

stock bool:Entrance_IsReuseBlocked(playerid, idx)
{
    if(PlayerBlockedEntranceID[playerid] != idx) {
        return false;
    }

    if(GetTickCount() >= PlayerBlockedEntranceUntil[playerid]) {
        Entrance_ClearReuseBlock(playerid);
        return false;
    }
    return true;
}

stock bool:Entrance_IsInteriorExitForPlayer(playerid, idx) {
    return (
        Iter_Contains(entranceID, idx) &&
        entranceIsExitPoint[idx] &&
        GetPlayerVirtualWorld(playerid) == entranceVirtualID[idx] &&
        GetPlayerInterior(playerid) == entranceInteriorID[idx]
    );
}

stock Entrance_ShowPrompt(playerid, idx) {
    if(!Iter_Contains(entranceID, idx)) {
        return 0;
    }

    if(Entrance_IsReuseBlocked(playerid, idx)) {
        return 0;
    }

    new
        caption[56],
        action[16];

    Entrance_PreloadTarget(playerid, idx);

    if(entranceIsExitPoint[idx]) {
        strcpy(action, "ra ngoai", sizeof(action));
        format(caption, sizeof(caption), "~g~~h~LOI RA: %s", isnull(entranceStoreName[idx]) ? ("Ben ngoai") : entranceStoreName[idx]);
    }
    else {
        strcpy(action, "vao", sizeof(action));
        format(caption, sizeof(caption), "~y~~h~%s", isnull(entranceStoreName[idx]) ? ("Loi vao") : entranceStoreName[idx]);
    }

    return Interaction_ShowPrompt(playerid, INTERACTION_TYPE_ENTRANCE, idx, caption, action);
}

stock Entrance_FindUsable(playerid) {
    if(Interaction_IsActive(playerid, INTERACTION_TYPE_ENTRANCE)) {
        new activeid = Interaction_GetActiveID(playerid);

        if(!Entrance_IsReuseBlocked(playerid, activeid)) {
            return activeid;
        }

        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_ENTRANCE, activeid);
    }

    foreach(new idx : entranceID) {
        if(Entrance_IsReuseBlocked(playerid, idx)) {
            continue;
        }

        if(IsPlayerInDynamicArea(playerid, EnExArea[idx])) {
            return idx;
        }
    }

    new exitid = PlayerInteriorExitID[playerid];
    if(Entrance_IsInteriorExitForPlayer(playerid, exitid) && !Entrance_IsReuseBlocked(playerid, exitid)) {
        return exitid;
    }

    foreach(new exitidx : entranceID) {
        if(Entrance_IsReuseBlocked(playerid, exitidx)) {
            continue;
        }

        if(Entrance_IsInteriorExitForPlayer(playerid, exitidx)) {
            return exitidx;
        }
    }

    return INTERACTION_INVALID_ID;
}

stock Entrance_Use(playerid, idx) {
    if(!Iter_Contains(entranceID, idx)) {
        return 0;
    }

    if(!Entrance_IsPlayerAllowed(playerid)) {
        return SendErrorMsg(playerid, "Vui long cho vai giay roi thu lai.");
    }

    if(IsPlayerInAnyVehicle(playerid)) {
        return 1;
    }

    if(Entrance_IsReuseBlocked(playerid, idx)) {
        return 1;
    }

    if(
        !Interaction_IsActive(playerid, INTERACTION_TYPE_ENTRANCE, idx) &&
        !IsPlayerInDynamicArea(playerid, EnExArea[idx]) &&
        !Entrance_IsInteriorExitForPlayer(playerid, idx)
    ) {
        return 1;
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:a,
        teleportDelay = entranceFreezePlayer[idx] ? 2000 : INTERACTION_TELEPORT_DELAY;

    new targetid = entranceLinkedID[idx];

    Entrance_GetTargetPoint(idx, x, y, z, a);
    Interaction_ClearPrompt(playerid);
    Interaction_LockPrompts(playerid, teleportDelay + GetPlayerPing(playerid) + 1000);
    Entrance_BlockReuse(playerid, targetid, teleportDelay + GetPlayerPing(playerid) + ENTRANCE_REUSE_BLOCK_MS);
    Interaction_LoadSafeTeleport(
        playerid,
        x,
        y,
        z,
        a,
        exitVirtualID[idx],
        exitInteriorID[idx],
        true,
        teleportDelay
    );

    Entrance_UpdatePlayerTick(playerid, 1);

    if(entranceIsExitPoint[idx]) {
        PlayerInteriorExitID[playerid] = INTERACTION_INVALID_ID;
        getShopName[playerid][0] = EOS;
    }
    else {
        PlayerInteriorExitID[playerid] = entranceLinkedID[idx];
        strcpy(getShopName[playerid], entranceStoreName[idx], MAX_STORE_NAME);
        SendServerMsg(playerid, "Neu khong thay diem ra, dung /ra de ra ngoai.");
    }

    CallRemoteFunction("OnPlayerEnterEntrance", "i", playerid);
    return 1;
}

static Entrance_CommandUse(playerid)
{
    new idx = Entrance_FindUsable(playerid);

    if(!Iter_Contains(entranceID, idx)) {
        return SendErrorMsg(playerid, "Ban khong dung gan loi ra vao nao.");
    }

    Entrance_Use(playerid, idx);
    return 1;
}

// Object Visual (Animation)
hook OnDynamicObjectMoved(objectid) {
    new
        arr[2];

    Streamer_GetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, arr, sizeof(arr));

    if(arr[0] == ENEX_STREAMER_IDENTIFIER) {
        new
            idx = arr[1];
        if(EnExObject[idx] == objectid) {
            new
                Float:x,
                Float:y,
                Float:z;
            if(EnExObjectState[idx] == ENTRANCE_UP) {
                Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_X, x);
                Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_Y, y);
                Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_Z, z);

                EnExObjectState[idx] = ENTRANCE_DOWN;
                MoveDynamicObject(objectid, x, y, (z - 1), 1.0);
            }
            else {
                Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_X, x);
                Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_Y, y);
                Streamer_GetFloatData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_Z, z);

                EnExObjectState[idx] = ENTRANCE_UP;
                MoveDynamicObject(objectid, x, y, (z + 1), 1.0);
            }
        }
    }
    return 1;
}

// Handler
hook OnPlayerEnterDynArea(playerid, areaid) {
    if (IsPlayerInAnyVehicle(playerid)) {
        return 1;
    }

    new
        arr[2];

    Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, arr);

    if (arr[0] == ENEX_STREAMER_IDENTIFIER) {
        new
            idx = arr[1];

        if (areaid == EnExArea[idx]) {
            Entrance_ShowPrompt(playerid, idx);
        }
    }
    return 1;
}

hook OnPlayerConnect(playerid) {
    PlayerInteriorExitID[playerid] = INTERACTION_INVALID_ID;
    Entrance_ClearReuseBlock(playerid);
    PlayerAdminEntranceListCount[playerid] = 0;
    return 1;
}

CMD:vao(playerid, params[])
{
    #pragma unused params
    return Entrance_CommandUse(playerid);
}

CMD:ra(playerid, params[])
{
    #pragma unused params
    return Entrance_CommandUse(playerid);
}

hook OnPlayerLeaveDynArea(playerid, areaid) {
    new
        arr[2];

    Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, arr);

    if(arr[0] == ENEX_STREAMER_IDENTIFIER) {
        new
            idx = arr[1];

        if(areaid == EnExArea[idx]) {
            if(PlayerBlockedEntranceID[playerid] == idx) {
                Entrance_ClearReuseBlock(playerid);
            }

            Interaction_ClearPrompt(playerid, INTERACTION_TYPE_ENTRANCE, idx);
        }
    }
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    if(PRESSED(KEY_NO) && Interaction_IsActive(playerid, INTERACTION_TYPE_ENTRANCE)) {
        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_ENTRANCE, Interaction_GetActiveID(playerid));
        return 1;
    }

    if(PRESSED(KEY_YES) && Interaction_IsActive(playerid, INTERACTION_TYPE_ENTRANCE)) {
        return Entrance_Use(playerid, Interaction_GetActiveID(playerid));
    }
    return 1;
}
