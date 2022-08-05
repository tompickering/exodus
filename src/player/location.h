#ifndef GUARD_LOCATION_H
#define GUARD_LOCATION_H

#include <cstdint>

// TARGET_NONE is unique to CPU players and used as a flag in AI tactics
extern const int TARGET_NONE;

// Location of a player's fleet
class PlayerLocation {
    public:
        PlayerLocation();
        bool advance();
        void complete();
        bool in_flight();
        int get();
        void set(int);
        void set_target(int, int);
        int get_target() const;
        void set_planet_target(int);
        int get_planet_target();
        bool has_visited(int);
        void unset_target();
        bool is_target_set();
        int nopirates;
    private:
        int location;
        int target;            // -1=guild, 0+=star index - Orig: TS
        int planet_target;     // I think only used for CPU lords - Orig: TP
        int months_to_arrive;  // 'In flight' if > 0
        bool just_arrived;
        uint64_t visited;
};

#endif
