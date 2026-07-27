/*
     Written by s4marium

     todo:
     - need to add dynamic trees for this that is connected tree state,
       that can be respawned and destroy.
     - or create an dynamic tree for both labels and model that can be edited ingame,
       just like creating an furniture or atms (database is required for this).
 */

#include <YSI_Coding\y_hooks>

// ====== ORANGE FARM CONSTANTS ======
#define ORANGE_FARM_DEBUG true
#define ORANGE_COLLECTION_TIME 1000
#define ORANGE_PROCESSING_TIME 5000
#define ORANGE_RESPAWN_TIME 15000
#define ORANGE_TO_JUICE_RATIO 5
#define ORANGE_SALE_PRICE_JUICE 100
#define ORANGE_COLLECTION_REWARD_MONEY 300
// ====== ORANGE FARM ENUMS ======
enum E_ORANGE_STATE {
     bool:IsCollecting,
     CurrentTree,
     CollectionTimer
};
// ====== GLOBAL VARIABLES ======
new OrangePlayerState[MAX_PLAYERS][E_ORANGE_STATE];
new bool:TreeAvailable[8];  // Global tree availability (shared by all players)
new Text3D:OrangeTreeLabels[8];
new TreeRespawnTime[8]; // Track respawn timer for each tree
new TreeRespawnTimer[8]; // Timer ID for each tree

// Orange tree locations (Float: X, Y, Z)
new const Float:OrangeTrees[][3] = {
     {-1203.791748, -1209.510904, 129.218750},
     {-1201.816040, -1195.226196, 129.218750},
     {-1200.274902, -1182.626098, 129.218750},
     {-1187.609497, -1182.177490, 129.218750},
     {-1188.238891, -1195.244873, 129.218750},
     {-1189.113525, -1209.489135, 129.218750},
     {-1172.880371, -1195.020385, 129.218750},
     {-1173.075439, -1181.932250, 129.218750}
};

// Processing location
#define ORANGE_PROCESSOR_X -1060.543823
#define ORANGE_PROCESSOR_Y -1205.530273
#define ORANGE_PROCESSOR_Z 129.218750
#define ORANGE_PROCESSOR_RANGE 5.0

// ====== FORWARD DECLARATIONS ======
forward Orange_ProcessComplete(playerid, amount);
forward Orange_RespawnTree(treeIndex);
forward Orange_UpdateTreeTimer(treeIndex);
forward Orange_MiniGame_OnComplete(playerid, customData);
forward Orange_MiniGame_OnFail(playerid, customData);
// ====== INITIALIZATION ======
hook OnGameModeInit() {
     LoadOrangeJob();

     // Initialize respawn times and tree availability
     for(new i = 0; i < 8; i++) {
          TreeRespawnTime[i] = 0;
          TreeRespawnTimer[i] = 0;
          TreeAvailable[i] = true;  // Initialize all trees as available
     }
     return 1;
}

hook OnPlayerConnect(playerid) {
     OrangePlayerState[playerid][IsCollecting] = false;
     OrangePlayerState[playerid][CurrentTree] = -1;
     OrangePlayerState[playerid][CollectionTimer] = 0;

     // Initialize mini-game textdraws for this player
     Keymash_InitPlayer(playerid);

     return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
     if(OrangePlayerState[playerid][CollectionTimer]) {
          KillTimer(OrangePlayerState[playerid][CollectionTimer]);
          OrangePlayerState[playerid][CollectionTimer] = 0;
     }
     if(OrangePlayerState[playerid][IsCollecting]) {
          new treeIndex = OrangePlayerState[playerid][CurrentTree];

          if(treeIndex >= 0 && treeIndex < sizeof(OrangeTrees)) {
               // Release the tree lock
               TreeAvailable[treeIndex] = true;

               // Start respawn timer to mark tree as empty
               TreeRespawnTime[treeIndex] = (ORANGE_RESPAWN_TIME / 1000);
               if(TreeRespawnTimer[treeIndex]) KillTimer(TreeRespawnTimer[treeIndex]);
               TreeRespawnTimer[treeIndex] = SetTimerEx("Orange_UpdateTreeTimer", 1000, true, "i", treeIndex);
               SetTimerEx("Orange_RespawnTree", ORANGE_RESPAWN_TIME, false, "i", treeIndex);

               // Update label to show empty state
               new labelText[128];
               format(labelText, sizeof(labelText), "[ORANGE TREE]\n"C_RED"EMPTY - Respawn: %ds", ORANGE_RESPAWN_TIME / 1000);
               UpdateDynamic3DTextLabelText(OrangeTreeLabels[treeIndex], COLOR_YELLOW, labelText);

               #if ORANGE_FARM_DEBUG
               printf("[ORANGE] Player %d disconnected mid-harvest at tree %d - tree released", playerid, treeIndex);
               #endif
          }

          // Reset player state
          OrangePlayerState[playerid][IsCollecting] = false;
          OrangePlayerState[playerid][CurrentTree] = -1;
     }
     return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
     // Handle mini-game key input
     Keymash_OnKeyStateChange(playerid, newkeys, oldkeys);

     if(PRESSED(KEY_NO)) {
          new treeIndex = Orange_GetNearestTree(playerid);

          if(treeIndex != -1) {
               // Player is already collecting
               if(!OrangePlayerState[playerid][IsCollecting]) {
                    // Tree is respawning (empty)
                    if(!TreeAvailable[treeIndex] && TreeRespawnTime[treeIndex] > 0) {
                         new msg[128];
                         format(msg, sizeof(msg), "~r~[EMPTY]~w~ Tree respawns in ~y~%ds~w~!", TreeRespawnTime[treeIndex]);
                         Alert_Send(playerid, msg, 3000);
                    }
                    // Tree is being collected by another player
                    else if(!TreeAvailable[treeIndex]) {
                         Alert_Send(playerid, "~r~[ERROR]~w~ This tree is already being collected by another player!", 3000);
                    }
                    // Tree is available - start collection
                    else if(TreeAvailable[treeIndex]) {
                         Orange_StartCollection(playerid, treeIndex);
                    }
               } else {
                    Alert_Send(playerid, "~r~[ERROR]~w~ You are already collecting from a tree!", 3000);
               }
          }

          if(IsPlayerInRangeOfPoint(playerid, ORANGE_PROCESSOR_RANGE, ORANGE_PROCESSOR_X, ORANGE_PROCESSOR_Y, ORANGE_PROCESSOR_Z)) {
               Orange_ShowProcessDialog(playerid);
          }
     }
     return 1;
}

// ====== COLLECTION FUNCTIONS ======
Orange_StartCollection(playerid, treeIndex) {
     if(Player_GetFruitPicker(playerid) <= 0) {
          Alert_Send(playerid, "~r~ERROR:~w~ You need a fruit picker to harvest!", 3000);
          return;
     }

     if(Player_GetOranges(playerid) >= MAX_ORANGES) {
          Alert_Send(playerid, "~r~ERROR:~w~ Your orange inventory is full!", 3000);
          return;
     }

     OrangePlayerState[playerid][IsCollecting] = true;
     OrangePlayerState[playerid][CurrentTree] = treeIndex;
     TreeAvailable[treeIndex] = false;
     MessageBox_Show(playerid, TYPE_INFORMATION, "~y~Orange Farm Sessions", "To fill the skill bar press ~g~SPACE~w~ or ~g~SHIFT~w~. To cancel the Orange Farm Session press ~y~ALT~w~ button!", 5000);
     new labelText[128];
     format(labelText, sizeof(labelText), "[ORANGE TREE]\n"C_RED"BEING COLLECTED");
     UpdateDynamic3DTextLabelText(OrangeTreeLabels[treeIndex], COLOR_YELLOW, labelText);

     TogglePlayerControllable(playerid, false);
     ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.0, true, false, false, false, 0, 1);
     SetPlayerAttachedObject(playerid, 9, 19583, 6);

     Keymash_Start(playerid, KEYMASH_TYPE_HARVEST, treeIndex, "[ORANGE HARVEST]", "Press 'SPACE' to harvest");

     #if ORANGE_FARM_DEBUG
     printf("[ORANGE] Player %d started collecting at tree %d", playerid, treeIndex);
     #endif
}




Orange_ShowProcessDialog(playerid) {
     new oranges = Player_GetOranges(playerid);

     if(oranges <= 0) {
          Alert_Send(playerid, "~r~ERROR:~w~ You don't have any oranges!", 3000);
          return;
     }

     inline _response(ply_id, dialogid, response, listitem, string:inputtext[]) {
          #pragma unused ply_id, dialogid, listitem

          if(!response) return;

          new amount = strval(inputtext);
          if(!amount || amount <= 0) {
               Alert_Send(playerid, "~r~ERROR:~w~ Invalid amount!", 3000);
               return;
          }

          new required = amount * ORANGE_TO_JUICE_RATIO;
          new currentOranges = Player_GetOranges(playerid);
          new currentBottles = Player_GetBottles(playerid);

          if(currentOranges < required) {
               new msg[128];
               format(msg, sizeof(msg), "~r~Need~w~ %d oranges (have %d)", required, currentOranges);
               Alert_Send(playerid, msg, 3000);
               return;
          }

          if(currentBottles < amount) {
               new msg[128];
               format(msg, sizeof(msg), "~r~Need~w~ %d bottles (have %d)", amount, currentBottles);
               Alert_Send(playerid, msg, 3000);
               return;
          }

          new removeOrangesMsg[128];
          format(removeOrangesMsg, sizeof(removeOrangesMsg), "~r~Oranges~w~ %d Removed from inventory!", required, currentOranges);
          Notify_Send(playerid, removeOrangesMsg, 3000);
          Player_RemoveOranges(playerid, required);

          new removeBottlesMsg[128];
          format(removeBottlesMsg, sizeof(removeBottlesMsg), "~r~Bottles~w~ %d Removed from inventory!", amount, currentBottles);
          Notify_Send(playerid, removeBottlesMsg, 3000);
          Player_RemoveBottles(playerid, amount);

          TogglePlayerControllable(playerid, false);
          ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.0, true, false, false, false, 0, 1);

          new processingMsg[128];
          format(processingMsg, sizeof(processingMsg), "~w~PROCESSING~n~~y~%d Orange Juice", amount);
          GameTextForPlayer(playerid, processingMsg, ORANGE_PROCESSING_TIME, 5);

          SetTimerEx("Orange_ProcessComplete", ORANGE_PROCESSING_TIME, false, "ii", playerid, amount);

          #if ORANGE_FARM_DEBUG
          printf("[ORANGE] Player %d processing %d juice (removed %d oranges, %d bottles)", playerid, amount, required, amount);
          #endif
     }

     new currentOranges = Player_GetOranges(playerid);
     new currentBottles = Player_GetBottles(playerid);
     new dialogText[256];
     format(dialogText, sizeof(dialogText), "Your Inventory:\n\
          Oranges: %d | Bottles: %d\n\n\
          Recipe per batch:\n\
          5 Oranges + 1 Bottle = 1 Juice ($50)\n\n\
          How many batches?", currentOranges, currentBottles);

     Dialog_ShowCallback(playerid, using inline _response, DIALOG_STYLE_INPUT,
          "[ORANGE JUICE FACTORY]",
          dialogText,
          "Process", "Cancel");
}

public Orange_ProcessComplete(playerid, amount) {
     Player_GiveOrangeJuice(playerid, amount);
     new reward = amount * ORANGE_SALE_PRICE_JUICE;
     GivePlayerXp(playerid, reward);
     GivePlayerMoney(playerid, reward);

     TogglePlayerControllable(playerid, true);
     ClearAnimations(playerid, true);

     new msg[128];
     format(msg, sizeof(msg), "~y~[PROCESSED]:~w~ +~y~%d Juice~w~ (~g~+$%d~w~)", amount, reward);
     Notify_Send(playerid, msg, 3000);

     #if ORANGE_FARM_DEBUG
     printf("[ORANGE] Player %d finished processing %d juice", playerid, amount);
     #endif
}

// ====== SYSTEM INITIALIZATION ======
LoadOrangeJob() {
     for(new i = 0; i < sizeof(OrangeTrees); i++) {
          OrangeTreeLabels[i] = CreateDynamic3DTextLabel(
               "[ORANGE TREE]\n"C_WHITE"Press 'N' to harvest",
               COLOR_YELLOW,
               OrangeTrees[i][0], OrangeTrees[i][1], OrangeTrees[i][2],
               20.0
          );
     }

     CreateDynamic3DTextLabel(
          "[ORANGE PROCESSOR]\n"C_WHITE"Press 'N' to process",
          COLOR_GREEN,
          ORANGE_PROCESSOR_X, ORANGE_PROCESSOR_Y, ORANGE_PROCESSOR_Z,
          20.0
     );

     print("[ORANGE FARM] System loaded successfully!");
     return 1;
}

// ====== TREE RESPAWN ======
public Orange_RespawnTree(treeIndex) {
     if(treeIndex < 0 || treeIndex >= sizeof(OrangeTrees)) return 0;

     TreeAvailable[treeIndex] = true;

     if(TreeRespawnTimer[treeIndex]) {
          KillTimer(TreeRespawnTimer[treeIndex]);
          TreeRespawnTimer[treeIndex] = 0;
     }

     UpdateDynamic3DTextLabelText(OrangeTreeLabels[treeIndex], COLOR_YELLOW, "[ORANGE TREE]\n"C_WHITE"Press 'N' to harvest");

     #if ORANGE_FARM_DEBUG // debuger to track tree respawns
     printf("[ORANGE] Tree %d respawned", treeIndex);
     #endif

     return 1;
}

// Update tree respawn timer display
public Orange_UpdateTreeTimer(treeIndex) {
     if(treeIndex < 0 || treeIndex >= sizeof(OrangeTrees)) return 0;

     TreeRespawnTime[treeIndex]--;

     if(TreeRespawnTime[treeIndex] > 0) {
          new labelText[128];
          format(labelText, sizeof(labelText), "[ORANGE TREE]\n"C_RED"EMPTY - Respawn: %ds", TreeRespawnTime[treeIndex]);
          UpdateDynamic3DTextLabelText(OrangeTreeLabels[treeIndex], COLOR_YELLOW, labelText);
     } else {
          if(TreeRespawnTimer[treeIndex]) {
               KillTimer(TreeRespawnTimer[treeIndex]);
               TreeRespawnTimer[treeIndex] = 0;
          }
     }

     return 1;
}

// ====== HELPER FUNCTIONS ======
Orange_GetNearestTree(playerid) {
     for(new i = 0; i < sizeof(OrangeTrees); i++) {
          if(IsPlayerInRangeOfPoint(playerid, 5.0, OrangeTrees[i][0], OrangeTrees[i][1], OrangeTrees[i][2])) {
               return i;
          }
     }
     return -1;
}

// ====== MINI-GAME CALLBACKS ======
public Orange_MiniGame_OnComplete(playerid, customData) {
     new randomAmount = random(5) + 2; // 2-6 oranges
     Player_GiveOranges(playerid, randomAmount);
     GivePlayerMoney(playerid, ORANGE_COLLECTION_REWARD_MONEY);

     RemovePlayerAttachedObject(playerid, 9);

     new labelText[128];
     format(labelText, sizeof(labelText), "[ORANGE TREE]\n"C_RED"EMPTY - Respawn: %ds", ORANGE_RESPAWN_TIME / 1000);
     UpdateDynamic3DTextLabelText(OrangeTreeLabels[customData], COLOR_YELLOW, labelText);

     new msg[128];
     format(msg, sizeof(msg), "~y~[HARVEST]:~w~ +~y~%d Oranges~w~ (~g~+$%d~w~)", randomAmount, ORANGE_COLLECTION_REWARD_MONEY);
     Notify_Send(playerid, msg, 3000);

     SetTimerEx("Orange_RespawnTree", ORANGE_RESPAWN_TIME, false, "i", customData);
     TreeRespawnTime[customData] = (ORANGE_RESPAWN_TIME / 1000);
     if(TreeRespawnTimer[customData]) KillTimer(TreeRespawnTimer[customData]);
     TreeRespawnTimer[customData] = SetTimerEx("Orange_UpdateTreeTimer", 1000, true, "i", customData);

     OrangePlayerState[playerid][IsCollecting] = false;
     OrangePlayerState[playerid][CurrentTree] = -1;

     return 1;
}

public Orange_MiniGame_OnFail(playerid, customData) {
     TreeAvailable[customData] = true;

     new labelText[128];
     format(labelText, sizeof(labelText), "[ORANGE TREE]\n"C_WHITE"Press 'N' to harvest");
     UpdateDynamic3DTextLabelText(OrangeTreeLabels[customData], COLOR_YELLOW, labelText);

     RemovePlayerAttachedObject(playerid, 9);

     OrangePlayerState[playerid][IsCollecting] = false;
     OrangePlayerState[playerid][CurrentTree] = -1;

     return 1;
}
