#ifndef GUARD_FLY_H
#define GUARD_FLY_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

#include "mixins/frame_drawer.h"
#include "mixins/panel_drawer.h"



class Fly : ModeBase, PanelDrawer, FrameDrawer {
    public:
        Fly();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        float time;
        bool arriving;
        int current_thrust;
        int warp_stage;

        char text[128];
};

#endif
