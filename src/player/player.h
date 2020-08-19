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
    unsigned int minerals;
    unsigned int food;
    unsigned int plutonium;
    unsigned int robots;
    unsigned int infantry;
    unsigned int gliders;
    unsigned int artillery;
} Freight;

typedef struct {
    unsigned int scouts;
    unsigned int transporters;
    unsigned int warships;
    unsigned int bombers;
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

enum Gender {
    GENDER_Female,
    GENDER_Male,
    GENDER_Other,
};

class Player {
    public:
        Player();
        void init_alien_name(int);
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
        bool is_active();
        int get_mc();
        int get_flag_idx();
        void set_flag_idx(int);
        bool intro_seen();
        void set_intro_seen();
        void give_mc(int);
        bool can_afford(unsigned int);
        bool attempt_spend(unsigned int);
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

    // The ExodusState sets up the player data during game init.
    friend class ExodusState;
};

#endif
