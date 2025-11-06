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
            HQ_GuildbotBecomeMember,
            HQ_GuildbotQuitMembership,
            HQ_GuildbotPayFine,
            HQ_GuildbotCloseOnClick,
            HQ_ClaimGuildmaster,
            HQ_GuildmasterClaimed,
            HQ_FadeToEnding,
        };

        Stage stage;
        void set_stage(Stage);

        bool guildbot_active;
        float guildbot_interp;
        float eyes_loop;
        float ending_delay;

        int fine;
        int text_y;

        void draw_panel();
        void draw_choice();
        void clear_bot_options();
        void close_bot_panel();
};

#endif
