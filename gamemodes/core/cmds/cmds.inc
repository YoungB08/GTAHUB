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
#include <cmds_role> // test role commands & callbacks
#include <cmds_cef_test> // lenh test tat ca CEF UI

static
	cmd_Cooldown[MAX_PLAYERS],
	cmd_Count[MAX_PLAYERS];

public OnPlayerCommandReceived(playerid, cmd[])
{
    // Cho phep lenh test CEF va Help hoat dong
    if (strequal(cmd, "cef") || strequal(cmd, "testcef") || strequal(cmd, "help") || strequal(cmd, "commands") || strequal(cmd, "trogiup"))
    {
        return 1;
    }

    // Tất cả người chơi đã log in và active nhân vật đều được dùng lệnh
    if(!Player_IsLoggedIn(playerid) || !Char_IsActive(playerid))
    {
        SendErrorMsg(playerid, "Ban phai dang nhap va chon nhan vat de dung lenh.");
        return 0;
    }

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

CMD:help(playerid, params[]) {
    #pragma unused params
    SendServerMsg(playerid, "===== DANH SACH LENH GTAHUB =====");
    SendClientMessage(playerid, -1, "{FFA500}Co ban: {FFFFFF}/help, /v, /stats, /info, /pm, /r");
    SendClientMessage(playerid, -1, "{FFA500}CEF UI: {FFFFFF}/cef [login|hud|bank|inv|radial|phone|licenses]");
    SendClientMessage(playerid, -1, "{FFA500}Trang thai: {FFFFFF}Dang mo bang Menu Tro Giup...");
    return cmd_commands(playerid, params);
}
ALTCMD:cmds(help);
ALTCMD:trogiup(help);

CMD:commands(playerid, params[]) {
    #pragma unused params
    inline _response(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, inputtext
        if (!response) {
            return 0;
        }

        switch(listitem) {
            case 0: _HelpMenu_General(pid);
            case 1: _HelpMenu_Jobs(pid);
            case 2: _HelpMenu_SideJobs(pid);
            case 3: _HelpMenu_House(pid);
            case 4: _HelpMenu_Gang(pid);
            case 5: _HelpMenu_Attachment(pid);
            case 6: _HelpMenu_Animations(pid);
        }
    }
    Dialog_ShowCallback(
        playerid,
        using inline _response,
        DIALOG_STYLE_LIST,
        ""C_ORANGE"Tro Giup & Lenh GTAHUB",
        ""C_WHITE"Chung & Lien Lac\n\
        "C_WHITE"Nghe Nghiep Hop Phap\n\
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
        "C_GREY"/re <noi dung> "C_WHITE"- Tra loi PM gan nhat nhan duoc.\n\
        "C_GREY"/r <noi dung> "C_WHITE"- Chat Global toan server.\n\
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
        "C_GREY"/phone "C_WHITE"- Mo dien thoai di dong.\n\
        "C_GREY"/interact [id] "C_WHITE"- Tuong tac voi nguoi choi gan ban.\n");

    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        CommandExecute(pid, "commands");
    }
    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX, ""C_ORANGE"Chung & Lien Lac", string, "Quay lai", "");
    return 1;
}

// ---------------------------------------------------------------------------
// JOBS
// ---------------------------------------------------------------------------

static _HelpMenu_Jobs(playerid) {
    inline _back(pid, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused dialogid, response, listitem, inputtext
        CommandExecute(pid, "commands");
    }

    Dialog_ShowCallback(playerid, using inline _back, DIALOG_STYLE_MSGBOX,
        ""C_ORANGE"Nghe Nghiep Hop Phap",
        ""C_GREY"/sidejobs "C_WHITE"- Mo bang viec lam hop phap.\n\
        "C_GREY"/jobxp "C_WHITE"- Xem level va XP tung nghe.\n\
        "C_GREY"/drivingtest "C_WHITE"- Dang ky thi giay phep.\n\
        "C_GREY"/mechfix "C_WHITE"- Sua xe neu ban la tho may.\n\n\
        "C_ORANGE"Ghi chu: "C_WHITE"Doi nghe tai Toa Thi Chinh.",
        "Quay lai", ""
    );
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
        "C_GREY"/buyfarm "C_WHITE"- Mua dung cu nong trai gan NPC Nguoi Ban Vat Tu.\n\
        "C_GREY"/sellfarm "C_WHITE"- Ban nong san gan NPC Nguoi Mua Nong San.\n\n");

    strcat(string, ""C_ORANGE"Khac\n\
        "C_WHITE"- Luc thung rac: tim do trong thung rac quanh ban do.\n\
        "C_WHITE"- Nghien xe: ban xe phe lieu tai Willowfield.\n");

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
