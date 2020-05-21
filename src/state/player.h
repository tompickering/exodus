#ifndef GUARD_PLAYER_H
#define GUARD_PLAYER_H

#include <cstdint>

#define MAX_PLAYER_NAME      12
#define MAX_PLAYER_TITLE     12
#define MAX_PLAYER_REFERENCE 12
#define MAX_PLAYER_FULLNAME MAX_PLAYER_TITLE + 1 + MAX_PLAYER_NAME

// Location of a player's fleet
class PlayerLocation {
    public:
        PlayerLocation() {
            months_to_arrive = 0;
            target = -1;
            planet_target = 0;
            just_arrived = false;
            visited = 0;
        };
        void advance() {
            just_arrived = false;
            if (months_to_arrive > 0) {
                just_arrived = (--months_to_arrive == 0);
            }
            if (just_arrived && target >= 0) {
                visited |= (uint64_t)1 << target;
            }
        };
        bool in_flight() { return months_to_arrive > 0; };
        void set(int nt) { target = nt; months_to_arrive = 1; advance(); };
        void set_target(int nt, int m) { target = nt; months_to_arrive = m; };
        int get_target() { return target; };
        void set_planet_target(int nt) { planet_target = nt; };
        int get_planet_target() { return planet_target; };
        bool has_visited(int query_target) {
            // Guild (-1) is always true
            if (query_target < 0)
                return true;
            return (bool)(visited & ((uint64_t)1 << query_target));
        }
    private:
        int target; // -1=guild, 0+=star index - Orig: TS
        int planet_target; // I think only used for CPU lords - Orig: TP
        int months_to_arrive; // 'In flight' if > 0
        bool just_arrived;
        uint64_t visited;
};

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

typedef struct {
    Race race;
    char name[MAX_PLAYER_NAME + 1];
    char title[MAX_PLAYER_TITLE + 1];
    char ref[MAX_PLAYER_REFERENCE + 1];
    char full_name[MAX_PLAYER_FULLNAME + 1];
    bool dead;
    int flag_idx;
    unsigned int mc;
    bool intro_seen;
    const char *fleet_marker;
    PlayerLocation location;
    Fleet fleet;
    uint16_t inventions;
    bool attempt_spend(unsigned int cost) {
        if (mc >= cost) {
            mc -= cost;
            return true;
        }
        return false;
    }
} PlayerInfo;

#endif
