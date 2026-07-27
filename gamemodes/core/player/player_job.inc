#include <YSI_Coding\y_hooks>

static
    PlayerJob[MAX_PLAYERS],
    bool:PlayerNew[MAX_PLAYERS],
    Statement:stmt_savePlayerJob;

static const JOB_LIST[] = "\
    Buon Ma Tuy\n\
    Buon Vu Khi\n\
    Hitman\n\
    Khung Bo\n\
    Ke Truyen Benh"
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
                title = "~r~Buon Ma Tuy";
                objective = "Muc tieu cua ban la san xuat va ban ma tuy de kiem loi.";
                jobID = TYPE_DRUGDEALER;
            }
            case TYPE_WEAPONDEALER: {
                title = "~r~Buon Vu Khi";
                objective = "Muc tieu cua ban la ban cac loai vu khi de kiem tien.";
                jobID = TYPE_WEAPONDEALER;
            }
            case TYPE_HITMAN: {
                title = "~r~Hitman";
                objective = "Muc tieu cua ban la thuc hien hop dong sat thu de kiem tien lon.";
                jobID = TYPE_HITMAN;
            }
            case TYPE_TERRORIST: {
                title = "~r~Khung Bo";
                objective = "Muc tieu cua ban la dat bom vao xe, ngan hang va khu nha tu.";
                jobID = TYPE_TERRORIST;
            }
            case TYPE_RAPIST: {
                title = "~r~Ke Truyen Benh";
                objective = "Muc tieu cua ban la lam nguoi choi khac nhiem benh.";
                jobID = TYPE_RAPIST;
            }
            case TYPE_MECHANIC: {
                title = "~r~Tho May";
                objective = "Tho may co the sua xe cua ban va xe cua nguoi choi khac.";
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
        "Buon Ma Tuy",
        "Buon Vu Khi",
        "Hitman",
        "Khung Bo",
        "Ke Truyen Benh",
        "Tho May"
    };

    return job_rankname[Player_GetJob(playerid)];
}

stock Player_GiveJobWeapons(playerid) {
    switch(Player_GetJob(playerid)) {
        case TYPE_DRUGDEALER: {
            GivePlayerWeapon(playerid, WEAPON_COLT45,   200);
            GivePlayerWeapon(playerid, WEAPON_SHOTGUN,  200);
        }
        case TYPE_WEAPONDEALER: {
            GivePlayerWeapon(playerid, WEAPON_M4,       200);
            GivePlayerWeapon(playerid, WEAPON_SHOTGSPA, 200);
        }
        case TYPE_HITMAN: {
            GivePlayerWeapon(playerid, WEAPON_KNIFE,    1);
            GivePlayerWeapon(playerid, WEAPON_SILENCED, 200);
            GivePlayerWeapon(playerid, WEAPON_SNIPER,   200);
        }
        case TYPE_TERRORIST: {
            GivePlayerWeapon(playerid, WEAPON_RIFLE,    200);
            GivePlayerWeapon(playerid, WEAPON_AK47,     200);
        }
        case TYPE_RAPIST: {
            GivePlayerWeapon(playerid, WEAPON_DILDO,    1);
            GivePlayerWeapon(playerid, WEAPON_COLT45,   200);
            GivePlayerWeapon(playerid, WEAPON_SNIPER,   200);
        }
        case TYPE_MECHANIC: {
            GivePlayerWeapon(playerid, WEAPON_AK47,     200);
            GivePlayerWeapon(playerid, WEAPON_SHOTGSPA, 200);
            GivePlayerWeapon(playerid, WEAPON_COLT45,   200);
        }
    }
    return 1;
}
