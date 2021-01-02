#ifndef GUARD_PLAYER_H
#define GUARD_PLAYER_H

#include <cstdint>

#include "location.h"

#define MAX_PLAYER_NAME      12
#define MAX_PLAYER_TITLE     12
#define MAX_PLAYER_REFERENCE 12
#define MAX_PLAYER_FULLNAME MAX_PLAYER_TITLE + 1 + MAX_PLAYER_NAME

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
} Starship;

typedef struct {
    int minerals;
    int food;
    int plutonium;
    int robots;
    int infantry;
    int gliders;
    int artillery;
} Freight;

typedef struct {
    int scouts;
    int transporters;
    int warships;
    int bombers;
    Freight freight;
} Fleet;

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
        void init_alien_name(int);
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
        bool is_guild_member();
        int get_mc();
        int get_flag_idx();
        void set_flag_idx(int);
        bool intro_seen();
        void set_intro_seen();
        void give_mc(int);
        bool can_afford(int);
        bool attempt_spend(int);
        bool attempt_spend_with_remaining(int, int);
        const char *get_fleet_marker();
        void set_fleet_marker(const char*);
        PlayerLocation& get_location();
        Starship& get_starship();
        int get_reputation();
        void adjust_reputation(int);
        bool can_research(Invention);
        bool research_invention(Invention);
        bool has_invention(Invention);
        InventionType get_invention_type(Invention);
        const Fleet& get_fleet();
        void init_tax();
        int get_tax();
        void set_tax(int);
        void adjust_tax(int);
        OfficerQuality get_officer(Officer);
        void set_officer(Officer, OfficerQuality);

        // AI
        AIFlag get_flag(int);
        int get_tactic();
        void set_tactic(int);
        bool is_hostile_to(int);
        int get_hostile_to();
        void set_hostile_to(int);
        void clear_hostility();
    private:
        Race race;
        Gender gender;
        char name[MAX_PLAYER_NAME + 1];
        char title[MAX_PLAYER_TITLE + 1];
        char ref[MAX_PLAYER_REFERENCE + 1];
        char full_name[MAX_PLAYER_FULLNAME + 1];
        bool dead;
        bool participating_in_game;
        int flag_idx;
        int mc;
        bool _intro_seen;
        const char *fleet_marker;
        PlayerLocation location;
        Starship starship;
        Fleet fleet;
        uint16_t inventions;
        int reputation;
        void refresh_full_name();
        bool guild_member;
        int tax;  // Orig: t%
        OfficerQuality officers[OFF_MAX];  // Orig: Ps%

        // AI
        AIFlag ai_flags[9];
        int ai_tactic;
        int ai_hostile_to;

    // The ExodusState sets up the player data during game init.
    friend class ExodusState;
};

#endif
