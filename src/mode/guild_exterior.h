#ifndef GUARD_GUILD_EXTERIOR_H
#define GUARD_GUILD_EXTERIOR_H

#include "mode_base.h"

#include "draw/draw.h"

class GuildExterior : ModeBase {
    public:
        GuildExterior();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
};

#endif
