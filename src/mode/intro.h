#ifndef GUARD_INTRO_H
#define GUARD_INTRO_H

#include "mode_base.h"

#include "platform.h"

class Intro : ModeBase {
    public:
        Intro();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        TIMER timer;
        TIMER text_timer;

        enum Stage {
            None,
            Artex,
            Earth,
            City,
            Starport,
            Corridor,
            Guard,
            Shoot,
            GuardShot,
            Keypad,
            Success,
            DepartShuttle,
            DepartShip,
            Title,
            End,
            Fail,
        };

        Stage stage;
        bool stage_started;
        void next_stage();
        int text_idx;
        void draw_text();
        unsigned char determine_text_brightness(float, float);
        float kp_num_held_seconds[9];
        unsigned int kp_held_nums(float);
};

#endif
