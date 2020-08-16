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

#include "player/player.h"

#include "galaxy/galaxy.h"
#include "galaxy/star.h"
#include "galaxy/planet.h"

#include "exodus_debug.h"

#define MAX_HUMAN_PLAYERS    5

#define N_PLAYERS 15

extern const char* flags[];

enum GalaxySize {
    GAL_Small,
    GAL_Medium,
    GAL_Large,
};

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
    Player players[MAX_HUMAN_PLAYERS];
    Aim aim;
    EnemyStart enemy_start;
} GameConfig;

class ExodusState {
    public:
        ExodusState();
        void init(GameConfig);
        void generate_galaxy();
        void finalise_galaxy();
        void init_cpu_lords();
        Galaxy* get_galaxy();
        unsigned int get_month();
        unsigned int get_orig_month();
        void advance_month();
        Player* get_active_player();
        int get_active_player_idx();
        Player* set_active_player(unsigned int);
        Star* get_active_star();
        unsigned int get_active_star_idx();
        Planet* get_active_planet();
        int get_active_planet_idx();
        FlyTarget* get_active_flytarget();
        void set_active_flytarget(FlyTarget*);
        void set_active_planet(int);
        int tgt2loc(FlyTarget*); // Convert between PlayerLocation
        FlyTarget* loc2tgt(int); // indicies and FlyTarget objects.
        Player* get_player(int);
        int get_player_idx(Player*);
        unsigned int get_n_planets(Player*);
        bool active_player_local(); // Is the active player at the active flytarget?
        bool first_city_done;
        bool first_spaceport_done;
    private:
        GalaxySize size;
        unsigned int n_players;
        unsigned int month;
        unsigned int active_player;
        unsigned int active_star;
        int active_planet;
        Player players[N_PLAYERS];
        Aim aim;
        EnemyStart enemy_start;
        Galaxy galaxy;
        bool galaxy_finalised;
        bool active_flytarget_is_guild;

    friend class ExodusDebug;
};

#endif
