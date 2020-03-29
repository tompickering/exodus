#ifndef GUARD_GUILD_EXTERIOR_H
#define GUARD_GUILD_EXTERIOR_H

#include "mode_base.h"

#include "draw/draw.h"

class GuildExterior : ModeBase {
    public:
        GuildExterior();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        float blink_cycle;
        float ufo0_x;
        float ufo0_y;
        float ufo1_x;
        float ufo1_y;
        float transport_time;
};

#endif
