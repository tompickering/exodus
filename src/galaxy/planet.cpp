#include "planet.h"

#include <cstring>

#include "state/exodus_state.h"

#include "planet_names.h"
#include "loadplans_data.h"

#include "assetpaths.h"
#include "shared.h"

#include "util/value.h"

extern ExodusState exostate;

PlanetSpriteSet sprite_sets[7];
MoonSpriteSet moon_sprite_sets[3];
bool sprite_sets_initialised = false;


const char** get_names_for_class(PlanetClass cls) {
    if (cls == Forest)     return PLANET_NAMES_FOREST;
    if (cls == Desert)     return PLANET_NAMES_DESERT;
    if (cls == Volcano)    return PLANET_NAMES_VOLCANO;
    if (cls == Rock)       return PLANET_NAMES_ROCK;
    if (cls == Ice)        return PLANET_NAMES_ICE;
    if (cls == Terra)      return PLANET_NAMES_TERRA;
    L.fatal("Can't get names for invalid class %d", (int)cls);
    return nullptr;
}

int Planet::stone_cost(Stone st) {
    switch(st) {
        case STONE_Base:
            return 20;
        case STONE_Agri:
            return 3;
        case STONE_Mine:
            return 15;
        case STONE_Plu:
            return 25;
        case STONE_City:
            return 30;
        case STONE_Clear:
            return 5;
        case STONE_Inf:
            return 10;
        case STONE_Gli:
            return 20;
        case STONE_Art:
            return 30;
        case STONE_Port0:
            return 15;
        case STONE_Port1:
            return 10;
        case STONE_Port2:
            return 20;
        case STONE_Trade:
            return 100;
        case STONE_Park:
            return 10;
        default:
            L.warn("Checking cost of unbuildable stone %d", (int)st);
            return 0;
    }
}

Planet::Planet() {
    _exists = false;
    name[0] = '\0';
    traded = 0;
    laws = 0;
    surfchange_this_month = false;
    processing_in_progress = false;
    moon_cls = MOON_Dirt;
}

Planet::Planet(PlanetClass _cls) : cls(_cls) {
    init();
}

bool Planet::exists() {
    return _exists;
}

void Planet::destroy() {
    L.info("PLANET DESTROYED");
    _exists = false;
}

int Planet::get_construction_phase() {
    return construction_phase;
}

bool Planet::advance_construction_phase() {
    if (get_class() != Artificial) {
        return false;
    }
    if (construction_phase < 3) {
        if (++construction_phase >= 3) {
            _exists = true;
        }
        return true;
    }
    return false;
}

int Planet::get_quality() {
    if (!exists() || is_owned()) {
        return 0;
    }

    int quality = 1;

    switch (cls) {
        case Forest:
            quality = 5;
            break;
        case Desert:
            quality = 2;
            break;
        case Volcano:
            quality = 1;
            break;
        case Rock:
            quality = 4;
            break;
        case Ice:
            quality = 1;
            break;
        case Terra:
            quality = 5;
            break;
        case Artificial:
            quality = 10;
            break;
    }

    if (is_named()) {
        quality += 2;
    }

    return quality;
}

const int* get_loadplans_data(int file, PlanetClass cls, int r) {
    int offset = 0;
    // File offset
    offset += file * PLANET_BLOCKS_LG * PLANET_BLOCKS_LG * 6 * 5;
    // Planet class offset
    offset += PLANET_BLOCKS_LG * PLANET_BLOCKS_LG * (int)cls * 5;
    // Random 0-4 offset 'R'
    offset += PLANET_BLOCKS_LG * PLANET_BLOCKS_LG * r;
    return &loadplans_data[offset];
}

Stone stone_orig2new(int code) {
    /*
     * Only as much as needed for loadplans data.
     * Use new enums as much as possible!
     */
    if (code ==  0) return STONE_Clear;
    if (code ==  2) return STONE_Agri;
    if (code ==  8) return STONE_NaturalLarge;
    if (code ==  9) return STONE_NaturalSmall;
    if (code == 22) return STONE_Village;
    if (code == 29) return STONE_NaturalAnim;
    L.fatal("Translation for stone code %d not implemented", code);
    return STONE_Clear;
}

PlanetClass Planet::get_class() {
    return cls;
}

const char* Planet::get_name() {
    return name;
}

MoonClass Planet::get_moon_class() {
    return moon_cls;
}

void Planet::set_name(const char* _name) {
    strncpy(name, _name, PLANET_MAX_NAME);
}

bool Planet::is_named() {
    return strnlen(name, 1) > 0;
}

const char* Planet::get_name_suggestion() {
    PlanetClass cls = get_class();
    if (cls == Artificial)
        return "Genesis";
    return get_names_for_class(cls)[rand() % N_PLANET_NAMES];
}

void Planet::init() {
    int r;

    name[0] = '\0';

    construction_phase = 0;

    if (cls == Artificial) {
        _exists = false;
        construction_phase = 1;
        moon_cls = MOON_Ice;
    } else {
        _exists = true;
        moon_cls = (MoonClass)(rand() % 3);
    }

    owner = -1;
    traded = 0;
    taxes_collected = false;
    paid_to_leave = 0;

    pspeed = (RND(5) + 5);
    lunar_base = false;

    for (int i = 0; i < N_UNREST; ++i) {
        unrest[i] = 0;
    }

    army_funding = 0;
    airdef_guns = 0;
    robots = 0;

    reserves_min = 0;
    reserves_food = 0;
    reserves_plu = 0;

    army_inf = 0;
    army_gli = 0;
    army_art = 0;

    laws = 0;
    set_law(LAW_TradeMinerals,      true);
    set_law(LAW_TradeFood,          true);
    set_law(LAW_PrivateIndustry,    true);
    set_law(LAW_DifferentReligions, true);

    surfchange_this_month = false;
    processing_in_progress = false;

    for (int i = 0; i < (PLANET_BLOCKS_LG * PLANET_BLOCKS_LG); ++i) {
        surf[i] = STONE_Clear;
    }

    randomise_trade_quality();

    if (cls != Artificial) {
        diameter = RND(8)*1000 + 7*1000 + RND(9)*100;

        minerals = RND(diameter/40);

        if (get_class() == Volcano || get_class() == Ice) {
            minerals *= 2;
        }

        // TODO: Initialise surface from the data read in from PROCloadplans
        const int* initial_surf_data = get_loadplans_data(rand() % 3, cls, rand() % 5);
        for (int i = 0; i < PLANET_BLOCKS_LG * PLANET_BLOCKS_LG; ++i) {
            // The original uses y=0 for the bottom of the screen. We use the inverse.
            int orig_x = i % PLANET_BLOCKS_LG;
            int orig_y = i / PLANET_BLOCKS_LG;
            int new_y = PLANET_BLOCKS_LG - 1 - orig_y;
            surf[new_y*PLANET_BLOCKS_LG + orig_x] = stone_orig2new(initial_surf_data[i]);
        }

        cull_stones_to_size();

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
        diameter = 12000;
        airdef_guns = 10;
        minerals = 0;
        sim = 2;
        pspeed = 10;

        army_inf = 15;
        army_gli = 15;
        army_art = 15;

        // TODO: Why does SIna (n agruculture zones) get set to 6?

        int sz = get_size_blocks();
        for (int j = 1; j < sz-1; ++j) {
            for (int i = 1; i < sz-1; ++i) {
                int r = RND(6);
                if (r == 1) {
                    set_stone(i, j, STONE_NaturalLarge);
                }
                if (r > 1) {
                    set_stone(i, j, STONE_NaturalSmall);
                }
                if (r == 2 && onein(3)) {
                    set_stone(i, j, STONE_NaturalAnim);
                }
                if (r > 3) {
                    set_stone(i, j, STONE_Clear);
                }
            }
        }
        set_stone(10, 3, STONE_Agri);
        set_stone(10, 4, STONE_Agri);
        set_stone( 9, 3, STONE_Agri);
        set_stone( 9, 4, STONE_Agri);
        set_stone( 6, 7, STONE_City);
        set_stone( 7, 7, STONE_Base);
        set_stone( 5, 7, STONE_Base);
        set_stone( 6, 6, STONE_Base);
        set_stone( 6, 8, STONE_Base);
        set_stone( 2, 4, STONE_Gli);

        army_funding = 2;
    }

    // Shouldn't be necessary but makes sense
    month_reset();

    L.verb("Generated type %d planet", (int)cls);
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

int Planet::get_size_blocks() {
    PlanetSize size = get_size();
    if (size == PLANET_Small)
        return PLANET_BLOCKS_SM;
    if (size == PLANET_Medium)
        return PLANET_BLOCKS_MD;
    return PLANET_BLOCKS_LG;
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

    int size_adjustment = 0;
    if (get_size() == PLANET_Small) size_adjustment = -1;
    if (get_size() == PLANET_Large) size_adjustment = 1;
    return base + size_adjustment;
}

int Planet::get_income() {
    return get_base_income() + get_n_cities() * CITY_INCOME;
}

int Planet::get_net_income() {
    return get_income() - get_army_funding();
}

int Planet::get_population() {
    // In the original, there seems to be a bug in PROCstatus
    // whereby we count agri instead of villages!
    int mul = 1;
    unsigned int month = exostate.get_month();
    if (month > 3) mul = 5;
    if (month > 10) mul = 10;
    if (month > 25) mul = 15;
    if (month > 40) mul = 20;
    return mul * get_n_cities() + count_stones(STONE_Village);
}

int Planet::get_day_hours() {
    // Not certain why the original stored pspeed as 1/4 of the hours,
    // but we'll follow suit and convert within this accessor.
    return 4 * pspeed;
}

int Planet::get_n_cities() {
    // Orig SIc
    return count_stones(STONE_City);
}

int Planet::get_n_agri() {
    // Orig SIna
    return count_stones(STONE_Agri);
}

bool Planet::agri_sufficient() {
    return get_n_agri() >= (get_n_cities()*CITY_FOOD_REQ + 1);
}

bool Planet::has_lunar_base() {
    return lunar_base;
}

void Planet::build_lunar_base() {
    lunar_base = true;
}

void Planet::destroy_lunar_base() {
    lunar_base = false;
}

bool Planet::has_spaceport() {
    return has_stone(STONE_Port0)
        && has_stone(STONE_Port1)
        && has_stone(STONE_Port2);
}

int Planet::get_owner() {
    return owner;
}

void Planet::set_owner(int new_owner) {
    L.info("[%s] owner: %d -> %d", is_named() ? get_name() : "NEW PLANET", owner, new_owner);
    owner = new_owner;
}

void Planet::unset_owner() {
    set_owner(-1);
}

/*
 * get_stone() and set_stone() index the surface according
 * to the planet's size - i.e. bounded by get_size_blocks().
 */
Stone Planet::get_stone(int x, int y) {
    int real_x; int real_y;
    _to_real(x, y, real_x, real_y);
    return _get_stone(real_x, real_y);
}

void Planet::set_stone(int x, int y, Stone stone) {
    int real_x; int real_y;
    _to_real(x, y, real_x, real_y);
    _set_stone(real_x, real_y, stone);
}

Stone Planet::get_stone_wrap(int x, int y) {
    int size = get_size_blocks();
    while (x < 0) x += size;
    while (y < 0) y += size;
    return get_stone(x % size, y % size);
}

void Planet::set_stone_wrap(int x, int y, Stone stone) {
    int size = get_size_blocks();
    while (x < 0) x += size;
    while (y < 0) y += size;
    return set_stone(x % size, y % size, stone);
}

bool Planet::has_stone(Stone st) {
    for (int j = 0; j < get_size_blocks(); ++j) {
        for (int i = 0; i < get_size_blocks(); ++i) {
            if (get_stone(i, j) == st)
                return true;
        }
    }

    return false;
}

Stone Planet::get_random_point(int& x, int& y) {
    int sz = get_size_blocks();
    x = rand() % sz;
    y = rand() % sz;
    return get_stone(x, y);
}

bool Planet::find_random_stone(Stone st, int& x, int& y) {
    int count = count_stones(st);
    if (!count)
        return false;
    int idx = (rand() % count);
    int sz = get_size_blocks();
    for (int j = 0; j < sz; ++j) {
        for (int i = 0; i < sz; ++i) {
            if (get_stone(i, j) == st) {
                if (idx == 0) {
                    x = i; y = j;
                    return true;
                } else {
                    --idx;
                }
            }
        }
    }

    L.fatal("Could not find nth stone when we verified at least n exist");
    return false;
}

bool Planet::find_random_buildable_stone(int& x, int& y) {
    int count = count_stones(STONE_Clear) + count_stones(STONE_NaturalSmall);
    if (!count)
        return false;
    int idx = (rand() % count);
    int sz = get_size_blocks();
    for (int j = 0; j < sz; ++j) {
        for (int i = 0; i < sz; ++i) {
            Stone st = get_stone(i, j);
            if (st == STONE_Clear || st == STONE_NaturalSmall) {
                if (idx == 0) {
                    x = i; y = j;
                    return true;
                } else {
                    --idx;
                }
            }
        }
    }

    L.fatal("Could not find nth stone when we verified at least n exist");
    return false;
}

/*
 * First identify a random stone of type st which has >=1 buildable area around it.
 * Then, return a random buildable location around that stone.
 *
 * As such, the selection is biased in favour of spaces with fewer buildable spaces
 * adjacent to the target. I.E. if there are 3 viable areas - two next to target A
 * and one next to target B, then the probabilities of selection are 25%, 25% and 50%
 * respectively, rather than 33% apiece.
 */
bool Planet::find_random_buildable_stone_next_to_8(Stone st, int& x, int& y) {
    int count = 0;
    int sz = get_size_blocks();
    for (int j = 0; j < sz; ++j) {
        for (int i = 0; i < sz; ++i) {
            if (get_stone(i, j) == st) {
                if (next_to_8(i, j, STONE_Clear) || next_to_8(i, j, STONE_NaturalSmall)) {
                    ++count;
                }
            }
        }
    }
    if (!count)
        return false;
    int idx = (rand() % count);
    for (int j = 0; j < sz; ++j) {
        for (int i = 0; i < sz; ++i) {
            if (get_stone(i, j) == st) {
                if (next_to_8(i, j, STONE_Clear) || next_to_8(i, j, STONE_NaturalSmall)) {
                    if (idx == 0) {
                        // We'll select this target
                        // Randomly select a buildable neighbour
                        int buildable_count = 0;
                        for (int ii = -1; ii <= 1; ++ii) {
                            for (int jj = -1; jj <= 1; ++jj) {
                                if (ii == 0 && jj == 0) {
                                    continue;
                                }
                                Stone neighbour = get_stone(i + ii, j + jj);
                                if (neighbour == STONE_Clear || neighbour == STONE_NaturalSmall) {
                                    ++buildable_count;
                                }
                            }
                        }
                        int buildable_idx = (rand() % buildable_count);
                        for (int ii = -1; ii <= 1; ++ii) {
                            for (int jj = -1; jj <= 1; ++jj) {
                                if (ii == 0 && jj == 0) {
                                    continue;
                                }
                                Stone neighbour = get_stone(i + ii, j + jj);
                                if (neighbour == STONE_Clear || neighbour == STONE_NaturalSmall) {
                                    if (buildable_idx == 0) {
                                        x = i + ii;
                                        y = j + jj;
                                        return true;
                                    } else {
                                        --buildable_idx;
                                    }
                                }
                            }
                        }
                        L.fatal("Could not find buildable neighbour when we verified at least one existed");
                    } else {
                        --idx;
                    }
                }
            }
        }
    }

    L.fatal("Could not find neighbour stone when we verified at least one existed");
    return false;
}

bool Planet::next_to_4(int x, int y, Stone st) {
    if (get_stone_wrap(x - 1, y) == st) return true;
    if (get_stone_wrap(x + 1, y) == st) return true;
    if (get_stone_wrap(x, y + 1) == st) return true;
    if (get_stone_wrap(x, y - 1) == st) return true;
    return false;
}

bool Planet::next_to_8(int x, int y, Stone st) {
    for (int j = -1; j <= 1; ++j) {
        for (int i = -1; i <= 1; ++i) {
            if (i == 0 && j == 0)
                continue;
            if (get_stone_wrap(x+i, y+j) == st)
                return true;
        }
    }
    return false;
}

int Planet::count_stones(Stone st) {
    int count = 0;
    for (int j = 0; j < get_size_blocks(); ++j) {
        for (int i = 0; i < get_size_blocks(); ++i) {
            if (get_stone(i, j) == st)
                ++count;
        }
    }

    return count;
}

int Planet::get_food_production() {
    return count_stones(STONE_Agri);
}

int Planet::get_food_consumption() {
    // One food unit is implicitly needed
    // This is enough to support ALL command stations
    return count_stones(STONE_City)*3 + 1;
}

bool Planet::food_prod_sufficient() {
    return get_food_consumption() <= get_food_production();
}

int Planet::get_plu_production() {
    int mul = (cls == Volcano) ? 2 : 1;
    return count_stones(STONE_Plu) * mul;
}

int Planet::get_plu_consumption() {
    return count_stones(STONE_Mine)
         + count_stones(STONE_Inf)
         + count_stones(STONE_Gli)
         + count_stones(STONE_Art);
}

bool Planet::plu_prod_sufficient() {
    return get_plu_consumption() <= get_plu_production();
}

int Planet::get_army_funding() {
    validate_army_funding();
    return army_funding;
}

void Planet::adjust_army_funding(int adjustment) {
    army_funding += adjustment;
    validate_army_funding();
}

void Planet::validate_army_funding() {
    if (army_funding < 0)
        army_funding = 0;
    if (army_funding > get_income()) {
        army_funding = get_income();
    }
}

void Planet::discard_excess_resources() {
    int lim = get_resource_cap();
    int rlim = get_robot_cap();
    int d[8]; for (int i = 0; i < 8; ++i) d[i] = 0;

    if (reserves_food > lim) {
        d[0] = reserves_food - lim;
        reserves_food = lim;
    }
    if (reserves_min > lim) {
        d[1] = reserves_min - lim;
        reserves_min = lim;
    }
    if (reserves_plu > lim) {
        d[2] = reserves_plu - lim;
        reserves_plu = lim;
    }
    if (army_inf > lim) {
        d[3] = army_inf - lim;
        army_inf = lim;
    }
    if (army_gli > lim) {
        d[4] = army_gli - lim;
        army_gli = lim;
    }
    if (army_art > lim) {
        d[5] = army_art - lim;
        army_art = lim;
    }
    if (airdef_guns > lim) {
        d[6] = airdef_guns - lim;
        airdef_guns = lim;
    }
    if (robots > rlim) {
        d[7] = robots - rlim;
        robots = rlim;
    }

    int total = 0;
    for (int i = 0; i < 8; ++i) total += d[i];
    if (total > 0) {
        L.info("%s: DISCARDED %d/%d/%d/%d/%d/%d/%d/%d", get_name(),
               d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]);
    }
}

int Planet::get_army_required_mc() {
    int required = 0;
    for (int j = 0; j < get_size_blocks(); ++j) {
        for (int i = 0; i < get_size_blocks(); ++i) {
            Stone st = get_stone(i, j);
            if (st == STONE_Inf) required += 1;
            if (st == STONE_Gli) required += 2;
            if (st == STONE_Art) required += 3;
        }
    }
    return required;
}

bool Planet::army_funding_sufficient() {
    return get_army_funding() >= get_army_required_mc();
}

int Planet::get_minerals() {
    return minerals;
}

int Planet::get_unrest() {
    return get_unrest(0);
}

int Planet::get_unrest(int months_ago) {
    if (months_ago >= N_UNREST) {
        L.error("Request for unrest data earlier than we have");
        return -1;
    }
    return unrest[months_ago];
}

int Planet::get_airdef_guns() {
    return airdef_guns;
}

int Planet::get_robots() {
    return robots;
}

void Planet::adjust_airdef_guns(int adj) {
    airdef_guns = max(airdef_guns + adj, 0);
    if (airdef_guns > get_resource_cap()) {
        airdef_guns = get_resource_cap();
    }
}

void Planet::adjust_robots(int adj) {
    robots = max(robots + adj, 0);
    if (robots > get_robot_cap()) {
        robots = get_robot_cap();
    }
}

bool Planet::is_owned() {
    if (owner <0) return false;

    Player *ownerplayer = exostate.get_player(owner);
    return ownerplayer->is_participating();
}

/*
 * This shouldn't really be necessary as we should never consider
 * any surface indices outside of the planet boundary.
 * However, things seem cleaner this way.
 */
void Planet::cull_stones_to_size() {
    for (int i = 0; i < (PLANET_BLOCKS_LG - get_size_blocks()) / 2; ++i) {
        for (int j = 0; j < PLANET_BLOCKS_LG; ++j) {
            _set_stone(i, j, STONE_Clear);
            _set_stone(PLANET_BLOCKS_LG - 1 - i, j, STONE_Clear);
            _set_stone(j, i, STONE_Clear);
            _set_stone(j, PLANET_BLOCKS_LG - 1 - i, STONE_Clear);
        }
    }
}

// As opposed to get_stone() and set_stone() - do not map surface
// co-ords into a space constrained by the planet's size.
Stone Planet::_get_stone(int x, int y) {
    if (!_real_in_bounds(x, y))
        return STONE_Clear;
    return surf[y*PLANET_BLOCKS_LG + x];
}

void Planet::_set_stone(int x, int y, Stone stone) {
    surf[y*PLANET_BLOCKS_LG + x] = stone;
    // Ensure that changes to number of cities etc are
    // accompanied by an army funding cap as appropriate.
    validate_army_funding();
}

void Planet::_to_real(int x, int y, int& real_x, int& real_y) {
    int sz = get_size_blocks();
    while (x < 0) x += sz;
    while (y < 0) y += sz;
    int offset = (PLANET_BLOCKS_LG - sz) / 2;
    real_x = (x % sz) + offset;
    real_y = (y % sz) + offset;
}

bool Planet::_real_in_bounds(int x, int y) {
    int offset = (PLANET_BLOCKS_LG - get_size_blocks()) / 2;
    if (x < offset || x >= offset + get_size_blocks()) return false;
    if (y < offset || y >= offset + get_size_blocks()) return false;
    return true;
}

void init_sprite_sets() {
    sprite_sets[Forest].panel_icon                     = IMG_TS1_ST1;
    sprite_sets[Forest].landscape                      = IMG_LS_FOREST;
    sprite_sets[Forest].map_bg                         = IMG_PLANET_FOREST;
    sprite_sets[Forest].surf                           = IMG_SF1;
    sprite_sets[Forest].bulletin_bg                    = IMG_PL1_PLANET1;
    sprite_sets[Forest].spaceport                      = IMG_HARBOUR_FOREST;

    sprite_sets[Desert].panel_icon                     = IMG_TS1_ST2;
    sprite_sets[Desert].landscape                      = IMG_LS_DESERT;
    sprite_sets[Desert].map_bg                         = IMG_PLANET_DESERT;
    sprite_sets[Desert].surf                           = IMG_SF2;
    sprite_sets[Desert].bulletin_bg                    = IMG_PL2_PLANET2;
    sprite_sets[Desert].spaceport                      = IMG_HARBOUR_DESERT;

    sprite_sets[Volcano].panel_icon                    = IMG_TS1_ST3;
    sprite_sets[Volcano].landscape                     = IMG_LS_VOLCANO;
    sprite_sets[Volcano].map_bg                        = IMG_PLANET_VOLCANO;
    sprite_sets[Volcano].surf                          = IMG_SF3;
    sprite_sets[Volcano].bulletin_bg                   = IMG_PL3_PLANET3;
    sprite_sets[Volcano].spaceport                     = IMG_HARBOUR_VOLCANO;

    sprite_sets[Rock].panel_icon                       = IMG_TS1_ST4;
    sprite_sets[Rock].landscape                        = IMG_LS_ROCK;
    sprite_sets[Rock].map_bg                           = IMG_PLANET_ROCK;
    sprite_sets[Rock].surf                             = IMG_SF4;
    sprite_sets[Rock].bulletin_bg                      = IMG_PL4_PLANET4;
    sprite_sets[Rock].spaceport                        = IMG_HARBOUR_ROCK;

    sprite_sets[Ice].panel_icon                        = IMG_TS1_ST5;
    sprite_sets[Ice].landscape                         = IMG_LS_ICE;
    sprite_sets[Ice].map_bg                            = IMG_PLANET_ICE;
    sprite_sets[Ice].surf                              = IMG_SF5;
    sprite_sets[Ice].bulletin_bg                       = IMG_PL5_PLANET5;
    sprite_sets[Ice].spaceport                         = IMG_HARBOUR_ICE;

    sprite_sets[Terra].panel_icon                      = IMG_TS1_ST6;
    sprite_sets[Terra].landscape                       = IMG_LS_TERRA;
    sprite_sets[Terra].map_bg                          = IMG_PLANET_TERRA;
    sprite_sets[Terra].surf                            = IMG_SF6;
    sprite_sets[Terra].bulletin_bg                     = IMG_PL6_PLANET6;
    sprite_sets[Terra].spaceport                       = IMG_HARBOUR_TERRA;

    sprite_sets[Artificial].panel_icon                 = IMG_TS1_ST8;
    sprite_sets[Artificial].landscape                  = IMG_LS_ART;
    sprite_sets[Artificial].map_bg                     = IMG_PLANET_ART;
    sprite_sets[Artificial].surf                       = IMG_SF8;
    sprite_sets[Artificial].bulletin_bg                = IMG_PL8_PLANET8;
    sprite_sets[Artificial].spaceport                  = IMG_HARBOUR_ART;

    moon_sprite_sets[MOON_Dirt].bg                     = IMG_GF1;
    moon_sprite_sets[MOON_Dirt].cover0                 = IMG_GF1_30;
    moon_sprite_sets[MOON_Dirt].cover1                 = IMG_GF1_31;

    moon_sprite_sets[MOON_Ice].bg                      = IMG_GF2;
    moon_sprite_sets[MOON_Ice].cover0                  = IMG_GF2_30;
    moon_sprite_sets[MOON_Ice].cover1                  = IMG_GF2_31;

    moon_sprite_sets[MOON_Grass].bg                    = IMG_GF3;
    moon_sprite_sets[MOON_Grass].cover0                = IMG_GF3_30;
    moon_sprite_sets[MOON_Grass].cover1                = IMG_GF3_31;

    sprite_sets_initialised = true;
}

const PlanetSpriteSet* Planet::sprites() {
    if (!sprite_sets_initialised) init_sprite_sets();
    return &sprite_sets[get_class()];
}

const MoonSpriteSet* Planet::moon_sprites() {
    if (!sprite_sets_initialised) init_sprite_sets();
    return &moon_sprite_sets[get_moon_class()];
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

// Implements PROCgivestation - renamed because we do more than just that!
void Planet::prepare_for_cpu_lord() {
    bool no_station = !has_stone(STONE_Base);
    int x, y;
    Stone s = STONE_Clear;
    int blocks = get_size_blocks();

    if (no_station) {
        // Place a HQ randomly
        while (true) {
            x = rand() % blocks;
            y = rand() % blocks;
            s = get_stone(x, y);
            if (   s == STONE_Clear
                || s == STONE_NaturalSmall
                || s == STONE_Radiation
                || s == STONE_AgriDead
                || s == STONE_Rubble) {
                set_stone(x, y, STONE_Base);
                break;
            }
        }

        // Place a city randomly
        // (I presume this is to ensure we can fund the military installation)
        while (true) {
            x = rand() % blocks;
            y = rand() % blocks;
            s = get_stone(x, y);
            if (   s == STONE_Clear
                || s == STONE_NaturalSmall
                || s == STONE_Radiation
                || s == STONE_AgriDead
                || s == STONE_Rubble) {
                set_stone(x, y, STONE_City);
                break;
            }
        }

        // Place a military installation randomly
        while (true) {
            x = rand() % blocks;
            y = rand() % blocks;
            s = get_stone(x, y);
            if (   s == STONE_Clear
                || s == STONE_NaturalSmall
                || s == STONE_Radiation
                || s == STONE_AgriDead
                || s == STONE_Rubble) {
                int r = rand() % 3;
                Stone military = STONE_Inf;
                army_funding = 1;
                if (r == 1) { military = STONE_Gli; army_funding = 2; }
                if (r == 2) { military = STONE_Art; army_funding = 3; }
                set_stone(x, y, military);
                break;
            }
        }

        // Place a plutonium plant randomly
        while (true) {
            x = rand() % blocks;
            y = rand() % blocks;
            s = get_stone(x, y);
            if (   s == STONE_Clear
                || s == STONE_NaturalSmall
                || s == STONE_Radiation
                || s == STONE_AgriDead
                || s == STONE_Rubble) {
                set_stone(x, y, STONE_Plu);
                break;
            }
        }

        // Place some agriculture
        int iterations = (get_class() == Desert || get_class() == Volcano) ? 2 : 1;
        for (int i = 0; i < iterations; ++i) {
            int agri_placed = 0;
            while (agri_placed <= 3) {
                int r = rand() % 4;
                if (r == 0) {x = 1; y = 1;}
                if (r == 1) {x = blocks - 5; y = blocks - 5;}
                if (r == 2) {x = 1; y = blocks - 5;}
                if (r == 3) {x = blocks - 5; y = 1;}
                for (int iy = y; iy < y + 3; ++iy) {
                    for (int ix = x; ix < x + 3; ++ix) {
                        s = get_stone(ix, iy);
                        if (   s == STONE_Clear
                            || s == STONE_NaturalSmall
                            || s == STONE_Radiation
                            || s == STONE_AgriDead
                            || s == STONE_Rubble) {
                            set_stone(ix, iy, STONE_Agri);
                            ++agri_placed;
                        }
                    }
                }
            }
        }
    } else {
        // Ensure that there's enough agri to sustain it
        int agri_placed = 0;
        while (agri_placed <= 3) {
            x = rand() % blocks;
            y = rand() % blocks;
            s = get_stone(x, y);
            if (   s == STONE_Clear
                || s == STONE_NaturalSmall
                || s == STONE_Radiation
                || s == STONE_AgriDead
                || s == STONE_Rubble) {
                set_stone(x, y, STONE_Agri);
                ++agri_placed;
            }

            if (rand() % 30 == 0)
                break;
        }
    }
}

void Planet::reset_unrest() {
    unrest[0] = 0;
}

void Planet::adjust_unrest(int adjustment) {
    unrest[0] += adjustment;
    if (unrest[0] < 0) unrest[0] = 0;
    // Can't see any ref to a maximum in the original
    // Riots happen when >8, rebel attacks at >9
}

void Planet::surfchange() {
    // Artifical planet climates don't change
    if (cls == Artificial)
        return;

    PlanetClass new_cls = Forest;
    if (onein(2)) {
        if (cls == Forest)  new_cls = Desert;
        if (cls == Desert)  new_cls = Terra;
        if (cls == Volcano) new_cls = Ice;
        if (cls == Rock)    new_cls = Desert;
        if (cls == Ice)     new_cls = Rock;
        if (cls == Terra)   new_cls = Forest;
    } else {
        if (cls == Forest)  new_cls = Desert;
        if (cls == Desert)  new_cls = Volcano;
        if (cls == Volcano) new_cls = Ice;
        if (cls == Rock)    new_cls = Ice;
        if (cls == Ice)     new_cls = Volcano;
        if (cls == Terra)   new_cls = Rock;
    }

    change_class(new_cls);
    surfchange_this_month = true;
}

bool Planet::surfchange_happened_this_month() {
    return surfchange_this_month;
}

void Planet::change_class(PlanetClass new_cls) {
    if (cls == new_cls) {
        L.warn("Attempt to set class to its current one");
        return;
    }

    L.info("CLIMATE CHANGE: %d -> %d", (int)cls, (int)new_cls);
    cls = new_cls;

    // Ensure that changes to income due to class are
    // accompanied by an army funding cap as appropriate.
    validate_army_funding();
}

bool Planet::trade_possible(int player_idx) {
    return (bool)(!(traded & (1 << player_idx)));
}

TradeQuality Planet::initiate_trade(int player_idx) {
    if (!trade_possible(player_idx)) {
        return TRADE_None;
    }

    traded |= (1 << player_idx);
    return trade;
}

void Planet::month_reset() {
    // Reset trade records and tax collection status
    traded = 0;
    taxes_collected = false;
    paid_to_leave = 0;
    surfchange_this_month = false;
    processing_in_progress = false;
}

void Planet::randomise_trade_quality() {
    int r = RND(5);
    trade = TRADE_Bad;
    if (r == 1) trade = TRADE_Fair;
    if (r == 2) trade = TRADE_Good;
    L.debug("%s: Trade is now %d", name, (int)trade);
}

bool Planet::can_collect_taxes() {
    return is_owned() && !taxes_collected;
}

void Planet::collect_taxes() {
    if (!can_collect_taxes()) {
        L.warn("Attempt to collect taxes when we can't at %s", name);
        return;
    }

    Player *p = exostate.get_player(get_owner());
    adjust_unrest(4);
    p->give_mc(get_tax_amount());
    taxes_collected = true;
}

int Planet::get_tax_amount() {
    return get_income() / 2;
}

bool Planet::may_be_attacked_by(int player_idx) {
    return !((bool)(paid_to_leave & (1 << player_idx)));
}

void Planet::this_month_prevent_attacks_by(int player_idx) {
    // TODO: This should be called when we are paid to cease an attack
    paid_to_leave |= (1 << player_idx);
}

void Planet::set_law(Law law, bool legal) {
    laws |= (1 << (int)law);
}

bool Planet::has_law(Law law) {
    return (bool)(laws & (1 << (int)law));
}

bool Planet::laws_cause_unrest() {
    // This can probably be simpler and I really shouldn't be using the bit values...
    // ...but I've just copied PROCcal_plan here.
    uint16_t _laws = laws & 0xFFF8;
    bool ok = (_laws == 0xC8 || _laws == 0xD8 || _laws == 0x80 || _laws == 0x90
            || _laws == 0xA0 || _laws == 0xB0 || _laws == 0x32 || _laws == 0x30);
    if (has_law(LAW_FreeSpeech) ^ has_law(LAW_AllowSystemEnemies)) ok = false;
    if (has_law(LAW_CivilianWeapons)) ok = false;
    return !ok;
}

void Planet::clear_radiation() {
    int size = get_size_blocks();
    for (int j = 0; j < size; ++j) {
        for (int i = 0; i < size; ++i) {
            if (get_stone(i, j) == STONE_Radiation) {
                if (onein(10)) {
                    // This looks a bit weird on Volcano / Ice planets
                    set_stone(i, j, STONE_AgriDead);
                }
            }
        }
    }
}

void Planet::produce_food() {
    reserves_food += get_food_production();
}

void Planet::produce_plutonium() {
    reserves_plu += get_plu_production();
}

ProductionReport Planet::produce_military() {
    ProductionReport rpt;

    rpt.no_money = false;
    rpt.no_space = false;
    rpt.no_plu = false;
    rpt.inf = 0;
    rpt.gli = 0;
    rpt.art = 0;
    rpt.not_produced = 0;

    if (!is_owned()) {
        return rpt;
    }

    int cap = get_resource_cap();
    int funds = get_army_funding();

    int max_inf = count_stones(STONE_Inf);
    int max_gli = count_stones(STONE_Gli);
    int max_art = count_stones(STONE_Art);

    /*
     * SUGGEST:
     *
     * It would be kinder to the player to perform these in reverse order:
     * Artillery -> Gliders -> Infantry
     * This would help to ensure maximum funding utility, as the player isn't
     * receiving this money whatever the case.
     * E.g. when funding is 3 and we have 1 inf and 1 artillery, we can't afford
     * both, and we end up with just 1 inf with 2MC wasted. In the alternate case,
     * we have one artillery and 0 MC wasted.
     */

    int prod_inf = max_inf;
    int prod_gli = max_gli;
    int prod_art = max_art;

    if (prod_inf > reserves_plu) {
        prod_inf = reserves_plu;
        rpt.no_plu = true;
    }
    if (army_inf + prod_inf > cap) {
        prod_inf = (cap - army_inf);
        rpt.no_space = true;
    }
    while (prod_inf*1 > funds) {
        prod_inf--;
        rpt.no_money = true;
    }
    army_inf += prod_inf;
    reserves_plu -= prod_inf;
    funds -= prod_inf*1;

    if (prod_gli > reserves_plu) {
        prod_gli = reserves_plu;
        rpt.no_plu = true;
    }
    if (army_gli + prod_gli > cap) {
        prod_gli = (cap - army_gli);
        rpt.no_space = true;
    }
    while (prod_gli*2 > funds) {
        prod_gli--;
        rpt.no_money = true;
    }
    army_gli += prod_gli;
    reserves_plu -= prod_gli;
    funds -= prod_gli*2;

    if (prod_art > reserves_plu) {
        prod_art = reserves_plu;
        rpt.no_plu = true;
    }
    if (army_art + prod_art > cap) {
        prod_art = (cap - army_art);
        rpt.no_space = true;
    }
    while (prod_art*3 > funds) {
        prod_art--;
        rpt.no_money = true;
    }
    army_art += prod_art;
    reserves_plu -= prod_art;
    funds -= prod_art*3;

    rpt.inf = prod_inf;
    rpt.gli = prod_gli;
    rpt.art = prod_art;

    rpt.not_produced = (max_inf + max_gli + max_art)
                     - (prod_inf + prod_gli + prod_art);

    return rpt;
}

int Planet::consume_food() {
    int needed = get_food_consumption();
    if (reserves_food < needed) {
        // Not enough food
        needed -= reserves_food;
        reserves_food = 0;
        return needed;
    }
    reserves_food -= needed;
    return 0;
}

TradeReport Planet::monthly_trade() {
    TradeReport rpt;
    int n = count_stones(STONE_Trade);

    if (n == 0) {
        L.error("Trade requested on planet with no trade centre");
    }

    if (!is_owned()) {
        L.fatal("Trade requested on unowned planet");
    }

    Player *o = exostate.get_player(get_owner());


    rpt.mi = 0;
    rpt.fd = 0;
    rpt.pl = 0;
    rpt.mc = 0;

    int max_trade_mi = min(reserves_min,   50*n);
    int max_trade_fd = min(reserves_food, 100*n);
    int max_trade_pl = min(reserves_plu,   50*n);

    if (has_law(LAW_TradeMinerals) && max_trade_mi > 1) {
        rpt.mi = RND(max_trade_mi);
    }

    if (has_law(LAW_TradeFood) && max_trade_fd > 5) {
        rpt.fd = RND(max_trade_fd - 4);
    }

    if (has_law(LAW_TradePlutonium) && max_trade_pl > 1) {
        rpt.pl = RND(max_trade_pl);
    }

    rpt.mc += rpt.mi * 2;
    reserves_min -= rpt.mi;

    rpt.mc += max(rpt.fd / 2, rpt.fd > 0 ? 1 : 0);
    reserves_food -= rpt.fd;

    rpt.mc += rpt.pl * 2;
    reserves_plu -= rpt.pl;

    if (rpt.pl > 0) {
        // TODO: Criminal record for plutonium trade!
    }

    o->give_mc(rpt.mc);

    return rpt;
}

void Planet::mine() {
    int madd = 0;

    if (minerals <= 0) {
        return;
    }

    if (cls == Rock) {
        int size = get_size_blocks();
        for (int j = 0; j < size; ++j) {
            for (int i = 0; i < size; ++i) {
                if (get_stone(i, j) == STONE_Mine && next_to_4(i, j, STONE_NaturalAnim)) {
                    madd++;
                }
            }
        }
    }

    int units_to_mine = count_stones(STONE_Mine);
    // We can only mine as much as we have plutonium for
    if (reserves_plu < units_to_mine) units_to_mine = reserves_plu;
    units_to_mine += madd;
    // We can only mine as much as exists!
    if (units_to_mine > minerals) units_to_mine = minerals;
    reserves_min += units_to_mine;
    // Any more than we can hold are discarded!
    if (reserves_min > get_resource_cap())
        reserves_min = get_resource_cap();
    reserves_plu -= (units_to_mine - madd);
    minerals -= units_to_mine;
}

void Planet::perish_food() {
    reserves_food -= (reserves_food / 6);
}

int Planet::get_resource_cap() {
    return (diameter / 200);
}

int Planet::get_robot_cap() {
    return (diameter / 100);
}

void Planet::update_unrest_history() {
    for (int i = N_UNREST - 1; i > 0; --i) {
        unrest[i] = unrest[i - 1];
    }
}

const char* Planet::get_moon_bg() {
    if (moon_cls == MOON_Dirt) return IMG_GF1;
    if (moon_cls == MOON_Ice) return IMG_GF2;
    return IMG_GF3;
}

bool Planet::has_army() {
    return (army_inf > 0) || (army_gli > 0) || (army_art > 0);
}

void Planet::get_army(int& inf, int& gli, int& art) {
    inf = army_inf;
    gli = army_gli;
    art = army_art;
}

int Planet::get_army_size() {
    return army_inf + army_gli + army_art;
}

int Planet::get_army_size_weighted() {
    return army_inf + army_gli*2 + army_art*3;
}

void Planet::adjust_army(int inf, int gli, int art) {
    army_inf += inf;
    army_gli += gli;
    army_art += art;
    if (army_inf < 0) army_inf = 0;
    if (army_gli < 0) army_gli = 0;
    if (army_art < 0) army_art = 0;
    int max = get_resource_cap();
    if (army_inf > max) army_inf = max;
    if (army_gli > max) army_gli = max;
    if (army_art > max) army_art = max;
}

void Planet::clear_army() {
    army_inf = 0;
    army_gli = 0;
    army_art = 0;
}

bool Planet::army_full() {
    return get_army_size() >= 3*get_resource_cap();
}

void Planet::adjust_reserves(int adj_min, int adj_fd, int adj_plu) {
    reserves_min += adj_min;
    reserves_food += adj_fd;
    reserves_plu += adj_plu;
    if (reserves_min < 0) reserves_min = 0;
    if (reserves_food < 0) reserves_food = 0;
    if (reserves_plu < 0) reserves_plu = 0;
    int max = get_resource_cap();
    if (reserves_min > max) reserves_min = max;
    if (reserves_food > max) reserves_food = max;
    if (reserves_plu > max) reserves_plu = max;
}

int Planet::get_reserves_min() {
    return reserves_min;
}

int Planet::get_reserves_food() {
    return reserves_food;
}

int Planet::get_reserves_plu() {
    return reserves_plu;
}

int Planet::get_total_reserves() {
    return reserves_min + reserves_food + reserves_plu;
}

void Planet::disown() {
    unset_owner();
    army_inf = 0;
    army_gli = 0;
    army_art = 0;
    unrest[0] = 0;
    army_funding = 0;
}

bool Planet::expand_city() {
    return expand(STONE_City);
}

bool Planet::expand_village() {
    return expand(STONE_Village);
}

bool Planet::expand(Stone st) {
    L.info("Expanding %d on %s", st, get_name());

    // Find expansion candidates
    int count = 0;
    int sz = get_size_blocks();
    for (int j = 0; j < sz; ++j) {
        for (int i = 0; i < sz; ++i) {
            if (get_stone(i, j) == st) {
                bool valid = false;
                if (next_to_4(i, j, STONE_Clear)) valid = true;
                if (next_to_4(i, j, STONE_NaturalSmall)) valid = true;
                // Villages can expand onto cities!  This is explicit in PROCcs_alexpand.
                if (st == STONE_Village && next_to_4(i, j, STONE_City)) valid = true;

                if (valid) {
                    ++count;
                }
            }
        }
    }

    if (count == 0) {
        // Can't find an expansion candidate
        return false;
    }

    int idx = rand() % count;

    for (int j = 0; j < sz; ++j) {
        for (int i = 0; i < sz; ++i) {
            if (get_stone(i, j) == st) {
                bool valid = false;
                if (next_to_4(i, j, STONE_Clear)) valid = true;
                if (next_to_4(i, j, STONE_NaturalSmall)) valid = true;
                // Villages can expand onto cities!  This is explicit in PROCcs_alexpand.
                if (st == STONE_Village && next_to_4(i, j, STONE_City)) valid = true;

                if (valid) {
                    if (idx == 0) {
                        // Expand this stone
                        uint8_t dirs_tried = 0;
                        while (dirs_tried < 0xF) {
                            int dir = rand() % 4;
                            if (dirs_tried & (1 << dir))
                                continue;
                            dirs_tried |= (1 << dir);
                            int i_off = 0;
                            int j_off = 0;
                            if (dir == 0) i_off++;
                            if (dir == 1) i_off--;
                            if (dir == 2) j_off++;
                            if (dir == 3) j_off--;
                            Stone tgt = get_stone_wrap(i + i_off, j + j_off);
                            bool tgt_ok = tgt == STONE_Clear || tgt == STONE_NaturalSmall;
                            if (st == STONE_Village) {
                                tgt_ok = tgt_ok || tgt == STONE_City;
                            }
                            if (tgt_ok) {
                                set_stone_wrap(i + i_off, j + j_off, st);
                                L.info("Expansion %d,%d", i, j);
                                return true;
                            }
                        }
                    } else {
                        --idx;
                    }
                }
            }
        }
    }

    L.fatal("Could not find suitable expansion after verifying it must be possible");
    return false;
}

bool Planet::agri_collapse() {
    // Lakes and oases on Forest, Terra and Rock protect agri
    bool natural_protection = false;
    int r = 0;
    switch (cls) {
        case Forest:
            r = 1; natural_protection = true;
            break;
        case Desert:
            r = 4; natural_protection = true;
            break;
        case Volcano:
            r = 5;
            break;
        case Rock:
            r = 3;
            break;
        case Ice:
            r = 5;
            break;
        case Terra:
            r = 2; natural_protection = true;
            break;
        case Artificial:
            r = -5;
            break;
    }

    if (!onein(10 - r)) {
        return false;
    }

    if (is_owned() && exostate.get_player(get_owner())->has_invention(INV_Acid)) {
        return false;
    }

    int collapsed = 0;
    int to_kill = RND(r*2);
    int sz = get_size_blocks();
    L.info("%s: Up to %d agri collapsing", get_name(), to_kill);
    for (int y = (rand() % sz); y < sz; ++y) {
        for (int x = (rand() % sz); x < sz; ++x) {
            if (to_kill > 0 && get_stone(x, y) == STONE_Agri) {
                if (natural_protection) {
                    if (next_to_8(x, y, STONE_NaturalAnim)) {
                        continue;
                    }
                }
                set_stone(x, y, STONE_AgriDead);
                ++collapsed;
                --to_kill;
            }
        }
    }

    L.info("%s: %d agri collapsed", get_name(), collapsed);
    return collapsed > 0;
}

void Planet::monthly_processing_start() {
    processing_in_progress = true;
}

bool Planet::monthly_processing_in_progress() {
    return processing_in_progress;
}

void Planet::plunder() {
    // Orig sets to 2 - so if we have none then pirates give us food!
    reserves_food = min(reserves_food, 2);
    int sz = get_size_blocks();
    for (int j = 0; j < sz; ++j) {
        for (int i = 0; i < sz; ++i) {
            if (onein(2)) {
                Stone st = get_stone(i, j);
                if (st == STONE_Agri) set_stone(i, j, STONE_AgriDead);
                if (st == STONE_City) set_stone(i, j, STONE_Rubble);
            }
        }
    }

}

Stone random_military() {
    int i = rand() % 3;
    if (i == 0) return STONE_Inf;
    if (i == 1) return STONE_Gli;
    return STONE_Art;
}

void Planet::ai_update() {
    // AI planet developments originally done in PROCenemytactics / PROCeta*

    if (!is_owned()) {
        L.warn("AI update on unowned planet");
        return;
    }

    Player *owner = exostate.get_player(get_owner());

    if (owner->is_human()) {
        L.warn("AI update on human-owned planet");
        return;
    }

    if (!owner->is_participating()) {
        L.warn("AI update on planet with non-participating owner");
        return;
    }

    bool stop = false;
    do {
        if (owner->has_invention(INV_WeatherInfluence)) {
            PlanetClass c = get_class();
            bool viable = (c == Desert || c == Volcano || c == Ice || c == Rock);
            if (viable && onein(c == Rock ? 20 : 10)) {
                // FIXME: Should be a cost variable somewhere
                if (owner->attempt_spend(500)) {
                    PlanetClass new_class = Desert;
                    switch (c) {
                        case Desert:
                            new_class = Ice;
                            break;
                        case Volcano:
                            new_class = Desert;
                            break;
                        case Rock:
                            new_class = Terra;
                            break;
                        case Ice:
                            new_class = Rock;
                            break;
                        default:
                            L.fatal("Terraform target not viable");
                            break;
                    }
                    change_class(new_class);
                }
            }
        }

        // FIXME: Can count these in one pass!
        int free = count_stones(STONE_Clear) + count_stones(STONE_NaturalSmall);
        const int stations = count_stones(STONE_Base);
        const int food = count_stones(STONE_Agri);
        const int mining = count_stones(STONE_Mine);
        const int pluton = count_stones(STONE_Plu);
        const int possfree = count_stones(STONE_NaturalLarge);
        const int dirt = count_stones(STONE_AgriDead) + count_stones(STONE_Rubble);
        const int produce_inf = count_stones(STONE_Inf);
        const int produce_gli = count_stones(STONE_Gli);
        const int produce_art = count_stones(STONE_Art);
        const int port_a = count_stones(STONE_Port0);
        const int port_b = count_stones(STONE_Port1);
        const int port_c = count_stones(STONE_Port2);
        const int tcentre = count_stones(STONE_Trade);
        const int cities = count_stones(STONE_City);

        int action = 0;

        const int m = exostate.get_orig_month();

        // FIXME: Should probably have a method for this
        bool spaceport = (port_a > 0 && port_b > 0 && port_c > 0);

        if (cities*CITY_FOOD_REQ + 5 < food) {
            action = 7;
        }

        if (possfree + free > 6 && cities < 40) {
            action = 1;
        }

        if (get_robots() <= cities*2) {
            action = 11;
        }

        if (   get_minerals() > 15
            && owner->get_mc() > 40
            && mining < 3
            && m > 5) {
            action = 4;
        }

        if (onein(3)) {
            action = 9;
        }

        if (   get_n_agri() > 60
            && owner->get_mc() >= 100
            && tcentre == 0) {
            action = 5;
        }

        if (   reserves_min > 40
            && owner->get_mc() >= 100
            && tcentre == 0) {
            action = 5;
        }

        if (dirt > 20 && m > 30) {
            action = 12;
        }

        if (   owner->get_mc() > COST_LUNAR_BASE
            && m > 10
            && get_class() != Artificial) {
            action = 6;
        }

        if (cities < 4) {
            action = 1;
        }

        if (get_army_size() < 120 && m > 10) {
            action = 2;
        }

        if (!spaceport) {
            action = 3;
        }

        if (produce_inf + produce_gli + produce_art < 2 && cities > 2) {
            action = 2;
        }

        if (produce_inf + produce_gli + produce_art > cities) {
            action = 1;
        }

        if (cities == 0) {
            action = 1;
        }

        if (produce_inf + produce_gli + produce_art == 0) {
            action = 2;
        }

        if (get_class() == Artificial && cities < 5) {
            action = 1;
        }

        if (pluton < produce_inf + produce_gli + produce_art + mining) {
            action = 13;
        }

        if (stations < 2) {
            action = 10;
        }

        if (free <= 7 || (free <= 10 && m > 50)) {
            action = 8;
        }

        if (!agri_sufficient() || food < 1) {
            action = 7;
        }

        L.verb("[%s] BUILD TACTIC: %d", owner->get_full_name(), action);

        stop = stop || action == 0;

        // Apply action (Orig: PROCeta*)
        switch (action) {
            case 1:
                {
                    // BUILD CITIES + AGRI
                    /*
                     * TODO: There are two apparent bugs in PROCeta1.
                     *         * The charge is 45MC regardless of agri_needed
                     *         * This happens when >=42 MC are avaiable, allowing -ve MC
                     *
                     *       These two issues are redressed here, but this could impact
                     *       the balance of the game. Consider restoring these issues.
                     */
                    int agri_needed = 4;
                    if (cls == Desert || cls == Volcano) {
                        agri_needed = 7;
                    }
                    int cost = stone_cost(STONE_City) + agri_needed*stone_cost(STONE_Agri);
                    for (int i = 0; i < RND(5); ++i) {
                        if (free >= agri_needed+1) {
                            if (owner->attempt_spend(cost)) {
                                free -= ai_place_stone(1, STONE_City, STONE_City);
                                free -= ai_place_stone(agri_needed, STONE_Agri, STONE_Agri);
                            }
                        }
                    }
                }
                break;
            case 2:
                // BUILD MILITARY + PLU
                for (int i = 0; i < RND(5); ++i) {
                    Stone to_build = random_military();
                    Stone target_neighbour = random_military();
                    int cost = stone_cost(to_build) + stone_cost(STONE_Plu);
                    if (free > 1 && owner->attempt_spend(cost)) {
                        free -= ai_place_stone(1, to_build, target_neighbour);
                        free -= ai_place_stone(1, STONE_Plu, STONE_NaturalLarge);
                    }
                }
                break;
            case 3:
                // BUILD SPACEPORT
                {
                    int cost = stone_cost(STONE_Port0)
                             + stone_cost(STONE_Port1)
                             + stone_cost(STONE_Port2);
                    if (free >= 3 && owner->attempt_spend(cost)) {
                        free -= ai_place_stone(1, STONE_Port0, STONE_Base);
                        free -= ai_place_stone(1, STONE_Port1, STONE_Port0);
                        free -= ai_place_stone(1, STONE_Port2, STONE_Port1);
                    }
                }
                break;
            case 4:
                // BUILD MINE + PLU
                {
                    int cost = stone_cost(STONE_Mine) + stone_cost(STONE_Plu);
                    for (int i = 0; i < RND(4); ++i) {
                        if (free > 1 && owner->attempt_spend(cost)) {
                            free -= ai_place_stone(1, STONE_Mine, STONE_Mine);
                            free -= ai_place_stone(1, STONE_Plu, STONE_NaturalLarge);
                        }
                    }
                }
                break;
            case 5:
                // BUILD TRADE CENTRE
                // TODO: PROCeta5 doesn't check that the owner can afford the cost
                // and can result in -ve MC. Should we retain that behaviour here?
                if (free > 0 && owner->attempt_spend(stone_cost(STONE_Trade))) {
                    free -= ai_place_stone(1, STONE_Trade, STONE_City);
                }
                break;
            case 6:
                // BUILD LUNAR BASE
                // TODO: PROCeta6 doesn't check that the owner can afford the cost
                // and can result in -ve MC. Should we retain that behaviour here?
                if (!has_lunar_base() && owner->attempt_spend(COST_LUNAR_BASE)) {
                    build_lunar_base();
                } else {
                    // TODO: PROCeta6 sets action=2 in this case. It looks ineconsequential,
                    // but verify.
                }
                break;
            case 7:
                // HANDLE HUNGER BY REPLACING CITIES WITH AGRI
                if (!agri_sufficient() && free < 5) {
                    _ai_make_space();
                    // TODO: PROCeta7 doesn't recalculate free after potentially
                    // creating space - should we retain that behaviour?
                    free = count_stones(STONE_Clear) + count_stones(STONE_NaturalSmall);
                }
                if (free > 0) {
                    if (!agri_sufficient() && (free + possfree) < 5) {
                        // TODO: Original always finds first cities
                        int n = 2;
                        int x, y;
                        while (n > 0 && find_random_stone(STONE_City, x, y)) {
                            int cost = stone_cost(STONE_Clear) + stone_cost(STONE_Agri);
                            if (owner->attempt_spend(cost)) {
                                set_stone(x, y, STONE_Agri);
                            } else {
                                break;
                            }
                            --n;
                        }
                    }
                    int r = RND(5) + 5;
                    if (r > free) {
                        r = rand() % free;
                    }
                    // TODO: PROCeta7 skips MC check here
                    if (owner->attempt_spend(r * stone_cost(STONE_Agri))) {
                        free -= ai_place_stone(r, STONE_Agri, STONE_Agri);
                    }
                }
                break;
            case 8:
                // CLEAR USELESS STONES
                _ai_make_space();
                free = count_stones(STONE_Clear) + count_stones(STONE_NaturalSmall);
                break;
            case 9:
                // BUILD AIRDEF GUNS
                {
                    int p = get_resource_cap();
                    int guns = get_airdef_guns();
                    if (guns < p) {
                        int r = RND(20);
                        if (!owner->can_afford(r*COST_AIRDEF)) {
                            r = owner->get_mc() / COST_AIRDEF;
                        }
                        if (r + guns > p) {
                            r = p - guns;
                        }
                        if (owner->attempt_spend(r * COST_AIRDEF)) {
                            airdef_guns += r;
                        } else {
                            L.error("Could not afford MC that we checked we had");
                        }
                    }
                }
                break;
            case 10:
                // BUILD A BACKUP BASE
                // TODO: PROCeta10 skips MC check here
                if (owner->attempt_spend(stone_cost(STONE_Base))) {
                    free -= ai_place_stone(1, STONE_Base, STONE_Base);
                }
                break;
            case 11:
                // BUILD ROBOTS
                {
                    int r = RND(3);
                    // TODO: PROCeta11 skips MC check here
                    // TODO: Should we check we're within resource limits here?! (Note get_robot_cap())
                    // These robots might be scrapped on discard_excess_resources()!
                    if (owner->attempt_spend(r * COST_ROBOT)) {
                        robots += r;
                    }
                }
                break;
            case 12:
                // CLEAR DEAD AGRI / RUBBLE
                {
                    int sz = get_size_blocks();
                    for (int i = 0; i < 20; ++i) {
                        bool placed = false;
                        for (int y = rand() % sz; y < sz; ++y) {
                            for (int x = rand() % sz; x < sz; ++x) {
                                Stone s = get_stone(x, y);
                                if (s == STONE_AgriDead || s == STONE_Rubble) {
                                    // TODO: PROCeta12 skips MC check
                                    if (owner->attempt_spend(stone_cost(STONE_Clear))) {
                                        set_stone(x, y, STONE_Clear);
                                        break;
                                    }
                                }
                            }
                            if (placed) {
                                break;
                            }
                        }
                    }
                }
                break;
            case 13:
                // BUILD PLUTONIUM
                {
                    // TODO: It appears PROCeta13 doesn't account for the extra
                    // plutonium produced on volcanic planets, as get_plu_production()
                    // does, and just takes the number of plutonium reactors.
                    // Should we preserve this behaviour?
                    int to_build = get_plu_consumption() - get_plu_production();
                    // TODO: Orig zeroes credits here if <0 - if we allow -ve MC, we should add this
                    // FIXME: Should we halve (rounding up) build count of volcano worlds?
                    int crd = owner->get_mc();
                    int cost = stone_cost(STONE_Plu);
                    if (to_build > (crd/cost)+3) {
                        to_build = (crd/cost)+1;
                    }
                    for (int i = 0; i < to_build; ++i) {
                        // TODO: PROCeta13 skips MC check
                        if (owner->attempt_spend(cost)) {
                            free -= ai_place_stone(1, STONE_Plu, STONE_NaturalLarge);
                        }
                    }
                }
                break;
            default:
                break;
        }

        free = count_stones(STONE_Clear) + count_stones(STONE_NaturalSmall);

        stop = stop || m > 2;
        stop = stop || owner->get_mc() < 30;
        stop = stop || free < 5;
        stop = stop || onein(5);
        stop = stop || action == 0;
    } while (!stop);

    // Allocate max MC to army without overspending
    army_funding = get_army_required_mc();
    validate_army_funding();

    laws = 0;
    set_law(LAW_TradeMinerals, true);
    set_law(LAW_TradeFood, true);
    switch (owner->get_flag(4)) {
        case AI_Hi:
            set_law(LAW_DrugLegalisation, true);
            set_law(LAW_TradePlutonium, true);
            break;
        case AI_Md:
            set_law(LAW_PrivateIndustry, true);
            set_law(LAW_DifferentReligions, true);
            set_law(LAW_TradePlutonium, true);
            break;
        case AI_Lo:
            set_law(LAW_FreeSpeech, true);
            set_law(LAW_PrivateIndustry, true);
            set_law(LAW_AllowSystemEnemies, true);
            set_law(LAW_DifferentReligions, true);
            break;
    }
}

void Planet::_ai_make_space() {
    if (!is_owned()) {
        L.fatal("Tried to make space on unowned planet");
        return;
    }

    Player *owner = exostate.get_player(get_owner());

    if (!owner->is_participating()) {
        L.fatal("Tried to make space on a planet owned by an inert player");
        return;
    }

    if (!owner->can_afford(stone_cost(STONE_Clear))) {
        return;
    }

    int ab = owner->get_flag(4) == AI_Hi ? STONE_Village : STONE_Rubble;
    Stone ac = (get_minerals() <= 0) ? STONE_Mine : STONE_Rubble;
    int sz = get_size_blocks();
    for (int i = 0; i < 25; ++i) {
        bool placed = false;
        for (int y = rand() % sz; y < sz; ++y) {
            for (int x = rand() % sz; x < sz; ++x) {
                Stone s = get_stone(x, y);
                if (   s == STONE_AgriDead
                    || s == STONE_NaturalLarge
                    || s == STONE_NaturalAnim
                    || s == STONE_Rubble
                    || s == ab
                    || s == ac) {
                    if (owner->attempt_spend(stone_cost(STONE_Clear))) {
                        set_stone(x, y, STONE_Clear);
                        placed = true;
                        break;
                    } else {
                        // No point in continuing if we can't afford to clear space anyway
                        return;
                    }
                }
            }
            if (placed) {
                break;
            }
        }
    }
}

bool Planet::_ai_place_random(Stone to_place) {
    int x; int y;
    bool found = find_random_buildable_stone(x, y);
    if (found) {
        L.verb("%s: AI placing stone randomly at (%d, %d)", get_name(), x, y);
        set_stone(x, y, to_place);
        return true;
    }
    return false;
}

bool Planet::_ai_place_tactical(Stone to_place, Stone target_neighbour) {
    int x; int y;
    bool found = find_random_buildable_stone_next_to_8(target_neighbour, x, y);
    if (found) {
        L.verb("%s: AI placing stone %d tactically at (%d, %d)", get_name(), (int)to_place, x, y);
        set_stone(x, y, to_place);
        return true;
    }
    return false;
}

// Orig: PROCpl_stone
// Places n to_place stones in a free spot next to a target_neighbour
// (8-connectivity) if possible - otherwise place randomly.
// Return number placed.
int Planet::ai_place_stone(int n, Stone to_place, Stone target_neighbour) {
    int placed = n;
    for (int i = 0; i < n; ++i) {
        if (!_ai_place_tactical(to_place, target_neighbour)) {
            if (!_ai_place_random(to_place)) {
                --placed;
            }
        }
    }
    if (placed < n) {
        L.warn("Tried to place %d of %d but only %d could be placed", n, (int)to_place, placed);
    }
    return placed;
}
