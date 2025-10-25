#include "exodus_debug.h"

#include "shared.h"

#include "exodus_state.h"
#include "ephemeral_state.h"

#include "player/player.h"


extern DRAWMANAGER draw_manager;

void ExodusDebug::init() {
    show_player_markers = false;
    for (int i = 0; i < N_PLAYERS; ++i) {
        player_markers[i] = draw_manager.new_sprite_id();
    }
}

void ExodusDebug::add_mc(int mc) {
    Player* p = exostate().get_active_player();
    if (p) {
        p->give_mc(mc);
    }
}

void ExodusDebug::add_units(int n) {
    Player* p = exostate().get_active_player();
    if (p) {
        p->transfer_inf(n);
        p->transfer_gli(n);
        p->transfer_art(n);
    }
}

void ExodusDebug::add_goods(int n) {
    Player* p = exostate().get_active_player();
    if (p) {
        p->transfer_fd(n);
        p->transfer_min(n);
        p->transfer_plu(n);
    }
}

void ExodusDebug::unlock_all_inventions() {
    Player *p = exostate().get_active_player();
    if (p) {
        L.debug("*CHEAT* ALL INVENTIONS");
        uint32_t unlocked_prev = 0;
        while (true) {
            uint32_t unlocked = 0;
            for (int i = 0; i < INV_MAX; ++i) {
                if (p->research_invention((Invention)i)) {
                    unlocked |= (1<<i);
                }
            }
            if (unlocked == unlocked_prev) {
                break;
            }
            unlocked_prev = unlocked;
        }
    }
}

void ExodusDebug::set_all_alliances() {
    Player *p = exostate().get_active_player();
    if (p) {
        L.debug("*CHEAT* ALL ALLIANCES");
        int idx = exostate().get_player_idx(p);
        for (int i = 0; i < N_PLAYERS; ++i) {
            if (i == idx)
                continue;
            exostate().set_all_alliances(i, idx);
        }
    }
}

void ExodusDebug::own_sys_planets() {
    Star* star = exostate().get_active_star();
    if (star) {
        for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
            Planet *p = star->get_planet(i);
            if (p && p->exists()) {
                p->set_owner(0, POCR_Seized);
                p->set_name("DEBUG");
            }
        }
    }
}
