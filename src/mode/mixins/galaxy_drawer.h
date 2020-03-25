#ifndef GUARD_GALAXY_DRAWER_H
#define GUARD_GALAXY_DRAWER_H

#include "../../draw/draw.h"

#include "../../state/exodus_state.h"

extern DRAWMANAGER draw_manager;

extern ExodusState exostate;

class GalaxyDrawer {
    public:
        GalaxyDrawer();
    protected:
        void draw_galaxy(bool);
};

#endif
