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
            Text0,
            BadDeeds,
            BadDeedsWait,
            GoodDeeds,
            GoodDeedsWait,
            DeedsFadeOut,
            DeedsFade,
            StarsFadeIn1,
            Text1,
            FadeText1,
            Outro,
            TheEndDelay,
            TheEnd,
            FadeEnd,
            ForNow,
        };

        Stage stage;

        int text_faded_in;
        int text_faded_out;

        bool update_text(const char*[], int);
        void set_stage(Stage);
};

#endif
