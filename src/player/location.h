#ifndef GUARD_LOCATION_H
#define GUARD_LOCATION_H

#include <cstdint>

// Location of a player's fleet
class PlayerLocation {
    public:
        PlayerLocation();
        bool advance();
        bool in_flight();
        void set(int);
        void set_target(int, int);
        int get_target();
        void set_planet_target(int);
        int get_planet_target();
        bool has_visited(int);
    private:
        int target;            // -1=guild, 0+=star index - Orig: TS
        int planet_target;     // I think only used for CPU lords - Orig: TP
        int months_to_arrive;  // 'In flight' if > 0
        bool just_arrived;
        uint64_t visited;
};

#endif
