#ifndef GUARD_GUILD_H
#define GUARD_GUILD_H

#include "flytarget.h"

class Guild : public FlyTarget {
    public:
        Guild() : Guild(-1, -1) {};
        Guild(int, int);
};

#endif
