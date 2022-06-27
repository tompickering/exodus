#ifndef GUARD_GUILD_BAR_H
#define GUARD_GUILD_BAR_H

#include "mode_base.h"

#include "draw/draw.h"

#define SHERIFF_N_SHIPS 5
#define SHERIFF_N_HIGHSCORES 3

struct SheriffShip {
    int x;
    int y;
    int z;
    float delay;
    float anim_interp;
    bool live;
    float explosion_interp;
    float blasters_interp;

    SprID id;
    SprID blasters_id;

    enum InitType {
        Demo,
        Game,
        Respawn,
    };

    void init(InitType);
};

struct SheriffHighScore {
    int player_idx;
    int score;
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
            GB_StarSheriffSetupGame,
            GB_StarSheriff,
            GB_StarSheriffHighScores,
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

        enum SheriffAnnounce {
            SA_Go,
            SA_Stage,
            SA_GameOver,
            SA_BonusKill,
            SA_BonusShield,
        };

        bool sheriff_demo;
        int sheriff_level;
        float sheriff_shot_interp;
        SheriffShip sheriff_ships[SHERIFF_N_SHIPS];
        float sheriff_hittime;
        int sheriff_laser_x;
        int sheriff_laser_y;
        int sheriff_shield;
        float sheriff_game_time;
        int sheriff_score;
        int sheriff_tokill;

        bool sheriff_bonus;
        SprID sheriff_bonus_id;
        int sheriff_bonus_x;
        int sheriff_bonus_y;
        int sheriff_bonus_z;
        float sheriff_bonus_interp;

        SheriffAnnounce sheriff_announce;
        float sheriff_announce_time;

        bool sheriff_gameover;

        SheriffHighScore sheriff_high_scores[SHERIFF_N_HIGHSCORES];
        void sheriff_insert_high_score(const SheriffHighScore&);
};

#endif
