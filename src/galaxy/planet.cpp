#include "planet.h"

#include <cstring>

#include "shared.h"

Planet::Planet() {
    _exists = false;
    name[0] = '\0';
}

Planet::Planet(PlanetClass _cls) : cls(_cls) {
    init();
}

bool Planet::exists() {
    return _exists;
}

PlanetClass Planet::get_class() {
    return cls;
}

const char* Planet::get_name() {
    return name;
}

void Planet::set_name(const char* _name) {
    strncpy(name, _name, PLANET_MAX_NAME);
}

void Planet::init() {
    int r;

    _exists = true;
    name[0] = '\0';
    owner = -1;

    income_adj = 0;
    day_hours = 4 * (RND(5) + 5);
    lunar_base = false;
    unrest = 0;
    army_funding = 0;
    airdef_guns = 0;
    robots = 0;

    reserves_min = 0;
    reserves_food = 0;
    reserves_plu = 0;

    army_inf = 0;
    army_gli = 0;
    army_art = 0;

    if (cls != Artificial) {
        r = RND(5);
        sit = r > 3 ? 1 : r;

        sid = RND(8)*1000 + 7*1000 + RND(9)*100;

        minerals = RND(sid/40);

        if (get_class() == Volcano || get_class() == Ice) {
            minerals *= 2;
        }

        r = sid / 1000;
        unsigned int a = 2;
        unsigned int b = 15;

        if (r < 10) {
            a = 3;
            b = 14;
            income_adj--;
        } else if (r > 12) {
            a = 1;
            b = 16;
            income_adj++;
        }

        // TODO: Initialise surface from the data read in from PROCloadplans

        sim = 0;

        r = RND(4);
        int c = 0;
        for (int d = 0; d <= 6; d += 2) {
            c++;
            if (r >= c) {
                int b = 1;
                if (RND(3) == 1) b = 2;
                if (RND(6) == 1) b = 3;
                sim += b << d;
            }
        }
    } else {
        // Artificial planets are initialised in orig: PROCdowp

        // Default for enemy lords; players may override
        set_name("Genesis");
        lunar_base = true;
        sit = 0;
        army_funding = 2;
        sid = 12000;
        airdef_guns = 10;
        minerals = 0;
        sim = 2;
        day_hours = 40;

        // TODO: PROCdowp sets SIc (city count) to 1
        // Not reflected here because we want to compute city count from surface data
        // Should this planet get a default city somehow?
        // I believe the line assigning surf%(8,8)=5 is the city placement...
        // Why does SIna (n agruculture zones) get set to 6?
    }

    L.info("Generated type %d planet", (int)cls);
}

int Planet::get_income() {
    unsigned int base = 0;
    switch(cls) {
        case Forest:
            base = 12;
            break;
        case Desert:
            base = 4;
            break;
        case Volcano:
            base = 2;
            break;
        case Rock:
            base = 6;
            break;
        case Ice:
            base = 6;
            break;
        case Terra:
            base = 10;
            break;
        case Artificial:
            // Artificial planets generate no inherent income
            break;
    }

    return base + income_adj;
}

int Planet::get_n_cities() {
    // TODO - Orig SIc
    return 0;
}

int Planet::get_n_agri() {
    // TODO - Orig SIna
    return 0;
}
