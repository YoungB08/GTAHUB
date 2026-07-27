CMD:buyhouse(playerid, params[]) {
    new
        houseid = Player_GetInHouseID(playerid);

    if(!Player_InHouseEntrance(playerid, houseid)) {
        return SendErrorMsg(playerid, "Ban khong o gan loi vao nha nao.");
    }

    if(House_HasOwner(houseid)) {
        return SendErrorMsg(playerid, "Nha nay da co nguoi so huu.");
    }

    if(GetPlayerMoney(playerid) < House_GetPrice(houseid)) {
        return SendErrorMsg(playerid, "Ban khong du tien de mua nha nay.");
    }

    if(Player_GetTotalHouses(playerid) >= MAX_HOUSES_PER_PLAYER) {
        return SendErrorMsg(playerid, "Ban da dat gioi han so nha co the so huu.");
    }
    GivePlayerMoney(playerid, -House_GetPrice(houseid));
    SendServerMsgF(playerid, "Chuc mung! Ban da mua nha voi gia "C_GREEN"$%i", House_GetPrice(houseid));
    House_Buy(playerid, houseid);
    return 1;
}

CMD:hgivekey(playerid, params[]) {

    new
        targetid;
    if(sscanf(params, "u", targetid))
        return SendSyntaxMsg(playerid, "/hgivekey <playerid/username>");

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Username / targetid khong hop le.");
    }

    new
        houseid = Player_GetInHouseID(playerid),
        ret = House_SetKeyHolder(targetid, houseid);

    if(!House_PlayerIsOwner(playerid, houseid)) {
        return SendErrorMsg(playerid, "Ban khong phai chu nha nay.");
    }

    if(ret == -1) {
        return SendErrorMsg(playerid, "Nguoi choi nay da co chia khoa.");
    }

    SendServerMsgF(playerid, "Ban da them "C_GREY"%p(%i) "C_WHITE"lam nguoi giu chia khoa.", targetid, targetid);
    SendServerMsgF(targetid, "Ban da duoc them lam nguoi giu chia khoa nha cua "C_GREY"%p(%i)", playerid, playerid);
    return 1;
}

CMD:hremovekey(playerid, params[]) {
    new
        targetid;
    if(sscanf(params, "u", targetid))
        return SendSyntaxMsg(playerid, "/hremovekey <playerid/username>");

    if(!IsPlayerConnected(targetid)) {
        return SendErrorMsg(playerid, "Username / targetid khong hop le.");
    }

    new
        houseid = Player_GetInHouseID(playerid),
        ret = House_RevokeKeyHolder(targetid, houseid);

    if(!House_PlayerIsOwner(playerid, houseid)) {
        return SendErrorMsg(playerid, "Ban khong phai chu nha nay.");
    }

    if(!ret) {
        return SendErrorMsg(playerid, "Nguoi choi nay khong co chia khoa.");
    }

    SendServerMsgF(playerid, "Ban da xoa "C_GREY"%p(%i) "C_WHITE"khoi danh sach giu chia khoa.", targetid, targetid);
    SendServerMsgF(targetid, "Ban da bi xoa khoi danh sach giu chia khoa nha cua "C_GREY"%p(%i)", playerid, playerid);
    return 1;
}


CMD:hlock(playerid, params[]) {
    new
        houseid = Player_GetInHouseID(playerid);

    if(!Player_InHouseEntrance(playerid, houseid)) {
        return SendErrorMsg(playerid, "Ban khong o gan loi vao nha nao.");
    }

    new
        isKeyHolder = House_IsKeyHolder(playerid, houseid);
    printf("%s", isKeyHolder ? "YES" : "NO");
    if(!isKeyHolder) {
        return SendErrorMsg(playerid, "Ban khong co chia khoa nha nay.");
    }

    SendServerMsgF(
        playerid,
        "Ban da %s nha cua minh",
        (House_IsLocked(houseid) ? "mo khoa" : "khoa")
    );

    House_Lock(houseid);
    return 1;
}

CMD:hdescription(playerid, params[]) {
    if(isnull(params)) {
        return SendErrorMsg(playerid, "/hdescription <mo ta moi>");
    }

    new
        houseid = Player_GetInHouseID(playerid);

    if(!Player_IsInsideHouse(playerid)) {
        return SendErrorMsg(playerid, "Ban can o trong nha minh so huu.");
    }

    if(!House_PlayerIsOwner(playerid, houseid)) {
        return SendErrorMsg(playerid, "Ban khong so huu nha nay.");
    }

    new
        ret = House_UpdateDescription(playerid, houseid, params);
    if(!ret) {
        return SendErrorMsg(playerid, "Mo ta khong duoc qua 24 ky tu.");
    }

    SendServerMsgF(playerid, "Ban da doi mo ta nha thanh "C_GREY"%s", params);
    return 1;
}


ACMD:[4]hcreate(playerid, params[])
{
    new
        price,
        interior;

    if(sscanf(params, "ii", price, interior)) {
        return SendSyntaxMsg(playerid, "/hcreate <price> <interior id>");
    }

    if(!(0 <= interior < House_GetInteriorArraySize())){
        return SendErrorMsgF(playerid, "Hay nhap gia tri tu 0 den %i", House_GetInteriorArraySize());
    }

    new
        Float:x,
        Float:y,
        Float:z,
        Float:a;

    GetPlayerPos(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, a);

    new
        ret = House_Create(
        x,
        y,
        z,
        a,
        price,
        interior,
        .save = true
    );

    if(ret == -1) {
        return SendErrorMsg(playerid, "Khong the tao them nha, hay tang MAX_HOUSES.");
    }
    return true;
}

ACMD:[4]hdelete(playerid, params[]) {

    if(isnull(params)) {
        return SendSyntaxMsg(playerid, "/hdelete <house id>");
    }

    if(!IsNumeric(params)) {
        return SendErrorMsg(playerid, "Du lieu nhap khong hop le.");
    }

    new
        houseid = strval(params),
        ret = House_Delete(houseid);

    if(!ret) {
        SendErrorMsgF(playerid, "House ID: ~r~%i ~w~khong ton tai.", houseid);
    } else {
        SendServerMsgF(playerid, "Ban da xoa thanh cong house id "C_GREY"%i", houseid);
    }
    return 1;
}
