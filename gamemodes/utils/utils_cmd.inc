#include <sscanf2>


//ALT
#define ALTCMD:%1(%2); CMD:%1(playerid, params[]) return cmd_%2(playerid, params);

// NORMAL
#define CMD:%1(%2) forward cmd_%1(%2); public cmd_%1(%2)

// ADMIN
_@ACMD_CHECK_ADM(playerid, const cmd[], const params[], const plevel) {
    if(Admin_GetPlayerLevel(playerid) < plevel) {
        return false;
    }
    SendAdminCmdUsage(playerid, cmd, params);
    return true;
}
#define @ACMD_PARAMS_REPLACE:%0[] %0
#define ACMD:[%0]%1(%2,%3) CMD:%1(%2,%3)if(!_@ACMD_CHECK_ADM(%2,#%1,@ACMD_PARAMS_REPLACE:%3,%0))return 0;else


forward OnPlayerCommandReceived(playerid, cmd[]);
forward OnPlayerCommandPerformed(playerid, cmd[], success);

public OnPlayerCommandText(playerid, cmdtext[])
{
    static cmd[28], funcname[32];
    static params[126];

    printf("[CMD Debug] OnPlayerCommandText entered: playerid=%d raw='%s'", playerid, cmdtext);

    params = "\1";
    sscanf(cmdtext[1], "s[28]s[126]", cmd, params);

    for (new i = strlen(cmd); --i != -1;)
    {
        cmd[i] |= 0x20;
    }

    printf("[CMD Debug] Parsed command: cmd='%s' params='%s'", cmd, params);

    if (funcidx("OnPlayerCommandReceived") != -1)
    {
        if (!CallLocalFunction("OnPlayerCommandReceived", "is", playerid, cmd))
        {
            printf("[CMD Debug] OnPlayerCommandReceived rejected cmd='%s'", cmd);
            return 1;
        }
    }

    // Build the full public function name ("cmd_" + command name) and dispatch.
    format(funcname, sizeof(funcname), "cmd_%s", cmd);
    new success = CallLocalFunction(funcname, "is", playerid, (params[0] != '\0') ? (params) : ("\1"));
    printf("[CMD Debug] CallLocalFunction returned %d for cmd='%s'", success, cmd);

    if (funcidx("OnPlayerCommandPerformed") != -1)
    {
        return CallLocalFunction("OnPlayerCommandPerformed", "isi", playerid, cmd, success);
    }
    return success;
}

stock CommandExecute(playerid, const cmd[], const params[] = "\1", {GLOBAL_TAG_TYPTES, _}:...) {
    if(cmd[0] == '/') {
        Logger_Err(
            "[CommandExecute] contains '/' (slash) - remove slash!",
                Logger_P(playerid),
                Logger_S("command", cmd)
        );
        return 0;
    }
    if(!CommandExists(cmd)) {
        return 0;
    }

    new
        prefix[] = "cmd_",
        cmd_name[28];

    strcat(cmd_name, prefix);
    strcat(cmd_name, cmd);

    new
        tmp[32];
    formatex(tmp, sizeof(tmp), params, ___(3));
    CallLocalFunction(cmd_name, "ds", playerid, tmp);
    return 1;
}

stock CommandExists(const cmd[]) {
    new
        prefix[] = "cmd_",
        cmd_name[28];

    strcat(cmd_name, prefix);
    strcat(cmd_name, cmd);

    return (funcidx(cmd_name) != -1);
}
