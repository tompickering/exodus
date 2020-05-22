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
    INV_MassProduction,
    INV_UltraRangeScanner,
    INV_FusionEngine,
    INV_MicroMechanicElements,
    INV_UniversalVaccine,
    INV_OrbitalBombs,
    INV_OrbitalMassConstruction,
    INV_OrbitalMassThrust,
    INV_WeatherInfluence,
    INV_MultiFunctionalVaccine,
    INV_Acid,
    INV_IndustryGuard,
    INV_DreamInfluence,
    INV_RaderExtension,
};

class Player {
    public:
        Player();
        const char* get_name();
        const char* get_title();
        const char* get_ref();
        const char* get_full_name();
        void set_name(const char*);
        void set_title(const char*);
        void set_ref(const char*);
        Race get_race();
        bool is_human();
        bool is_alive();
        int get_mc();
        int get_flag_idx();
        void set_flag_idx(int);
        bool intro_seen();
        void set_intro_seen();
        bool attempt_spend(unsigned int);
        const char *get_fleet_marker();
        void set_fleet_marker(const char*);
        PlayerLocation& get_location();
    private:
        Race race;
        char name[MAX_PLAYER_NAME + 1];
        char title[MAX_PLAYER_TITLE + 1];
        char ref[MAX_PLAYER_REFERENCE + 1];
        char full_name[MAX_PLAYER_FULLNAME + 1];
        bool dead;
        int flag_idx;
        int mc;
        bool _intro_seen;
        const char *fleet_marker;
        PlayerLocation location;
        Fleet fleet;
        uint16_t inventions;
        void refresh_full_name();

    // The ExodusState sets up the player data during game init.
    friend class ExodusState;
};

#endif
