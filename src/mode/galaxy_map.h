#ifndef GUARD_GALAXY_MAP_H
#define GUARD_GALAXY_MAP_H

#include "mode_base.h"
#include "mixins/galaxy_drawer.h"
#include "mixins/panel_drawer.h"
#include "mixins/comm_panel_drawer.h"

#include "platform.h"
#include "draw/draw.h"

#include "state/exodus_state.h"

extern ExodusState exostate;

class GalaxyMap : ModeBase, GalaxyDrawer, PanelDrawer, CommPanelDrawer {
    public:
        GalaxyMap();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            GM_SwapIn,
            GM_Idle,
            GM_Zoom2Guild,
            GM_Zoom2Star,
            GM_MonthPassing,
            GM_Report,
            GM_FlyConfirm,
        };

        // Enum order determines order of processing
        enum MonthPassStage {
            MP_None,
            MP_CheckMissionFail,
            MP_UpdateFly,
            MP_End,
        };

        Stage stage;
        FlyTarget *selected_ft;
        float selected_ft_blink;

        MonthPassStage mp_stage;
        float month_pass_time;
        int mp_player_idx;

        ExodusMode month_pass_update();
        void month_pass_start();
        void month_pass_end();
        void next_mp_stage();
};

#endif
