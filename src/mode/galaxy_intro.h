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
        char title_text[64];
        float time;
        bool fade_started;
};

#endif
