#ifndef GUARD_MENU_H
#define GUARD_MENU_H

#include "mode_base.h"

#include "platform.h"
#include "draw/draw.h"
#include "save/save.h"

class Menu : ModeBase {
    public:
        Menu();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            Main,
            Load,
            LoadWelcome,
            LoadWelcomeFade,
            LoadFinish,
            Size,
            NPlayers,
            Name,
            Gender,
            CustomTitle,
            CustomRef,
            Welcome,
            Flag,
            Aim,
            Difficulty,
            Confirm,
            End,
        };

        enum TransitionState {
            None,
            Started,
            Done,
        };

        Stage stage;
        TransitionState trans_state;
        unsigned char current_player;

        float timer;

        void set_stage(Stage);

        // The purpose of the Menu is to init this game config
        GameConfig config;

        SprID load_game_ids[MAX_SLOTS];
        const SaveMeta *savemeta;
};

#endif
