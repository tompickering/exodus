#ifndef GUARD_EXODUS_DEBUG_H
#define GUARD_EXODUS_DEBUG_H

#include "exodus_state.h"

#include "shared.h"

#include "draw/draw.h"

class ExodusDebug {
    public:
        void init();
        void add_mc(int);
        bool show_player_markers;
        SprID player_markers[N_PLAYERS];
};

#endif
