#include <YSI_Coding\y_hooks>
#if defined INVALID_PATH_ID
    #undef INVALID_PATH_ID
#endif
#include <GPS>

#define INVALID_GPS_PATH_ID INVALID_PATH_ID

#define MAX_GPS_ARROWS 25


// GPS System - Core logic for GPS checkpoint pathfinding with distance tracking

static
    PlayerGPSCheckpoint[MAX_PLAYERS] = { -1, ... },
    Path:PlayerGPSPath[MAX_PLAYERS] = { INVALID_GPS_PATH_ID, ... },
    PlayerGPSPathSize[MAX_PLAYERS],
    PlayerGPSCurrentNode[MAX_PLAYERS],
    Float:PlayerGPSTargetX[MAX_PLAYERS],
    Float:PlayerGPSTargetY[MAX_PLAYERS],
    Float:PlayerGPSTargetZ[MAX_PLAYERS],
    PlayerGPSTimer[MAX_PLAYERS] = { -1, ... },
    PlayerGPSName[MAX_PLAYERS][32],
    PlayerGPSArrows[MAX_PLAYERS][MAX_GPS_ARROWS] = { {-1, ...}, ... },
    PlayerGPSArrowIdx[MAX_PLAYERS][MAX_GPS_ARROWS] = { {-1, ...}, ... },
    bool:PlayerGPSRecalculating[MAX_PLAYERS] = { false, ... }
;

forward GPS_UpdateDistance(playerid);
forward OnGPSPathCalculated(Path:pathid, playerid);

hook OnPlayerConnect(playerid) {
    PlayerGPSCheckpoint[playerid] = -1;
    PlayerGPSPath[playerid] = INVALID_GPS_PATH_ID;
    PlayerGPSTimer[playerid] = -1;
    PlayerGPSRecalculating[playerid] = false;
    for(new i = 0; i < MAX_GPS_ARROWS; i++) {
        PlayerGPSArrows[playerid][i] = -1;
        PlayerGPSArrowIdx[playerid][i] = -1;
    }
    PlayerGPSName[playerid][0] = EOS;
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    GPS_ClearLocation(playerid);
    return 1;
}

hook OnPlayerEnterDynamicCP(playerid, checkpointid) {
    if(PlayerGPSCheckpoint[playerid] != -1 && checkpointid == PlayerGPSCheckpoint[playerid]) {
        new string[64];
        format(string, sizeof(string), "Ban da den %s.", PlayerGPSName[playerid]);
        SendServerMsg(playerid, string);
        GPS_ClearLocation(playerid);
        return 1;
    }
    return 1;
}

stock GPS_CreateArrowObject(playerid, index) {
    new MapNode:nodeid, MapNode:nextnode;
    new Float:x, Float:y, Float:z, Float:ang;
    
    GetPathNode(PlayerGPSPath[playerid], index, nodeid);
    GetMapNodePos(nodeid, x, y, z);
    
    if (GetPathNode(PlayerGPSPath[playerid], index + 1, nextnode) == GPS_ERROR_NONE) {
        GetAngleBetweenMapNodes(nextnode, nodeid, ang);
        return CreateDynamicObject(1318, x, y, z + 0.5, -179.899993, 70.299980, ang, .playerid = playerid, .streamdistance = 250.0);
    } else {
        return CreateDynamicObject(1318, x, y, z + 0.5, 0.0, 0.0, 0.0, .playerid = playerid, .streamdistance = 250.0);
    }
}

stock GPS_UpdateArrows(playerid, currentIndex) {
    new bool:kept[MAX_GPS_ARROWS];
    new targetIndices[MAX_GPS_ARROWS];
    new targetCount = 0;
    
    new pathSize = PlayerGPSPathSize[playerid];
    for (new i = 0; i < MAX_GPS_ARROWS; i++) {
        new idx = currentIndex + i;
        if (idx < pathSize) {
            targetIndices[targetCount++] = idx;
        }
    }
    
    // 1. Mark existing objects that we can keep
    for (new i = 0; i < MAX_GPS_ARROWS; i++) {
        if (PlayerGPSArrows[playerid][i] != -1) {
            new currentIdx = PlayerGPSArrowIdx[playerid][i];
            new bool:found = false;
            for (new j = 0; j < targetCount; j++) {
                if (currentIdx == targetIndices[j]) {
                    kept[i] = true;
                    found = true;
                    break;
                }
            }
            if (!found) {
                DestroyDynamicObject(PlayerGPSArrows[playerid][i]);
                PlayerGPSArrows[playerid][i] = -1;
                PlayerGPSArrowIdx[playerid][i] = -1;
            }
        }
    }
    
    // 2. Form new objects for target indices that aren't rendered
    for (new j = 0; j < targetCount; j++) {
        new targetIdx = targetIndices[j];
        new bool:alreadyRendered = false;
        
        for (new i = 0; i < MAX_GPS_ARROWS; i++) {
            if (kept[i] && PlayerGPSArrowIdx[playerid][i] == targetIdx) {
                alreadyRendered = true;
                break;
            }
        }
        
        if (!alreadyRendered) {
            for (new i = 0; i < MAX_GPS_ARROWS; i++) {
                if (PlayerGPSArrows[playerid][i] == -1) {
                    PlayerGPSArrows[playerid][i] = GPS_CreateArrowObject(playerid, targetIdx);
                    PlayerGPSArrowIdx[playerid][i] = targetIdx;
                    kept[i] = true;
                    break;
                }
            }
        }
    }
}

public GPS_UpdateDistance(playerid) {
    if(!IsPlayerConnected(playerid) || PlayerGPSCheckpoint[playerid] == -1) {
        if(PlayerGPSTimer[playerid] != -1) {
            KillTimer(PlayerGPSTimer[playerid]);
            PlayerGPSTimer[playerid] = -1;
        }
        return 0;
    }

    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    new Float:dist = GetPlayerDistanceFromPoint(playerid, PlayerGPSTargetX[playerid], PlayerGPSTargetY[playerid], PlayerGPSTargetZ[playerid]);
    GPS_UI_Update(playerid, dist);

    // If extremely close, rely on DynamicCP hook
    if (dist <= 15.0) return 1;

    if (PlayerGPSRecalculating[playerid]) return 1;
    if (!IsValidPath(PlayerGPSPath[playerid])) return 1;

    new pathSize = PlayerGPSPathSize[playerid];
    new bestIndex = -1;
    new Float:minDist = 9999.0;
    
    new startIndex = PlayerGPSCurrentNode[playerid] - 3;
    if (startIndex < 0) startIndex = 0;
    new endIndex = PlayerGPSCurrentNode[playerid] + 15;
    if (endIndex > pathSize) endIndex = pathSize;
    
    for (new i = startIndex; i < endIndex; i++) {
        new MapNode:node;
        if (GetPathNode(PlayerGPSPath[playerid], i, node) == GPS_ERROR_NONE) {
            new Float:ndist;
            if (GetMapNodeDistanceFromPoint(node, px, py, pz, ndist) == GPS_ERROR_NONE) {
                if (ndist < minDist) {
                    minDist = ndist;
                    bestIndex = i;
                }
            }
        }
    }
    
    if (bestIndex != -1 && minDist < 45.0) {
        if (bestIndex != PlayerGPSCurrentNode[playerid]) {
            PlayerGPSCurrentNode[playerid] = bestIndex;
            GPS_UpdateArrows(playerid, PlayerGPSCurrentNode[playerid]);
        }
    } else {
        // Check if player is completely off-route
        new MapNode:closest;
        if (GetClosestMapNodeToPoint(px, py, pz, closest) == GPS_ERROR_NONE) {
            new Float:cdist;
            if (GetMapNodeDistanceFromPoint(closest, px, py, pz, cdist) == GPS_ERROR_NONE && cdist < 45.0) {
                PlayerGPSRecalculating[playerid] = true;
                new MapNode:target;
                if (GetClosestMapNodeToPoint(PlayerGPSTargetX[playerid], PlayerGPSTargetY[playerid], PlayerGPSTargetZ[playerid], target) == GPS_ERROR_NONE) {
                    FindPathThreaded(closest, target, "OnGPSPathCalculated", "i", playerid);
                } else {
                    PlayerGPSRecalculating[playerid] = false;
                }
            }
        }
    }
    return 1;
}

stock GPS_ClearLocation(playerid) {
    if(PlayerGPSCheckpoint[playerid] != -1) {
        if(IsValidDynamicCP(PlayerGPSCheckpoint[playerid])) {
            DestroyDynamicCP(PlayerGPSCheckpoint[playerid]);
        }
        PlayerGPSCheckpoint[playerid] = -1;
    }

    for (new i = 0; i < MAX_GPS_ARROWS; i++) {
        if (PlayerGPSArrows[playerid][i] != -1) {
            DestroyDynamicObject(PlayerGPSArrows[playerid][i]);
            PlayerGPSArrows[playerid][i] = -1;
        }
        PlayerGPSArrowIdx[playerid][i] = -1;
    }

    if (PlayerGPSPath[playerid] != INVALID_GPS_PATH_ID) {
        if (IsValidPath(PlayerGPSPath[playerid])) {
            DestroyPath(PlayerGPSPath[playerid]);
        }
        PlayerGPSPath[playerid] = INVALID_GPS_PATH_ID;
    }

    if(PlayerGPSTimer[playerid] != -1) {
        KillTimer(PlayerGPSTimer[playerid]);
        PlayerGPSTimer[playerid] = -1;
    }

    PlayerGPSRecalculating[playerid] = false;
    PlayerGPSName[playerid][0] = EOS;
    GPS_UI_Hide(playerid);
}

public OnGPSPathCalculated(Path:pathid, playerid) {
    PlayerGPSRecalculating[playerid] = false;

    if (!IsPlayerConnected(playerid) || PlayerGPSCheckpoint[playerid] == -1) {
        if (IsValidPath(pathid)) DestroyPath(pathid);
        return;
    }

    if (!IsValidPath(pathid)) {
        SendServerMsg(playerid, "Khong tim thay tuyen GPS. Dang hien dich den truc tiep.");
        return;
    }

    if (IsValidPath(PlayerGPSPath[playerid])) {
        DestroyPath(PlayerGPSPath[playerid]);
    }
    for (new i = 0; i < MAX_GPS_ARROWS; i++) {
        if (PlayerGPSArrows[playerid][i] != -1) {
            DestroyDynamicObject(PlayerGPSArrows[playerid][i]);
            PlayerGPSArrows[playerid][i] = -1;
        }
        PlayerGPSArrowIdx[playerid][i] = -1;
    }

    PlayerGPSPath[playerid] = pathid;
    GetPathSize(pathid, PlayerGPSPathSize[playerid]);
    
    PlayerGPSCurrentNode[playerid] = 0;
    
    if (PlayerGPSPathSize[playerid] > 1) {
        new MapNode:nodeid;
        new Float:x, Float:y, Float:z;
        GetPathNode(pathid, 0, nodeid);
        GetMapNodePos(nodeid, x, y, z);
        if (GetPlayerDistanceFromPoint(playerid, x, y, z) < 25.0) {
            PlayerGPSCurrentNode[playerid] = 1;
        }
    }

    GPS_UpdateArrows(playerid, PlayerGPSCurrentNode[playerid]);
}

stock GPS_SetLocation(playerid, Float:x, Float:y, Float:z, const string:name[]) {
    GPS_ClearLocation(playerid);

    PlayerGPSCheckpoint[playerid] = CreateDynamicCP(x, y, z, 5.0, .worldid = -1, .interiorid = -1, .playerid = playerid, .streamdistance = GPS_STREAMDISTANCE);

    PlayerGPSTargetX[playerid] = x;
    PlayerGPSTargetY[playerid] = y;
    PlayerGPSTargetZ[playerid] = z;

    format(PlayerGPSName[playerid], 32, "%s", name);
    GPS_UI_Show(playerid, name);
    PlayerGPSTimer[playerid] = SetTimerEx("GPS_UpdateDistance", 1000, true, "d", playerid);

    new MapNode:start, MapNode:target;
    new Float:px, Float:py, Float:pz;
    GetPlayerPos(playerid, px, py, pz);

    PlayerGPSRecalculating[playerid] = true;
    if (GetClosestMapNodeToPoint(px, py, pz, start) == GPS_ERROR_NONE && GetClosestMapNodeToPoint(x, y, z, target) == GPS_ERROR_NONE) {
        FindPathThreaded(start, target, "OnGPSPathCalculated", "i", playerid);
    } else {
        PlayerGPSRecalculating[playerid] = false;
    }

    new msg[64];
    format(msg, sizeof(msg), "Da dat GPS den %s. Dang tinh duong...", name);
    SendServerMsg(playerid, msg);
}
