#include <YSI_Coding\y_hooks>


// VERY IMPORTANT!
// Do not change the priority of how the tables are being created
hook OnMySQLSetupTable() {
    Logger_Log("TABLE SETUP BEGIN");
    mysql_tquery_file(MySQL_GetHandle(), "gtahub.sql");
    Logger_Log("setting up all tables from gtahub.sql...");
    Logger_Log("TABLE SETUP END");
    return 1;
}
