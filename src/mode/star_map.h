#ifndef GUARD_STAR_MAP_H
#define GUARD_STAR_MAP_H

#include "mode_base.h"
#include "mixins/panel_drawer.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

extern ExodusState exostate;

class StarMap : ModeBase, PanelDrawer {
    public:
        StarMap();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
};

#endif
