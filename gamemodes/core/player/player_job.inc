#include <YSI_Coding\y_hooks>

static
    PlayerJob[MAX_PLAYERS],
    bool:PlayerNew[MAX_PLAYERS],
    Statement:stmt_savePlayerJob;

static const JOB_LIST[] = "\
    Giao Hang\n\
    Lai Xe Tai\n\
    Tai Xe Taxi\n\
    Nhan Vien Kho\n\
    Nong Dan\n\
    Tho May"
;

// Hook
hook OnMySQLConnected() {
    stmt_savePlayerJob = MySQL_PrepareStatement(MySQL_GetHandle(), "UPDATE player_stats SET job_id = ? WHERE u_id = ?");
    return 1;
}

hook OnPlayerRegister(playerid) {
    PlayerNew[playerid] = true;
    return 1;
}

hook OnPlayerConnect(playerid) {
    PlayerJob[playerid] = TYPE_NO_JOB;
    PlayerNew[playerid] = false;
}

// Functions
Player_SetJob(playerid, jobid, bool:save = false) {


    if(save) {
        MySQL_BindInt(stmt_savePlayerJob, 0, jobid);
        MySQL_BindInt(stmt_savePlayerJob, 1, Player_GetAccountID(playerid));
        MySQL_ExecuteThreaded(stmt_savePlayerJob);
    }

    PlayerJob[playerid] = jobid;
    return 1;
}

Player_GetJob(playerid) {
    return PlayerJob[playerid];
}

Player_ShowJobMenu(playerid) {
    inline _response(ply_id, dialogid, response, listitem, string:inputtext[]) {
        #pragma unused ply_id, dialogid, inputtext
        if (!response) {
            SendErrorMsg(playerid, "Ban phai chon mot nghe!");
            return Player_ShowJobMenu(playerid);
        }

        new
            jobID,
            title[24],
            objective[64 + 24];

        switch(listitem) {
            case TYPE_DRUGDEALER:  {
                title = "~g~Giao Hang";
                objective = "Nhan va giao don hang cho cac cua hang trong thanh pho.";
                jobID = TYPE_DRUGDEALER;
            }
            case TYPE_WEAPONDEALER: {
                title = "~g~Lai Xe Tai";
                objective = "Van chuyen hang hoa bang xe tai va nhan thuong theo chuyen.";
                jobID = TYPE_WEAPONDEALER;
            }
            case TYPE_HITMAN: {
                title = "~g~Tai Xe Taxi";
                objective = "Dua don hanh khach va xay dung nghe lai xe dich vu.";
                jobID = TYPE_HITMAN;
            }
            case TYPE_TERRORIST: {
                title = "~g~Nhan Vien Kho";
                objective = "Sap xep, boc do va hoan thanh cac viec trong kho hang.";
                jobID = TYPE_TERRORIST;
            }
            case TYPE_RAPIST: {
                title = "~g~Nong Dan";
                objective = "Thu hoach nong san, che bien va ban hang de kiem tien.";
                jobID = TYPE_RAPIST;
            }
            case TYPE_MECHANIC: {
                title = "~g~Tho May";
                objective = "Sua chua xe va ho tro nguoi choi tren duong.";
                jobID = TYPE_MECHANIC;
            }
        }
        MessageBox_Show(playerid, TYPE_INFORMATION, title, objective, 1000 * 10);
        Player_SetJob(playerid, jobID, true);
        if(PlayerNew[playerid]) {
            Player_GiveJobWeapons(playerid);
            PlayerNew[playerid] = false;
        }
    }
    Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_LIST, "Menu Nghe", JOB_LIST , "Chon", "Huy");
    return 1;
}

stock Player_GetJobName(playerid)
{
    new job_rankname[6][24] =
    {
        "Giao Hang",
        "Lai Xe Tai",
        "Tai Xe Taxi",
        "Nhan Vien Kho",
        "Nong Dan",
        "Tho May"
    };

    return job_rankname[Player_GetJob(playerid)];
}

stock Player_GiveJobWeapons(playerid) {
    #pragma unused playerid
    return 1;
}
