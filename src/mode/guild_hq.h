#ifndef GUARD_GUILD_HQ_H
#define GUARD_GUILD_HQ_H

#include "mode_base.h"

#include "draw/draw.h"

class GuildHQ : ModeBase {
    public:
        GuildHQ();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            HQ_Idle,
            HQ_Guildbot,
        };

        Stage stage;
        bool guildbot_active;
        float guildbot_interp;
        float eyes_loop;
};

#endif
