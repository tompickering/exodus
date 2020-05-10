#ifndef GUARD_PLANET_MAP_H
#define GUARD_PLANET_MAP_H

#include "mode_base.h"

#include "draw/draw.h"

#include "galaxy/planet.h"
#include "state/exodus_state.h"

extern ExodusState exostate;

class PlanetMap : ModeBase {
    public:
        PlanetMap();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            PM_Idle,
            PM_Placement,
        };

        Stage stage;
        Planet *planet;
        float anim_cycle;
        int surf_x;
        int surf_y;
};

#endif
