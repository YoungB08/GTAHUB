
#include <ui_connect-msg>
#include <ui_fpspingpacket>
#include <ui_info-box>
#include <ui_wanted-lvl>
#include <ui_class-selection>
#include <ui_peds>
//#include <ui_tdmenu>
#include <ui_time-left>
#include <ui_location>
#include <ui_gps>
#include <ui_error>

// Do not change the order
// we prioritise 'ui_prev-dialog'
#include <ui_prev-dialog>
#include <YSI_Visual\y_dialog>

#include <ui_dealership>
#include <ui_gametime>
#include <ui_money-notification>

// exp bar
#include <ui_xp-bar>

// notifications and alerts idk if this ui is too modern for samp but it looks cool so here we are
#include <ui_notify> // For general notifications - middle right ui (green text)
#include <ui_alert> // For alerts - top right ui (red text)

// notifications that has an fade in and fade out effect, used for the cash change and subtitles and also the popups etc..
#include <ui_gamefeed>

// ============== MINIGAME UI SYSTEM ==============
// Reusable UI system for all minigames (hold-key, keymash, etc)
// Saves resources by reusing textdraws instead of creating new ones for each minigame
#include <ui_minigame>
