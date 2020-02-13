#ifndef GUARD_INTRO_H
#define GUARD_INTRO_H

#include "state_base.h"

#include "../platform.h"
#include "../draw/draw.h"

class Intro : StateBase {
    public:
        Intro();
        virtual void enter();
        virtual void exit();
        virtual void update(float) override;
    private:
        TIMER timer;
        TIMER text_timer;

        enum Stage {
            None,
            Init,
            Artex,
            Earth,
            City,
            Starport,
            Corridor,
            Guard,
            Shoot,
            GuardShot,
            Code,
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
};

#endif
