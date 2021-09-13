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
            StarsFadeIn0,
            InitialText,
            BadDeeds,
            GoodDeeds,
            FutureText,
            Outro,
            TheEndDelay,
            TheEnd,
            FadeEnd,
            ForNow,
        };

        Stage stage;

        void set_stage(Stage);
};

#endif
