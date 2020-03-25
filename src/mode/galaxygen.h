#ifndef GUARD_GALAXYGEN_H
#define GUARD_GALAXYGEN_H

#include "mode_base.h"
#include "mixins/galaxy_drawer.h"

#include "platform.h"
#include "draw/draw.h"

#include "state/exodus_state.h"

extern ExodusState exostate;

class GalaxyGen : ModeBase, GalaxyDrawer {
    public:
        GalaxyGen();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            GGSTG_Gen,
            GGSTG_SwapIn,
            GGSTG_FadeIn,
            GGSTG_Wait,
            GGSTG_FadeOut,
            GGSTG_ToGame,
        };

        Stage stage;
        float fade_interp;
        bool accepted;
};

#endif
