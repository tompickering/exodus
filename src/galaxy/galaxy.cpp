#include "galaxy.h"

#include <cstdlib>

#include "guild.h"
#include "star.h"
#include "planet.h"

#include "shared.h"

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

/*
// Data from the original game's font/plandat, read into DIM plandat(15,6)
const int PLANDAT[15][6] = {
    {1, 2, 3, 4, 5, 6},
    {1, 2, 3, 3, 4, 6},
    {1, 4, 4, 5, 5, 6},
    {2, 2, 3, 3, 4, 4},
    {1, 2, 3, 4, 5, 6},
    {1, 4, 4, 5, 5, 6},
    {2, 2, 3, 3, 4, 4},
    {1, 2, 3, 4, 5, 6},
    {1, 2, 4, 4, 5, 6},
    {4, 4, 4, 5, 5, 5},
    {2, 2, 3, 3, 4, 4},
    {1, 2, 2, 3, 4, 6},
    {1, 2, 3, 4, 4, 6},
    {1, 2, 4, 5, 5, 6},
    {4, 4, 4, 5, 5, 5}
};
*/

/*
 * The data that we actually use from 'plandat' expressed with enum values.
 * This determines the distribution of planet types within a system - so
 * the first planet's type is selected randomly from the first row etc.
 * Note that the planet types can change later. It is the case in the original
 * also that planets are never assigned to be Ice-type during initial generation.
 */
const PlanetClass PLANET_DISTRIB[5][6] = {
    {Forest, Desert, Volcano, Volcano, Rock, Terra},
    {Forest, Desert, Volcano, Volcano, Rock, Terra},
    {Desert, Desert, Volcano, Volcano, Rock, Rock},
    {Desert, Desert, Volcano, Volcano, Rock, Rock},
    {Desert, Desert, Volcano, Volcano, Rock, Rock},
};

Galaxy::Galaxy(int _n_stars) {
    n_stars = _n_stars;

    if (n_stars > GALAXY_MAX_STARS) {
        L.warn("Requested stars beyond max: %d", n_stars);
        n_stars = GALAXY_MAX_STARS;
    }

    new(&guild) Guild(GUILD_POS_X, GUILD_POS_Y);

    int star_x, star_y;
    uint64_t names_taken = 0;
    Star *star;
    for (int i = 0; i < n_stars; ++i) {
        while (true) {
            bool ok = true;

            star_x = rand() % GALAXY_COLS;
            star_y = rand() % GALAXY_ROWS;

            if (star_x == GUILD_POS_X && star_y == GUILD_POS_Y) {
                ok = false;
            }

            for (int j = 0; j < i; ++j) {
                if (star_x == stars[j].x  && star_y == stars[j].y) {
                    ok = false;
                    break;
                }
            }

            if (ok) break;
        }

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

        // Generate the star with name and position
        star = new(&stars[i]) Star(star_x, star_y, (STAR_NAMES[name_idx]));

        // Generate the star's planets
        int n_plans = RND(5);
        L.info("Star %s: Generating %d planets...", star->name, n_plans);
        for (int i = 0; i < n_plans; ++i) {
            Planet *tgt = star->next_planet_slot();

            if (!tgt) {
                // We are generating up to 5 planets in a star that should
                // have 5 slots and contain none already. There should be room!
                L.fatal("No planet slots during initial generation!");
            }

            PlanetClass cls = PLANET_DISTRIB[i][rand() % 6];
            new(tgt) Planet(cls);
        }
    }
}

Star* Galaxy::get_stars() {
    int i;
    return get_stars(i);
}

Star* Galaxy::get_stars(int &_n_stars) {
    _n_stars = n_stars;
    return stars;
}

Guild* Galaxy::get_guild() {
    return &guild;
}
