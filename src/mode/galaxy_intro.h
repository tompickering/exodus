#ifndef GUARD_GALAXY_INTRO_H
#define GUARD_GALAXY_INTRO_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

extern ExodusState exostate;

class GalaxyIntro : ModeBase {
    public:
        GalaxyIntro();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        float time;
};

#endif