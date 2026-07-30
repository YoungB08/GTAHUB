#include <YSI_Coding\y_hooks>

ACMD:[5]doorcreate(playerid, params[])
{
    new name[64];
    if(sscanf(params, "s[64]", name)) {
        return SendSyntaxMsg(playerid, "/doorcreate (ten door)");
    }

    new Float:x, Float:y, Float:z, Float:a;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);
    new intid = GetPlayerInterior(playerid);
    new vworld = GetPlayerVirtualWorld(playerid);

    new doorid = Door_CreateNew(name, x, y, z, a, intid, vworld, playerid);
    if(doorid <= 0) {
        return SendErrorMsg(playerid, "Khong the tao door moi.");
    }

    SendServerMsgF(playerid, "Da tao Door ID %d ('%s') tai vi tri hien tai (Exterior). Hay dung /doorinterior %d de dat vi tri interior.", doorid, name, doorid);
    return 1;
}

ACMD:[5]doorexterior(playerid, params[])
{
    new doorid;
    if(sscanf(params, "i", doorid)) {
        return SendSyntaxMsg(playerid, "/doorexterior (doorid)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    new Float:x, Float:y, Float:z, Float:a;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);
    new intid = GetPlayerInterior(playerid);
    new vworld = GetPlayerVirtualWorld(playerid);

    Door_SetExteriorPos(doorid, x, y, z, a, intid, vworld);
    SendServerMsgF(playerid, "Da cap nhat vi tri Exterior cho Door ID %d thanh cong.", doorid);
    return 1;
}

ACMD:[5]doorinterior(playerid, params[])
{
    new doorid;
    if(sscanf(params, "i", doorid)) {
        return SendSyntaxMsg(playerid, "/doorinterior (doorid)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    new Float:x, Float:y, Float:z, Float:a;
    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);
    new intid = GetPlayerInterior(playerid);
    new vworld = GetPlayerVirtualWorld(playerid);

    Door_SetInteriorPos(doorid, x, y, z, a, intid, vworld);
    SendServerMsgF(playerid, "Da cap nhat vi tri Interior cho Door ID %d thanh cong.", doorid);
    return 1;
}

ACMD:[5]doorname(playerid, params[])
{
    new doorid, name[64];
    if(sscanf(params, "is[64]", doorid, name)) {
        return SendSyntaxMsg(playerid, "/doorname (doorid) (ten moi)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    Door_SetName(doorid, name);
    SendServerMsgF(playerid, "Da cap nhat ten Door ID %d thanh '%s'.", doorid, name);
    return 1;
}

ACMD:[5]doorlabel(playerid, params[])
{
    new doorid, text[128];
    if(sscanf(params, "is[128]", doorid, text)) {
        return SendSyntaxMsg(playerid, "/doorlabel (doorid) (noi dung label)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    Door_SetLabel(doorid, text);
    SendServerMsgF(playerid, "Da cap nhat text label Door ID %d thanh '%s'.", doorid, text);
    return 1;
}

ACMD:[5]doorlock(playerid, params[])
{
    new doorid;
    if(sscanf(params, "i", doorid)) {
        return SendSyntaxMsg(playerid, "/doorlock (doorid)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    Door_SetLocked(doorid, true);
    SendServerMsgF(playerid, "Da KHOA Door ID %d.", doorid);
    return 1;
}

ACMD:[5]doorunlock(playerid, params[])
{
    new doorid;
    if(sscanf(params, "i", doorid)) {
        return SendSyntaxMsg(playerid, "/doorunlock (doorid)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    Door_SetLocked(doorid, false);
    SendServerMsgF(playerid, "Da MO KHOA Door ID %d.", doorid);
    return 1;
}

ACMD:[5]doortype(playerid, params[])
{
    new doorid, type;
    if(sscanf(params, "ii", doorid, type)) {
        return SendSyntaxMsg(playerid, "/doortype (doorid) (type_id)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    Door_SetType(doorid, type);
    SendServerMsgF(playerid, "Da cap nhat Door Type cho Door ID %d thanh %d.", doorid, type);
    return 1;
}

ACMD:[5]doorpickup(playerid, params[])
{
    new doorid, model;
    if(sscanf(params, "ii", doorid, model)) {
        return SendSyntaxMsg(playerid, "/doorpickup (doorid) (pickup_model)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    Door_SetPickupModel(doorid, model);
    SendServerMsgF(playerid, "Da cap nhat Pickup Model cho Door ID %d thanh %d.", doorid, model);
    return 1;
}

ACMD:[5]doorenable(playerid, params[])
{
    new doorid;
    if(sscanf(params, "i", doorid)) {
        return SendSyntaxMsg(playerid, "/doorenable (doorid)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    Door_SetEnabled(doorid, true);
    SendServerMsgF(playerid, "Da ENABLE Door ID %d.", doorid);
    return 1;
}

ACMD:[5]doordisable(playerid, params[])
{
    new doorid;
    if(sscanf(params, "i", doorid)) {
        return SendSyntaxMsg(playerid, "/doordisable (doorid)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    Door_SetEnabled(doorid, false);
    SendServerMsgF(playerid, "Da DISABLE Door ID %d.", doorid);
    return 1;
}

ACMD:[5]doordelete(playerid, params[])
{
    new doorid;
    if(sscanf(params, "i", doorid)) {
        return SendSyntaxMsg(playerid, "/doordelete (doorid)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    Door_Delete(doorid);
    SendServerMsgF(playerid, "Da XOA VINH VIEN Door ID %d khoi he thong.", doorid);
    return 1;
}

ACMD:[5]doorreload(playerid, params[])
{
    new doorid;
    if(sscanf(params, "i", doorid)) {
        return SendSyntaxMsg(playerid, "/doorreload (doorid)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    Door_Reload(doorid);
    SendServerMsgF(playerid, "Da reload Door ID %d.", doorid);
    return 1;
}

ACMD:[1]gotodoor(playerid, params[])
{
    new doorid, location[16];
    if(sscanf(params, "is[16]", doorid, location)) {
        return SendSyntaxMsg(playerid, "/gotodoor (doorid) (exterior/interior)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    new Float:x, Float:y, Float:z, Float:a, vworld, intid;
    if(!strcmp(location, "interior", true)) {
        Door_GetInteriorPos(doorid, x, y, z, a, intid, vworld);
    }
    else {
        Door_GetExteriorPos(doorid, x, y, z, a, intid, vworld);
    }

    SetPlayerInterior(playerid, intid);
    SetPlayerVirtualWorld(playerid, vworld);
    SetPlayerPos(playerid, x, y, z);
    SetPlayerFacingAngle(playerid, a);
    SetCameraBehindPlayer(playerid);

    SendServerMsgF(playerid, "Da teleport den Door ID %d (%s).", doorid, location);
    return 1;
}

ACMD:[1]doorinfo(playerid, params[])
{
    new doorid;
    if(sscanf(params, "i", doorid)) {
        return SendSyntaxMsg(playerid, "/doorinfo (doorid)");
    }

    if(!Door_IsValid(doorid)) {
        return SendErrorMsg(playerid, "Door ID khong ton tai.");
    }

    Door_ShowInfo(playerid, doorid);
    return 1;
}

ACMD:[1]doorlist(playerid, params[])
{
    #pragma unused params
    Door_ShowList(playerid);
    return 1;
}
