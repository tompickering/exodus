#ifndef GUARD_GALAXY_DRAWER_H
#define GUARD_GALAXY_DRAWER_H

#include "mode_base.h"

class GalaxyDrawer : ModeBase {
    public:
        GalaxyDrawer(const char*);
    protected:
        void draw_galaxy(bool);
};

#endif
