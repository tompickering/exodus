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
        float talk_loop;
};

#endif
