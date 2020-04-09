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
};

class Star : public FlyTarget {
    public:
        Star() : Star(-1, -1, (char*)"NONAME") {};
        Star(int, int, const char*);
        StarSize get_size() const;
        Planet* next_planet_slot();
    private:
        StarSize size;
        unsigned int n_planets;
        Planet planets[STAR_MAX_PLANETS];
};

#endif
