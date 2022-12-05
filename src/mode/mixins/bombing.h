#ifndef GUARD_BOMBING_H
#define GUARD_BOMBING_H

#include "state/exodus_state.h"
#include "state/ephemeral_state.h"

class Bombing {
    public:
        Bombing();
        void bomb_planet(StoneSet, bool, int&, int&);
};

#endif
