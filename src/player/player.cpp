#include "player.h"

#include <cstdio>
#include <cstring>

#include "alien_names.h"
#include "shared.h"

static bool alien_names_offsets_initialised = false;
static int alien_names_offsets[N_RACES];

const AIFlag ai_hum[9] = {AI_Md, AI_Md, AI_Md, AI_Md, AI_Md, AI_Md, AI_Md, AI_Md, AI_Md};
const AIFlag ai_yok[9] = {AI_Md, AI_Hi, AI_Md, AI_Md, AI_Md, AI_Lo, AI_Hi, AI_Lo, AI_Md};
const AIFlag ai_ter[9] = {AI_Hi, AI_Hi, AI_Lo, AI_Lo, AI_Lo, AI_Hi, AI_Md, AI_Hi, AI_Lo};
const AIFlag ai_urk[9] = {AI_Lo, AI_Hi, AI_Hi, AI_Hi, AI_Hi, AI_Md, AI_Md, AI_Lo, AI_Hi};
const AIFlag ai_gor[9] = {AI_Hi, AI_Lo, AI_Md, AI_Hi, AI_Md, AI_Md, AI_Lo, AI_Md, AI_Hi};

const char* get_alien_name(Race race, int idx) {
    if (idx >= N_ALIEN_NAMES) {
        L.fatal("Request name with invalid index %d", idx);
    }

    if (race == RACE_Yokon)   return ALIEN_NAMES_YOKON[idx];
    if (race == RACE_Teri)    return ALIEN_NAMES_TERI[idx];
    if (race == RACE_Urkash)  return ALIEN_NAMES_URKASH[idx];
    if (race == RACE_Gordoon) return ALIEN_NAMES_GORDOON[idx];
    L.fatal("Request alien name for invalid race %d", (int)race);
    return "";
}

Player::Player() {
    guild_title = GUILDTITLE_None;
    exists = true;
    dead = false;
    left_galaxy = false;
    _intro_seen = false;
    reputation = 3;
    inventions = 0;
    guild_member = false;
    nopirates = 0;
    ai_tactic = 0;
    clear_hostility();
    clear_ai_attack();

    set_tax(30);

    ap_phase = AP_None;
}

void Player::init_alien_name() {
    if (!alien_names_offsets_initialised) {
        for (int i = 0; i < N_RACES; ++i) {
            alien_names_offsets[i] = rand() % N_ALIEN_NAMES;
        }
        alien_names_offsets_initialised = true;
    }

    int& alien_names_offset = alien_names_offsets[(int)race];

    int name_idx = (alien_names_offset++) % N_ALIEN_NAMES;
    set_name(get_alien_name(race, name_idx));
    int gender_boundary = 5;
    if (race == RACE_Yokon) gender_boundary = 6;
    if (race == RACE_Urkash) gender_boundary = 6;
    set_gender(name_idx <= gender_boundary ? GENDER_Female : GENDER_Male);
    if (get_gender() == GENDER_Female) {
        set_title("Lady");
        set_ref("Milady");
    } else {
        set_title("Lord");
        set_ref("Milord");
    }
}

void Player::init_race(Race _race) {
    race = _race;
    const AIFlag *flags;
    switch (race) {
        case RACE_Human:
            flags = ai_hum;
            break;
        case RACE_Yokon:
            flags = ai_yok;
            break;
        case RACE_Teri:
            flags = ai_ter;
            break;
        case RACE_Urkash:
            flags = ai_urk;
            break;
        case RACE_Gordoon:
            flags = ai_gor;
            break;
        default:
            L.fatal("Unknown race when setting AI flags");
            break;
    }

    for (int i = 0; i < 9; ++i) {
        ai_flags[i] = flags[i];
    }
}

const char* Player::get_name() {
    return name;
}

const char* Player::get_title() {
    return title;
}

const char* Player::get_ref() {
    return ref;
}

const char* Player::get_full_name() {
    return full_name;
}

Gender Player::get_gender() {
    return gender;
}

void Player::set_name(const char *new_name) {
    strncpy(name, new_name, MAX_PLAYER_NAME);
    refresh_full_name();
}

void Player::set_title(const char *new_title) {
    strncpy(title, new_title, MAX_PLAYER_TITLE);
    refresh_full_name();
}

void Player::set_ref(const char *new_ref) {
    strncpy(ref, new_ref, MAX_PLAYER_REFERENCE);
}

void Player::set_gender(Gender new_gender) {
    gender = new_gender;
}

void Player::refresh_full_name() {
    snprintf(
        full_name,
        MAX_PLAYER_TITLE + 1 + MAX_PLAYER_NAME + 1,
        "%s %s",
        title,
        name);
}

Race Player::get_race() {
    return race;
}

const char* Player::get_race_str() {
    switch (get_race()) {
        case RACE_Human:
            return "Human";
        case RACE_Yokon:
            return "Yo' kon";
        case RACE_Teri:
            return "Teri";
        case RACE_Urkash:
            return "Urkash";
        case RACE_Gordoon:
            return "Gordoon";
    }
    return "<NO RACE>";
}

bool Player::is_human() {
    return race == RACE_Human;
}

bool Player::is_alive() {
    return !dead;
}

bool Player::is_participating() {
    // Indicate whether this player should be 'taking part' in the game
    return exists && !left_galaxy && is_alive();
}

bool Player::leave_galaxy() {
    if (is_participating()) {
        left_galaxy = true;
        return true;
    }
    return false;
}

bool Player::return_to_galaxy() {
    if (left_galaxy) {
        left_galaxy = false;
        bool returned = is_participating();
        if (returned) {
            mc = 0;
            fleet.freight.clear();
        }
        return returned;
    }
    return false;
}

bool Player::is_guild_member() {
    return guild_member;
}

void Player::set_guild_member(bool member) {
    guild_member = member;
}

GuildTitle Player::get_guild_title() {
    return guild_title;
}

const char* Player::get_guild_title_str() {
    GuildTitle t = get_guild_title();
    if (t == GUILDTITLE_Warrior)   return "The Warrior";
    if (t == GUILDTITLE_Strong)    return "The Strong";
    if (t == GUILDTITLE_Conqueror) return "The Conqueror";
    if (t == GUILDTITLE_Mighty)    return "The Mighty";
    if (t == GUILDTITLE_Great)     return "The Great";
    if (t == GUILDTITLE_Warlord)   return "The Warlord";
    if (t == GUILDTITLE_Emperor)   return "The Emperor";
    return "";
}

void Player::set_guild_title(GuildTitle newtitle) {
    if ((int)newtitle >= GUILDTITLE_MAX) {
        L.error("Can't set invalid title %d", (int)newtitle);
        newtitle = GUILDTITLE_Emperor;
    }
    guild_title = newtitle;
    L.info("%s: SET TITLE %d", get_full_name(), (int)guild_title);
}

int Player::get_flag_idx() {
    return flag_idx;
}

void Player::set_flag_idx(int new_idx) {
    flag_idx = new_idx;
}

int Player::get_mc() {
    return mc;
}

bool Player::intro_seen() {
    return _intro_seen;
}

void Player::set_intro_seen() {
    _intro_seen = true;
}

void Player::give_mc(int extra_mc) {
    if (extra_mc < 0) {
        L.fatal("Attempt to grant invalid MC amount %d", extra_mc);
    }

    mc += extra_mc;
}

bool Player::can_afford(int cost) {
    if (cost < 0) {
        L.warn("Calling can_afford() with -ve value %d", cost);
    }
    return mc >= (int)cost;
}

bool Player::attempt_spend(int cost) {
    return attempt_spend_with_remaining(cost, 0);
}

bool Player::attempt_spend_with_remaining(int cost, int remaining) {
    if (cost < 0) {
        L.warn("Calling attempt_spend_with_remaining() with -ve value %d", cost);
    }
    if (remaining < 0) {
        L.warn("Calling attempt_spend_with_remaining() with -ve remaining value %d", cost);
    }

    if (can_afford(cost + remaining)) {
        mc -= cost;
        return true;
    }

    return false;
}

int Player::get_fleet_marker_idx() {
    return fleet_marker_idx;
}

PlayerLocation& Player::get_location() {
    return location;
}

Starship& Player::get_starship() {
    return starship;
}

int Player::get_reputation() {
    return reputation;
}

void Player::adjust_reputation(int adjustment) {
    reputation += adjustment;
    L.debug("%s: +%d REP (%d)", get_full_name(), adjustment, reputation);
}

bool Player::can_research(Invention inv) {
    if (has_invention(inv))
        return false;

    // Invention prerequisites
    uint32_t pre = get_invention_prerequisites(inv);
    for (int i = 0; i < INV_MAX; ++i) {
        if (pre & (1<<i)) {
            if (!has_invention((Invention)i)) {
                return false;
            }
        }
    }

    return true;
}

bool Player::research_invention(Invention inv) {
    if (!can_research(inv)) {
        return false;
    }

    inventions |= (1 << (int)inv);

    return true;
}

bool Player::has_invention(Invention inv) {
    return (bool)(inventions & (1 << (int)inv));
}

bool Player::has_all_inventions() {
    for (int i = 0; i < (int)INV_MAX; ++i) {
        if (!has_invention((Invention)i)) {
            return false;
        }
    }
    return true;
}

Invention Player::get_random_researchable_invention() {
    int count = 0;
    for (int i = 0; i < (int)INV_MAX; ++i) {
        if (can_research((Invention)i)) {
            ++count;
        }
    }
    if (count == 0) {
        return INV_MAX;
    }
    int idx = rand() % count;
    for (int i = 0; i < (int)INV_MAX; ++i) {
        if (can_research((Invention)i)) {
            if (idx == 0) {
                return (Invention)i;
            }
            --idx;
        }
    }
    L.fatal("Could not find researchable invention which we checked exists");
    return INV_MAX;
}

InventionType Player::get_invention_type(Invention inv) {
    if (inv == INV_MassProduction)          return IT_Physical;
    if (inv == INV_UltraRangeScanner)       return IT_Electronical;
    if (inv == INV_FusionEngine)            return IT_Physical;
    if (inv == INV_MicroMechanicElements)   return IT_Mechanical;
    if (inv == INV_UniversalVaccine)        return IT_Medical;
    if (inv == INV_OrbitalBombs)            return IT_Weapon;
    if (inv == INV_OrbitalMassConstruction) return IT_Physical;
    if (inv == INV_OrbitalMassThrust)       return IT_Physical;
    if (inv == INV_WeatherInfluence)        return IT_Electronical;
    if (inv == INV_MultiFunctionalVaccine)  return IT_Medical;
    if (inv == INV_Acid)                    return IT_Electronical;
    if (inv == INV_IndustryGuard)           return IT_Electronical;
    if (inv == INV_DreamInfluence)          return IT_Medical;
    if (inv == INV_RadarExtension)          return IT_Mechanical;

    L.fatal("Requested type for invalid invention %d", inv);
    return IT_Mechanical;
}

const char* Player::get_invention_str(Invention inv) {
    if (inv == INV_MassProduction)          return "Mass Production System";
    if (inv == INV_UltraRangeScanner)       return "Ultra Range Scanner";
    if (inv == INV_FusionEngine)            return "Fusion Engine";
    if (inv == INV_MicroMechanicElements)   return "Micro Mechanic Elements";
    if (inv == INV_UniversalVaccine)        return "Universal Vaccine";
    if (inv == INV_OrbitalBombs)            return "Orbital Bombs";
    if (inv == INV_OrbitalMassConstruction) return "Orbital Mass Construction";
    if (inv == INV_OrbitalMassThrust)       return "Orbital Mass Thrust";
    if (inv == INV_WeatherInfluence)        return "Weather Influence";
    if (inv == INV_MultiFunctionalVaccine)  return "Multi-Functional Vaccine";
    if (inv == INV_Acid)                    return "Highly Sensitive Acid";
    if (inv == INV_IndustryGuard)           return "Industry Guard Computer";
    if (inv == INV_DreamInfluence)          return "Dream Influencing System";
    if (inv == INV_RadarExtension)          return "Radar Extension & Manipul.";
    L.fatal("Requested name for invalid invention %d", (int)inv);
    return "<NO INVENTION>";
}

const char* Player::get_invention_type_str(Invention inv) {
    InventionType t = get_invention_type(inv);
    if (t == IT_Physical)     return "Physical";
    if (t == IT_Mechanical)   return "Mechanical";
    if (t == IT_Electronical) return "Electronic";
    if (t == IT_Medical)      return "Medical";
    if (t == IT_Weapon)       return "Weapon";
    L.fatal("Requested name for invalid type %d of invention %d", (int)t, (int)inv);
    return "<NO INVENTION TYPE>";
}

uint32_t Player::get_invention_prerequisites(Invention inv) {
    uint32_t pre = 0;

    switch (inv) {
        case INV_OrbitalMassConstruction:
            pre |= (1 << (int)INV_MassProduction);
            pre |= (1 << (int)INV_FusionEngine);
            pre |= (1 << (int)INV_MicroMechanicElements);
            break;
        case INV_OrbitalMassThrust:
            pre |= (1 << (int)INV_MassProduction);
            pre |= (1 << (int)INV_FusionEngine);
            pre |= (1 << (int)INV_MicroMechanicElements);
            break;
        case INV_WeatherInfluence:
            pre |= (1 << (int)INV_UltraRangeScanner);
            break;
        case INV_MultiFunctionalVaccine:
            pre |= (1 << (int)INV_UniversalVaccine);
            break;
        case INV_Acid:
            pre |= (1 << (int)INV_UltraRangeScanner);
            break;
        case INV_IndustryGuard:
            pre |= (1 << (int)INV_MassProduction);
            pre |= (1 << (int)INV_MicroMechanicElements);
            break;
        case INV_DreamInfluence:
            // Oh god...
            // What a game to be remaking in 2021...
            // PLEASE BELIEVE I AM NOT AN ANTI-VAXXER!!!
            pre |= (1 << (int)INV_UniversalVaccine);
            break;
        case INV_RadarExtension:
            pre |= (1 << (int)INV_FusionEngine);
            pre |= (1 << (int)INV_MicroMechanicElements);
            break;
        default:
            break;
    }

    return pre;
}

const Fleet& Player::get_fleet() {
    return fleet;
}

Fleet& Player::get_fleet_nonconst() {
    return fleet;
}

void Player::init_tax() {
    tax = 30;
    if (race != RACE_Human) {
        if (get_flag(4) == AI_Hi) {
            adjust_tax(-10);
        }
        if (get_flag(8) == AI_Hi) {
            adjust_tax(10);
        }
        if (get_flag(8) == AI_Lo) {
            adjust_tax(-10);
        }
    }
}

int Player::get_tax() {
    return tax;
}

void Player::adjust_tax(int adj) {
    tax += adj;
    if (tax < 0) tax = 0;
    if (tax > 100) tax = 100;
}

void Player::set_tax(int t) {
    tax = t;
    if (tax < 0) tax = 0;
    if (tax > 100) tax = 100;
}

OfficerQuality Player::get_officer(Officer off) {
    if (off >= OFF_MAX) {
        L.fatal("Requested invalid officer %d (max is %d)", off, OFF_MAX-1);
    }
    return officers[off];
}

int Player::get_officer_cost(Officer off) {
    return get_officer_cost(get_officer(off));
}

int Player::get_officer_cost(OfficerQuality q) {
    if (q == OFFQ_Average) return OFF_COST_AVG;
    if (q == OFFQ_Good) return OFF_COST_GOOD;
    return OFF_COST_POOR;
}

int Player::get_officer_initial_cost(OfficerQuality q) {
    if (q == OFFQ_Average) return OFF_INITCOST_AVG;
    if (q == OFFQ_Good) return OFF_INITCOST_GOOD;
    return OFF_INITCOST_POOR;
}

int Player::get_total_officer_costs(Officer off) {
    int total = 0;
    for (int i = 0; i < (int)OFF_MAX; ++i) {
        total += get_officer_cost((Officer)i);
    }
    return total;
}

void Player::set_officer(Officer off, OfficerQuality off_q) {
    if (off >= OFF_MAX) {
        L.fatal("Setting invalid officer %d (max is %d)", off, OFF_MAX-1);
    }
    if (off_q >= OFFQ_MAX) {
        L.fatal("Setting invalid officer quality %d (max is %d)", off, OFFQ_MAX-1);
    }
    L.debug("%s: NEW OFFICER %d: %d->%d", get_full_name(), off, officers[off], off_q);
    officers[off] = off_q;
}

int Player::get_freight_capacity() {
    return fleet.transporters - fleet.freight.size();
}

void Player::cap_freight_randomly() {
    // FIXME: There are much more efficient ways to do this...!
    while (fleet.freight.size() > fleet.transporters) {
        int *c = nullptr;
        switch (rand() % 7) {
            case 0:
                c = &fleet.freight.minerals;
                break;
            case 1:
                c = &fleet.freight.food;
                break;
            case 2:
                c = &fleet.freight.plutonium;
                break;
            case 3:
                c = &fleet.freight.robots;
                break;
            case 4:
                c = &fleet.freight.infantry;
                break;
            case 5:
                c = &fleet.freight.gliders;
                break;
            case 6:
                c = &fleet.freight.artillery;
                break;
        }

        if (c && (*c) > 0) {
            (*c)--;
        }
    }
}

int Player::transfer_min(int n) {
    return transfer(n, &fleet.freight.minerals);
}
int Player::transfer_fd(int n) {
    return transfer(n, &fleet.freight.food);
}
int Player::transfer_plu(int n) {
    return transfer(n, &fleet.freight.plutonium);
}
int Player::transfer_robots(int n) {
    return transfer(n, &fleet.freight.robots);
}
int Player::transfer_inf(int n) {
    return transfer(n, &fleet.freight.infantry);
}
int Player::transfer_gli(int n) {
    return transfer(n, &fleet.freight.gliders);
}
int Player::transfer_art(int n) {
    return transfer(n, &fleet.freight.artillery);
}

int Player::transfer(int n, int* val) {
    int capacity = get_freight_capacity();
    int actual = 0;
    if (n > 0) {
        if (n <= capacity) {
            actual = n;
        } else {
            actual = capacity;
        }
    } else if (n < 0) {
        if ((-n) <= (*val)) {
            actual = n;
        } else {
            actual = -(*val);
        }
    }
    (*val) += actual;
    return -actual;
}

bool Player::has_mission() {
   return mission.type != MT_None;
}

void Player::set_mission_type(MissionType type) {
    mission.type = type;
}

int& Player::get_mission_planet_ref() {
    return mission.planet_idx;
}

int& Player::get_mission_star_ref() {
    return mission.star_idx;
}

const Mission& Player::get_mission() {
    return mission;
}

void Player::clear_mission() {
    set_mission_type(MT_None);
}

void Player::commit_infraction(Infraction inf) {
    infraction_mask |= (1 << inf);
}

bool Player::committed_infraction(Infraction inf) {
    return (infraction_mask & (1 << inf)) > 0;
}

void Player::clear_infractions() {
    infraction_mask = 0;
}

AIFlag Player::get_flag(int idx) {
    if (idx >= 9) {
        L.fatal("Tried to access AI flag > maximum (%d)", idx);
    }
    return ai_flags[idx];
}

int Player::get_tactic() {
    return ai_tactic;
}

void Player::set_tactic(int tactic) {
    L.debug("[%s] TACTIC %d -> %d", get_full_name(), ai_tactic, tactic);
    if (ai_tactic < 0) {
        L.warn("[%s] AI tactic set to %d", get_full_name(), ai_tactic);
    }
    ai_tactic = tactic;
}

void Player::next_tactic() {
    set_tactic(ai_tactic+1);
}

void Player::prev_tactic() {
    set_tactic(ai_tactic-1);
}

bool Player::is_hostile_to(int p) {
    return p == ai_hostile_to;
}

int Player::get_hostile_to() {
    return ai_hostile_to;
}

void Player::set_hostile_to(int p) {
    L.debug("%s: HOSTILE TO %d", get_full_name(), p);
    ai_hostile_to = p;
}

void Player::clear_hostility() {
    set_hostile_to(-1);
}

void Player::clear_ai_attack() {
    ai_attack_star = -1;
    ai_attack_planet = -1;
}

void Player::get_ai_attack(int& s, int& p) {
    s = ai_attack_star;
    p = ai_attack_planet;
}

void Player::set_ai_attack(int s, int p) {
    ai_attack_star = s;
    ai_attack_planet = p;
}
