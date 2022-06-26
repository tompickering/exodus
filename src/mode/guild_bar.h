#ifndef GUARD_GUILD_BAR_H
#define GUARD_GUILD_BAR_H

#include "mode_base.h"

#include "draw/draw.h"

class GuildBar : ModeBase {
    public:
        GuildBar();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            GB_Idle,
            GB_Pinboard,
            GB_Barkeeper,
            GB_StarSheriff,
            GB_StarSheriffExit,
        };

        void update_pin_and_rumours();

        Stage stage;
        int last_update_month;
        int pin;
        int rumour_headings[2];
        int rumour_indices[2];
        float talk_loop;
        float light_loop;
        float screen_loop;
};

#endif
