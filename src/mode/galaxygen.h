#ifndef GUARD_GALAXYGEN_H
#define GUARD_GALAXYGEN_H

#include "galaxy_drawer.h"

#include "../platform.h"
#include "../draw/draw.h"

#include "../state/exodus_state.h"

extern ExodusState exostate;

class GalaxyGen : GalaxyDrawer {
    public:
        GalaxyGen();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        bool gen_required;
};

#endif
