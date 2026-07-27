
CMD:gangs(playerid, params[]) {
    Gang_ShowGangs(playerid);
    return 1;
}

CMD:gang(playerid, params[]) {
    if(isnull(params))
		return SendSyntaxMsg(playerid, "/gang create | colour | disband | invite | kick | accept");

	if(!strcmp(params, "create", false, 6)) {
		new
			gangName[MAX_GANG_NAME];

		if(sscanf(params, "{s[7]}s["#MAX_GANG_NAME"]", gangName))
			return SendSyntaxMsg(playerid, "/gang create <ten gang>");

    	if(!IS_IN_RANGE(strlen(gangName), 5, MAX_GANG_NAME - 1)) {
			return SendErrorMsg(playerid, "Ten gang phai tu 5 den 16 ky tu.");
        }

        new
            ret = Gang_Create(playerid, gangName);

        if(ret == 0) {
			return SendErrorMsg(playerid, "Ten gang da ton tai.");
        }

		if(ret == -2) {
            return SendErrorMsg(playerid, "Khong the tao them gang, server da dat gioi han. Hay lien he admin.");
        }

        SendMsgF(playerid, COLOR_PINK, "** GANG ** "C_WHITE"Ban da tao thanh cong gang ten "C_PINK"'%s'", gangName);
        SendMsgF(playerid, COLOR_PINK, "** GANG ** "C_WHITE"Ban co the moi thanh vien bang "C_GREY"/gang invite"C_WHITE" va kick bang "C_GREY"/gang kick");
        SendMsgF(playerid, COLOR_PINK, "** GANG ** "C_WHITE"Ban co the doi mau gang bang "C_GREY"/gang colour"C_WHITE", mau mac dinh la 'WHITE'");

        SendMsgAllF(COLOR_PINK, "** GANG ** "C_GREY"%p(%i) "C_WHITE"da tao gang moi ten "C_PINK"'%s'", playerid, playerid, gangName);
	}
    else if(!strcmp(params, "disband", false, 7)){
        new
            ret = Gang_Disband(playerid, Player_GetGangID(playerid));

        if(ret == -1) {
            return SendErrorMsg(playerid, "Ban khong phai leader cua gang nay.");
        }
    }
    else if(!strcmp(params, "leave", false, 5)) {
        new
            ret = Gang_Leave(playerid);

        if(ret == -1) {
            return SendErrorMsg(playerid, "Ban khong o trong gang nao.");
        }

        SendMsgF(playerid, COLOR_PINK, "** GANG ** "C_WHITE"Ban da roi gang thanh cong.");
    }
    else if(!strcmp(params, "invite", false, 6)) {
        new
            targetid;

        if(sscanf(params, "{s[7]}r", targetid))
			return SendSyntaxMsg(playerid, "/gang invite <playerid>");

        new
            ret = Gang_InvitePlayer(playerid, targetid);

        if(!ret) {
            return SendErrorMsg(playerid, "Nguoi choi khong ton tai.");
        }

        if(ret == -1) {
            return SendErrorMsg(playerid, "Nguoi choi nay da o trong gang.");
        }

        if(ret == -2) {
            return SendErrorMsg(playerid, "Ban khong phai leader cua gang nay.");
        }
        SendMsgF(playerid, COLOR_PINK, "** GANG ** "C_WHITE"Ban da moi "C_GREY"%p[%i] "C_WHITE"vao gang.", targetid, targetid);
		SendMsgF(targetid, COLOR_PINK, "** GANG ** "C_WHITE"Ban duoc "C_GREY"%p[%i] "C_WHITE"moi vao gang.", playerid, playerid);
        SendMsgF(targetid, COLOR_PINK, "** GANG ** "C_WHITE"Dung "C_GREY"/gang accept "C_WHITE"de chap nhan loi moi.", playerid, playerid);
    }
    else if(!strcmp(params, "accept", false, 6)) {

        new
            gangID,
            gangColour,
            inviterID,
            gangName[MAX_GANG_NAME],
            ret = Gang_AcceptInvite(playerid, gangID, gangColour, inviterID, gangName);

        if(!ret) {
            return SendErrorMsg(playerid, "Chua ai moi ban vao gang.");
        }

        if(ret == -1) {
            return SendErrorMsg(playerid, "Nguoi da moi ban khong con online.");
        }

        SendMsgF(inviterID, COLOR_PINK, "** GANG ** "C_GREY"%p[%i] "C_WHITE"da chap nhan loi moi vao gang cua ban.", playerid, playerid);
		SendMsgF(playerid, COLOR_PINK, "** GANG ** "C_WHITE"Ban da vao Gang ID: ["C_GREY"%i"C_WHITE"] - Ten Gang: "C_GREY"'%s'", gangID, gangName);
		SetPlayerColor(playerid,  gangColour);

    }
    else if(!strcmp(params, "members", false, 7)) {
        Gang_ShowMembers(playerid);
    }
    return 1;
}
