#include "player.h"

#include <cstdio>
#include <cstring>

// FIXME: Cyclic dependency - used for achievement checking - best solution probably to set hostility via exostate() in the first place
#include "state/exodus_state.h"

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
    invalid_placement_seen = false;
    ai_tactic = 0;

    infraction_mask = 0;
    ever_committed_infraction = false;

    clear_hostility();
    clear_ai_attack();

    set_tax(30);
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
    const AIFlag *flags = ai_hum;

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

    if (is_human() && member) {
        achievement_manager.unlock(ACH_GuildMember);
    }
}

GuildTitle Player::get_guild_title() {
    return guild_title;
}

const char* Player::get_guild_title_str() {
    const char* withspace = get_guild_title_str_with_space();
    if (withspace[0] != '\0') {
        return &withspace[1];
    }
    return "";
}

const char* Player::get_guild_title_str_with_space() {
    GuildTitle t = get_guild_title();
    if (t == GUILDTITLE_Warrior)   return " The Warrior";
    if (t == GUILDTITLE_Strong)    return " The Strong";
    if (t == GUILDTITLE_Conqueror) return " The Conqueror";
    if (t == GUILDTITLE_Mighty)    return " The Mighty";
    if (t == GUILDTITLE_Great)     return " The Great";
    if (t == GUILDTITLE_Warlord)   return " The Sovereign";
    if (t == GUILDTITLE_Emperor)   return " The Emperor";
    return "";
}

void Player::set_guild_title(GuildTitle newtitle) {
    if ((int)newtitle >= GUILDTITLE_MAX) {
        L.error("Can't set invalid title %d", (int)newtitle);
        newtitle = GUILDTITLE_Emperor;
    }
    guild_title = newtitle;

    switch (newtitle)
    {
        case GUILDTITLE_Warrior:
            achievement_manager.unlock(ACH_GuildWarrior);
            break;
        case GUILDTITLE_Strong:
            achievement_manager.unlock(ACH_GuildStrong);
            break;
        case GUILDTITLE_Conqueror:
            achievement_manager.unlock(ACH_GuildConqueror);
            break;
        case GUILDTITLE_Mighty:
            achievement_manager.unlock(ACH_GuildMighty);
            break;
        case GUILDTITLE_Great:
            achievement_manager.unlock(ACH_GuildGreat);
            break;
        case GUILDTITLE_Warlord:
            achievement_manager.unlock(ACH_GuildSovereign);
            break;
        case GUILDTITLE_Emperor:
            achievement_manager.unlock(ACH_GuildEmperor);
            break;
        default:
            break;
    }

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

bool Player::attempt_spend_allowing_writeoff(int cost, int lim) {
    if (cost <= mc) {
        return attempt_spend(cost);
    }

    // We're trying to overspend, but within the accepted limit
    // Zero MC and write off difference
    if (cost <= mc+lim) {
        mc = 0;
        return true;
    }

    return false;
}

bool Player::attempt_spend_cpuforce(int cost) {
    if (is_human()) {
        return attempt_spend(cost);
    }

    mc -= cost;

    if (mc < 0) {
        L.info("%s now has negative MC: %d", get_full_name(), mc);
    }

    return true;
}

int Player::remove_all_mc() {
    if (mc < 0) {
        return 0;
    }

    int mc_removed = mc;
    mc = 0;

    L.debug("[%s]: Funds zeroed (was %dMC)", get_full_name(), mc_removed);

    return mc_removed;
}

void Player::cpu_write_off_debt() {
    if (!is_human()) {
        L.error("Attempt to write off debt of human player, who should never be in debt");
        return;
    }

    if (mc < 0) {
        L.info("[%s] - Writing off debt (%d)", get_full_name(), mc);
        mc = 0;
    }
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

    if (is_human() && reputation >= 3) {
        achievement_manager.unlock(ACH_VeryGoodRep);
    }
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

    if (is_human() && has_all_inventions()) {
        achievement_manager.unlock(ACH_AllInventions);
    }

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

void Player::get_invention_description(Invention inv, const char*& d0, const char*& d1, const char*& d2) {
    const char* empty = nullptr;
    d0 = d1 = d2 = empty;

    switch (inv) {
        case INV_MassProduction:
            d0 = "This system enables the industry";
            d1 = "to produce gigantic masses of material";
            d2 = "in a very short time.";
            break;
        case INV_UltraRangeScanner:
            d0 = "This system makes it possible to scan";
            d1 = "an entire globe's atmosphere and surface";
            d2 = "in less than four seconds.";
            break;
        case INV_FusionEngine:
            d0 = "This engine needs much less power";
            d1 = "than any other fusion engines constructed";
            d2 = "in the past.";
            break;
        case INV_MicroMechanicElements:
            d0 = "These elements allow a complex machine";
            d1 = "to be stored in minimal space. This";
            d2 = "invention is most important.";
            break;
        case INV_UniversalVaccine:
            d0 = "This vaccine's molecular structure is";
            d1 = "very easy to manipulate and can be used";
            d2 = "both to inoculate and to damage.";
            break;
        case INV_OrbitalBombs:
            d0 = "These bombs can be dropped onto a planet";
            d1 = "from orbit which means that it is";
            d2 = "impossible to avoid an attack.";
            break;
        case INV_OrbitalMassConstruction:
            d0 = "This is an improvement to the Mass";
            d1 = "Production System and can even be used";
            d2 = "to construct entire planets.";
            break;
        case INV_OrbitalMassThrust:
            d0 = "This can be used for Spaceship High Speed";
            d1 = "Flights (SHSF) and for the moving of an";
            d2 = "artificial planet.";
            break;
        case INV_WeatherInfluence:
            d0 = "This invention makes it possible to control";
            d1 = "the planet's weather by thermal manipulation.";
            d2 = "This can result in a global geological change.";
            break;
        case INV_MultiFunctionalVaccine:
            d0 = "This vaccine can inoculate against previously";
            d1 = "unknown viral strains that could lead to";
            d2 = "city-wide epidemics and closures.";
            break;
        case INV_Acid:
            d0 = "This acid can prevent sudden and unknown";
            d1 = "plagues that lead to the destruction of";
            d2 = "important agricultural areas.";
            break;
        case INV_IndustryGuard:
            d0 = "These machines will be installed in every";
            d1 = "important industry area to detect and kill";
            d2 = "enemy (or rebel) terrorists.";
            break;
        case INV_DreamInfluence:
            d0 = "This DIS is a top-secret invention and is";
            d1 = "used to extract information from captured";
            d2 = "terrorists by manipulating their dreams.";
            break;
        case INV_RadarExtension:
            d0 = "This system can prevent damage caused by";
            d1 = "meteor fragments, and can also cloak the";
            d2 = "space fleet from alien radars.";
            break;
        default:
            L.error("No invention: %d", (int)inv);
            break;
    }
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
    if (off >= OFFICER_MAX) {
        L.fatal("Requested invalid officer %d (max is %d)", off, OFFICER_MAX-1);
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

int Player::get_total_officer_costs() {
    int total = 0;
    for (int i = 0; i < (int)OFFICER_MAX; ++i) {
        total += get_officer_cost((Officer)i);
    }
    return total;
}

void Player::set_officer(Officer off, OfficerQuality off_q) {
    if (off >= OFFICER_MAX) {
        L.fatal("Setting invalid officer %d (max is %d)", off, OFFICER_MAX-1);
    }
    if (off_q >= OFFQ_MAX) {
        L.fatal("Setting invalid officer quality %d (max is %d)", off, OFFQ_MAX-1);
    }
    L.debug("%s: NEW OFFICER %d: %d->%d", get_full_name(), off, officers[off], off_q);
    officers[off] = off_q;

    bool all_good = true;

    for (int i = 0; i < (int)OFFICER_MAX; ++i) {
        if (officers[i] != OFFQ_Good) {
            all_good = false;
            break;
        }
    }

    if (all_good) {
        achievement_manager.unlock(ACH_AllGoodOfficers);
    }
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

StarMarker* Player::get_marker(int i) {
    StarMarker* marker = &star_markers[i];
    return marker;
}

void Player::set_marker_tag(int i, const char* new_marker) {
    L.debug("Setting tag %d = %s", i, new_marker);
    star_markers[i].set_tag(new_marker);
}

void Player::commit_infraction(Infraction inf) {
    ever_committed_infraction = true;
    infraction_mask |= (1 << inf);
}

bool Player::committed_infraction(Infraction inf) {
    return (infraction_mask & (1 << inf)) > 0;
}

bool Player::committed_any_infractions() {
    return infraction_mask > 0;
}

bool Player::ever_committed_any_infractions() {
    return ever_committed_infraction;
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

    if (p >= 0) {
        if (exostate().count_hostile_players(p) >= 5) {
            achievement_manager.unlock(ACH_ManyEnemies);
        }
    }
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

void Player::set_trade_charge() {
    trade_charge = RND(3) * 10;
}

int Player::get_trade_charge() {
    return trade_charge;
}

int Player::get_trace(Trace t) {
    return trace[t];
}

void Player::add_trace(Trace t) {
    add_trace(t, 1);
}

void Player::add_trace(Trace t, int n) {
    trace[t] += n;
    L.debug("%s: TRACE %d -> %d", get_full_name(), (int)t, trace[t]);
}

void Player::save(cJSON* j) const
{
    SAVE_NUM(j, nopirates);
    SAVE_BOOL(j, invalid_placement_seen);
    SAVE_ENUM(j, race);
    SAVE_ENUM(j, gender);
    SAVE_STR(j, name);
    SAVE_STR(j, title);
    SAVE_STR(j, ref);
    SAVE_STR(j, full_name);
    SAVE_ENUM(j, guild_title);
    SAVE_BOOL(j, exists);
    SAVE_BOOL(j, dead);
    SAVE_BOOL(j, left_galaxy);
    SAVE_NUM(j, flag_idx);
    SAVE_NUM(j, mc);
    SAVE_BOOL(j, _intro_seen);
    SAVE_NUM(j, fleet_marker_idx);
    SAVE_SAVEABLE(j, location);
    SAVE_SAVEABLE(j, starship);
    SAVE_SAVEABLE(j, fleet);
    SAVE_NUM(j, inventions);
    SAVE_NUM(j, reputation);
    SAVE_BOOL(j, guild_member);
    SAVE_NUM(j, tax);
    SAVE_ARRAY_OF_ENUM(j, officers);
    SAVE_SAVEABLE(j, mission);
    SAVE_ARRAY_OF_SAVEABLE(j, star_markers);
    SAVE_NUM(j, infraction_mask);
    SAVE_BOOL(j, ever_committed_infraction);
    SAVE_ARRAY_OF_ENUM(j, ai_flags);
    SAVE_NUM(j, ai_tactic);
    SAVE_NUM(j, ai_hostile_to);
    SAVE_NUM(j, ai_attack_star);
    SAVE_NUM(j, ai_attack_planet);
    SAVE_NUM(j, trade_charge);
    SAVE_ARRAY_OF_NUM(j, trace);
}

void Player::load(cJSON* j)
{
    LOAD_NUM(j, nopirates);
    LOAD_BOOL(j, invalid_placement_seen);
    LOAD_ENUM(j, race);
    LOAD_ENUM(j, gender);
    LOAD_STR(j, name);
    LOAD_STR(j, title);
    LOAD_STR(j, ref);
    LOAD_STR(j, full_name);
    LOAD_ENUM(j, guild_title);
    LOAD_BOOL(j, exists);
    LOAD_BOOL(j, dead);
    LOAD_BOOL(j, left_galaxy);
    LOAD_NUM(j, flag_idx);
    LOAD_NUM(j, mc);
    LOAD_BOOL(j, _intro_seen);
    LOAD_NUM(j, fleet_marker_idx);
    LOAD_SAVEABLE(j, location);
    LOAD_SAVEABLE(j, starship);
    LOAD_SAVEABLE(j, fleet);
    LOAD_NUM(j, inventions);
    LOAD_NUM(j, reputation);
    LOAD_BOOL(j, guild_member);
    LOAD_NUM(j, tax);
    LOAD_ARRAY_OF_ENUM(j, officers);
    LOAD_SAVEABLE(j, mission);
    LOAD_ARRAY_OF_SAVEABLE(j, star_markers);
    LOAD_NUM(j, infraction_mask);
    LOAD_BOOL(j, ever_committed_infraction);
    LOAD_ARRAY_OF_ENUM(j, ai_flags);
    LOAD_NUM(j, ai_tactic);
    LOAD_NUM(j, ai_hostile_to);
    LOAD_NUM(j, ai_attack_star);
    LOAD_NUM(j, ai_attack_planet);
    LOAD_NUM(j, trade_charge);
    LOAD_ARRAY_OF_NUM(j, trace);
}
