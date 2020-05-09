#ifndef GUARD_PLANET_STATUS_H
#define GUARD_PLANET_STATUS_H

#include "mode_base.h"

#include "draw/draw.h"

class PlanetStatus : ModeBase {
    public:
        PlanetStatus();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
};

#endif
