#include "iter.h"

#include <algorithm>
#include <cstdlib>



Iterator::Iterator() : idx(0), end(0), randomise(false), random_data(nullptr) {
}

void Iterator::operator++() {
    ++idx;
}

bool Iterator::complete() {
    return idx >= end;
}

void Iterator::reset_idx() {
    idx = 0;
    if (randomise) {
        set_random();
    }
}

void Iterator::set_random() {
    randomise = true;

    if (!random_data) {
        random_data = (int*)malloc(end * sizeof(int));
    }

    for (int i = 0; i < end; ++i) {
        random_data[i] = i;
    }

    std::random_shuffle(random_data, random_data+end);
}

int Iterator::get_effective_idx() {
    if (randomise) {
        return random_data[idx];
    }

    return idx;
}

StarIterator::StarIterator() : Iterator() {
    Galaxy *gal = exostate().get_galaxy();
    s = gal->get_stars(end);
}

Star* StarIterator::get() {
    if (complete()) return nullptr;
    return &s[get_effective_idx()];
}

int StarIterator::get_idx() {
    if (complete()) return -1;
    return get_effective_idx();
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
            reset_idx();
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
    return s->get_planet(get_effective_idx());
}

Star* PlanetIterator::get_star() {
    if (complete()) return nullptr;
    return star_iter.get();
}

int PlanetIterator::get_idx() {
    if (complete()) return -1;
    return get_effective_idx();
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

void PlanetIteratorRandom::set_random() {
    PlanetIterator::set_random();

    if (!valid()) {
        ++(*this);
    }
}
