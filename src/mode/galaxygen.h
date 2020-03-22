#ifndef GUARD_GALAXYGEN_H
#define GUARD_GALAXYGEN_H

#include "mode_base.h"

#include "../platform.h"
#include "../draw/draw.h"

#include "../state/exodus_state.h"

extern ExodusState exostate;

class GalaxyGen : ModeBase {
    public:
        GalaxyGen();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
};

#endif
