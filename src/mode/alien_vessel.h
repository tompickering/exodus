#ifndef GUARD_ALIEN_VESSEL_H
#define GUARD_ALIEN_VESSEL_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

#include "mixins/frame_drawer.h"
#include "mixins/panel_drawer.h"

extern ExodusState exostate;
extern EphemeralState ephstate;

class AlienVessel : ModeBase, PanelDrawer, FrameDrawer {
    public:
        AlienVessel();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            AV_Approach,
            AV_Idle,
            AV_Comm,
            AV_Exit,
        };

        Stage stage;

        enum VesselType {
            VESSEL_NoID,
            VESSEL_GuildPatrol,
            VESSEL_Trading,
            VESSEL_TradingConvoy,
            VESSEL_Travelling,
            VESSEL_Science,
            VESSEL_Mining,
            VESSEL_Religious,
            VESSEL_Unknown,
        };

        VesselType type;

        const char *ship_img;

        float approach;
};

#endif
