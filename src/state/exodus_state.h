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

#include "save/saveable.h"

#include "newsitem.h"
#include "planet_report.h"

#include "player/player.h"

#include "galaxy/galaxy.h"
#include "galaxy/star.h"
#include "galaxy/planet.h"

#define MAX_HUMAN_PLAYERS    5

#define N_PLAYERS 15

#include "exodus_debug.h"

extern int QUICKSAVE_SLOT;

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
    int planet_idx;
    int star_idx;
    int index;
} PlanetInfo;

enum AllianceType : uint8_t {
    ALLY_Trade,
    ALLY_NonAttack,
    ALLY_War,
};

enum ArtificialPlanetViable : uint8_t {
    APV_No_StarFull,
    APV_No_MoveDest,
    APV_Yes,
};

class ExodusState : public Saveable {
    public:
        virtual void save(cJSON*) const override;
        virtual void load(cJSON*) override;

        ExodusState();
        void init(GameConfig);
        void generate_galaxy();
        void finalise_galaxy();
        void init_cpu_lords();
        Galaxy* get_galaxy();
        int get_n_human_players();
        bool multiplayer();
        int get_month();
        int get_orig_month();
        Aim get_aim();
        GalaxySize get_galaxy_size();
        EnemyStart get_enemy_start();
        void advance_month();
        bool final_month();
        bool mission_complete();
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
        bool player_has_visited_all_stars(Player*);
        Player* get_player(int);
        int get_player_idx(Player*);
        int get_n_active_players();
        int get_n_active_cpu_players();
        Player* get_random_active_player();
        uint32_t get_active_cpu_player_mask();
        int get_star_idx(Star*);
        int get_random_star_idx();
        int get_n_owned_planets();
        int get_n_unowned_planets();
        ArtificialPlanetViable artificial_planet_viable(Star*);
        bool construct_artificial_planet(Star*, int, const char*);
        Planet* get_planet_under_construction(int);
        PlanetInfo get_random_owned_planet_info();
        int get_n_planets();
        int get_n_planets(Player*);
        bool owns_a_planet(Player*);
        bool any_human_has_visited(int);
        bool active_player_local(); // Is the active player at the active flytarget?
        PlanetInfo recommend_planet();
        Player* get_hostile_to(Player&);
        void set_random_hostility(Player&);
        int count_hostile_players(int);
        int get_total_income_ignoring_army(int);
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
        bool can_request_alliance(int, AllianceType);
        void register_request_alliance(int, AllianceType);
        void reset_alliance_requests();
        bool kill(Player*);
        NewsItem& register_news(NewsItemType);
        NewsItem& register_news_force(NewsItemType);
        const NewsItem& get_news(int);
        bool get_star_planet_idx(Planet*, int&, int&);
        bool has_artificial_planet(int);
        Star* get_star_for_planet(Planet*);
        bool planet_name_taken(const char*);
        void stabilise_disappeared_planet(Planet*);
        void cancel_worlds_under_construction(int);

        void prevent_attack(Planet*);
        bool attack_prevented(Planet*);
        void clear_attack_preventions();

        void prevent_bombing(Planet*);
        bool bombing_prevented(Planet*);
        void clear_bombing_preventions();

        void save_recommended_planet(const PlanetInfo& info);
        bool is_recommended_planet(Planet* p);

        void reset_planet_reports();
        void save_planet_report(const PlanetReport&);

        bool first_city_done;
        bool first_spaceport_done;

    private:
        int _get_n_owned_planets(bool);
        uint8_t get_alliances(int, int);
        void set_alliances(int, int, uint8_t);

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
        uint8_t alliance_matrix[N_PLAYERS * N_PLAYERS];
        char alliance_requests[N_PLAYERS];
        int newsitem_head;
        NewsItem newsitems[MAX_NEWSITEMS];
        int planet_reports_head;
        PlanetReport planet_reports[MAX_REPORTS];
        uint8_t attack_preventions[GALAXY_MAX_STARS];
        uint8_t bombing_preventions[GALAXY_MAX_STARS];

        int recommended_planet_star_idx;
        int recommended_planet_idx;

    friend class ExodusDebug;
};

#endif
