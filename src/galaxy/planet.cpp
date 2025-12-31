#include "planet.h"

#include <cstring>

#include "state/exodus_state.h"

#include "planet_names.h"
#include "loadplans_data.h"

#include "assetpaths.h"
#include "shared.h"
#include "exodus_features.h"

#include "util/iter.h"
#include "util/value.h"



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

    if (cls == Artificial) return PLANET_NAMES_ART;

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
        case STONE_Academy:
            return 150;
        case STONE_TradePort:
            return 100;
        default:
            L.warn("Checking cost of unbuildable stone %d", (int)st);
            return 0;
    }
}

Planet::Planet() {
    _exists = false;
    name[0] = '\0';
    construction_phase = 0;
    traded = 0;
    laws = 0;
    most_recent_previous_owner = -1;
    most_recent_previous_owner_change_reason = POCR_Init;
    owner_changes_this_month_head = 0;
    academy_source0[0] = '\0';
    academy_source1[0] = '\0';
    academy_source2[0] = '\0';
    academy_source3[0] = '\0';
    academy_source4[0] = '\0';
    academy_sources_this_month_head = 0;

    festival_this_month = false;
    surfchange_this_month = false;
    comms_this_month = 0;
    failed_attacks_this_month = 0;
    bombings_this_month = 0;
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
    exostate().stabilise_disappeared_planet(this);
}

int Planet::get_construction_phase() {
    return construction_phase;
}

bool Planet::advance_construction_phase() {
    if (get_class() != Artificial) {
        return false;
    }
    if (construction_phase < 3) {
        ++construction_phase;
        /*
         * N.B. We don't 'exist' until finalise_construction()
         */
        return true;
    }
    return false;
}

int Planet::get_star_target() {
    return star_target;
}

void Planet::set_star_target(int tgt) {
    L.info("%s: Setting star target: %d", get_name(), tgt);
    star_target = tgt;
}

void Planet::clear_star_target() {
    star_target = -1;
}

bool Planet::finalise_construction() {
    if (get_class() != Artificial) {
        return false;
    }
    if (construction_phase >= 3) {
        _exists = true;
        construction_phase = 0;
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

void Planet::set_unique_name_if_unnamed() {
    if (is_named()) {
        return;
    }

    // Try to ensure unique name here. Orig suffixes ' 2', ' 3' etc

    const char* _name = "";

    // Try a bit harder to find a good name before numbering
    if (FEATURE(EF_IMPROVED_PLANET_NAMING)) {
        for (int i = 0; i < 20; ++i) {
            _name = get_name_suggestion();

            if (!exostate().planet_name_taken(_name)) {
                set_name(_name);
                return;
            }
        }
    }

    for (int i = 0; i < 20; ++i) {
        _name = get_name_suggestion();

        if (!exostate().planet_name_taken(_name)) {
            set_name(_name);
            return;
        }

        if (i > 10) {
            char _name2[PLANET_MAX_NAME];
            snprintf(_name2, sizeof(_name2), "%s2", _name);

            if (!exostate().planet_name_taken(_name2)) {
                set_name(_name2);
                return;
            }
        }
    }

    // Whatever
    set_name(get_name_suggestion());
}

bool Planet::is_named() {
    return strnlen(name, 1) > 0;
}

const char* Planet::get_name_suggestion() {
    PlanetClass cls = get_class();

    if (cls == Artificial) {
        if (FEATURE(EF_IMPROVED_PLANET_NAMING)) {
            return get_art_name_suggestion();
        } else {
            return "Genesis";
        }
    }

    return get_names_for_class(cls)[rand() % N_PLANET_NAMES];
}

const char* Planet::get_art_name_suggestion() {
    return get_names_for_class(Artificial)[rand() % N_ART_PLANET_NAMES];
}

void Planet::init() {
    int r;

    name[0] = '\0';

    construction_phase = 0;
    star_target = -1;

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

    most_recent_previous_owner = -1;
    most_recent_previous_owner_change_reason = POCR_Init;

    owner_changes_this_month_head = 0;
    academy_source0[0] = '\0';
    academy_source1[0] = '\0';
    academy_source2[0] = '\0';
    academy_source3[0] = '\0';
    academy_source4[0] = '\0';
    academy_sources_this_month_head = 0;
    festival_this_month = false;
    surfchange_this_month = false;
    comms_this_month = 0;
    failed_attacks_this_month = 0;
    bombings_this_month = 0;
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
    return get_base_income() + get_city_income();
}

int Planet::get_city_income() {
    return get_n_cities() * CITY_INCOME;
}

int Planet::get_net_income() {
    return get_income() - get_army_funding();
}

int Planet::get_population() {
    // In the original, there seems to be a bug in PROCstatus
    // whereby we count agri instead of villages!
    int mul = 1;
    unsigned int month = exostate().get_month();
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

int Planet::get_prev_owner() {
    return most_recent_previous_owner;
}

void Planet::set_owner(int new_owner, PlanetOwnerChangedReason reason) {
    L.info("[%s] owner: %d -> %d", is_named() ? get_name() : "NEW PLANET", owner, new_owner);

    if (owner >= 0) {
        most_recent_previous_owner = owner;
        most_recent_previous_owner_change_reason = reason;
    }

    if (owner_changes_this_month_head < MAX_OWNER_CHANGES) {
        owner_changes_this_month[owner_changes_this_month_head++].set(owner, new_owner, reason);
    } else {
        L.error("Too many ownership changes tracked");
    }

    owner = new_owner;
    /*
     * When an artificial planet is taken over, ensure any scheduled moves are cleared.
     * In original, this is part of PROCenemyattack and PROCenemyplayerattack; i.e. it
     * is only cleared when the CPU takes over a planet.
     * However, we clear it on any owner change.
     */
    clear_star_target();
}

void Planet::unset_owner(PlanetOwnerChangedReason reason) {
    set_owner(-1, reason);
}

/*
 * get_stone() and set_stone() index the surface according
 * to the planet's size - i.e. bounded by get_size_blocks().
 */
Stone Planet::get_stone(int x, int y) {
    int real_x; int real_y;
    if (_to_real(x, y, real_x, real_y)) {
        return _get_stone(real_x, real_y);
    }

    return STONE_Invalid;
}

void Planet::set_stone(int x, int y, Stone stone) {
    int real_x; int real_y;
    if (_to_real(x, y, real_x, real_y)) {
        _set_stone(real_x, real_y, stone);
    }
}

Stone Planet::get_stone_wrap(int x, int y) {
    if (wrap(x, y)) {
        return get_stone(x, y);
    }

    return STONE_Invalid;
}

void Planet::set_stone_wrap(int x, int y, Stone stone) {
    if (wrap(x, y)) {
        set_stone(x, y, stone);
    }
}

bool Planet::wrap(int& x, int& y) {
    int size = get_size_blocks();
    while (x < 0) x += size;
    x = x % size;

    if (y < 0 || y >= size) {
        return false;
    }

    return true;
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

bool Planet::find_random_stone(StoneSet st_set, Stone& res, int& x, int& y) {
    int count = count_stones(st_set);
    if (!count)
        return false;
    int idx = (rand() % count);
    int sz = get_size_blocks();
    st_set.start_iter();
    // FIXME: It would be good to deduplicate this with the other find_random_stone()
    // (perhaps abstract a find_nth_stone())
    // FIXME: We'd ideally only iterate planet stones once (this could be the outer loop)
    for (Stone st = st_set.get_stone(); st != STONE_END; st = st_set.get_stone()) {
        for (int j = 0; j < sz; ++j) {
            for (int i = 0; i < sz; ++i) {
                if (get_stone(i, j) == st) {
                    if (idx == 0) {
                        res = st; x = i; y = j;
                        return true;
                    } else {
                        --idx;
                    }
                }
            }
        }
    }

    L.fatal("Could not find nth stone in set when we verified at least n exist");
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

bool Planet::has_next_to_4(Stone a, Stone b) {
    for (int j = 0; j < get_size_blocks(); ++j) {
        for (int i = 0; i < get_size_blocks(); ++i) {
            if (get_stone(i, j) == a) {
                if (next_to_4(i, j, b)) {
                    return true;
                }
            }
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

int Planet::count_stones(StoneSet st_set) {
    int count = 0;
    st_set.start_iter();
    for (Stone s = st_set.get_stone(); s != STONE_END; s = st_set.get_stone()) {
        count += count_stones(s);
    }
    return count;
}

/*
 * Take a number of bombers and set of stones which are targets
 * Return number of targets which can be hit
 * Set destroyed to the number of bombers which would be destroyed in doing so
 */
int Planet::plan_bomb(int bombers, StoneSet sset, int& destroyed, bool bomb_airdef) {
    destroyed = get_destroyed_bombers(bombers, bomb_airdef);

    int max_hits = bomb_airdef ? get_airdef_guns() : count_stones(sset);
    int hits = 0;

    for (int i = 0; i < bombers; ++i) {
        if (onein(6)) ++hits;
    }

    return min(hits, max_hits);
}

int Planet::get_destroyed_bombers(int bombers, bool bomb_airdef) {
    int destroyed = 0;
    for (int i = 0; i < get_airdef_guns(); ++i) {
        if (onein(bomb_airdef ? 3 : 4)) ++destroyed;
    }
    return min(destroyed, bombers);
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

bool Planet::food_prod_surplus() {
    return get_food_consumption() < get_food_production();
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

bool Planet::plu_prod_surplus() {
    return get_plu_consumption() < get_plu_production();
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
    // N.B. Artificial worlds under construction do NOT exist but ARE owned

    if (owner <0) return false;

    Player *ownerplayer = exostate().get_player(owner);
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
        return STONE_Invalid;
    return surf[y*PLANET_BLOCKS_LG + x];
}

void Planet::_set_stone(int x, int y, Stone stone) {
    surf[y*PLANET_BLOCKS_LG + x] = stone;
    // Ensure that changes to number of cities etc are
    // accompanied by an army funding cap as appropriate.
    validate_army_funding();
}

bool Planet::_to_real(int x, int y, int& real_x, int& real_y) {
    int sz = get_size_blocks();
    while (x < 0) x += sz;
    int offset = (PLANET_BLOCKS_LG - sz) / 2;
    real_x = (x % sz) + offset;
    real_y = y + offset;

    if (y < 0 || y >= sz) {
        return false;
    }

    return true;
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
    sprite_sets[Forest].bulletin_bg_trans              = IMG_PL1_TRANS;
    sprite_sets[Forest].spaceport                      = IMG_HARBOUR_FOREST;
    sprite_sets[Forest].spaceport2                     = IMG_HF1_PICTURE2;
    sprite_sets[Forest].marker                         = IMG_PLM1;

    sprite_sets[Desert].panel_icon                     = IMG_TS1_ST2;
    sprite_sets[Desert].landscape                      = IMG_LS_DESERT;
    sprite_sets[Desert].map_bg                         = IMG_PLANET_DESERT;
    sprite_sets[Desert].surf                           = IMG_SF2;
    sprite_sets[Desert].bulletin_bg                    = IMG_PL2_PLANET2;
    sprite_sets[Desert].bulletin_bg_trans              = IMG_PL2_TRANS;
    sprite_sets[Desert].spaceport                      = IMG_HARBOUR_DESERT;
    sprite_sets[Desert].spaceport2                     = IMG_HF2_PICTURE2;
    sprite_sets[Desert].marker                         = IMG_PLM2;

    sprite_sets[Volcano].panel_icon                    = IMG_TS1_ST3;
    sprite_sets[Volcano].landscape                     = IMG_LS_VOLCANO;
    sprite_sets[Volcano].map_bg                        = IMG_PLANET_VOLCANO;
    sprite_sets[Volcano].surf                          = IMG_SF3;
    sprite_sets[Volcano].bulletin_bg                   = IMG_PL3_PLANET3;
    sprite_sets[Volcano].bulletin_bg_trans             = IMG_PL3_TRANS;
    sprite_sets[Volcano].spaceport                     = IMG_HARBOUR_VOLCANO;
    sprite_sets[Volcano].spaceport2                    = IMG_HF3_PICTURE2;
    sprite_sets[Volcano].marker                        = IMG_PLM3;

    sprite_sets[Rock].panel_icon                       = IMG_TS1_ST4;
    sprite_sets[Rock].landscape                        = IMG_LS_ROCK;
    sprite_sets[Rock].map_bg                           = IMG_PLANET_ROCK;
    sprite_sets[Rock].surf                             = IMG_SF4;
    sprite_sets[Rock].bulletin_bg                      = IMG_PL4_PLANET4;
    sprite_sets[Rock].bulletin_bg_trans                = IMG_PL4_TRANS;
    sprite_sets[Rock].spaceport                        = IMG_HARBOUR_ROCK;
    sprite_sets[Rock].spaceport2                       = IMG_HF4_PICTURE2;
    sprite_sets[Rock].marker                           = IMG_PLM4;

    sprite_sets[Ice].panel_icon                        = IMG_TS1_ST5;
    sprite_sets[Ice].landscape                         = IMG_LS_ICE;
    sprite_sets[Ice].map_bg                            = IMG_PLANET_ICE;
    sprite_sets[Ice].surf                              = IMG_SF5;
    sprite_sets[Ice].bulletin_bg                       = IMG_PL5_PLANET5;
    sprite_sets[Ice].bulletin_bg_trans                 = IMG_PL5_TRANS;
    sprite_sets[Ice].spaceport                         = IMG_HARBOUR_ICE;
    sprite_sets[Ice].spaceport2                        = IMG_HF5_PICTURE2;
    sprite_sets[Ice].marker                            = IMG_PLM5;

    sprite_sets[Terra].panel_icon                      = IMG_TS1_ST6;
    sprite_sets[Terra].landscape                       = IMG_LS_TERRA;
    sprite_sets[Terra].map_bg                          = IMG_PLANET_TERRA;
    sprite_sets[Terra].surf                            = IMG_SF6;
    sprite_sets[Terra].bulletin_bg                     = IMG_PL6_PLANET6;
    sprite_sets[Terra].bulletin_bg_trans               = IMG_PL6_TRANS;
    sprite_sets[Terra].spaceport                       = IMG_HARBOUR_TERRA;
    sprite_sets[Terra].spaceport2                      = IMG_HF6_PICTURE2;
    sprite_sets[Terra].marker                          = IMG_PLM6;

    sprite_sets[Artificial].panel_icon                 = IMG_TS1_ST8;
    sprite_sets[Artificial].landscape                  = IMG_LS_ART;
    sprite_sets[Artificial].map_bg                     = IMG_PLANET_ART;
    sprite_sets[Artificial].surf                       = IMG_SF8;
    sprite_sets[Artificial].bulletin_bg                = IMG_PL8_PLANET8;
    sprite_sets[Artificial].bulletin_bg_trans          = IMG_PL8_TRANS;
    sprite_sets[Artificial].spaceport                  = IMG_HARBOUR_ART;
    sprite_sets[Artificial].spaceport2                 = IMG_HF8_PICTURE2;
    sprite_sets[Artificial].marker                     = IMG_PLM8;

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

    Player *owner = exostate().get_player(get_owner());

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

        /* Character perks */

        adjust_army(owner->perk_starts_with_inf,
                    owner->perk_starts_with_gli,
                    owner->perk_starts_with_art);

        ai_place_stone(owner->perk_starts_with_agri, STONE_Agri, STONE_Agri);
        ai_place_stone(owner->perk_starts_with_mine, STONE_Mine, STONE_NaturalAnim);
        ai_place_stone(owner->perk_starts_with_city, STONE_City, STONE_City);
        ai_place_stone(owner->perk_starts_with_plu, STONE_Plu, STONE_NaturalLarge);
        ai_place_stone(owner->perk_starts_with_park, STONE_Park, STONE_NaturalAnim);
        ai_place_stone(owner->perk_starts_with_trade_port, STONE_TradePort, STONE_City);
        ai_place_stone(owner->perk_starts_with_academy, STONE_Academy, STONE_City);

        if (owner->perk_starts_with_lunar_base) {
            build_lunar_base();
        }

        adjust_airdef_guns(owner->perk_starts_with_airdef);
        adjust_robots(owner->perk_starts_with_robots);
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

    if (FEATURE(EF_IMPROVED_PLANET_NAMING)) {
        const char* race_name = exostate().get_next_planet_name_for_race(owner->get_race());
        if (race_name) {
            set_name(race_name);
        }
    }

    set_unique_name_if_unnamed();
}

void Planet::reset_unrest() {
    unrest[0] = 0;
}

void Planet::adjust_unrest(int adjustment) {
    L.debug("%s: Unrest %d + %d", name, unrest[0], adjustment);

    int initial_unrest = unrest[0];

    unrest[0] += adjustment;
    if (unrest[0] < 0) unrest[0] = 0;
    // Can't see any ref to a maximum in the original
    // Riots happen when >8, rebel attacks at >9

    if (initial_unrest != unrest[0] && unrest[0] <= 3) {
        bool population_angry_recently = false;

        if (is_owned()) {
            Player *owner = exostate().get_player(get_owner());

            if (owner->is_human()) {
                for (int i = 0; i < N_UNREST; ++i) {
                    if (unrest[i] >= 8) {
                        population_angry_recently = true;
                        break;
                    }
                }
            }

            if (population_angry_recently) {
                achievement_manager.unlock(ACH_PopulationAppeased);
            }
        }
    }
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

void Planet::changeclimate() {
    // Artifical planet climates don't change
    if (cls == Artificial)
        return;

    PlanetClass new_cls = Forest;
    if (cls == Forest)  new_cls = Terra;
    if (cls == Desert)  new_cls = Rock;
    if (cls == Volcano) new_cls = Desert;
    if (cls == Rock)    new_cls = Terra;
    if (cls == Ice)     new_cls = Rock;
    if (cls == Terra)   new_cls = Forest;

    change_class(new_cls);
}

bool Planet::festival_happened_this_month() {
    return festival_this_month;
}

bool Planet::surfchange_happened_this_month() {
    return surfchange_this_month;
}

void Planet::change_class(PlanetClass new_cls) {
    if (cls == new_cls) {
        L.warn("Attempt to set class to its current one");
        return;
    }

    /*
     * Orig doesn't have a blanket ban on artificial worlds changing
     * class (nuking, star expansion etc can change it), but it does
     * in some cases. I think the intention is that these can never
     * change, and those corner cases in the original are oversights.
     */
    if (cls == Artificial) {
        L.warn("Attempt to change class of an artificial world");
        return;
    }

    L.info("CLIMATE CHANGE: %d -> %d", (int)cls, (int)new_cls);
    cls = new_cls;

    // Ensure that changes to income due to class are
    // accompanied by an army funding cap as appropriate.
    validate_army_funding();
}

void Planet::register_festival() {
    festival_this_month = true;
}

bool Planet::register_comm(int player_idx) {
    bool res = (comms_this_month & (1 << player_idx));
    comms_this_month = comms_this_month | (1 << player_idx);
    return res;
}

void Planet::register_failed_attack(int player_idx) {
    failed_attacks_this_month = (failed_attacks_this_month | (1 << player_idx));
}

uint32_t Planet::get_failed_attacks() {
    return failed_attacks_this_month;
}

void Planet::register_bombing(int player_idx) {
    bombings_this_month = (bombings_this_month | (1 << player_idx));
}

uint32_t Planet::get_bombings() {
    return bombings_this_month;
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
    festival_this_month = false;
    surfchange_this_month = false;
    processing_in_progress = false;
}

void Planet::owner_change_event_reset() {
    /*
     * Ideally this would be part of month_reset, but that
     * doesn't fire at the exact time we need it to, and
     * I'm reluctant to change the order at this point in
     * case it has unintended consequences.
     */
    comms_this_month = 0;
    owner_changes_this_month_head = 0;
    academy_source0[0] = '\0';
    academy_source1[0] = '\0';
    academy_source2[0] = '\0';
    academy_source3[0] = '\0';
    academy_source4[0] = '\0';
    academy_sources_this_month_head = 0;
    failed_attacks_this_month = 0;
    bombings_this_month = 0;
}

void Planet::randomise_trade_quality() {
    int r = RND(5);
    trade = TRADE_Fair;
    if (r == 1) trade = TRADE_Bad;
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

    Player *p = exostate().get_player(get_owner());
    adjust_unrest(4);
    p->give_mc(get_tax_amount(), MC_ExtraTaxes);
    taxes_collected = true;
}

int Planet::get_tax_amount() {
    return get_income() / 2;
}

void Planet::set_law(Law law, bool legal) {
    if (legal) {
        laws |= (1 << (int)law);
    } else {
        laws &= ~(1 << (int)law);
    }
}

bool Planet::has_law(Law law) {
    return (bool)(laws & (1 << (int)law));
}

void Planet::toggle_law(Law law) {
    set_law(law, !has_law(law));
}

bool Planet::laws_cause_unrest() {
    // This can probably be simpler and I really shouldn't be using the bit values...
    // ...but I've just copied PROCcal_plan here.
    uint16_t _laws = laws & 0xFFF8;
    bool ok = (_laws == 0xC8 || _laws == 0xD8 || _laws == 0x80 || _laws == 0x90
            || _laws == 0xA0 || _laws == 0xB0 || _laws == 0x20 || _laws == 0x30);
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

static void try_produce(int& prod,
                        int& plu,
                        int& army,
                        const int cap,
                        int& funds,
                        int cost,
                        ProductionReport& rpt) {
    if (prod > plu) {
        prod = plu;
        rpt.no_plu = true;
    }
    if (army + prod > cap) {
        prod = (cap - army);
        rpt.no_space = true;
    }
    while (prod*cost > funds) {
        prod--;
        rpt.no_money = true;
    }
    army += prod;
    plu -= prod;
    funds -= prod*cost;
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

    int prod_inf = max_inf;
    int prod_gli = max_gli;
    int prod_art = max_art;

    bool optimise = false;

    if (FEATURE(EF_OPTIMISE_ARMY_PRODUCTION_PRIORITIES)) {
        /*
         * It's kinder to the player to perform these in reverse order:
         * Artillery -> Gliders -> Infantry
         * This would help to ensure maximum funding utility, as the player isn't
         * receiving this money whatever the case.
         * E.g. when funding is 3 and we have 1 inf and 1 artillery, we can't afford
         * both, and we end up with just 1 inf with 2MC wasted. In the alternate case,
         * we have one artillery and 0 MC wasted.
         */

        optimise = exostate().get_player(get_owner())->is_human();
    }

    if (optimise) {
        try_produce(prod_art, reserves_plu, army_art, cap, funds, 3, rpt);
        try_produce(prod_gli, reserves_plu, army_gli, cap, funds, 2, rpt);
        try_produce(prod_inf, reserves_plu, army_inf, cap, funds, 1, rpt);
    } else {
        try_produce(prod_inf, reserves_plu, army_inf, cap, funds, 1, rpt);
        try_produce(prod_gli, reserves_plu, army_gli, cap, funds, 2, rpt);
        try_produce(prod_art, reserves_plu, army_art, cap, funds, 3, rpt);
    }

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
    return do_monthly_trade(STONE_Trade);
}

TradeReport Planet::monthly_trade_port() {
    if (!trade_port_operational()) {
        return TradeReport();
    }
    return do_monthly_trade(STONE_TradePort);
}

TradeReport Planet::do_monthly_trade(Stone st) {
    TradeReport rpt;
    int n = count_stones(st);

    if (n == 0) {
        L.error("Trade requested on planet with no trade centre");
    }

    if (!is_owned()) {
        L.fatal("Trade requested on unowned planet");
    }

    Player *o = exostate().get_player(get_owner());


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
        // punish%(1,0)=1
        o->commit_infraction(INF_TradePlu);

        if (o->is_human()) {
            achievement_manager.unlock(ACH_IllicitTrade);
        }
    }

    if (o->is_human()) {
        if (rpt.mc >= 20) {
            achievement_manager.unlock(ACH_TradeGood);
        }

        if (rpt.mc >= 50) {
            achievement_manager.unlock(ACH_TradeVGood);
        }

        if (rpt.mc >= 100) {
            achievement_manager.unlock(ACH_TradeExcellent);
        }

        if (rpt.pl == 0 && rpt.mc >= 50) {
            achievement_manager.unlock(ACH_TradeClean);
        }
    }

    o->add_trace(TRACE_MineralsSold, rpt.mi);
    o->add_trace(TRACE_FoodSold, rpt.fd);
    o->add_trace(TRACE_PlutoniumSold, rpt.pl);

    o->give_mc(rpt.mc, MC_TradingCentre);

    return rpt;
}

bool Planet::trade_port_operational() {
    if (!is_owned()) {
        return false;
    }

    int owner_idx = get_owner();
    Player *owner = exostate().get_player(owner_idx);

    // Building AI to exploit ports is tricky - cheat and allow them to operate
    if (!owner->is_human()) {
        return true;
    }

    Star *this_star = exostate().get_star_for_planet(this);

    for (StarIterator siter; !siter.complete(); ++siter) {
        Star *s = siter.get();

        // This star is always in range
        bool in_range = (s == this_star);

        // Irrelevant as we allow AI ports to operate - see above
        /*
        // AI planets are always in range of each other
        if (!in_range && (!owner->is_human())) {
            in_range = true;
        }
        */

        if (!in_range && owner->has_invention(INV_UltraRangeScanner)) {
            if (exostate().get_months_between(this_star, s) <= 1) {
                in_range = true;
            }
        }

        if (!in_range && owner->has_invention(INV_OrbitalMassThrust)) {
            in_range = true;
        }

        if (!in_range) {
            continue;
        }

        for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
            Planet *p = s->get_planet(i);
            if (p == this) continue;
            if (!(p && p->exists() && p->is_owned())) continue;
            int p_owner = p->get_owner();

            if (p_owner == owner_idx) {
                return true;
            }

            if (exostate().has_alliance(owner_idx, p_owner, ALLY_Trade)) {
                return true;
            }
        }
    }

    return false;
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

    if (minerals <= 0) {
        if (is_owned() && exostate().get_player(get_owner())->is_human()) {
            achievement_manager.unlock(ACH_PlanetMined);
        }
    }
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

void Planet::disown(PlanetOwnerChangedReason reason) {
    unset_owner(reason);
    army_inf = 0;
    army_gli = 0;
    army_art = 0;
    unrest[0] = 0;
    army_funding = 0;
    L.debug("%s: DISOWN: Unrest reset to 0", name);
}

bool Planet::expand_city() {
    return expand(STONE_City, true);
}

bool Planet::expand_city_possible() {
    return expand(STONE_City, false);
}

bool Planet::expand_village() {
    return expand(STONE_Village, true);
}

bool Planet::expand(Stone st, bool do_place) {
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
                                if (do_place) {
                                    set_stone_wrap(i + i_off, j + j_off, st);
                                    L.info("Expansion %d,%d", i, j);
                                }
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

bool Planet::do_academy_immigration(const char* source_name) {
    if (!expand_city()) {
        // Caller should verify expansion possible first
        L.error("Academy immigration called when city cannot expand");
        return false;
    }

    char* s = nullptr;
    if (academy_sources_this_month_head == 0) s = academy_source0;
    if (academy_sources_this_month_head == 1) s = academy_source1;
    if (academy_sources_this_month_head == 2) s = academy_source2;
    if (academy_sources_this_month_head == 3) s = academy_source3;
    if (academy_sources_this_month_head == 4) s = academy_source4;

    if (!s) {
        L.error("Cannot not support this many sources in one month");
        return false;
    }

    snprintf(s, PLANET_MAX_NAME+1, "%s", source_name);
    ++academy_sources_this_month_head;

    return true;
}

bool Planet::agri_collapse() {
    // Lakes and oases on Forest, Terra and Desert protect agri
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

    if (is_owned() && exostate().get_player(get_owner())->has_invention(INV_Acid)) {
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

const PlanetOwnerChangedEvent* Planet::get_human_lost_planet_event() const {
    for (int i = 0; i < owner_changes_this_month_head; ++i) {
        const PlanetOwnerChangedEvent& e = owner_changes_this_month[i];
        if (e.prev_owner >= 0) {
            Player *prev_owner = exostate().get_player(e.prev_owner);
            if (prev_owner && prev_owner->is_human()) {
                return &e;
            }
        }
    }

    return nullptr;
}

PlanetTrafficLight Planet::get_traffic_light(PlanetTrafficLightProperty prop) {
    PlanetTrafficLight sufficient_light = PTL_Green;

    switch (prop) {
        case PTLP_Food:
            if (food_prod_surplus() || get_food_consumption() == 0) {
                return PTL_Green;
            }
            if (food_prod_sufficient()) {
                return sufficient_light;
            }
            if (FEATURE(EF_SMARTER_STATUS_LIGHTS)) {
                // Insufficient production, but we have enough to last us for a little while
                // More cautious than plu because A) food starvation is more serious, B) food perishes and C) legal to trade away
                if (reserves_food > 5*(get_food_consumption() - get_food_production())) {
                    return PTL_Amber;
                }
            }
            return PTL_Red;
        case PTLP_Plu:
            if (plu_prod_surplus() || get_plu_consumption() == 0) {
                return PTL_Green;
            }
            if (plu_prod_sufficient()) {
                return sufficient_light;
            }
            if (FEATURE(EF_SMARTER_STATUS_LIGHTS)) {
                // Insufficient production, but we have enough to last us for a little while
                if (reserves_plu >= 3*(get_plu_consumption() - get_plu_production())) {
                    return PTL_Amber;
                }
            }
            return PTL_Red;
        case PTLP_Unrest:
            {
                int unrest = get_unrest();
                if (unrest >= 8) return PTL_Red;
                if (unrest >= 4) return PTL_Amber;
                return PTL_Green;
            }
        case PTLP_Army:
            return army_funding_sufficient() ? PTL_Green : PTL_Red;
    }

    L.error("No case for property %d", prop);
    return PTL_Green;
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

    Player *owner = exostate().get_player(get_owner());

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
                if (owner->attempt_spend(500, MC_ChangeGlobalClimate)) {
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
        const int tcentre = count_stones(STONE_Trade) + count_stones(STONE_TradePort);
        const int cities = count_stones(STONE_City);
        const int academies = count_stones(STONE_Academy);

        int other_inhabited_planets = 0;
        int cost_academy = stone_cost(STONE_Academy);

        if (owner->get_race() == RACE_Teri) {
            cost_academy = 50;
        }

        Star *s = exostate().get_star_for_planet(this);
        for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
            Planet *other = s->get_planet(i);
            if (other == this) {
                continue;
            }
            if (!(other && other->exists() && other->is_owned())) {
                continue;
            }

            other_inhabited_planets++;
        }

        int action = 0;

        const int m = exostate().get_orig_month();

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

        if (FEATURE(EF_ACADEMIES)) {
            if (   owner->get_mc() > cost_academy
                && (academies < (m/((owner->get_race() == RACE_Teri) ? 10 : 20)))
                && other_inhabited_planets > 0) {
                int chance = 0;
                if (owner->get_race() == RACE_Yokon) chance = 4;
                if (owner->get_race() == RACE_Teri) chance = 2;
                if (owner->get_race() == RACE_Gordoon) chance = 3;
                chance *= (academies+1);
                if (chance > 0 && onein(chance)) {
                    action = 100;
                }
            }
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

        Stone trade_stone = FEATURE(EF_TRADE_PORTS) ? STONE_TradePort : STONE_Trade;

        // Apply action (Orig: PROCeta*)
        switch (action) {
            case 1:
                {
                    // BUILD CITIES + AGRI
                    int agri_needed = 4;
                    if (cls == Desert || cls == Volcano) {
                        agri_needed = 7;
                    }
                    int cost = stone_cost(STONE_City) + agri_needed*stone_cost(STONE_Agri);
                    for (int i = 0; i < RND(5); ++i) {
                        /*
                         * Orig is a bit weird here - it hard-codes a cost of 45MC
                         * and charges it if the lord has 42MC, allowing -ve MC for
                         * CPU lords. This can happen elsewhere also - we implement
                         * the same behaviour...
                         */
                        if (owner->can_afford(cost-3) && free >= agri_needed+1) {
                            if (owner->attempt_spend_cpuforce(cost, MC_Building)) {
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
                    if (free > 1 && owner->attempt_spend(cost, MC_Building)) {
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
                    if (free >= 3 && owner->attempt_spend(cost, MC_Building)) {
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
                        if (free > 1 && owner->attempt_spend(cost, MC_Building)) {
                            free -= ai_place_stone(1, STONE_Mine, STONE_Mine);
                            free -= ai_place_stone(1, STONE_Plu, STONE_NaturalLarge);
                        }
                    }
                }
                break;
            case 5:
                // BUILD TRADE CENTRE
                if (free > 0 && owner->attempt_spend_cpuforce(stone_cost(trade_stone), MC_Building)) {
                    free -= ai_place_stone(1, trade_stone, STONE_City);
                }
                break;
            case 6:
                // BUILD LUNAR BASE
                if (!has_lunar_base() && owner->attempt_spend_cpuforce(COST_LUNAR_BASE, MC_BuildingLunarBase)) {
                    build_lunar_base();
                } else {
                    // PROCeta6 sets action=2 in this case, but it looks inconsequential
                }
                break;
            case 7:
                // HANDLE HUNGER BY REPLACING CITIES WITH AGRI
                if (!agri_sufficient() && free < 5) {
                    _ai_make_space();
                    // CHECK: PROCeta7 doesn't recalculate free after potentially
                    // creating space - should we retain that behaviour?
                    free = count_stones(STONE_Clear) + count_stones(STONE_NaturalSmall);
                }
                if (free > 0) {
                    if (!agri_sufficient() && (free + possfree) < 5) {
                        // Original always finds first cities
                        int n = 2;
                        int x, y;
                        while (n > 0 && find_random_stone(STONE_City, x, y)) {
                            int cost = stone_cost(STONE_Clear) + stone_cost(STONE_Agri);
                            if (owner->attempt_spend(cost, MC_Building)) {
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
                    if (owner->attempt_spend_cpuforce(r * stone_cost(STONE_Agri), MC_Building)) {
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
                        if (owner->attempt_spend(r * COST_AIRDEF, MC_FleetProd)) {
                            airdef_guns += r;
                        } else {
                            L.error("Could not afford MC that we checked we had");
                        }
                    }
                }
                break;
            case 10:
                // BUILD A BACKUP BASE
                if (owner->attempt_spend_cpuforce(stone_cost(STONE_Base), MC_Building)) {
                    free -= ai_place_stone(1, STONE_Base, STONE_Base);
                }
                break;
            case 11:
                // BUILD ROBOTS
                {
                    int r = RND(3);
                    // SUGGEST: Don't buy robots outside resource limits (get_robot_cap())
                    // These robots might be scrapped on discard_excess_resources()!
                    if (owner->attempt_spend_cpuforce(r * COST_ROBOT, MC_FleetProd)) {
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
                                    if (owner->attempt_spend_cpuforce(stone_cost(STONE_Clear), MC_Building)) {
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
                    int to_build = get_plu_consumption() - get_plu_production();
                    owner->cpu_write_off_debt();

                    if (FEATURE(EF_CPU_EXPLOITS_VOLCANO)) {
                        /*
                         * PROCeta13 doesn't account for the extra plutonium produced on
                         * volcanic planets, as get_plu_production() does, and just takes
                         * the number of plutonium reactors. Optional fix.
                         */

                        // Halve (rounding up) build count of volcano worlds
                        if (get_class() == Volcano) {
                            to_build = (to_build+1)/2;
                        }
                    }

                    int crd = owner->get_mc();
                    int cost = stone_cost(STONE_Plu);
                    if (to_build > (crd/cost)+3) {
                        to_build = (crd/cost)+1;
                    }
                    for (int i = 0; i < to_build; ++i) {
                        if (owner->attempt_spend_cpuforce(cost, MC_Building)) {
                            free -= ai_place_stone(1, STONE_Plu, STONE_NaturalLarge);
                        }
                    }
                }
                break;
            case 100:
                if (owner->attempt_spend(cost_academy, MC_Building)) {
                    free -= ai_place_stone(1, STONE_Academy, STONE_City);
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

bool Planet::is_in_danger() {
    if (!exists()) {
        return false;
    }

    Star *s = exostate().get_star_for_planet(this);

    if ((int)(s->get_size()) > (int)STAR_Huge) {
        if (this == s->get_planet(0)) return true;
        if (this == s->get_planet(1)) return true;
        if (this == s->get_planet(2)) return true;
    }

    return false;
}

const char* Planet::get_academy_source(int i) {
    if (i == 0) return academy_source0;
    if (i == 1) return academy_source1;
    if (i == 2) return academy_source2;
    if (i == 3) return academy_source3;
    if (i == 4) return academy_source4;
    L.error("Cannot return source %d", i);
    return "<NONE>";
}

int Planet::get_n_academy_sources() {
    return academy_sources_this_month_head;
}

void Planet::_ai_make_space() {
    if (!is_owned()) {
        L.fatal("Tried to make space on unowned planet");
        return;
    }

    Player *owner = exostate().get_player(get_owner());

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
                    if (owner->attempt_spend(stone_cost(STONE_Clear), MC_Building)) {
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

void Planet::save(cJSON* j) const {
    SAVE_BOOL(j, _exists);
    SAVE_ENUM(j, cls);
    SAVE_ENUM(j, moon_cls);
    SAVE_ENUM(j, moon_cls);
    SAVE_STR(j, name);
    SAVE_ARRAY_OF_ENUM(j, surf);
    SAVE_NUM(j, construction_phase);
    SAVE_NUM(j, star_target);
    SAVE_NUM(j, pspeed);
    SAVE_NUM(j, diameter);
    SAVE_NUM(j, minerals);
    SAVE_NUM(j, sim);
    SAVE_ENUM(j, trade);
    SAVE_NUM(j, traded);
    SAVE_BOOL(j, taxes_collected);
    SAVE_NUM(j, laws);
    SAVE_NUM(j, owner);
    SAVE_BOOL(j, lunar_base);
    SAVE_ARRAY_OF_NUM(j, unrest);
    SAVE_NUM(j, army_funding);
    SAVE_NUM(j, airdef_guns);
    SAVE_NUM(j, robots);
    SAVE_NUM(j, reserves_min);
    SAVE_NUM(j, reserves_food);
    SAVE_NUM(j, reserves_plu);
    SAVE_NUM(j, army_inf);
    SAVE_NUM(j, army_gli);
    SAVE_NUM(j, army_art);
    SAVE_NUM(j, most_recent_previous_owner);
    SAVE_ENUM(j, most_recent_previous_owner_change_reason);
    SAVE_ARRAY_OF_SAVEABLE(j, owner_changes_this_month);
    SAVE_NUM(j, owner_changes_this_month_head);
    SAVE_STR(j, academy_source0);
    SAVE_STR(j, academy_source1);
    SAVE_STR(j, academy_source2);
    SAVE_STR(j, academy_source3);
    SAVE_STR(j, academy_source4);
    SAVE_NUM(j, academy_sources_this_month_head);
    SAVE_BOOL(j, festival_this_month);
    SAVE_BOOL(j, surfchange_this_month);
    SAVE_NUM(j, comms_this_month);
    SAVE_NUM(j, failed_attacks_this_month);
    SAVE_NUM(j, bombings_this_month);
    SAVE_BOOL(j, processing_in_progress);
}

void Planet::load(cJSON* j) {
    LOAD_BOOL(j, _exists);
    LOAD_ENUM(j, cls);
    LOAD_ENUM(j, moon_cls);
    LOAD_ENUM(j, moon_cls);
    LOAD_STR(j, name);
    LOAD_ARRAY_OF_ENUM(j, surf);
    LOAD_NUM(j, construction_phase);
    LOAD_NUM(j, star_target);
    LOAD_NUM(j, pspeed);
    LOAD_NUM(j, diameter);
    LOAD_NUM(j, minerals);
    LOAD_NUM(j, sim);
    LOAD_ENUM(j, trade);
    LOAD_NUM(j, traded);
    LOAD_BOOL(j, taxes_collected);
    LOAD_NUM(j, laws);
    LOAD_NUM(j, owner);
    LOAD_BOOL(j, lunar_base);
    LOAD_ARRAY_OF_NUM(j, unrest);
    LOAD_NUM(j, army_funding);
    LOAD_NUM(j, airdef_guns);
    LOAD_NUM(j, robots);
    LOAD_NUM(j, reserves_min);
    LOAD_NUM(j, reserves_food);
    LOAD_NUM(j, reserves_plu);
    LOAD_NUM(j, army_inf);
    LOAD_NUM(j, army_gli);
    LOAD_NUM(j, army_art);
    LOAD_NUM(j, most_recent_previous_owner);
    LOAD_ENUM(j, most_recent_previous_owner_change_reason);
    LOAD_ARRAY_OF_SAVEABLE(j, owner_changes_this_month);
    LOAD_NUM(j, owner_changes_this_month_head);
    LOAD_STR(j, academy_source0);
    LOAD_STR(j, academy_source1);
    LOAD_STR(j, academy_source2);
    LOAD_STR(j, academy_source3);
    LOAD_STR(j, academy_source4);
    LOAD_NUM(j, academy_sources_this_month_head);
    LOAD_BOOL(j, festival_this_month);
    LOAD_BOOL(j, surfchange_this_month);
    LOAD_NUM(j, comms_this_month);
    LOAD_NUM(j, failed_attacks_this_month);
    LOAD_NUM(j, bombings_this_month);
    LOAD_BOOL(j, processing_in_progress);
}
