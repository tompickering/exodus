#ifndef GUARD_MENU_H
#define GUARD_MENU_H

#include "mode_base.h"

#include "../platform.h"
#include "../draw/draw.h"

#include "../state/exodus_state.h"

extern ExodusState exostate;

class Menu : ModeBase {
    public:
        Menu();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            Main,
            Load,
            Size,
            NPlayers,
            Name,
            Title,
            Flag,
            Aim,
            Difficulty,
            Confirm,
        };

        enum TransitionState {
            None,
            Started,
            Done,
        };

        Stage stage;
        TransitionState trans_state;
        unsigned char current_player;

        void set_stage(Stage);

        // The purpose of the Menu is to init this game config
        GameConfig config;
};

#endif
