#include "galaxy.h"

#include <cstdlib>

#include "guild.h"
#include "star.h"

#include "../shared.h"

const int N_STAR_NAMES = 50;
const char* STAR_NAMES[] = {
    "Phoenix", "Outpost", "Storm", "Spider", "Perseus",
    "Avalon", "Hades", "Paradise", "Dusk", "Raven",
    "Asgard", "Midgard", "Odin", "Thor", "Shadow",
    "Hydra", "Nexus", "Poseidon", "Apollo", "Walhalla",
    "Wind", "Nebula", "Fire", "Hope", "Genesis",
    "Behemoth", "Hawk", "Nirvana", "Spirit", "Illusion",
    "Ruby", "Pearl", "Onyx", "Diamond", "Origin",
    "Miracle", "Europe", "Asia", "River", "Beast",
    "Caseopeia", "Ocean", "Gladiator", "Capricorn", "Curse",
    "Bond", "Ring", "Lost", "Victoria", "Fenris",
};

Galaxy::Galaxy(unsigned int _n_stars) {
    n_stars = _n_stars;

    if (n_stars > GALAXY_MAX_STARS) {
        L.warn("Requested stars beyond max: %d", n_stars);
        n_stars = GALAXY_MAX_STARS;
    }

    new(&guild) Guild(GUILD_POS_X, GUILD_POS_Y);
    uint64_t names_taken = 0;
    for (unsigned int i = 0; i < n_stars; ++i) {
        int star_x = rand() % GALAXY_COLS;
        int star_y = rand() % GALAXY_ROWS;
        int name_idx;
        uint64_t name_flag;
        while (true) {
            name_idx = rand() % N_STAR_NAMES;
            name_flag = (uint64_t)1 << name_idx;
            if (!(names_taken & name_flag)) {
                names_taken |= name_flag;
                break;
            }
        }

        new(&stars[i]) Star(star_x, star_y, (STAR_NAMES[name_idx]));
    }
}

Star* Galaxy::get_stars(unsigned int &_n_stars) {
    _n_stars = n_stars;
    return stars;
}
