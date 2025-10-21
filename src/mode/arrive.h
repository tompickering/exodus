#ifndef GUARD_ARRIVE_H
#define GUARD_ARRIVE_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

#include "mixins/frame_drawer.h"
#include "mixins/panel_drawer.h"



class Arrive : ModeBase, PanelDrawer, FrameDrawer {
    public:
        Arrive();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            ARR_Spaceport,
            ARR_Hangar,
        };

        Stage stage;

        Player *player;
        Planet *planet;
        Player *owner;

        float time;

        void base_draw(const char*);
};

#endif
