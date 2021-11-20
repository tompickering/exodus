#ifndef GUARD_PLAYER_H
#define GUARD_PLAYER_H

#include <cstdint>

#include "location.h"

#define MAX_PLAYER_NAME      12
#define MAX_PLAYER_TITLE     12
#define MAX_PLAYER_REFERENCE 12
#define MAX_PLAYER_FULLNAME MAX_PLAYER_TITLE + 1 + MAX_PLAYER_NAME

#define N_RACES 5

#define OFF_COST_POOR 0
#define OFF_COST_AVG  4
#define OFF_COST_GOOD 8
#define OFF_INITCOST_POOR 100
#define OFF_INITCOST_AVG  200
#define OFF_INITCOST_GOOD 400

enum GuildTitle {
    GUILDTITLE_None,
    GUILDTITLE_Warrior,
    GUILDTITLE_Strong,
    GUILDTITLE_Conqueror,
    GUILDTITLE_Mighty,
    GUILDTITLE_Great,
    GUILDTITLE_Warlord,
    GUILDTITLE_Emperor,
    GUILDTITLE_MAX,
};

enum Race {
    RACE_Human,
    RACE_Yokon,
    RACE_Teri,
    RACE_Urkash,
    RACE_Gordoon,
};

typedef struct {
    int shield_generators;   // 1  -
    int laser_guns;          // 2   |
    int missile_launchers;   // 3   |
    int crew;                // 4   |
    int bionic_probes;       // 5    \  Original codes for
    bool escape_capsule;     // 6    /  spaceship equipment
    bool repair_hangar;      // 7   |
    int pct_damage_thrust;   // 8   |
    int pct_damage_comms;    // 9   |
    int pct_damage_struct;   // 10 -
    void reset() {
        shield_generators = 0;
        laser_guns = 1;
        missile_launchers = 0;
        crew = 0;
        bionic_probes = 0;
        escape_capsule = false;
        repair_hangar = false;
        pct_damage_thrust = 0;
        pct_damage_comms = 0;
        pct_damage_struct = 0;
    }
} Starship;

typedef struct {
    int minerals;
    int food;
    int plutonium;
    int robots;
    int infantry;
    int gliders;
    int artillery;
    void clear() {
        minerals  = 0;
        food      = 0;
        plutonium = 0;
        robots    = 0;
        infantry  = 0;
        gliders   = 0;
        artillery = 0;
    }
    int size() const {
        return minerals + food + plutonium + robots + army_size();
    }
    int army_size() const {
        return infantry + gliders + artillery;
    }
    int army_size_weighted() const {
        return infantry + gliders*2 + artillery*3;
    }
} Freight;

typedef struct {
    int scouts;
    int transporters;
    int warships;
    int bombers;
    Freight freight;
    int size() const {
        return scouts + transporters + warships + bombers;
    }
} Fleet;

enum ArtificialPlanetPhase {
    AP_None,
    AP_Phase1,
    AP_Phase2,
    AP_Complete,
};

// N.B. This is sensitive to order
enum Invention {
    INV_MassProduction,           // 1   -
    INV_UltraRangeScanner,        // 2    |
    INV_FusionEngine,             // 3    |
    INV_MicroMechanicElements,    // 4    |
    INV_UniversalVaccine,         // 5    |
    INV_OrbitalBombs,             // 6    |
    INV_OrbitalMassConstruction,  // 7     \ Original codes
    INV_OrbitalMassThrust,        // 8     / for inventions
    INV_WeatherInfluence,         // 9    |
    INV_MultiFunctionalVaccine,   // 10   |
    INV_Acid,                     // 11   |
    INV_IndustryGuard,            // 12   |
    INV_DreamInfluence,           // 13   |
    INV_RadarExtension,           // 14  -
    INV_MAX
};

enum InventionType {
    IT_Physical,
    IT_Mechanical,
    IT_Electronical,
    IT_Medical,
    IT_Weapon,
};

enum Officer {
    OFF_Science,
    OFF_Fleet,
    OFF_Battle,
    OFF_Secret,
    OFF_Counsellor,
    OFF_MAX
};

enum OfficerQuality {
    OFFQ_Poor,
    OFFQ_Average,
    OFFQ_Good,
    OFFQ_MAX,
};

enum MissionType {
    MT_None,
    MT_TerrorComm,
    MT_TerrorAgri,
    MT_TerrorPlu,
    MT_TerrorArmy,
    MT_TerrorPort,
    MT_TerrorTrade,
    MT_TerrorMine,
    MT_RandomBomb,
    MT_Nuclear,
};

typedef struct {
    MissionType type;
    int star_idx;
    int planet_idx;
} Mission;

enum Gender {
    GENDER_Female,
    GENDER_Male,
    GENDER_Other,
};

// Orig: lo%/md%/hi%
enum AIFlag {
    AI_Lo,
    AI_Md,
    AI_Hi,
};

class Player {
    public:
        Player();
        void init_alien_name();
        void init_race(Race);
        const char* get_name();
        const char* get_title();
        const char* get_ref();
        const char* get_full_name();
        Gender get_gender();
        void set_name(const char*);
        void set_title(const char*);
        void set_ref(const char*);
        void set_gender(Gender);
        Race get_race();
        const char* get_race_str();
        bool is_human();
        bool is_alive();
        bool is_participating();
        bool leave_galaxy();
        bool return_to_galaxy();
        bool is_guild_member();
        void set_guild_member(bool);
        GuildTitle get_guild_title();
        const char* get_guild_title_str();
        void set_guild_title(GuildTitle);
        int get_mc();
        int get_flag_idx();
        void set_flag_idx(int);
        bool intro_seen();
        void set_intro_seen();
        void give_mc(int);
        bool can_afford(int);
        bool attempt_spend(int);
        bool attempt_spend_with_remaining(int, int);
        int get_fleet_marker_idx();
        PlayerLocation& get_location();
        Starship& get_starship();
        int get_reputation();
        void adjust_reputation(int);
        bool can_research(Invention);
        bool research_invention(Invention);
        bool has_invention(Invention);
        bool has_all_inventions();
        Invention get_random_researchable_invention();
        InventionType get_invention_type(Invention);
        const char* get_invention_str(Invention);
        const char* get_invention_type_str(Invention);
        uint32_t get_invention_prerequisites(Invention);
        const Fleet& get_fleet();
        Fleet& get_fleet_nonconst();
        void init_tax();
        int get_tax();
        void set_tax(int);
        void adjust_tax(int);
        OfficerQuality get_officer(Officer);
        int get_officer_cost(Officer);
        int get_officer_cost(OfficerQuality);
        int get_officer_initial_cost(OfficerQuality);
        int get_total_officer_costs(Officer);
        void set_officer(Officer, OfficerQuality);
        int nopirates;
        ArtificialPlanetPhase ap_phase;

        int get_freight_capacity();
        void cap_freight_randomly();

        /*
         * The transfer_* functions should be used to adjust freight,
         * as they will ensure that bounds are always respenced.
         * When loading (+ve n), the function will load as much as possible,
         * returning the -ve total number actually loaded.
         * When unloading (-ve n), the function will unload as much as possible,
         * returning the +ve total number actually unloaded.
         *
         * The signs are such that they can be used by the caller to adjust
         * the source quantity.
         */
        int transfer_min(int);
        int transfer_fd(int);
        int transfer_plu(int);
        int transfer_robots(int);
        int transfer_inf(int);
        int transfer_gli(int);
        int transfer_art(int);

        bool has_mission();
        void set_mission_type(MissionType);
        int& get_mission_planet_ref();
        int& get_mission_star_ref();
        const Mission& get_mission();
        void clear_mission();

        // AI
        AIFlag get_flag(int);
        int get_tactic();
        void set_tactic(int);
        void next_tactic();
        void prev_tactic();
        bool is_hostile_to(int);
        int get_hostile_to();
        void set_hostile_to(int);
        void clear_hostility();
        void clear_ai_attack();
        void get_ai_attack(int&, int&);
        void set_ai_attack(int, int);
    private:
        Race race;
        Gender gender;
        char name[MAX_PLAYER_NAME + 1];
        char title[MAX_PLAYER_TITLE + 1];
        char ref[MAX_PLAYER_REFERENCE + 1];
        char full_name[MAX_PLAYER_FULLNAME + 1];
        GuildTitle guild_title;
        bool exists;
        bool dead;
        bool left_galaxy;
        int flag_idx;
        int mc;
        bool _intro_seen;
        int fleet_marker_idx;
        PlayerLocation location;
        Starship starship;
        Fleet fleet;
        uint16_t inventions;
        int reputation;
        void refresh_full_name();
        bool guild_member;
        int tax;  // Orig: t%
        OfficerQuality officers[OFF_MAX];  // Orig: Ps%
        Mission mission;

        int transfer(int, int*);

        // AI
        AIFlag ai_flags[9];
        int ai_tactic;
        int ai_hostile_to;
        int ai_attack_star;   // Orig: lordvar(), lower 8 bits
        int ai_attack_planet; // Orig: lordvar(), upper 8 bits

    // The ExodusState sets up the player data during game init.
    friend class ExodusState;
};

#endif
