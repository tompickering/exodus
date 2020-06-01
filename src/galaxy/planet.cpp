#include "planet.h"

#include <cstring>

#include "state/exodus_state.h"

#include "planet_names.h"
#include "loadplans_data.h"

#include "assetpaths.h"
#include "shared.h"

extern ExodusState exostate;

PlanetSpriteSet sprite_sets[7];
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

Planet::Planet() {
    _exists = false;
    name[0] = '\0';
    traded = 0;
    laws = 0;
    surfchange_this_month = false;
    moon_cls = MOON_Dirt;
}

Planet::Planet(PlanetClass _cls) : cls(_cls) {
    init();
}

bool Planet::exists() {
    return _exists;
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

    _exists = true;
    name[0] = '\0';
    moon_cls = (MoonClass)(rand() % 3);
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

        // TODO: Surface init for Artificial worlds
    }

    // Shouldn't be necessary but makes sense
    month_reset();

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
    return get_income() - army_funding;
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

int Planet::get_owner() {
    return owner;
}

void Planet::set_owner(int new_owner) {
    L.info("Planet owner: %d -> %d", owner, new_owner);
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
    return army_funding;
}

void Planet::adjust_army_funding(int adjustment) {
    army_funding += adjustment;
    if (army_funding < 0)
        army_funding = 0;
    if (army_funding > get_income())
        army_funding = get_income();
}

void Planet::validate_army_funding() {
    if (army_funding > get_income()) {
        army_funding = get_income();
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
    return army_funding >= get_army_required_mc();
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

bool Planet::is_owned() {
    if (owner <0) return false;

    Player *ownerplayer = exostate.get_player(owner);
    return ownerplayer->is_active();
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
}

void Planet::_to_real(int x, int y, int& real_x, int& real_y) {
    int offset = (PLANET_BLOCKS_LG - get_size_blocks()) / 2;
    real_x = x + offset;
    real_y = y + offset;
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

    sprite_sets[Desert].panel_icon                     = IMG_TS1_ST2;
    sprite_sets[Desert].landscape                      = IMG_LS_DESERT;
    sprite_sets[Desert].map_bg                         = IMG_PLANET_DESERT;
    sprite_sets[Desert].surf                           = IMG_SF2;
    sprite_sets[Desert].bulletin_bg                    = IMG_PL2_PLANET2;

    sprite_sets[Volcano].panel_icon                    = IMG_TS1_ST3;
    sprite_sets[Volcano].landscape                     = IMG_LS_VOLCANO;
    sprite_sets[Volcano].map_bg                        = IMG_PLANET_VOLCANO;
    sprite_sets[Volcano].surf                          = IMG_SF3;
    sprite_sets[Volcano].bulletin_bg                   = IMG_PL3_PLANET3;

    sprite_sets[Rock].panel_icon                       = IMG_TS1_ST4;
    sprite_sets[Rock].landscape                        = IMG_LS_ROCK;
    sprite_sets[Rock].map_bg                           = IMG_PLANET_ROCK;
    sprite_sets[Rock].surf                             = IMG_SF4;
    sprite_sets[Rock].bulletin_bg                      = IMG_PL4_PLANET4;

    sprite_sets[Ice].panel_icon                        = IMG_TS1_ST5;
    sprite_sets[Ice].landscape                         = IMG_LS_ICE;
    sprite_sets[Ice].map_bg                            = IMG_PLANET_ICE;
    sprite_sets[Ice].surf                              = IMG_SF5;
    sprite_sets[Ice].bulletin_bg                       = IMG_PL5_PLANET5;

    sprite_sets[Terra].panel_icon                      = IMG_TS1_ST6;
    sprite_sets[Terra].landscape                       = IMG_LS_TERRA;
    sprite_sets[Terra].map_bg                          = IMG_PLANET_TERRA;
    sprite_sets[Terra].surf                            = IMG_SF6;
    sprite_sets[Terra].bulletin_bg                     = IMG_PL6_PLANET6;

    sprite_sets[Artificial].panel_icon                 = IMG_TS1_ST8;
    sprite_sets[Artificial].landscape                  = IMG_LS_ART;
    sprite_sets[Artificial].map_bg                     = IMG_PLANET_ART;
    sprite_sets[Artificial].surf                       = IMG_SF8;
    sprite_sets[Artificial].bulletin_bg                = IMG_PL8_PLANET8;
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

    int cap = get_army_cap();
    int funds = get_army_funding();

    int max_inf = count_stones(STONE_Inf);
    int max_gli = count_stones(STONE_Gli);
    int max_art = count_stones(STONE_Art);

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
    if (reserves_min > get_mineral_reserves_cap())
        reserves_min = get_mineral_reserves_cap();
    reserves_plu -= (units_to_mine - madd);
    minerals -= units_to_mine;
}

void Planet::perish_food() {
    reserves_food -= (reserves_food / 6);
}

int Planet::get_mineral_reserves_cap() {
    return (diameter / 200);
}

int Planet::get_army_cap() {
    return (diameter / 200);
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

void Planet::adjust_army(int inf, int gli, int art) {
    army_inf += inf;
    army_gli += gli;
    army_art += art;
    if (army_inf < 0) army_inf = 0;
    if (army_gli < 0) army_gli = 0;
    if (army_art < 0) army_art = 0;
    int max = get_army_cap();
    if (army_inf > max) army_inf = max;
    if (army_gli > max) army_gli = max;
    if (army_art > max) army_art = max;
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
    L.info("City expanding on %s", get_name());

    // Find expansion candidates
    int count = 0;
    int sz = get_size_blocks();
    for (int j = 0; j < sz; ++j) {
        for (int i = 0; i < sz; ++i) {
            if (get_stone(i, j) == STONE_City) {
                if (next_to_4(i, j, STONE_Clear) || next_to_4(i, j, STONE_NaturalSmall)) {
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
            if (get_stone(i, j) == STONE_City) {
                if (next_to_4(i, j, STONE_Clear) || next_to_4(i, j, STONE_NaturalSmall)) {
                    if (idx == 0) {
                        // Expand this city
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
                            if (tgt == STONE_Clear || tgt == STONE_NaturalSmall) {
                                set_stone_wrap(i + i_off, j + j_off, STONE_City);
                                L.info("City expansion %d,%d", i, j);
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

    L.fatal("Could not find suitable expanstion after verifying it must be possible");
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
