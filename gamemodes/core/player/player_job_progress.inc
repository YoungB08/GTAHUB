#include <YSI_Coding\y_hooks>

#define JOB_XP_REQUIRED(%0)     (((%0) + 2) * 250)

enum {
    JOB_PROGRESS_DELIVERY = 0,
    JOB_PROGRESS_TRUCKER,
    JOB_PROGRESS_ROAD_CLEANER,
    JOB_PROGRESS_GARBAGE,
    JOB_PROGRESS_ORANGE_FARM,
    JOB_PROGRESS_PILOT,
    JOB_PROGRESS_MINER,
    JOB_PROGRESS_FISHER,
    JOB_PROGRESS_WAREHOUSE,
    JOB_PROGRESS_POKER,
    JOB_PROGRESS_TOTAL
}

static
    PlayerJobXP[MAX_PLAYERS][JOB_PROGRESS_TOTAL],
    PlayerJobLevel[MAX_PLAYERS][JOB_PROGRESS_TOTAL],
    Statement:stmt_loadJobProgress,
    Statement:stmt_saveJobProgress;

static bool:JobProgress_IsValid(jobid)
{
    return (0 <= jobid < JOB_PROGRESS_TOTAL);
}

hook OnMySQLConnected()
{
    stmt_loadJobProgress = MySQL_PrepareStatement(
        MySQL_GetHandle(),
        "SELECT job_id, job_xp, job_level FROM player_job_progress WHERE u_id = ?"
    );

    stmt_saveJobProgress = MySQL_PrepareStatement(
        MySQL_GetHandle(),
        "INSERT INTO player_job_progress (u_id, job_id, job_xp, job_level) VALUES (?, ?, ?, ?) ON DUPLICATE KEY UPDATE job_xp = ?, job_level = ?"
    );
    return 1;
}

hook OnPlayerConnect(playerid)
{
    for(new i; i < JOB_PROGRESS_TOTAL; i++) {
        PlayerJobXP[playerid][i] = 0;
        PlayerJobLevel[playerid][i] = 1;
    }
    return 1;
}

hook OnPlayerLogin(playerid)
{
    JobProgress_Load(playerid);
    return 1;
}

static JobProgress_Load(playerid)
{
    inline OnJobProgressLoad()
    {
        new
            jobid,
            jobxp,
            joblevel;

        MySQL_BindResultInt(stmt_loadJobProgress, 0, jobid);
        MySQL_BindResultInt(stmt_loadJobProgress, 1, jobxp);
        MySQL_BindResultInt(stmt_loadJobProgress, 2, joblevel);

        while(MySQL_Statement_FetchRow(stmt_loadJobProgress)) {
            if(JobProgress_IsValid(jobid)) {
                PlayerJobXP[playerid][jobid] = jobxp;
                PlayerJobLevel[playerid][jobid] = max(1, joblevel);
            }
        }
    }

    MySQL_BindInt(stmt_loadJobProgress, 0, Player_GetAccountID(playerid));
    MySQL_ExecuteParallel_Inline(stmt_loadJobProgress, using inline OnJobProgressLoad);
    return 1;
}

stock JobProgress_Save(playerid, jobid)
{
    if(!JobProgress_IsValid(jobid) || Player_GetAccountID(playerid) <= 0) {
        return 0;
    }

    MySQL_BindInt(stmt_saveJobProgress, 0, Player_GetAccountID(playerid));
    MySQL_BindInt(stmt_saveJobProgress, 1, jobid);
    MySQL_BindInt(stmt_saveJobProgress, 2, PlayerJobXP[playerid][jobid]);
    MySQL_BindInt(stmt_saveJobProgress, 3, PlayerJobLevel[playerid][jobid]);
    MySQL_BindInt(stmt_saveJobProgress, 4, PlayerJobXP[playerid][jobid]);
    MySQL_BindInt(stmt_saveJobProgress, 5, PlayerJobLevel[playerid][jobid]);
    MySQL_ExecuteThreaded(stmt_saveJobProgress);
    return 1;
}

stock JobProgress_GetName(jobid, output[], len)
{
    switch(jobid) {
        case JOB_PROGRESS_DELIVERY: format(output, len, "Delivery");
        case JOB_PROGRESS_TRUCKER: format(output, len, "Trucker");
        case JOB_PROGRESS_ROAD_CLEANER: format(output, len, "Quet duong");
        case JOB_PROGRESS_GARBAGE: format(output, len, "Don rac");
        case JOB_PROGRESS_ORANGE_FARM: format(output, len, "Trang trai cam");
        case JOB_PROGRESS_PILOT: format(output, len, "Pilot");
        case JOB_PROGRESS_MINER: format(output, len, "Miner");
        case JOB_PROGRESS_FISHER: format(output, len, "Fisher");
        case JOB_PROGRESS_WAREHOUSE: format(output, len, "Warehouse");
        case JOB_PROGRESS_POKER: format(output, len, "Poker");
        default: format(output, len, "Khong xac dinh");
    }
    return 1;
}

stock JobProgress_GetXP(playerid, jobid)
{
    if(!IsPlayerConnected(playerid) || !JobProgress_IsValid(jobid)) {
        return 0;
    }
    return PlayerJobXP[playerid][jobid];
}

stock JobProgress_GetLevel(playerid, jobid)
{
    if(!IsPlayerConnected(playerid) || !JobProgress_IsValid(jobid)) {
        return 0;
    }
    return PlayerJobLevel[playerid][jobid];
}

stock JobProgress_AddXP(playerid, jobid, amount)
{
    if(!IsPlayerConnected(playerid) || !JobProgress_IsValid(jobid) || amount <= 0) {
        return 0;
    }

    PlayerJobXP[playerid][jobid] += amount;

    while(PlayerJobXP[playerid][jobid] >= JOB_XP_REQUIRED(PlayerJobLevel[playerid][jobid])) {
        PlayerJobXP[playerid][jobid] -= JOB_XP_REQUIRED(PlayerJobLevel[playerid][jobid]);
        PlayerJobLevel[playerid][jobid]++;

        new jobName[32];
        JobProgress_GetName(jobid, jobName, sizeof(jobName));
        SendServerMsgF(playerid, "Nghe %s cua ban da len level %d.", jobName, PlayerJobLevel[playerid][jobid]);
        PlayerPlaySound(playerid, 1057, 0.0, 0.0, 0.0);
    }

    if(amount >= 2) {
        GivePlayerXp(playerid, amount / 2);
    }

    JobProgress_Save(playerid, jobid);
    return 1;
}

stock JobProgress_CalculateReward(playerid, jobid, baseReward)
{
    if(baseReward <= 0) {
        return 0;
    }

    new bonusPercent = 0;

    if(JobProgress_IsValid(jobid)) {
        bonusPercent += min((JobProgress_GetLevel(playerid, jobid) - 1) * 5, 50);
    }

    if(License_PlayerHas(playerid, LICENSE_JOB)) {
        bonusPercent += 5;
    }

    bonusPercent += min(VIP_GetPlayerLevel(playerid) * 10, 30);

    return baseReward + ((baseReward * bonusPercent) / 100);
}

stock bool:JobProgress_PlayerMeets(playerid, jobid, minLevel = 1, licenseid = LICENSE_NONE)
{
    if(!IsPlayerConnected(playerid) || !JobProgress_IsValid(jobid)) {
        return false;
    }

    if(licenseid != LICENSE_NONE && !License_PlayerHas(playerid, licenseid)) {
        new licenseName[32];
        License_GetName(licenseid, licenseName, sizeof(licenseName));
        SendErrorMsgF(playerid, "Ban can %s de lam cong viec nay.", licenseName);
        return false;
    }

    if(JobProgress_GetLevel(playerid, jobid) < minLevel) {
        new jobName[32];
        JobProgress_GetName(jobid, jobName, sizeof(jobName));
        SendErrorMsgF(playerid, "Can level %d nghe %s de mo khoa cong viec nay.", minLevel, jobName);
        return false;
    }
    return true;
}

stock JobProgress_BuildList(playerid, output[], len)
{
    output[0] = '\0';

    new
        jobName[32],
        line[96];

    strcat(output, "Nghe\tLevel\tXP\n", len);

    for(new i; i < JOB_PROGRESS_TOTAL; i++) {
        JobProgress_GetName(i, jobName, sizeof(jobName));
        format(
            line,
            sizeof(line),
            "%s\t%d\t%d/%d\n",
            jobName,
            JobProgress_GetLevel(playerid, i),
            JobProgress_GetXP(playerid, i),
            JOB_XP_REQUIRED(JobProgress_GetLevel(playerid, i))
        );
        strcat(output, line, len);
    }
    return 1;
}

stock JobProgress_Show(playerid)
{
    new list[1024];
    JobProgress_BuildList(playerid, list, sizeof(list));
    Dialog_Show(playerid, DIALOG_STYLE_TABLIST_HEADERS, "Level nghe nghiep", list, "Dong", "");
    return 1;
}

CMD:jobxp(playerid, params[])
{
    #pragma unused params
    JobProgress_Show(playerid);
    return 1;
}
