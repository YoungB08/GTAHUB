#include <server_entrances>
#include <server_wanted-level>
#include <server_time>
#include <server_shop-items>
#include <server_msgs>
#include <server_motd>
#include <server_tax>

#include <server_atm-impl>
#include <server_atm-menu>

#include <dealership>

#include <YSI_Coding\y_hooks>
// Static storeData has been migrated to MySQL table 'doors'.
hook OnGameModeInit() {
    return 1;
}

static task _ServerSecondTimer[1000]() {
    CallRemoteFunction("OnServerSecondUpdate", "");
}





























