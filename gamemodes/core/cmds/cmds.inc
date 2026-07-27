#include <cmds_info>
#include <cmds_arrest>
#include <cmds_cuff>
#include <cmds_bail>
#include <cmds_license> // he thong giay phep
#include <cmds_bank>    // chuyen khoan ngan hang RP
#include <cmds_v> // vehicle
#include <cmds_pm> // private message
#include <cmds_gang>

#include <cmds_rob>

#include <cmds_breakcuff>
#include <cmds_breakout>
#include <cmds_ghosthunt>
#include <cmds_gps>
#include <cmds_license> // he thong giay phep
#include <cmds_bank>    // chuyen khoan ngan hang RP
#include <cmds_admin_rp> // ticket, warn, aduty

static
	cmd_Cooldown[MAX_PLAYERS],
	cmd_Count[MAX_PLAYERS];

public OnPlayerCommandReceived(playerid, cmd[])
{
	// if player is not spawned, do not allow him to use commands.
	if(!Player_IsSpawned(playerid))
		return 0;

	// anti command spam
	if(gettime() < cmd_Cooldown[playerid]) {
		cmd_Count[playerid] ++;

		if(cmd_Count[playerid] >= 3) {
	 		SendServerMsgF(playerid, "Dung spam lenh "C_GREY"'/%s' - so lan: %i", cmd, cmd_Count[playerid]);
	 		return 0;
		}
	}
	else {
		if(cmd_Count[playerid] > 0) {
			cmd_Count[playerid] --;
		}
	}

	//get time data everytime a command is recieved.
	cmd_Cooldown[playerid] = gettime() + 3;
	return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], success) {

    if (!success) {
        return SendErrorMsgF(playerid, "Lenh ~r~'/%s' ~w~khong ton tai. Dung ~y~'/help'~w~ de xem danh sach lenh.", cmd);
    }

    //get time data everytime a command is performed.
    cmd_Cooldown[playerid] = gettime() + 3;
    return 1;
}


// =============================================================================
// HELP / COMMANDS MENU
// =============================================================================

ALTCMD:help(commands);
ALTCMD:cmds(commands);
CMD:commands(playerid, params[]) {

    inline _response(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, inputtext
        if (!response) {
            return 0;
        }

        switch(listitem) {
            case 0: _HelpMenu_General(pid);
            case 1: _HelpMenu_LawEnforcement(pid);
            case 2: _HelpMenu_Jobs(pid);
            case 3: _HelpMenu_Criminal(pid);
            case 4: _HelpMenu_SideJobs(pid);
            case 5: _HelpMenu_House(pid);
            case 6: _HelpMenu_Gang(pid);
            case 7: _HelpMenu_Attachment(pid);
            case 8: _HelpMenu_Animations(pid);
        }
    }
    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_LIST,
        ""C_ORANGE"Tro Giup & Lenh GTAHUB",
        ""C_WHITE"Chung & Lien Lac\n\
        "C_WHITE"Luc Luong Chuc Nang (Canh Sat / Quan Doi)\n\
        "C_WHITE"Ky Nang Nghe Nghiep\n\
        "C_WHITE"Hoat Dong Pham Toi\n\
        "C_WHITE"Viec Phu & Hoat Dong\n\
        "C_WHITE"Nha\n\
        "C_WHITE"Gang\n\
        "C_WHITE"Phu Kien & Noi That\n\
        "C_WHITE"Hoat Anh",
        "Chon",
        "Dong"
    );
    return 1;
}

// ---------------------------------------------------------------------------
// GENERAL & COMMUNICATION
// ---------------------------------------------------------------------------

static _HelpMenu_General(playerid) {
    new string[768];

    strcat(string, ""C_ORANGE"Lien Lac\n\
        "C_GREY"/pm <id> <noi dung> "C_WHITE"- Gui tin nhan rieng.\n\
        "C_GREY"/r <noi dung> "C_WHITE"- Tra loi PM gan nhat ban nhan duoc.\n\
        "C_GREY"/dnd "C_WHITE"- Bat/tat che do khong lam phien (chan PM).\n\n");

    strcat(string, ""C_ORANGE"Thong Tin & Chi So\n\
        "C_GREY"/info "C_WHITE"- Xem thong tin va chi so nhan vat.\n\
        "C_GREY"/shots [id] "C_WHITE"- Xem thong ke do chinh xac khi ban.\n\
        "C_GREY"/getlevel "C_WHITE"- Kiem tra level va XP hien tai.\n\n");

    strcat(string, ""C_ORANGE"Phuong Tien\n\
        "C_GREY"/v park "C_WHITE"- Dau xe ca nhan va luu vi tri.\n\
        "C_GREY"/v lock "C_WHITE"- Khoa/mo khoa xe cua ban.\n\n");

    strcat(string, ""C_ORANGE"Khac\n\
        "C_GREY"/lotto <so> "C_WHITE"- Mua ve xo so.\n\
        "C_GREY"/animlist "C_WHITE"- Xem danh sach hoat anh.\n\
        "C_GREY"/surrender "C_WHITE"- Gio tay dau hang.\n");

    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        CommandExecute(pid, "commands");
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX, ""C_ORANGE"Chung & Lien Lac", string, "Quay lai", "");
    return 1;
}

// ---------------------------------------------------------------------------
// LAW ENFORCEMENT (POLICE / ARMY)
// ---------------------------------------------------------------------------

static _HelpMenu_LawEnforcement(playerid) {
    new string[768];

    strcat(string, ""C_ORANGE"Bat Giu & Kiem Soat\n\
        "C_GREY"/ar <id> "C_WHITE"- Bat nguoi dang bi truy na va dua vao tu.\n\
        "C_GREY"/cuff <id> "C_WHITE"- Cong tay nguoi choi gan ban.\n\
        "C_GREY"/uncuff <id> "C_WHITE"- Thao cong tay cho nguoi choi.\n\
        "C_GREY"/cuffcw "C_WHITE"- Cong tay toi pham truy na gan nhat.\n\
        "C_GREY"/arcw "C_WHITE"- Bat toi pham truy na gan nhat.\n\n");

    strcat(string, ""C_ORANGE"Lien Lac & Dieu Khien\n\
        "C_GREY"! <noi dung> "C_WHITE"- Radio noi bo doi.\n\
        "C_GREY"MMB (trong xe) "C_WHITE"- Yeu cau nghi pham tat vao le.\n\
        "C_GREY"MMB (di bo) "C_WHITE"- Cong tay/bat toi pham gan nhat.\n\n");

    strcat(string, ""C_ORANGE"Nhiem Vu Canh Sat\n\
        "C_GREY"/mission "C_WHITE"- Bat dau nhiem vu tuan tra canh sat tai LSPD.\n");

    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        CommandExecute(pid, "commands");
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX, ""C_ORANGE"Lenh Luc Luong Chuc Nang", string, "Quay lai", "");
    return 1;
}

// ---------------------------------------------------------------------------
// JOBS
// ---------------------------------------------------------------------------

static _HelpMenu_Jobs(playerid) {

    inline _response(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, inputtext
        if (!response) {
            // Go back to main help menu
            CommandExecute(pid, "commands");
            return 0;
        }
        switch(listitem) {
            case 0: _HelpMenu_Job_DrugDealer(pid);
            case 1: _HelpMenu_Job_WeaponDealer(pid);
            case 2: _HelpMenu_Job_Hitman(pid);
            case 3: _HelpMenu_Job_Terrorist(pid);
            case 4: _HelpMenu_Job_Rapist(pid);
            case 5: _HelpMenu_Job_Mechanic(pid);
        }
    }

    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_LIST,
        ""C_ORANGE"Ky Nang Nghe Nghiep",
        ""C_WHITE"Buon Ma Tuy\n\
        "C_WHITE"Buon Vu Khi\n\
        "C_WHITE"Hitman\n\
        "C_WHITE"Khung Bo\n\
        "C_WHITE"Ke Truyen Benh\n\
        "C_WHITE"Tho May",
        "Chon",
        "Quay lai"
    );
    return 1;
}

static _HelpMenu_Job_DrugDealer(playerid) {
    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        _HelpMenu_Jobs(pid);
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX,
        ""C_ORANGE"Lenh Buon Ma Tuy",
        ""C_GREY"/plantweed "C_WHITE"- Trong cay can sa.\n\
        "C_GREY"/harvestweed "C_WHITE"- Thu hoach cay can sa cua ban.\n\
        "C_GREY"/sellweed <id> "C_WHITE"- Rao ban weed cho nguoi choi.\n\
        "C_GREY"/sellcrack <id> "C_WHITE"- Rao ban crack cho nguoi choi.\n\n\
        "C_ORANGE"Ghi chu: "C_WHITE"Doi nghe tai City Hall ($7,500).",
        "Quay lai", ""
    );
    return 1;
}

static _HelpMenu_Job_WeaponDealer(playerid) {
    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        _HelpMenu_Jobs(pid);
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX,
        ""C_ORANGE"Lenh Buon Vu Khi",
        ""C_GREY"/sellgun <id> <weaponid> <gia> <ammo> "C_WHITE"- Rao ban vu khi.\n\
        "C_GREY"/acceptgun "C_WHITE"- Chap nhan giao dich vu khi.\n\
        "C_GREY"/sellarmour <id> <gia> <so luong> "C_WHITE"- Rao ban giap.\n\
        "C_GREY"/acceptarmour "C_WHITE"- Chap nhan giao dich giap.\n\n\
        "C_ORANGE"Ghi chu: "C_WHITE"Doi nghe tai City Hall ($7,500).",
        "Quay lai", ""
    );
    return 1;
}

static _HelpMenu_Job_Hitman(playerid) {
    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        _HelpMenu_Jobs(pid);
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX,
        ""C_ORANGE"Lenh Sat Thu",
        ""C_GREY"/hitlist "C_WHITE"- Xem danh sach muc tieu dang co tien thuong.\n\
        "C_GREY"/placehit <id> <so tien> "C_WHITE"- Dat tien thuong len nguoi choi.\n\n\
        "C_ORANGE"Ghi chu: "C_WHITE"Doi nghe tai City Hall ($7,500).",
        "Quay lai", ""
    );
    return 1;
}

static _HelpMenu_Job_Terrorist(playerid) {
    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        _HelpMenu_Jobs(pid);
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX,
        ""C_ORANGE"Lenh Khung Bo",
        ""C_GREY"/plantc4 "C_WHITE"- Dat bom C4.\n\
        "C_GREY"/detonate "C_WHITE"- Kich no C4 da dat.\n\n\
        "C_ORANGE"Mua C4: "C_WHITE"Den Bomb Dealer tren ban do.\n\
        "C_ORANGE"Ghi chu: "C_WHITE"Doi nghe tai City Hall ($7,500).",
        "Quay lai", ""
    );
    return 1;
}

static _HelpMenu_Job_Rapist(playerid) {
    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        _HelpMenu_Jobs(pid);
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX,
        ""C_ORANGE"Lenh Truyen Benh",
        ""C_GREY"/rape <id> "C_WHITE"- Lam nhiem benh nguoi choi gan ban.\n\n\
        "C_ORANGE"Ghi chu: "C_WHITE"Doi nghe tai City Hall ($7,500).",
        "Quay lai", ""
    );
    return 1;
}

static _HelpMenu_Job_Mechanic(playerid) {
    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        _HelpMenu_Jobs(pid);
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX,
        ""C_ORANGE"Lenh Tho May",
        ""C_GREY"/mechfix "C_WHITE"- Sua chua xe ban dang ngoi.\n\n\
        "C_ORANGE"Ghi chu: "C_WHITE"Doi nghe tai City Hall ($7,500).",
        "Quay lai", ""
    );
    return 1;
}

// ---------------------------------------------------------------------------
// CRIMINAL ACTIVITIES
// ---------------------------------------------------------------------------

static _HelpMenu_Criminal(playerid) {
    new string[768];

    strcat(string, ""C_ORANGE"Cuop Boc\n\
        "C_GREY"/rob <id> "C_WHITE"- Cuop nguoi choi gan ban.\n\
        "C_GREY"/robbery "C_WHITE"- Cuop cac diem trong thanh pho.\n\
        "C_GREY"/towerrobbery "C_WHITE"- Thu cuop toa thap.\n\n");

    strcat(string, ""C_ORANGE"Thoat Than & Vuot Nguc\n\
        "C_GREY"/breakcuff "C_WHITE"- Thoat khoi cong tay (can picklock).\n\
        "C_GREY"/breakout "C_WHITE"- Vuot nguc (can picklock).\n\
        "C_GREY"/bail "C_WHITE"- Dong tien bao lanh de ra tu.\n\n");

    strcat(string, ""C_ORANGE"Meo\n\
        "C_WHITE"- Mua picklock tai cua hang 24/7.\n\
        "C_WHITE"- C4 co the pha cua phong giam.\n\
        "C_WHITE"- May nghien xe tai Willowfield tra tien cho xe bi truy na.\n");

    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        CommandExecute(pid, "commands");
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX, ""C_ORANGE"Hoat Dong Pham Toi", string, "Quay lai", "");
    return 1;
}

// ---------------------------------------------------------------------------
// SIDE JOBS & ACTIVITIES
// ---------------------------------------------------------------------------

static _HelpMenu_SideJobs(playerid) {
    new string[768];

    strcat(string, ""C_ORANGE"Lai Xe Tai (Dan Thuong)\n\
        "C_GREY"/trucker "C_WHITE"- Bat dau nhiem vu xe tai khi ngoi trong Mule.\n\n");

    strcat(string, ""C_ORANGE"Giao Pizza (Dan Thuong)\n\
        "C_GREY"/pdelivery "C_WHITE"- Bat dau giao pizza khi ngoi tren Pizzaboy.\n\n");

    strcat(string, ""C_ORANGE"Thu Gom Rac\n\
        "C_GREY"/pickup "C_WHITE"- Nhat rac gan thung rac bang Trashmaster.\n\n");

    strcat(string, ""C_ORANGE"San Ma\n\
        "C_GREY"/starthunt "C_WHITE"- Bat dau phien san ma.\n\
        "C_GREY"/leavehunt "C_WHITE"- Roi phien san ma.\n\n");

    strcat(string, ""C_ORANGE"Nong Trai\n\
        "C_GREY"/buyfarm "C_WHITE"- Mua dung cu nong trai gan NPC Farm Supplier.\n\
        "C_GREY"/sellfarm "C_WHITE"- Ban nong san gan NPC Harvest Master.\n\n");

    strcat(string, ""C_ORANGE"Khac\n\
        "C_WHITE"- Luc thung rac: tim do trong thung rac quanh ban do.\n\
        "C_WHITE"- Nghien xe: ban xe bi truy na tai Willowfield.\n");

    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        CommandExecute(pid, "commands");
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX, ""C_ORANGE"Viec Phu & Hoat Dong", string, "Quay lai", "");
    return 1;
}

// ---------------------------------------------------------------------------
// HOUSE
// ---------------------------------------------------------------------------

static _HelpMenu_House(playerid) {
    new string[512];

    strcat(string, ""C_ORANGE"Lenh Nha\n\
        "C_GREY"/buyhouse "C_WHITE"- Mua nha.\n\
        "C_GREY"/hlock "C_WHITE"- Khoa/mo khoa nha cua ban.\n\
        "C_GREY"/hdescription <noi dung> "C_WHITE"- Doi mo ta nha.\n\
        "C_GREY"/hgivekey <id> "C_WHITE"- Dua chia khoa nha cho nguoi choi.\n\
        "C_GREY"/hremovekey <id> "C_WHITE"- Thu lai chia khoa cua nguoi choi.\n\
        "C_GREY"/furniture "C_WHITE"- Mo menu noi that khi dang trong nha.\n\n");

    strcat(string, ""C_ORANGE"Dieu Khien\n\
        "C_WHITE"~k~~VEHICLE_ENTER_EXIT~ - Vao/ra nha.\n\
        "C_WHITE"~k~~CONVERSATION_NO~ - Mo menu noi that khi dang trong nha.\n");

    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        CommandExecute(pid, "commands");
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX, ""C_ORANGE"Lenh Nha", string, "Quay lai", "");
    return 1;
}

// ---------------------------------------------------------------------------
// GANG
// ---------------------------------------------------------------------------

static _HelpMenu_Gang(playerid) {
    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        CommandExecute(pid, "commands");
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX,
        ""C_ORANGE"Lenh Gang",
        ""C_GREY"/gangs "C_WHITE"- Xem tat ca gang dang hoat dong.\n\
        "C_GREY"/gang create <ten> "C_WHITE"- Tao gang moi.\n\
        "C_GREY"/gang disband "C_WHITE"- Giai tan gang cua ban. "C_YELLOW"(Leader)\n\
        "C_GREY"/gang invite <id> "C_WHITE"- Moi nguoi choi vao gang. "C_YELLOW"(Leader)\n\
        "C_GREY"/gang accept "C_WHITE"- Chap nhan loi moi vao gang.\n\
        "C_GREY"/gang leave "C_WHITE"- Roi gang hien tai.\n",
        "Quay lai", ""
    );
    return 1;
}

// ---------------------------------------------------------------------------
// ATTACHMENTS & FURNITURE
// ---------------------------------------------------------------------------

static _HelpMenu_Attachment(playerid) {
    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        CommandExecute(pid, "commands");
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX,
        ""C_ORANGE"Phu Kien & Noi That",
        ""C_ORANGE"Phu Kien\n\
        "C_GREY"/attachment buy "C_WHITE"- Mua phu kien.\n\
        "C_GREY"/attachment delete "C_WHITE"- Xoa phu kien.\n\
        "C_GREY"/attachment edit "C_WHITE"- Chinh vi tri phu kien.\n\
        "C_GREY"/attachment view "C_WHITE"- Xem danh sach phu kien cua ban.\n\
        "C_GREY"/attachment attach "C_WHITE"- Trang bi phu kien.\n\
        "C_GREY"/attachment detach "C_WHITE"- Thao phu kien.\n\n\
        "C_ORANGE"Noi That\n\
        "C_GREY"/furniture "C_WHITE"- Mo menu noi that khi dang trong nha.\n",
        "Quay lai", ""
    );
    return 1;
}

// ---------------------------------------------------------------------------
// ANIMATIONS
// ---------------------------------------------------------------------------

static _HelpMenu_Animations(playerid) {
    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        CommandExecute(pid, "commands");
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX,
        ""C_ORANGE"Hoat Anh",
        ""C_GREY"/animlist "C_WHITE"- Xem danh sach day du cac lenh hoat anh.\n\n\
        "C_ORANGE"Tham Khao Nhanh\n\
        "C_GREY"/dance <1-3> "C_WHITE"- Hoat anh nhay.\n\
        "C_GREY"/sit <1-5> "C_WHITE"- Hoat anh ngoi.\n\
        "C_GREY"/surrender "C_WHITE"- Gio tay dau hang.\n\
        "C_GREY"/wave "C_WHITE"- Vay tay voi nguoi khac.\n\
        "C_GREY"/smoke <1-2> "C_WHITE"- Hoat anh hut thuoc.\n\
        "C_GREY"/sleep <1-2> "C_WHITE"- Hoat anh ngu.\n\
        "C_GREY"/gesture <1-15> "C_WHITE"- Hoat anh cu chi.\n\
        "C_GREY"/rap <1-3> "C_WHITE"- Hoat anh rap.\n\
        "C_GREY"/cross <1-5> "C_WHITE"- Hoat anh khoanh tay.\n\n\
        "C_ORANGE"Meo: "C_WHITE"Nhan "C_YELLOW"SPRINT "C_WHITE"de dung moi hoat anh.\n",
        "Quay lai", ""
    );
    return 1;
}
