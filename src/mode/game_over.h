#ifndef GUARD_GAME_OVER_H
#define GUARD_GAME_OVER_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

extern ExodusState exostate;
extern EphemeralState ephstate;

class GameOver : ModeBase {
    public:
        GameOver();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            GO_FailMessage,
            GO_AgainOrNot,
            GO_FadeOut,
        };

        Stage stage;

        float time;

        bool quit;
};

#endif
