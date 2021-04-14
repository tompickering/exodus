#ifndef GUARD_SPACE_BATTLE_H
#define GUARD_SPACE_BATTLE_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

extern ExodusState exostate;
extern EphemeralState ephstate;

class SpaceBattle : ModeBase {
    public:
        SpaceBattle();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            SB_Setup,
            SB_Exit,
        };

        Stage stage;
};

#endif
