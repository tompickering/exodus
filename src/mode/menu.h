#ifndef GUARD_MENU_H
#define GUARD_MENU_H

#include "mode_base.h"

#include "platform/platform.h"
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
            Quit,
            Load,
            LoadStart,
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
            Delay0,
            Delay1,
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

        Stage delayed_stage;
        int delay_border_to_reset;
        SprID delay_redraw_id;
        const char* delay_redraw_spr;
        DrawTransform delay_redraw_transform;
        void delay_stage(Stage, int, SprID, const char*, DrawTransform);
        void draw_border_opt(int, RGB);
        void draw_border_opt(DrawTarget, int, RGB);
        void draw_border_flag(int, RGB);

        void get_flag_pos(int, int&, int&);

        const char* img_diff_0;
        const char* img_diff_1;
        const char* img_diff_2;

        float timer;

        void set_stage(Stage);

        // The purpose of the Menu is to init this game config
        GameConfig config;

        SprID load_game_ids[MAX_SLOTS];
        const SaveMeta *savemeta;
        int load_slot;
        bool autosave_warned;
};

#endif
