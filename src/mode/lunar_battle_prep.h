#ifndef GUARD_LUNAR_BATTLE_PREP_H
#define GUARD_LUNAR_BATTLE_PREP_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

extern ExodusState exostate;

class LunarBattlePrep : ModeBase {
    public:
        LunarBattlePrep();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            LBP_Idle,
        };

        Stage stage;

        int mines_available;
        int mines_price;
};

#endif
