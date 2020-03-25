#ifndef GUARD_GALAXY_H
#define GUARD_GALAXY_H

#include "guild.h"
#include "star.h"

#define GALAXY_MAX_STARS 40

#define GALAXY_COLS 27
#define GALAXY_ROWS 17
#define GUILD_POS_X 14
#define GUILD_POS_Y 9

class Galaxy {
    public:
        Galaxy() {};
        Galaxy(unsigned int);
        Star* get_stars();
        Star* get_stars(unsigned int&);
        Guild* get_guild();
    private:
        Guild guild;
        Star stars[GALAXY_MAX_STARS];
        unsigned int n_stars;
};

#endif
