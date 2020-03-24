#include "galaxy.h"

#include <cstdlib>

#include "guild.h"
#include "star.h"

#include "../shared.h"

Galaxy::Galaxy(unsigned int n_stars) {
    if (n_stars > GALAXY_MAX_STARS) {
        L.warn("Requested stars beyond max: %d", n_stars);
        n_stars = GALAXY_MAX_STARS;
    }

    new(&guild) Guild(GUILD_POS_X, GUILD_POS_Y);
    for (unsigned int i = 0; i < n_stars; ++i) {
        int star_x = rand() % GALAXY_COLS;
        int star_y = rand() % GALAXY_ROWS;
        new(&stars[i]) Star(star_x, star_y);
    }
}
