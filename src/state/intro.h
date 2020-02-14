#ifndef GUARD_INTRO_H
#define GUARD_INTRO_H

#include "state_base.h"

#include "../platform.h"

class Intro : StateBase {
    public:
        Intro();
        virtual void enter();
        virtual void exit();
        virtual void update(float) override;
    private:
        TIMER timer;

        enum Stage {
            None,
            Init,
            Artex,
            Earth,
            City,
            Starport,
            Outlaw,
            Guard,
            Shoot,
            Code,
            Fail,
            Success,
            DepartShuttle,
            DepartShip,
            Title,
            End,
        };

        Stage stage;
        bool stage_started;
        void next_stage();
};

#endif
