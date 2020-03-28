#ifndef GUARD_EXODUS_STATE_H
#define GUARD_EXODUS_STATE_H

/*
 * This represents the current state of the game, encapsulating:
 * * The galaxy
 * * Player locations
 * * Lord properties (money, fleet, cargo, standings etc)
 *
 * This class should represent exactly all data which should be
 * present in a save file, and should be shallow-serialisable.
 */

#include "galaxy/galaxy.h"

#define MAX_HUMAN_PLAYERS    5
#define MAX_PLAYER_NAME      12
#define MAX_PLAYER_TITLE     12
#define MAX_PLAYER_REFERENCE 12

#define N_PLAYERS 15

extern const char* flags[];

enum GalaxySize {
    GAL_Small,
    GAL_Medium,
    GAL_Large,
};

typedef struct {
    bool human;
    char name[MAX_PLAYER_NAME + 1];
    char title[MAX_PLAYER_TITLE + 1];
    char ref[MAX_PLAYER_REFERENCE + 1];
    int flag_idx;
    unsigned int mc;
    bool intro_seen;
} PlayerInfo;

enum Aim {
    AIM_Might,
    AIM_Money,
    AIM_Civilization,
};

enum EnemyStart {
    ENEMY_None,
    ENEMY_Weak,
    ENEMY_Medium,
    ENEMY_Strong,
};

typedef struct {
    GalaxySize size;
    unsigned int n_players;
    PlayerInfo info[MAX_HUMAN_PLAYERS];
    Aim aim;
    EnemyStart enemy_start;
} GameConfig;

class ExodusState {
    public:
        ExodusState();
        void init(GameConfig);
        void generate_galaxy();
        void finalise_galaxy();
        Galaxy* get_galaxy();
        unsigned int get_month();
        PlayerInfo *get_active_player();
    private:
        GalaxySize size;
        unsigned int n_players;
        unsigned int month;
        unsigned int active_player;
        PlayerInfo player_info[N_PLAYERS];
        Aim aim;
        EnemyStart enemy_start;
        Galaxy galaxy;
        bool galaxy_finalised;
};

#endif
