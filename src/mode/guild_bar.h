#ifndef GUARD_GUILD_BAR_H
#define GUARD_GUILD_BAR_H

#include "mode_base.h"

#include "draw/draw.h"

#define SHERIFF_N_SHIPS 5

struct SheriffShip {
    int x;
    int y;
    int z;
    float delay;
    float anim_interp;
    bool live;
    float explosion_interp;

    SprID id;

    enum InitType {
        Demo,
        Game,
        Respawn,
    };

    void init(InitType);
};

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
            GB_StarSheriffSetup,
            GB_StarSheriff,
            GB_StarSheriffExit,
        };

        void update_pin_and_rumours();
        bool update_star_sheriff(float);

        Stage stage;
        int last_update_month;
        int pin;
        int rumour_headings[2];
        int rumour_indices[2];
        float talk_loop;
        float light_loop;
        float screen_loop;

        bool sheriff_demo;
        int sheriff_level;
        float sheriff_shot_interp;
        SheriffShip sheriff_ships[SHERIFF_N_SHIPS];
        float sheriff_hittime;
};

#endif
