#include "star.h"

#include <cstdlib>

#include "shared.h"

Star::Star(int _x, int _y, const char* _name) : FlyTarget(_x, _y, true, _name) {
    size = (StarSize)(rand() % (STAR_Huge + 1));
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

    L.debug("CONSTRUCTING ARTIFICIAL WORLD");

    // TODO: On completion, world should not appear until following month
    new(outer) Planet(Artificial);
    outer->set_owner(player_idx);
    if (name) {
        outer->set_name(name);
    } else {
        // TODO: Check this is unique for CPU lords?
        outer->set_name("Genesis");
    }

    return outer;
}

StarSize Star::expand() {
    if ((int)size < (int)STAR_Dwarf) {
        size = (StarSize)((int)(size+1));
    }
    return size;
}
