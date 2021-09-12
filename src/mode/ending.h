#ifndef GUARD_ENDING_H
#define GUARD_ENDING_H

#include "mode_base.h"

class Ending : ModeBase {
    public:
        Ending();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        float time;

        enum Stage {
            Intro,
            ZoomAndDoors,
            WelcomeFade,
            InitialText,
            BadDeeds,
            GoodDeeds,
            FutureText,
            Outro,
            TheEnd,
            FadeEnd,
            ForNow,
        };

        Stage stage;

        void set_stage(Stage);
};

#endif
