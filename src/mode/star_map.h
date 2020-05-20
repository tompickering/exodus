#ifndef GUARD_STAR_MAP_H
#define GUARD_STAR_MAP_H

#include "mode_base.h"
#include "mixins/panel_drawer.h"

#include "draw/draw.h"

#include "galaxy/star.h"
#include "state/exodus_state.h"

extern ExodusState exostate;

class StarMap : ModeBase, PanelDrawer {
    public:
        StarMap();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            SM_Idle,
            SM_Back2Gal,
        };

        Stage stage;
        Star *star;
        void draw_planets(float);
        float planet_progress[STAR_MAX_PLANETS];

        bool select_planet(int);
        void set_fleet_button(bool);
};

#endif
