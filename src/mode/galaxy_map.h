#ifndef GUARD_GALAXY_MAP_H
#define GUARD_GALAXY_MAP_H

#include "mode_base.h"
#include "mixins/galaxy_drawer.h"

#include "platform.h"
#include "draw/draw.h"

#include "state/exodus_state.h"

extern ExodusState exostate;

class GalaxyMap : ModeBase, GalaxyDrawer {
    public:
        GalaxyMap();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            GM_SwapIn,
            GM_Idle,
            GM_Zoom2Guild,
            GM_Zoom2Star,
            GM_MonthPassing,
        };

        Stage stage;
        FlyTarget *selected_ft;
        float selected_ft_blink;
        void draw_panel_bg(DrawTarget);
        void update_panel_info(DrawTarget, PlayerInfo*, FlyTarget*);
};

#endif
