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

class Star : FlyTarget {
    public:
        Star() : Star(-1, -1, (char*)"NONAME") {};
        Star(int, int, const char*);
    private:
        StarSize size;
        unsigned int n_planets;
        Planet planets[STAR_MAX_PLANETS];
};

#endif
