#include <YSI_Coding\y_hooks>

enum E_OBJECT_INTERACTION_DATA {
    E_OBJECT_NAME[32],
    E_OBJECT_MESSAGE[128],
    E_OBJECT_MODEL,
    Float:E_OBJECT_X,
    Float:E_OBJECT_Y,
    Float:E_OBJECT_Z,
    Float:E_OBJECT_A
}

static const ObjectInteractionData[][E_OBJECT_INTERACTION_DATA] = {
    {"Bang thong tin", "Dung /licenses, /jobxp, /phone, /interact de xem cac he thong moi.", 1239, 1483.8500, -1745.3000, 13.5469, 0.0},
    {"May thong bao viec lam", "Den bang viec lam gan Toa Thi Chinh de nhan viec phu co level va giay phep.", 1274, 1485.5000, -1745.3000, 13.5469, 0.0}
};

static ObjectInteractionArea[sizeof(ObjectInteractionData)];

hook OnGameModeInit()
{
    new label[160];

    for(new i; i < sizeof(ObjectInteractionData); i++) {
        Interaction_BuildPointLabel(label, sizeof(label), "Vat the", ObjectInteractionData[i][E_OBJECT_NAME], "San sang tuong tac", "xem thong tin");

        CreateDynamicObject(
            ObjectInteractionData[i][E_OBJECT_MODEL],
            ObjectInteractionData[i][E_OBJECT_X],
            ObjectInteractionData[i][E_OBJECT_Y],
            ObjectInteractionData[i][E_OBJECT_Z] - 0.3,
            0.0,
            0.0,
            ObjectInteractionData[i][E_OBJECT_A]
        );

        ObjectInteractionArea[i] = CreateDynamicCircle(ObjectInteractionData[i][E_OBJECT_X], ObjectInteractionData[i][E_OBJECT_Y], 2.0, 0, 0);
        CreateDynamic3DTextLabel(
            label,
            COLOR_ORANGE,
            ObjectInteractionData[i][E_OBJECT_X],
            ObjectInteractionData[i][E_OBJECT_Y],
            ObjectInteractionData[i][E_OBJECT_Z] + 0.8,
            INTERACTION_LABEL_DISTANCE,
            .streamdistance = INTERACTION_LABEL_DISTANCE
        );
    }
    return 1;
}

hook OnPlayerEnterDynArea(playerid, areaid)
{
    for(new i; i < sizeof(ObjectInteractionData); i++) {
        if(areaid == ObjectInteractionArea[i]) {
            Interaction_ShowPrompt(playerid, INTERACTION_TYPE_OBJECT, i, "~y~~h~Vat the", "xem thong tin");
            break;
        }
    }
    return 1;
}

hook OnPlayerLeaveDynArea(playerid, areaid)
{
    for(new i; i < sizeof(ObjectInteractionData); i++) {
        if(areaid == ObjectInteractionArea[i]) {
            Interaction_ClearPrompt(playerid, INTERACTION_TYPE_OBJECT, i);
            break;
        }
    }
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if(PRESSED(KEY_NO) && Interaction_IsActive(playerid, INTERACTION_TYPE_OBJECT)) {
        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_OBJECT, Interaction_GetActiveID(playerid));
        return 1;
    }

    if(PRESSED(KEY_YES) && Interaction_IsActive(playerid, INTERACTION_TYPE_OBJECT)) {
        new id = Interaction_GetActiveID(playerid);
        Interaction_ClearPrompt(playerid, INTERACTION_TYPE_OBJECT, id);
        SendServerMsg(playerid, ObjectInteractionData[id][E_OBJECT_MESSAGE]);
    }
    return 1;
}
