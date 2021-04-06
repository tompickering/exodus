#include "exodus_debug.h"

#include "shared.h"

#include "exodus_state.h"
#include "ephemeral_state.h"

#include "player/player.h"

extern ExodusState exostate;
extern DRAWMANAGER draw_manager;

void ExodusDebug::init() {
    show_player_markers = false;
    for (int i = 0; i < N_PLAYERS; ++i) {
        player_markers[i] = draw_manager.new_sprite_id();
    }
}

void ExodusDebug::add_mc(int mc) {
    Player* p = exostate.get_active_player();
    if (p) {
        p->give_mc(mc);
    }
}

void ExodusDebug::add_units(int n) {
    Player* p = exostate.get_active_player();
    if (p) {
        p->transfer_inf(n);
        p->transfer_gli(n);
        p->transfer_art(n);
    }
}
