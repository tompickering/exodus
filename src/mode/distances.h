#ifndef GUARD_DISTANCES_H
#define GUARD_DISTANCES_H

#include "mode_base.h"
#include "mixins/galaxy_drawer.h"
#include "mixins/panel_drawer.h"

class Distances : ModeBase, GalaxyDrawer, PanelDrawer {
    public:
        Distances();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
};

#endif
