#include "planet.h"

#include <cstring>

#include "state/exodus_state.h"

#include "assetpaths.h"
#include "shared.h"

extern ExodusState exostate;

PlanetSpriteSet sprite_sets[7];
bool sprite_sets_initialised = false;

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
    pspeed = (RND(5) + 5);
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

        diameter = RND(8)*1000 + 7*1000 + RND(9)*100;

        minerals = RND(diameter/40);

        if (get_class() == Volcano || get_class() == Ice) {
            minerals *= 2;
        }

        r = diameter / 1000;
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
        diameter = 12000;
        airdef_guns = 10;
        minerals = 0;
        sim = 2;
        pspeed = 10;

        // TODO: PROCdowp sets SIc (city count) to 1
        // Not reflected here because we want to compute city count from surface data
        // Should this planet get a default city somehow?
        // I believe the line assigning surf%(8,8)=5 is the city placement...
        // Why does SIna (n agruculture zones) get set to 6?
    }

    L.info("Generated type %d planet", (int)cls);
}

int Planet::get_diameter() {
    return diameter;
}

PlanetSize Planet::get_size() {
    if (get_diameter() < 10000)
        return PLANET_Small;
    if (get_diameter() > 12000)
        return PLANET_Large;
    return PLANET_Medium;
}

int Planet::get_settlement_cost() {
    switch(get_size()) {
        case PLANET_Small:
            return 90;
        case PLANET_Medium:
            return 100;
        case PLANET_Large:
            return 110;
    }

    L.fatal("Planet size is invalid");
    return 0;
}

int Planet::get_base_income() {
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

int Planet::get_income() {
    return get_base_income() + get_n_cities() * CITY_INCOME;
}

int Planet::get_population() {
    // TODO: This looks to be a combination of the number of cities
    // and particular natural surface features (orig: '2', 'alvil')
    int alvil = 0;
    return get_n_cities() + alvil;
}

int Planet::get_day_hours() {
    // Not certain why the original stored pspeed as 1/4 of the hours,
    // but we'll follow suit and convert within this accessor.
    return 4 * pspeed;
}

int Planet::get_n_cities() {
    // TODO - Orig SIc
    return 0;
}

int Planet::get_n_agri() {
    // TODO - Orig SIna
    return 0;
}

int Planet::get_owner() {
    return owner;
}

bool Planet::is_owned() {
    if (owner <0) return false;

    PlayerInfo *ownerinfo = exostate.get_player(owner);
    return !ownerinfo->dead;
}

void init_sprite_sets() {
    sprite_sets[Forest].panel_icon     = IMG_TS1_ST1;
    sprite_sets[Forest].landscape      = IMG_LS_FOREST;

    sprite_sets[Desert].panel_icon     = IMG_TS1_ST2;
    sprite_sets[Desert].landscape      = IMG_LS_DESERT;

    sprite_sets[Volcano].panel_icon    = IMG_TS1_ST3;
    sprite_sets[Volcano].landscape     = IMG_LS_VOLCANO;

    sprite_sets[Rock].panel_icon       = IMG_TS1_ST4;
    sprite_sets[Rock].landscape        = IMG_LS_ROCK;

    sprite_sets[Ice].panel_icon        = IMG_TS1_ST5;
    sprite_sets[Ice].landscape         = IMG_LS_ICE;

    sprite_sets[Terra].panel_icon      = IMG_TS1_ST6;
    sprite_sets[Terra].landscape       = IMG_LS_TERRA;

    sprite_sets[Artificial].panel_icon = IMG_TS1_ST8;
    sprite_sets[Artificial].landscape  = IMG_LS_ART;
}

const PlanetSpriteSet* Planet::sprites() {
    if (!sprite_sets_initialised) init_sprite_sets();
    return &sprite_sets[get_class()];
}

const char* Planet::get_class_str() {
    switch(get_class()) {
        case Forest:
            return "Forest";
        case Desert:
            return "Desert";
        case Volcano:
            return "Volcano";
        case Rock:
            return "Rock";
        case Ice:
            return "Ice";
        case Terra:
            return "Terra";
        case Artificial:
            return "Artificial";
    }

    L.fatal("No string for planet of class %d", get_class());
    return "";
}

const char* Planet::get_class_str_lower() {
    switch(get_class()) {
        case Forest:
            return "forest";
        case Desert:
            return "desert";
        case Volcano:
            return "volcano";
        case Rock:
            return "rock";
        case Ice:
            return "ice";
        case Terra:
            return "terra";
        case Artificial:
            return "artificial";
    }

    L.fatal("No lower-case string for planet of class %d", get_class());
    return "";
}
