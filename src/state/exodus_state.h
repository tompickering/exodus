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

#define MAX_PLAYERS           5
#define MAX_PLAYER_NAME      12
#define MAX_PLAYER_TITLE     12
#define MAX_PLAYER_REFERENCE 12

extern const char* flags[];

enum GalaxySize {
    GAL_Small,
    GAL_Medium,
    GAL_Large,
};

typedef struct {
    char name[MAX_PLAYER_NAME + 1];
    char title[MAX_PLAYER_TITLE + 1];
    char ref[MAX_PLAYER_REFERENCE + 1];
    int flag_idx;
    unsigned int mc;
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
    PlayerInfo info[MAX_PLAYERS];
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
    private:
        GalaxySize size;
        unsigned int n_players;
        PlayerInfo player_info[MAX_PLAYERS];
        Aim aim;
        EnemyStart enemy_start;
        Galaxy galaxy;
        bool galaxy_finalised;
};

#endif
