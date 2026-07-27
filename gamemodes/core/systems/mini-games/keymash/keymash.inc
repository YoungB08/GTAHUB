// keymash - Mash the key to fill the bar before it decays
// Written by s4marium
// Updated to use reusable MinigameUI system

#include <YSI_Coding\y_hooks>

// ====== KEYMASH MINIGAME CONSTANTS ======
#define KEYMASH_PROGRESS_MAX 100
#define KEYMASH_PROGRESS_INCREMENT 5
#define KEYMASH_PROGRESS_DECAY 10
#define KEYMASH_KEY_COOLDOWN 150 // Milliseconds between key presses
#define KEYMASH_DECAY_INTERVAL 1000 // Milliseconds between decay ticks
#define KEYMASH_MAX_TIME 60000 // 60 seconds max
#define KEYMASH_ANIMATION_INTERVAL 1000 // 1 second between animation loops

// ====== KEYMASH MINIGAME TYPES ======
#define KEYMASH_TYPE_HARVEST 1
#define KEYMASH_TYPE_COLLECTION 2
#define KEYMASH_TYPE_MINING 3
#define KEYMASH_TYPE_FISHING 4
#define KEYMASH_TYPE_COMBAT 5
#define KEYMASH_TYPE_LOCKPICKING 6

// ====== KEYMASH MINIGAME ENUMS ======
enum E_KEYMASH_STATE {
     bool:IsActive,
     PlayerID,
     Type,
     CustomData,
     Progress,
     LastKeyTime,
     DecayTimer,
     TimeoutTimer,
     AnimationTimer
};

// ====== KEYMASH MINIGAME VARIABLES ======
new KeymashState[MAX_PLAYERS][E_KEYMASH_STATE];

// Note: UI textdraws are now managed by MinigameUI system (see ui_minigame.inc)
// No need to create separate textdraws for each minigame

// ====== FORWARD DECLARATIONS ======
forward public Keymash_InitPlayer(playerid);
forward public Keymash_OnKeyStateChange(playerid, newkeys, oldkeys);
forward public Keymash_Start(playerid, type, customData, const title[], const instructions[]);
forward public Keymash_End(playerid, bool:success);
forward public Keymash_OnDecay(playerid);
forward public Keymash_OnTimeout(playerid);
forward public Keymash_LoopAnimation(playerid);
forward public Combat_MiniGame_OnComplete(playerid, customData);
forward public Combat_MiniGame_OnFail(playerid, customData);
forward public Lockpicking_MiniGame_OnComplete(playerid, customData);
forward public Lockpicking_MiniGame_OnFail(playerid, customData);

// ====== INITIALIZATION ======
hook OnGameModeInit() {
    LoadKeymashMinigame();
    return 1;
}

LoadKeymashMinigame() {
    print("[KEYMASH] Mini-game system loaded successfully!");
    return 1;
}

hook OnPlayerConnect(playerid) {
    Keymash_InitPlayer(playerid);
    return 1;
}

public Keymash_InitPlayer(playerid) {
    KeymashState[playerid][IsActive] = false;
    KeymashState[playerid][Progress] = 0;
    KeymashState[playerid][LastKeyTime] = 0;
    KeymashState[playerid][DecayTimer] = 0;
    KeymashState[playerid][TimeoutTimer] = 0;
    KeymashState[playerid][AnimationTimer] = 0;

    // UI is now managed by MinigameUI system - no textdraw creation needed!
    return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
    Keymash_OnKeyStateChange(playerid, newkeys, oldkeys);
    return 1;
}

public Keymash_OnKeyStateChange(playerid, newkeys, oldkeys) {
    if(!KeymashState[playerid][IsActive]) return 0;

    // Check for key presses (use multiple keys for better compatibility while locked)
    // KEY_FIRE = Left mouse button, KEY_ANALOG_RIGHT = Right mouse button, KEY_CTRL_BACK = W key
    if(PRESSED(KEY_FIRE) || PRESSED(KEY_ANALOG_RIGHT) || PRESSED(KEY_CTRL_BACK) || PRESSED(KEY_SPRINT)) {
        new currentTime = GetTickCount();

        // Check key cooldown
        if(currentTime - KeymashState[playerid][LastKeyTime] >= KEYMASH_KEY_COOLDOWN) {
            KeymashState[playerid][LastKeyTime] = currentTime;
            Keymash_UpdateProgress(playerid, KEYMASH_PROGRESS_INCREMENT);
        }
    }

    return 1;
}

// ====== PUBLIC FUNCTIONS ======
public Keymash_Start(playerid, type, customData, const title[], const instructions[]) {
    if(KeymashState[playerid][IsActive]) return 0;

    // Initialize state
    KeymashState[playerid][IsActive] = true;
    KeymashState[playerid][PlayerID] = playerid;
    KeymashState[playerid][Type] = type;
    KeymashState[playerid][CustomData] = customData;
    KeymashState[playerid][Progress] = 0;
    KeymashState[playerid][LastKeyTime] = GetTickCount();

    // Lock player
    TogglePlayerControllable(playerid, false);

    // Show UI using MinigameUI system (reusable, efficient)
    MinigameUI_Show(playerid, title, instructions, -1); // White color
    MinigameUI_SetProgressColor(playerid, -1); // White progress bar

    // Start decay timer
    KeymashState[playerid][DecayTimer] = SetTimerEx("Keymash_OnDecay", KEYMASH_DECAY_INTERVAL, true, "i", playerid);

    // Start timeout timer
    KeymashState[playerid][TimeoutTimer] = SetTimerEx("Keymash_OnTimeout", KEYMASH_MAX_TIME, false, "i", playerid);

    // Start animation loop for harvest type
    if(type == KEYMASH_TYPE_HARVEST) {
        KeymashState[playerid][AnimationTimer] = SetTimerEx("Keymash_LoopAnimation", KEYMASH_ANIMATION_INTERVAL, true, "i", playerid);
    }

    // Update display
    Keymash_UpdateDisplay(playerid);

    return 1;
}

static Keymash_UpdateProgress(playerid, amount) {
    if(!KeymashState[playerid][IsActive]) return;

    KeymashState[playerid][Progress] += amount;

    if(KeymashState[playerid][Progress] >= KEYMASH_PROGRESS_MAX) {
        KeymashState[playerid][Progress] = KEYMASH_PROGRESS_MAX;
        Keymash_UpdateDisplay(playerid);
        Keymash_Complete(playerid);
        return;
    }

    Keymash_UpdateDisplay(playerid);
}

public Keymash_OnDecay(playerid) {
    if(!KeymashState[playerid][IsActive]) return 0;

    KeymashState[playerid][Progress] -= KEYMASH_PROGRESS_DECAY;

    // Check if progress reached 0 (failure)
    if(KeymashState[playerid][Progress] <= 0) {
        KeymashState[playerid][Progress] = 0;
        Keymash_UpdateDisplay(playerid);
        Keymash_Fail(playerid);
        return 0;
    }

    Keymash_UpdateDisplay(playerid);
    return 1;
}

static Keymash_UpdateDisplay(playerid) {
    // Update progress bar value (0.0 to 100.0) using MinigameUI
    new Float:barValue = float(KeymashState[playerid][Progress]);
    MinigameUI_SetProgress(playerid, barValue);
}

static Keymash_Complete(playerid) {
    Keymash_End(playerid, true);
}

static Keymash_Fail(playerid) {
    Alert_Send(playerid, "~r~ERROR:~w~ You were too slow! Collection failed.", 3000);
    Keymash_End(playerid, false);
}

public Keymash_End(playerid, bool:success) {
    if(!KeymashState[playerid][IsActive]) return 0;

    // Kill timers
    if(KeymashState[playerid][DecayTimer]) {
        KillTimer(KeymashState[playerid][DecayTimer]);
        KeymashState[playerid][DecayTimer] = 0;
    }
    if(KeymashState[playerid][TimeoutTimer]) {
        KillTimer(KeymashState[playerid][TimeoutTimer]);
        KeymashState[playerid][TimeoutTimer] = 0;
    }
    if(KeymashState[playerid][AnimationTimer]) {
        KillTimer(KeymashState[playerid][AnimationTimer]);
        KeymashState[playerid][AnimationTimer] = 0;
    }

    // Hide UI using MinigameUI system
    MinigameUI_Hide(playerid);

    // Store state before clearing
    new type = KeymashState[playerid][Type];
    new customData = KeymashState[playerid][CustomData];

    // Reset state
    KeymashState[playerid][IsActive] = false;
    KeymashState[playerid][Progress] = 0;

    // Unlock player & clear animations
    TogglePlayerControllable(playerid, true);
    ClearAnimations(playerid, true);

    // Call appropriate callback based on type using CallLocalFunction
    if(success) {
        if(type == KEYMASH_TYPE_HARVEST) {
            CallLocalFunction("Orange_MiniGame_OnComplete", "ii", playerid, customData);
        } else if(type == KEYMASH_TYPE_COLLECTION || type == KEYMASH_TYPE_MINING || type == KEYMASH_TYPE_FISHING) {
            // Fishing minigame still in development
        } else if(type == KEYMASH_TYPE_COMBAT) {
            CallLocalFunction("Combat_MiniGame_OnComplete", "ii", playerid, customData);
        } else if(type == KEYMASH_TYPE_LOCKPICKING) {
            CallLocalFunction("Lockpicking_MiniGame_OnComplete", "ii", playerid, customData);
        }
    } else {
        if(type == KEYMASH_TYPE_HARVEST) {
            CallLocalFunction("Orange_MiniGame_OnFail", "ii", playerid, customData);
        } else if(type == KEYMASH_TYPE_COLLECTION || type == KEYMASH_TYPE_MINING || type == KEYMASH_TYPE_FISHING) {
            // Fishing minigame still in development
        } else if(type == KEYMASH_TYPE_COMBAT) {
            CallLocalFunction("Combat_MiniGame_OnFail", "ii", playerid, customData);
        } else if(type == KEYMASH_TYPE_LOCKPICKING) {
            CallLocalFunction("Lockpicking_MiniGame_OnFail", "ii", playerid, customData);
        }
    }

    return 1;
}

public Keymash_OnTimeout(playerid) {
    if(!KeymashState[playerid][IsActive]) return 0;

    Alert_Send(playerid, "~r~ERROR:~w~ Time's up!", 3000);
    Keymash_End(playerid, false);
    return 1;
}

public Keymash_LoopAnimation(playerid) {
    if(!KeymashState[playerid][IsActive]) return 0;

    // Reapply harvest animation to keep it looping
    ApplyAnimation(playerid, "BD_FIRE", "wash_up", 4.0, true, false, false, false, 0, 1);
    return 1;
}

// ====== HELPER FUNCTIONS ======
stock bool:Keymash_IsPlaying(playerid) {
    return KeymashState[playerid][IsActive];
}

stock Keymash_GetProgress(playerid) {
    return KeymashState[playerid][Progress];
}
