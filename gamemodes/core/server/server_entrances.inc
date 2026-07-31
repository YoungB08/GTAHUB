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
    gPlayerEntranceCooldown[MAX_PLAYERS],
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
    mysql_query_file(MySQL_GetHandle(), "entrances.sql", false);
    mysql_query_file(MySQL_GetHandle(), "doors.sql", false);
    Entrance_LoadPersisted();
    Door_LoadPersisted();
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
    #pragma unused playerid, idx
    // Streamer_UpdateEx removed to prevent premature auto-teleporting when walking near entrance prompts.
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

stock Entrance_BlockReuse(playerid, idx, duration = 3000)
{
    #pragma unused idx
    gPlayerEntranceCooldown[playerid] = GetTickCount() + duration;
    return 1;
}

stock bool:Entrance_IsReuseBlocked(playerid, idx)
{
    #pragma unused idx
    if(GetTickCount() < gPlayerEntranceCooldown[playerid]) {
        return true;
    }
    return false;
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

    if(GetTickCount() < gPlayerEntranceCooldown[playerid]) {
        return SendErrorMsg(playerid, "Vui long cho 3 giay de tiep tuc ra/vao.");
    }

    if(IsPlayerInAnyVehicle(playerid)) {
        return 1;
    }

    if(
        !Interaction_IsActive(playerid, INTERACTION_TYPE_ENTRANCE, idx) &&
        !IsPlayerInDynamicArea(playerid, EnExArea[idx]) &&
        !Entrance_IsInteriorExitForPlayer(playerid, idx)
    ) {
        return 1;
    }

    gPlayerEntranceCooldown[playerid] = GetTickCount() + 3000;

    new
        Float:x,
        Float:y,
        Float:z,
        Float:a,
        teleportDelay = entranceFreezePlayer[idx] ? 2000 : INTERACTION_TELEPORT_DELAY;

    new targetid = entranceLinkedID[idx];

    Entrance_GetTargetPoint(idx, x, y, z, a);
    Interaction_ClearPrompt(playerid);
    Interaction_LockPrompts(playerid, 3000);
    Entrance_BlockReuse(playerid, targetid, 3000);
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
    if(Door_TryInteract(playerid)) return 1;
    return Entrance_CommandUse(playerid);
}

CMD:ra(playerid, params[])
{
    #pragma unused params
    if(Door_TryInteract(playerid)) return 1;
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
        if(GetTickCount() < gPlayerEntranceCooldown[playerid]) {
            return SendErrorMsg(playerid, "Vui long cho 3 giay de tiep tuc ra/vao.");
        }
        return Entrance_Use(playerid, Interaction_GetActiveID(playerid));
    }

    if(PRESSED(KEY_YES) || PRESSED(KEY_SECONDARY_ATTACK)) {
        Door_TryInteract(playerid);
    }
    return 1;
}

// =============================================================================
// CENTRALIZED SQL DOORS SYSTEM IMPLEMENTATION
// =============================================================================

#define MAX_DOORS                       (256)
#define DOOR_STREAMER_IDENTIFIER        (0x444F4F52) // "DOOR"

static
    Iterator:DoorIndex<MAX_DOORS>,
    doorName[MAX_DOORS][64],
    doorSlug[MAX_DOORS][64],
    doorLabelText[MAX_DOORS][128],
    Float:doorExtX[MAX_DOORS],
    Float:doorExtY[MAX_DOORS],
    Float:doorExtZ[MAX_DOORS],
    Float:doorExtA[MAX_DOORS],
    doorExtInt[MAX_DOORS],
    doorExtVW[MAX_DOORS],
    Float:doorIntX[MAX_DOORS],
    Float:doorIntY[MAX_DOORS],
    Float:doorIntZ[MAX_DOORS],
    Float:doorIntA[MAX_DOORS],
    doorIntInt[MAX_DOORS],
    doorIntVW[MAX_DOORS],
    doorPickupModel[MAX_DOORS],
    doorType[MAX_DOORS],
    bool:doorLocked[MAX_DOORS],
    doorFaction[MAX_DOORS],
    doorBusiness[MAX_DOORS],
    doorHouse[MAX_DOORS],
    doorAdminLevel[MAX_DOORS],
    doorVipLevel[MAX_DOORS],
    bool:doorEnabled[MAX_DOORS],
    doorExtPickup[MAX_DOORS],
    doorIntPickup[MAX_DOORS],
    Text3D:doorExtLabel[MAX_DOORS],
    Text3D:doorIntLabel[MAX_DOORS],
    doorExtArea[MAX_DOORS],
    doorIntArea[MAX_DOORS];

stock bool:Door_IsValid(doorid) {
    return (doorid > 0 && doorid < MAX_DOORS && Iter_Contains(DoorIndex, doorid));
}

stock Door_DestroyObjects(doorid) {
    if(!Door_IsValid(doorid)) return 0;

    if(doorExtPickup[doorid] != INVALID_STREAMER_ID) {
        DestroyDynamicPickup(doorExtPickup[doorid]);
        doorExtPickup[doorid] = INVALID_STREAMER_ID;
    }
    if(doorIntPickup[doorid] != INVALID_STREAMER_ID) {
        DestroyDynamicPickup(doorIntPickup[doorid]);
        doorIntPickup[doorid] = INVALID_STREAMER_ID;
    }
    if(doorExtLabel[doorid] != INVALID_3DTEXT_ID) {
        DestroyDynamic3DTextLabel(doorExtLabel[doorid]);
        doorExtLabel[doorid] = INVALID_3DTEXT_ID;
    }
    if(doorIntLabel[doorid] != INVALID_3DTEXT_ID) {
        DestroyDynamic3DTextLabel(doorIntLabel[doorid]);
        doorIntLabel[doorid] = INVALID_3DTEXT_ID;
    }
    if(doorExtArea[doorid] != INVALID_STREAMER_ID) {
        DestroyDynamicArea(doorExtArea[doorid]);
        doorExtArea[doorid] = INVALID_STREAMER_ID;
    }
    if(doorIntArea[doorid] != INVALID_STREAMER_ID) {
        DestroyDynamicArea(doorIntArea[doorid]);
        doorIntArea[doorid] = INVALID_STREAMER_ID;
    }
    return 1;
}

stock Door_BuildObjects(doorid) {
    if(!Door_IsValid(doorid) || !doorEnabled[doorid]) return 0;

    Door_DestroyObjects(doorid);

    new model = (doorPickupModel[doorid] > 0) ? doorPickupModel[doorid] : 19130;
    new labelStr[180];

    // Exterior Pickup & Label
    doorExtPickup[doorid] = CreateDynamicPickup(model, 1, doorExtX[doorid], doorExtY[doorid], doorExtZ[doorid], doorExtVW[doorid], doorExtInt[doorid]);
    doorExtArea[doorid] = CreateDynamicSphere(doorExtX[doorid], doorExtY[doorid], doorExtZ[doorid], 2.0, doorExtVW[doorid], doorExtInt[doorid]);

    format(labelStr, sizeof(labelStr), "CUA VAO: %s\n"C_GREY"%s\n"C_YELLOW"Nhan 'Y' hoac /enter de vao", 
        doorName[doorid], doorLocked[doorid] ? ("{FF0000}[DA KHOA]") : ("{00FF00}[MO CUA]"));
    doorExtLabel[doorid] = CreateDynamic3DTextLabel(labelStr, COLOR_ORANGE, doorExtX[doorid], doorExtY[doorid], doorExtZ[doorid] + 0.8, 15.0, .worldid = doorExtVW[doorid], .interiorid = doorExtInt[doorid]);

    // Interior Pickup & Label
    doorIntPickup[doorid] = CreateDynamicPickup(model, 1, doorIntX[doorid], doorIntY[doorid], doorIntZ[doorid], doorIntVW[doorid], doorIntInt[doorid]);
    doorIntArea[doorid] = CreateDynamicSphere(doorIntX[doorid], doorIntY[doorid], doorIntZ[doorid], 2.0, doorIntVW[doorid], doorIntInt[doorid]);

    format(labelStr, sizeof(labelStr), "LOI RA: %s\n"C_YELLOW"Nhan 'Y' hoac /exit de ra", doorName[doorid]);
    doorIntLabel[doorid] = CreateDynamic3DTextLabel(labelStr, COLOR_GREEN, doorIntX[doorid], doorIntY[doorid], doorIntZ[doorid] + 0.8, 15.0, .worldid = doorIntVW[doorid], .interiorid = doorIntInt[doorid]);
    return 1;
}

stock Door_LoadPersisted() {
    inline OnDoorsLoad() {
        new rows = cache_num_rows();
        if(!rows) return 1;

        new id;
        for(new i = 0; i < rows; i++) {
            cache_get_value_name_int(i, "id", id);
            if(id <= 0 || id >= MAX_DOORS) continue;

            Iter_Add(DoorIndex, id);
            cache_get_value_name(i, "name", doorName[id], 64);
            cache_get_value_name(i, "slug", doorSlug[id], 64);
            cache_get_value_name(i, "label", doorLabelText[id], 128);

            cache_get_value_name_float(i, "exterior_x", doorExtX[id]);
            cache_get_value_name_float(i, "exterior_y", doorExtY[id]);
            cache_get_value_name_float(i, "exterior_z", doorExtZ[id]);
            cache_get_value_name_float(i, "exterior_a", doorExtA[id]);
            cache_get_value_name_int(i, "exterior_interior", doorExtInt[id]);
            cache_get_value_name_int(i, "exterior_vw", doorExtVW[id]);

            cache_get_value_name_float(i, "interior_x", doorIntX[id]);
            cache_get_value_name_float(i, "interior_y", doorIntY[id]);
            cache_get_value_name_float(i, "interior_z", doorIntZ[id]);
            cache_get_value_name_float(i, "interior_a", doorIntA[id]);
            cache_get_value_name_int(i, "interior_interior", doorIntInt[id]);
            cache_get_value_name_int(i, "interior_vw", doorIntVW[id]);

            cache_get_value_name_int(i, "pickup_model", doorPickupModel[id]);
            cache_get_value_name_int(i, "door_type", doorType[id]);

            new lockedInt, enabledInt;
            cache_get_value_name_int(i, "locked", lockedInt);
            doorLocked[id] = (lockedInt == 1);

            cache_get_value_name_int(i, "faction_id", doorFaction[id]);
            cache_get_value_name_int(i, "business_id", doorBusiness[id]);
            cache_get_value_name_int(i, "house_id", doorHouse[id]);
            cache_get_value_name_int(i, "admin_level", doorAdminLevel[id]);
            cache_get_value_name_int(i, "vip_level", doorVipLevel[id]);

            cache_get_value_name_int(i, "enabled", enabledInt);
            doorEnabled[id] = (enabledInt == 1);

            Door_BuildObjects(id);
        }
        printf("[SQL Doors] Successfully loaded %d doors from database.", rows);
    }

    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnDoorsLoad,
        "SELECT id, name, slug, label, exterior_x, exterior_y, exterior_z, exterior_a, exterior_interior, exterior_vw, \
         interior_x, interior_y, interior_z, interior_a, interior_interior, interior_vw, pickup_model, door_type, \
         locked, faction_id, business_id, house_id, admin_level, vip_level, enabled FROM doors WHERE enabled = 1 ORDER BY id ASC");
    return 1;
}

stock Door_CreateNew(const name[], Float:x, Float:y, Float:z, Float:a, intid, vworld, creatorid) {
    new query[512], creatorName[32];
    GetPlayerName(creatorid, creatorName, sizeof(creatorName));

    inline OnDoorInserted() {
        new doorid = cache_insert_id();
        if(doorid > 0 && doorid < MAX_DOORS) {
            Iter_Add(DoorIndex, doorid);
            strcpy(doorName[doorid], name, 64);
            doorExtX[doorid] = x;
            doorExtY[doorid] = y;
            doorExtZ[doorid] = z;
            doorExtA[doorid] = a;
            doorExtInt[doorid] = intid;
            doorExtVW[doorid] = vworld;
            doorPickupModel[doorid] = 19130;
            doorEnabled[doorid] = true;

            Door_BuildObjects(doorid);
        }
    }

    mysql_format(MySQL_GetHandle(), query, sizeof(query),
        "INSERT INTO doors (name, exterior_x, exterior_y, exterior_z, exterior_a, exterior_interior, exterior_vw, created_by) \
         VALUES ('%e', %f, %f, %f, %f, %d, %d, '%e')",
        name, x, y, z, a, intid, vworld, creatorName);
    MySQL_TQueryInline(MySQL_GetHandle(), using inline OnDoorInserted, query);
    return 1;
}

stock Door_SetExteriorPos(doorid, Float:x, Float:y, Float:z, Float:a, intid, vworld) {
    if(!Door_IsValid(doorid)) return 0;
    doorExtX[doorid] = x;
    doorExtY[doorid] = y;
    doorExtZ[doorid] = z;
    doorExtA[doorid] = a;
    doorExtInt[doorid] = intid;
    doorExtVW[doorid] = vworld;

    new query[256];
    mysql_format(MySQL_GetHandle(), query, sizeof(query),
        "UPDATE doors SET exterior_x=%f, exterior_y=%f, exterior_z=%f, exterior_a=%f, exterior_interior=%d, exterior_vw=%d WHERE id=%d",
        x, y, z, a, intid, vworld, doorid);
    mysql_tquery(MySQL_GetHandle(), query);

    Door_BuildObjects(doorid);
    return 1;
}

stock Door_SetInteriorPos(doorid, Float:x, Float:y, Float:z, Float:a, intid, vworld) {
    if(!Door_IsValid(doorid)) return 0;
    doorIntX[doorid] = x;
    doorIntY[doorid] = y;
    doorIntZ[doorid] = z;
    doorIntA[doorid] = a;
    doorIntInt[doorid] = intid;
    doorIntVW[doorid] = vworld;

    new query[256];
    mysql_format(MySQL_GetHandle(), query, sizeof(query),
        "UPDATE doors SET interior_x=%f, interior_y=%f, interior_z=%f, interior_a=%f, interior_interior=%d, interior_vw=%d WHERE id=%d",
        x, y, z, a, intid, vworld, doorid);
    mysql_tquery(MySQL_GetHandle(), query);

    Door_BuildObjects(doorid);
    return 1;
}

stock Door_SetName(doorid, const name[]) {
    if(!Door_IsValid(doorid)) return 0;
    strcpy(doorName[doorid], name, 64);

    new query[256];
    mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE doors SET name='%e' WHERE id=%d", name, doorid);
    mysql_tquery(MySQL_GetHandle(), query);

    Door_BuildObjects(doorid);
    return 1;
}

stock Door_SetLabel(doorid, const label[]) {
    if(!Door_IsValid(doorid)) return 0;
    strcpy(doorLabelText[doorid], label, 128);

    new query[256];
    mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE doors SET label='%e' WHERE id=%d", label, doorid);
    mysql_tquery(MySQL_GetHandle(), query);

    Door_BuildObjects(doorid);
    return 1;
}

stock Door_SetLocked(doorid, bool:locked) {
    if(!Door_IsValid(doorid)) return 0;
    doorLocked[doorid] = locked;

    new query[128];
    mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE doors SET locked=%d WHERE id=%d", locked ? 1 : 0, doorid);
    mysql_tquery(MySQL_GetHandle(), query);

    Door_BuildObjects(doorid);
    return 1;
}

stock Door_SetType(doorid, type) {
    if(!Door_IsValid(doorid)) return 0;
    doorType[doorid] = type;

    new query[128];
    mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE doors SET door_type=%d WHERE id=%d", type, doorid);
    mysql_tquery(MySQL_GetHandle(), query);
    return 1;
}

stock Door_SetPickupModel(doorid, model) {
    if(!Door_IsValid(doorid)) return 0;
    doorPickupModel[doorid] = model;

    new query[128];
    mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE doors SET pickup_model=%d WHERE id=%d", model, doorid);
    mysql_tquery(MySQL_GetHandle(), query);

    Door_BuildObjects(doorid);
    return 1;
}

stock Door_SetEnabled(doorid, bool:enabled) {
    if(!Door_IsValid(doorid)) return 0;
    doorEnabled[doorid] = enabled;

    new query[128];
    mysql_format(MySQL_GetHandle(), query, sizeof(query), "UPDATE doors SET enabled=%d WHERE id=%d", enabled ? 1 : 0, doorid);
    mysql_tquery(MySQL_GetHandle(), query);

    if(!enabled) Door_DestroyObjects(doorid);
    else Door_BuildObjects(doorid);
    return 1;
}

stock Door_Delete(doorid) {
    if(!Door_IsValid(doorid)) return 0;
    Door_DestroyObjects(doorid);

    new query[128];
    mysql_format(MySQL_GetHandle(), query, sizeof(query), "DELETE FROM doors WHERE id=%d", doorid);
    mysql_tquery(MySQL_GetHandle(), query);

    Iter_Remove(DoorIndex, doorid);
    return 1;
}

stock Door_Reload(doorid) {
    if(!Door_IsValid(doorid)) return 0;
    Door_BuildObjects(doorid);
    return 1;
}

stock Door_GetExteriorPos(doorid, &Float:x, &Float:y, &Float:z, &Float:a, &intid, &vworld) {
    if(!Door_IsValid(doorid)) return 0;
    x = doorExtX[doorid]; y = doorExtY[doorid]; z = doorExtZ[doorid]; a = doorExtA[doorid];
    intid = doorExtInt[doorid]; vworld = doorExtVW[doorid];
    return 1;
}

stock Door_GetInteriorPos(doorid, &Float:x, &Float:y, &Float:z, &Float:a, &intid, &vworld) {
    if(!Door_IsValid(doorid)) return 0;
    x = doorIntX[doorid]; y = doorIntY[doorid]; z = doorIntZ[doorid]; a = doorIntA[doorid];
    intid = doorIntInt[doorid]; vworld = doorIntVW[doorid];
    return 1;
}

stock Door_ShowInfo(playerid, doorid) {
    if(!Door_IsValid(doorid)) return 0;
    new info[512];
    format(info, sizeof(info),
        "ID: %d | Ten: %s\nKhoa: %s | Type: %d | Enabled: %s\nExterior: Pos(%.2f, %.2f, %.2f) Int(%d) VW(%d)\nInterior: Pos(%.2f, %.2f, %.2f) Int(%d) VW(%d)\nPickup: %d | Admin: %d | VIP: %d",
        doorid, doorName[doorid], doorLocked[doorid] ? "Co" : "Khong", doorType[doorid], doorEnabled[doorid] ? "Co" : "Khong",
        doorExtX[doorid], doorExtY[doorid], doorExtZ[doorid], doorExtInt[doorid], doorExtVW[doorid],
        doorIntX[doorid], doorIntY[doorid], doorIntZ[doorid], doorIntInt[doorid], doorIntVW[doorid],
        doorPickupModel[doorid], doorAdminLevel[doorid], doorVipLevel[doorid]);
    Dialog_Show(playerid, DIALOG_STYLE_MSGBOX, "Door Information", info, "Dong", "");
    return 1;
}

stock Door_ShowList(playerid) {
    new list[2048] = "ID\tTen Door\tTrang Thai\n";
    new item[128];
    foreach(new i : DoorIndex) {
        format(item, sizeof(item), "%d\t%s\t%s\n", i, doorName[i], doorLocked[i] ? "{FF0000}Khoa" : "{00FF00}Mo");
        strcat(list, item, sizeof(list));
    }
    Dialog_Show(playerid, DIALOG_STYLE_TABLIST_HEADERS, "Danh Sach Doors", list, "Dong", "");
    return 1;
}

stock Door_TryInteract(playerid) {
    if(IsPlayerInAnyVehicle(playerid)) return 0;

    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);
    new intid = GetPlayerInterior(playerid);
    new vworld = GetPlayerVirtualWorld(playerid);

    foreach(new i : DoorIndex) {
        if(!doorEnabled[i]) continue;

        // Check Exterior -> Interior
        if(vworld == doorExtVW[i] && intid == doorExtInt[i] && IsPlayerInRangeOfPoint(playerid, 2.5, doorExtX[i], doorExtY[i], doorExtZ[i])) {
            if(doorLocked[i]) {
                return SendErrorMsg(playerid, "Door nay dang bi KHOA.");
            }

            SetPlayerInterior(playerid, doorIntInt[i]);
            SetPlayerVirtualWorld(playerid, doorIntVW[i]);
            SetPlayerPos(playerid, doorIntX[i], doorIntY[i], doorIntZ[i]);
            SetPlayerFacingAngle(playerid, doorIntA[i]);
            Streamer_UpdateEx(playerid, doorIntX[i], doorIntY[i], doorIntZ[i], doorIntVW[i], doorIntInt[i], -1, 1, 0);
            SetCameraBehindPlayer(playerid);
            SendServerMsgF(playerid, "Ban da vao '%s'.", doorName[i]);
            return 1;
        }

        // Check Interior -> Exterior
        if(vworld == doorIntVW[i] && intid == doorIntInt[i] && IsPlayerInRangeOfPoint(playerid, 2.5, doorIntX[i], doorIntY[i], doorIntZ[i])) {
            SetPlayerInterior(playerid, doorExtInt[i]);
            SetPlayerVirtualWorld(playerid, doorExtVW[i]);
            SetPlayerPos(playerid, doorExtX[i], doorExtY[i], doorExtZ[i]);
            SetPlayerFacingAngle(playerid, doorExtA[i]);
            Streamer_UpdateEx(playerid, doorExtX[i], doorExtY[i], doorExtZ[i], doorExtVW[i], doorExtInt[i], -1, 1, 0);
            SetCameraBehindPlayer(playerid);
            SendServerMsgF(playerid, "Ban da ra ngoai '%s'.", doorName[i]);
            return 1;
        }
    }
    return 0;
}

CMD:enter(playerid, params[]) {
    #pragma unused params
    if(Door_TryInteract(playerid)) return 1;
    return Entrance_CommandUse(playerid);
}

CMD:exit(playerid, params[]) {
    #pragma unused params
    if(Door_TryInteract(playerid)) return 1;
    return Entrance_CommandUse(playerid);
}

CMD:door(playerid, params[]) {
    #pragma unused params
    if(Door_TryInteract(playerid)) return 1;
    return Entrance_CommandUse(playerid);
}
