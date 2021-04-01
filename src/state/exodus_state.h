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

#define MAX_HUMAN_PLAYERS    5

#define N_PLAYERS 15

#include "exodus_debug.h"

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
    int n_players;
    Player players[MAX_HUMAN_PLAYERS];
    Aim aim;
    EnemyStart enemy_start;
} GameConfig;

typedef struct {
    Planet* planet;
    Star* star;
    int index;
} PlanetInfo;

enum AllianceType : uint8_t {
    ALLY_Trade,
    ALLY_NonAttack,
    ALLY_War,
};

class ExodusState {
    public:
        ExodusState();
        void init(GameConfig);
        void generate_galaxy();
        void finalise_galaxy();
        void init_cpu_lords();
        Galaxy* get_galaxy();
        int get_n_human_players();
        int get_month();
        int get_orig_month();
        void advance_month();
        Player* get_active_player();
        int get_active_player_idx();
        Player* set_active_player(int);
        Star* get_active_star();
        int get_active_star_idx();
        Planet* get_active_planet();
        int get_active_planet_idx();
        FlyTarget* get_active_flytarget();
        void set_active_flytarget(FlyTarget*);
        void set_active_planet(int);
        int tgt2loc(FlyTarget*); // Convert between PlayerLocation
        FlyTarget* loc2tgt(int); // indicies and FlyTarget objects.
        Player* get_player(int);
        int get_player_idx(Player*);
        int get_n_active_players();
        Player* get_random_active_player();
        int get_random_star_idx();
        int get_n_owned_planets();
        Planet* get_planet_under_construction(int);
        PlanetInfo get_random_owned_planet_info();
        int get_n_planets(Player*);
        bool owns_a_planet(Player*);
        bool active_player_local(); // Is the active player at the active flytarget?
        PlanetInfo recommend_planet();
        Player* get_hostile_to(Player&);
        void set_random_hostility(Player&);
        bool first_city_done;
        bool first_spaceport_done;
        int get_total_net_income(int);
        bool is_allied(int, int);
        bool has_alliance(int, int, AllianceType);
        bool has_only_alliance(int, int, AllianceType);
        bool has_all_alliances(int, int);
        void set_alliance(int, int, AllianceType);
        void set_all_alliances(int, int);
        void unset_alliance(int, int, AllianceType);
        void unset_alliances(int, int);
        int count_alliances(int);
    private:
        GalaxySize size;
        int n_human_players;
        int month;
        int active_player;
        int active_star;
        int active_planet;
        Player players[N_PLAYERS];
        Aim aim;
        EnemyStart enemy_start;
        Galaxy galaxy;
        bool galaxy_finalised;
        bool active_flytarget_is_guild;

        uint8_t get_alliances(int, int);
        void set_alliances(int, int, uint8_t);
        uint8_t alliance_matrix[N_PLAYERS * N_PLAYERS];

    friend class ExodusDebug;
};

#endif
