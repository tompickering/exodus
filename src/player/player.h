#ifndef GUARD_PLAYER_H
#define GUARD_PLAYER_H

#include <cstdint>
#include <cstdio>
#include <cstring>

#include "save/saveable.h"

#include "location.h"
#include "mc_categories.h"

#define MAX_PLAYER_NAME      12
#define MAX_PLAYER_TITLE     12
#define MAX_PLAYER_REFERENCE 12
#define MAX_PLAYER_FULLNAME MAX_PLAYER_TITLE + 1 + MAX_PLAYER_NAME

#define N_MARKERS             8
#define MAX_MARKER           12

#define N_RACES 5

#define OFF_COST_POOR 0
#define OFF_COST_AVG  4
#define OFF_COST_GOOD 8
#define OFF_INITCOST_POOR 100
#define OFF_INITCOST_AVG  200
#define OFF_INITCOST_GOOD 400

#define MAX_MC_HISTORY 400

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
    RACE_Yokon,   // 1 -
    RACE_Teri,    // 2  \ Original codes for
    RACE_Urkash,  // 3  / player races
    RACE_Gordoon, // 4 -
};

enum Character {
    CHAR_None,
    CHAR_Human,
    CHAR_Yok0,
    CHAR_Ter0,
    CHAR_Urk0,
    CHAR_Gor0,
    CHAR_NEW,
    CHAR_Yok1,
    CHAR_Yok2,
    CHAR_Yok3,
    CHAR_Yok4,
    CHAR_Ter1,
    CHAR_Ter2,
    CHAR_Ter3,
    CHAR_Ter4,
    CHAR_Urk1,
    CHAR_Urk2,
    CHAR_Urk3,
    CHAR_Urk4,
    CHAR_Gor1,
    CHAR_Gor2,
    CHAR_Gor3,
    CHAR_Gor4,
    CHAR_LAST,
};

struct Starship : public Saveable {
    virtual void save(cJSON* j) const override {
        SAVE_NUM(j, shield_generators);
        SAVE_NUM(j, laser_guns);
        SAVE_NUM(j, missile_launchers);
        SAVE_NUM(j, crew);
        SAVE_NUM(j, bionic_probes);
        SAVE_BOOL(j, escape_capsule);
        SAVE_BOOL(j, repair_hangar);
        SAVE_NUM(j, pct_damage_thrust);
        SAVE_NUM(j, pct_damage_comms);
        SAVE_NUM(j, pct_damage_struct);
    }

    virtual void load(cJSON* j) override {
        LOAD_NUM(j, shield_generators);
        LOAD_NUM(j, laser_guns);
        LOAD_NUM(j, missile_launchers);
        LOAD_NUM(j, crew);
        LOAD_NUM(j, bionic_probes);
        LOAD_BOOL(j, escape_capsule);
        LOAD_BOOL(j, repair_hangar);
        LOAD_NUM(j, pct_damage_thrust);
        LOAD_NUM(j, pct_damage_comms);
        LOAD_NUM(j, pct_damage_struct);
    }

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
    bool damaged() const {
        return pct_damage_thrust + pct_damage_comms + pct_damage_struct > 0;
    }
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
};

struct Freight : public Saveable {
    virtual void save(cJSON* j) const override {
        SAVE_NUM(j, minerals);
        SAVE_NUM(j, food);
        SAVE_NUM(j, plutonium);
        SAVE_NUM(j, robots);
        SAVE_NUM(j, infantry);
        SAVE_NUM(j, gliders);
        SAVE_NUM(j, artillery);
    }

    virtual void load(cJSON* j) override {
        LOAD_NUM(j, minerals);
        LOAD_NUM(j, food);
        LOAD_NUM(j, plutonium);
        LOAD_NUM(j, robots);
        LOAD_NUM(j, infantry);
        LOAD_NUM(j, gliders);
        LOAD_NUM(j, artillery);
    }

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
};

struct Fleet : public Saveable {
    virtual void save(cJSON* j) const override {
        SAVE_NUM(j, scouts);
        SAVE_NUM(j, transporters);
        SAVE_NUM(j, warships);
        SAVE_NUM(j, bombers);
        SAVE_SAVEABLE(j, freight);
    }

    virtual void load(cJSON* j) override {
        LOAD_NUM(j, scouts);
        LOAD_NUM(j, transporters);
        LOAD_NUM(j, warships);
        LOAD_NUM(j, bombers);
        LOAD_SAVEABLE(j, freight);
    }

    int scouts;
    int transporters;
    int warships;
    int bombers;
    Freight freight;
    int size() const {
        return scouts + transporters + warships + bombers;
    }
};

struct StarMarker : public Saveable {
    virtual void save(cJSON* j) const override {
        SAVE_NUM(j, idx);
        SAVE_STR(j, tag);
    }

    virtual void load(cJSON* j) override {
        LOAD_NUM(j, idx);
        LOAD_STR(j, tag);
    }

    int idx;
    char tag[MAX_MARKER+1];

    bool valid() const {
        return idx >= 0 && tag[0] != '\0';
    }

    void reset() {
        idx = -1;
        memset(tag, '\0', MAX_MARKER+1);
    }

    void set_tag(const char* new_marker) {
        snprintf(tag, MAX_MARKER, new_marker);
    }
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
    OFFICER_MAX
};

enum OfficerQuality {
    OFFQ_Poor,
    OFFQ_Average,
    OFFQ_Good,
    OFFQ_MAX,
};

#define OFFICERS_TOTAL 15
static_assert(OFFICERS_TOTAL == (OFFICER_MAX*OFFQ_MAX));

enum Infraction : uint8_t {
    INF_TradePlu,
    INF_AttackGuildShip,
    INF_BombAttack,
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

struct Mission : public Saveable {
    virtual void save(cJSON* j) const override {
        SAVE_ENUM(j, type);
        SAVE_NUM(j, star_idx);
        SAVE_NUM(j, planet_idx);
    }

    virtual void load(cJSON* j) override {
        LOAD_ENUM(j, type);
        LOAD_NUM(j, star_idx);
        LOAD_NUM(j, planet_idx);
    }

    MissionType type;
    int star_idx;
    int planet_idx;

    void reset() {
        type = MT_None;
        star_idx = -1;
        planet_idx = -1;
    }
};

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

enum Trace {
    TRACE_None,
    // Bad things
    TRACE_BAD,
    TRACE_BattlesStarted,        //  1
    TRACE_AlliancesBroken,       //  2
    TRACE_RevolutionsCaused,     //  3
    TRACE_PeacefulShipsAttacked, //  4
    TRACE_CitiesDestroyed,       //  5
    TRACE_VillagesAnnihilated,   //  6
    TRACE_PlutoniumSold,         //  7
    TRACE_PlanetsNuked,          //  8
    TRACE_PlanetsBombed,         //  9
    TRACE_BAD_END,
    // Good things
    TRACE_GOOD,
    TRACE_PlanetsClaimed,        // 10
    TRACE_AlliancesCreated,      // 11
    TRACE_CitiesBuilt,           // 12
    TRACE_MineralsSold,          // 13
    TRACE_FoodSold,              // 14
    TRACE_AlliesHelped,          // 15
    TRACE_GOOD_END,
    TRACE_MAX
};

class Player : public Saveable {
    public:
        virtual void save(cJSON*) const override;
        virtual void load(cJSON*) override;

        Player();
        void init_alien_name();
        bool init_alien_name_character();
        void init_character(Character);
        void set_character_hostility();
        const char* get_name();
        const char* get_title();
        const char* get_ref();
        const char* get_full_name();
        Gender get_gender();
        void set_name(const char*);
        void set_title(const char*);
        void set_ref(const char*);
        void set_gender(Gender);
        Character get_character();
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
        const char* get_guild_title_str_with_space();
        void set_guild_title(GuildTitle);
        int get_mc();
        int get_flag_idx();
        const char* get_icon();
        void set_flag_idx(int);
        bool intro_seen();
        void set_intro_seen();
        bool intro_comm_seen();
        void set_intro_comm_seen();
        void give_mc(int, MCReason);
        bool can_afford(int);
        bool attempt_spend(int, MCReason);
        bool attempt_spend_with_remaining(int, int, MCReason);
        bool attempt_spend_allowing_writeoff(int, int, MCReason);
        bool attempt_spend_force(int, MCReason);
        bool attempt_spend_cpuforce(int, MCReason);
        int remove_all_mc(MCReason);
        void set_mc(int, MCReason);
        void cpu_write_off_debt();
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
        void get_invention_description(Invention, const char*&, const char*&, const char*&);
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
        int get_total_officer_costs();
        void set_officer(Officer, OfficerQuality);
        void register_officer_fired_nopay(Officer, OfficerQuality);
        void register_officer_quit(Officer, OfficerQuality);
        void register_officer_killed(Officer, OfficerQuality);
        bool officer_fired_nopay(Officer, OfficerQuality);
        bool officer_quit(Officer, OfficerQuality);
        bool officer_killed(Officer, OfficerQuality);

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

        StarMarker* get_marker(int);
        void set_marker_tag(int, const char*);

        void commit_infraction(Infraction);
        bool committed_infraction(Infraction);
        bool committed_any_infractions();
        bool ever_committed_any_infractions();
        void clear_infractions();

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

        void set_trade_charge();
        int get_trade_charge();

        int get_trace(Trace);
        void add_trace(Trace);
        void add_trace(Trace, int);

        void save_mc_month_end();
        void save_mc_month_start();
        int get_mc_month_end() { return mc_at_end_of_previous_month; }
        int get_mc_month_start() { return mc_at_start_of_current_month; }

        const int* get_gains_last_month(){ return mc_gains_last_month; }
        const int* get_losses_last_month(){ return mc_losses_last_month; }
        const int* get_gains_over_last_month(){ return mc_gains_over_last_month; }
        const int* get_losses_over_last_month(){ return mc_losses_over_last_month; }
        const int* get_gains_this_month(){ return mc_gains_this_month; }
        const int* get_losses_this_month(){ return mc_losses_this_month; }

        int nopirates;

        bool invalid_placement_seen;
        bool advanced_report_unlocked;
        bool advanced_galmap_unlocked;

        const char* get_officer_title(Officer);
        const char* get_officer_name(Officer);
        const char* get_officer_title_and_name(Officer);
        static const char* get_officer_character_desc(Officer);
        static const char* get_officer_character_title(Officer, OfficerQuality);
        static const char* get_officer_character_name(Officer, OfficerQuality);
        static const char* get_officer_character_title_and_name(Officer, OfficerQuality);

    private:
        void adjust_mc(int, MCReason);

        void refresh_full_name();
        int transfer(int, int*);

        Character character;
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
        int mc_at_end_of_previous_month;
        int mc_at_start_of_current_month;
        bool _intro_seen;
        bool _intro_comm_seen;
        int fleet_marker_idx;
        PlayerLocation location;
        Starship starship;
        Fleet fleet;
        uint16_t inventions;
        int reputation;
        bool guild_member;
        int tax;  // Orig: t%
        OfficerQuality officers[OFFICER_MAX];  // Orig: Ps%
        Mission mission;
        StarMarker star_markers[N_MARKERS];
        uint32_t infraction_mask;
        bool ever_committed_infraction;
        int trade_charge;
        int trace[TRACE_MAX];

        int mc_gains_lifetime[MAX_MC_CATEGORIES];
        int mc_losses_lifetime[MAX_MC_CATEGORIES];
        int mc_gains_last_month[MAX_MC_CATEGORIES];
        int mc_losses_last_month[MAX_MC_CATEGORIES];
        int mc_gains_over_last_month[MAX_MC_CATEGORIES];
        int mc_losses_over_last_month[MAX_MC_CATEGORIES];
        int mc_gains_this_month[MAX_MC_CATEGORIES];
        int mc_losses_this_month[MAX_MC_CATEGORIES];

        int mc_history[MAX_MC_HISTORY];

        int officer_idx(Officer, OfficerQuality);
        bool officers_fired_nopay[OFFICERS_TOTAL];
        bool officers_quit[OFFICERS_TOTAL];
        bool officers_deceased[OFFICERS_TOTAL];
        void officer_status_reset(Officer, OfficerQuality);

        // AI
        AIFlag ai_flags[9];
        int ai_tactic;
        int ai_hostile_to;
        int ai_attack_star;   // Orig: lordvar(), lower 8 bits
        int ai_attack_planet; // Orig: lordvar(), upper 8 bits

    // The ExodusState sets up the player data during game init.
    friend class ExodusState;
    friend class ExodusDebug;
};

#endif
