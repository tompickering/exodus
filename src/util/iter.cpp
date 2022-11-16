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

int StarIterator::get_idx() {
    if (complete()) return -1;
    return idx;
}

PlanetIterator::PlanetIterator() : PlanetIterator(-1) {
}

PlanetIterator::PlanetIterator(int o) : Iterator() {
    owner = o;
    end = STAR_MAX_PLANETS;

    if (!valid()) {
        ++(*this);
    }
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

        if (valid()) {
            break;
        }
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

Star* PlanetIterator::get_star() {
    if (complete()) return nullptr;
    return star_iter.get();
}

int PlanetIterator::get_idx() {
    if (complete()) return -1;
    return idx;
}

int PlanetIterator::get_star_idx() {
    if (complete()) return -1;
    return star_iter.get_idx();
}

bool PlanetIterator::valid() {
    Planet *p = get();
    if (!(p && p->exists())) {
        return false;
    }
    if (owner >= 0 && !(p->get_owner() == owner)) {
        return false;
    }
    return true;
}
