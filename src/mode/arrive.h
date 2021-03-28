#ifndef GUARD_ARRIVE_H
#define GUARD_ARRIVE_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

#include "mixins/frame_drawer.h"
#include "mixins/panel_drawer.h"

extern ExodusState exostate;

class Arrive : ModeBase, PanelDrawer, FrameDrawer {
    public:
        Arrive();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        float time;
};

#endif
