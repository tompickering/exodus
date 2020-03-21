#ifndef GUARD_GALAXY_H
#define GUARD_GALAXY_H

#include "star.h"

#define GALAXY_MAX_STARS 40

class Galaxy {
    private:
        Star stars[GALAXY_MAX_STARS];
};

#endif
