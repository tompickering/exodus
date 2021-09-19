#include "iter.h"

extern ExodusState exostate;

Iterator::Iterator() : idx(0), end(0) {
}

void Iterator::operator++() {
    ++idx;
}

bool Iterator::complete() {
    return idx >= end;
}

StarIterator::StarIterator() : Iterator() {
    Galaxy *gal = exostate.get_galaxy();
    s = gal->get_stars(end);
}

Star* StarIterator::get() {
    if (complete()) return nullptr;
    return &s[idx];
}

PlanetIterator::PlanetIterator() : Iterator() {
    owner = -1;
    end = STAR_MAX_PLANETS;
}

PlanetIterator::PlanetIterator(int o) : Iterator() {
    owner = o;
    end = STAR_MAX_PLANETS;
}

void PlanetIterator::operator++() {
    for (;;) {
        if (complete()) {
            return;
        }

        if (++idx >= end) {
            ++star_iter;
            idx = 0;
        }
        Planet *p = get();
        if (!p) continue;
        if (!p->exists()) continue;
        if (owner >= 0 && p->get_owner() != owner) continue;
        break;
    }
}

bool PlanetIterator::complete() {
    return star_iter.complete();
}

Planet* PlanetIterator::get() {
    if (complete()) return nullptr;
    Star *s = star_iter.get();
    return s->get_planet(idx);
}
