#ifndef GUARD_FLY_H
#define GUARD_FLY_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

#include "mixins/panel_drawer.h"

extern ExodusState exostate;

class Fly : ModeBase, PanelDrawer {
    public:
        Fly();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        float time;
        bool arriving;
        int current_thrust;
};

#endif
