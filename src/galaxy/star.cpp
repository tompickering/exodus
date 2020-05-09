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
    if (idx < 0 || idx >= STAR_MAX_PLANETS) {
        L.fatal("Attempt to index invalid planet index %d at %s", idx, name);
    }

    if (planets[idx].exists()) {
        return &planets[idx];
    }

    return nullptr;
}
