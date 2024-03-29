#ifndef GUARD_EXODUS_DEBUG_H
#define GUARD_EXODUS_DEBUG_H

#include "exodus_state.h"

#include "shared.h"

#include "draw/draw.h"

class ExodusDebug {
    public:
        void init();
        void add_mc(int);
        void add_units(int);
        void add_goods(int);
        void unlock_all_inventions();
        void set_all_alliances();
        bool show_player_markers;
        bool full_visibility;
        SprID player_markers[N_PLAYERS];
};

#endif
