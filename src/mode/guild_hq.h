#ifndef GUARD_GUILD_HQ_H
#define GUARD_GUILD_HQ_H

#include "mode_base.h"

#include "draw/draw.h"

class GuildHQ : ModeBase {
    public:
        GuildHQ();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
};

#endif
