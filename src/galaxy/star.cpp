#include "star.h"

#include <cstdlib>

#include "state/exodus_state.h"

#include "shared.h"



Star::Star(int _x, int _y, const char* _name) : FlyTarget(_x, _y, true, _name) {
    size = (StarSize)(rand() % (STAR_Huge + 1));
    most_recent_active_planet = 0;
    L.debug("Star %s\t: %d %d %d", name, x, y, size);
}

StarSize Star::get_size() const {
    return size;
}

Planet* Star::next_planet_slot() {
    for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
        if (!planets[i].exists()) {
            return &planets[i];
        }
    }

    return nullptr;
}

Planet* Star::get_planet(int idx) {
    Planet *p = get_planet_nocheck(idx);

    if (p->exists()) {
        return p;
    }

    return nullptr;
}

Planet* Star::get_planet_nocheck(int idx) {
    if (idx < 0 || idx >= STAR_MAX_PLANETS) {
        L.fatal("Attempt to index invalid planet index %d at %s", idx, name);
    }

    return &planets[idx];
}

Planet* Star::get_artificial_world_slot() {
    return get_planet_nocheck(STAR_MAX_PLANETS - 1);
}

Planet* Star::construct_artificial_world(int player_idx, const char* name) {
    Planet *outer = get_artificial_world_slot();

    /*
     * N.B. It's primarily the responsibility of the caller to ensure that
     * an artificial world can be constructed here. This is because the
     * viability is not *just* informed by whether the star's outer slot
     * is empty or not; but *also* by whether there are any other artificial
     * worlds elsewhere in the galaxy which are scheduled to move here.
     * The caller must have verified that there aren't any.
     */
    if (outer->exists() || outer->get_construction_phase() > 0) {
        return nullptr;
    }

    L.debug("CONSTRUCTING ARTIFICIAL WORLD");

    new(outer) Planet(Artificial);
    outer->set_owner(player_idx, POCR_Constructed);
    if (name) {
        outer->set_name(name);
    } else {
        const char* name = "Genesis";

        if (FEATURE(EF_IMPROVED_PLANET_NAMING)) {
            name = outer->get_art_name_suggestion();
        }

        if (!exostate().planet_name_taken(name)) {
            outer->set_name(name);
        } else {
            bool set = false;
            for (int x = 0; x < 10; ++x) {
                const char* name2 = outer->get_art_name_suggestion();
                if (!exostate().planet_name_taken(name2)) {
                    outer->set_name(name2);
                    set = true;
                    break;
                }
            }

            if (!set) {
                // Just duplicate 'Genesis'
                outer->set_name(name);
            }
        }
    }

    return outer;
}

StarSize Star::expand() {
    if ((int)size < (int)STAR_Dwarf) {
        size = (StarSize)((int)(size+1));
    }
    return size;
}

void Star::save(cJSON* j) const {
    FlyTarget::save(j);
    SAVE_ENUM(j, size);
    SAVE_NUM(j, n_planets);
    SAVE_ARRAY_OF_SAVEABLE(j, planets);
    SAVE_NUM(j, most_recent_active_planet);
}

void Star::load(cJSON* j) {
    FlyTarget::load(j);
    LOAD_ENUM(j, size);
    LOAD_NUM(j, n_planets);
    LOAD_ARRAY_OF_SAVEABLE(j, planets);
    LOAD_NUM(j, most_recent_active_planet);
}
