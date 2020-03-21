#ifndef GUARD_STAR_H
#define GUARD_STAR_H

#include "planet.h"

#define STAR_MAX_PLANETS 5

class Star {
    private:
        unsigned int n_planets;
        Planet planets[STAR_MAX_PLANETS];
};

#endif
