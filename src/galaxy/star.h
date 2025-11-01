#ifndef GUARD_STAR_H
#define GUARD_STAR_H

#include "flytarget.h"
#include "planet.h"

#define STAR_MAX_PLANETS 5

enum StarSize {
    STAR_Small,
    STAR_Medium,
    STAR_Large,
    STAR_Huge,
    STAR_Expand1,
    STAR_Expand2,
    STAR_Expand3,
    STAR_Expand4,
    STAR_Dwarf,
};

class Star : public FlyTarget {
    public:
        virtual void save(cJSON*) const override;
        virtual void load(cJSON*) override;

        Star() : Star(-1, -1, (char*)"NONAME") {};
        Star(int, int, const char*);
        StarSize get_size() const;
        Planet* next_planet_slot();
        Planet* get_planet(int);
        Planet* get_planet_nocheck(int);
        Planet* get_artificial_world_slot();
        Planet* construct_artificial_world(int, const char*);
        StarSize expand();

        int most_recent_active_planet;

    private:
        StarSize size;
        int n_planets;
        Planet planets[STAR_MAX_PLANETS];
};

#endif
