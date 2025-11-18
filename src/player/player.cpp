#include "player.h"

#include <cstdio>
#include <cstring>

// FIXME: Cyclic dependency - used for achievement checking - best solution probably to set hostility via exostate() in the first place
#include "state/exodus_state.h"

#include "alien_names.h"
#include "officer_names.h"
#include "shared.h"



static bool alien_names_offsets_initialised = false;
static int alien_names_offsets[N_RACES];

const AIFlag ai_hum[9] = {AI_Md, AI_Md, AI_Md, AI_Md, AI_Md, AI_Md, AI_Md, AI_Md, AI_Md};
const AIFlag ai_yok[9] = {AI_Md, AI_Lo, AI_Hi, AI_Lo, AI_Md, AI_Md, AI_Md, AI_Hi, AI_Md};
const AIFlag ai_ter[9] = {AI_Lo, AI_Hi, AI_Md, AI_Hi, AI_Lo, AI_Lo, AI_Lo, AI_Hi, AI_Hi};
const AIFlag ai_urk[9] = {AI_Hi, AI_Lo, AI_Md, AI_Md, AI_Hi, AI_Hi, AI_Hi, AI_Hi, AI_Lo};
const AIFlag ai_gor[9] = {AI_Hi, AI_Md, AI_Lo, AI_Md, AI_Md, AI_Hi, AI_Md, AI_Lo, AI_Hi};

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
    _intro_comm_seen = false;
    reputation = 3;
    inventions = 0;
    guild_member = false;
    nopirates = 0;
    invalid_placement_seen = false;
    advanced_report_unlocked = false;
    advanced_galmap_unlocked = false;
    ai_tactic = 0;

    infraction_mask = 0;
    ever_committed_infraction = false;

    clear_hostility();
    clear_ai_attack();

    set_tax(30);
}

void Player::init_alien_name() {
    if (FEATURE(EF_CHARACTERS)) {
        // N.B. This can fail if an 'original game' character is set
        // This can happen if a lord dies - a 'new character' is replaced
        // with one of the 'original characters'.
        if (init_alien_name_character()) {
            return;
        }
    }

    if (!alien_names_offsets_initialised) {
        for (int i = 0; i < N_RACES; ++i) {
            alien_names_offsets[i] = rand() % N_ALIEN_NAMES;
        }
        alien_names_offsets_initialised = true;
    }

    int& alien_names_offset = alien_names_offsets[(int)get_race()];

    int name_idx = (alien_names_offset++) % N_ALIEN_NAMES;
    set_name(get_alien_name(get_race(), name_idx));
    int gender_boundary = 5;
    if (get_race() == RACE_Yokon) gender_boundary = 6;
    if (get_race() == RACE_Urkash) gender_boundary = 6;
    set_gender(name_idx <= gender_boundary ? GENDER_Female : GENDER_Male);
    if (get_gender() == GENDER_Female) {
        set_title("Lady");
        set_ref("Milady");
    } else {
        set_title("Lord");
        set_ref("Milord");
    }
}

bool Player::init_alien_name_character() {
    // This is never visible for AI players
    set_ref("");

    switch (character) {
        case CHAR_Yok1:
            set_title("Regent");
            set_name("Y'Ry");
            return true;
        case CHAR_Yok2:
            set_title("Regent");
            set_name("Tk'Terk");
            return true;
        case CHAR_Yok3:
            set_title("Regent");
            set_name("Gar'Xa");
            return true;
        case CHAR_Yok4:
            set_title("Regent");
            set_name("Z'Ren");
            return true;
        case CHAR_Ter1:
            set_title("Lady");
            set_name("Ye Nara");
            return true;
        case CHAR_Ter2:
            set_title("Lord");
            set_name("Ya Ni Noi");
            return true;
        case CHAR_Ter3:
            set_title("Lady");
            set_name("Ye Yeno");
            return true;
        case CHAR_Ter4:
            set_title("Lord");
            set_name("Ya Bakka");
            return true;
        case CHAR_Urk1:
            set_title("Lady");
            set_name("Kror");
            return true;
        case CHAR_Urk2:
            set_title("Lord");
            set_name("Zeish");
            return true;
        case CHAR_Urk3:
            set_title("Lady");
            set_name("Mzaa");
            return true;
        case CHAR_Urk4:
            set_title("Lord");
            set_name("Gecc");
            return true;
        case CHAR_Gor1:
            set_title("Lady");
            set_name("Bautre");
            return true;
        case CHAR_Gor2:
            set_title("Lord");
            set_name("Noubas");
            return true;
        case CHAR_Gor3:
            set_title("Elder");
            set_name("Toya");
            return true;
        case CHAR_Gor4:
            set_title("Lord");
            set_name("Troche");
            return true;
        default:
            break;
    }

    return false;
}

void Player::init_character(Character _character) {
    character = _character;

    const AIFlag *flags = ai_hum;

    switch (get_race()) {
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

void Player::set_character_hostility() {
    if (is_hostile_to(0)) {
        clear_hostility();
    }

    if (character == CHAR_Ter4 || character == CHAR_Urk1) {
        set_hostile_to(0);
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

Character Player::get_character() {
    return character;
}

Race Player::get_race() {
    if (character == CHAR_None) {
        L.fatal("Character not set when getting race");
    }

    switch (character) {
        case CHAR_Yok0:
        case CHAR_Yok1:
        case CHAR_Yok2:
        case CHAR_Yok3:
        case CHAR_Yok4:
            return RACE_Yokon;
        case CHAR_Ter0:
        case CHAR_Ter1:
        case CHAR_Ter2:
        case CHAR_Ter3:
        case CHAR_Ter4:
            return RACE_Teri;
        case CHAR_Urk0:
        case CHAR_Urk1:
        case CHAR_Urk2:
        case CHAR_Urk3:
        case CHAR_Urk4:
            return RACE_Urkash;
        case CHAR_Gor0:
        case CHAR_Gor1:
        case CHAR_Gor2:
        case CHAR_Gor3:
        case CHAR_Gor4:
            return RACE_Gordoon;
        default:
            break;
    }

    return RACE_Human;
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
    return get_race() == RACE_Human;
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
            set_mc(0, MC_ReturnToGalaxyReset);
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

const char* Player::get_icon() {
    switch (get_character()) {
        case CHAR_Yok1:
            return IMG_LTHUMB_YOK1;
        case CHAR_Yok2:
            return IMG_LTHUMB_YOK2;
        case CHAR_Yok3:
            return IMG_LTHUMB_YOK3;
        case CHAR_Yok4:
            return IMG_LTHUMB_YOK4;
        case CHAR_Ter1:
            return IMG_LTHUMB_TER1;
        case CHAR_Ter2:
            return IMG_LTHUMB_TER2;
        case CHAR_Ter3:
            return IMG_LTHUMB_TER3;
        case CHAR_Ter4:
            return IMG_LTHUMB_TER4;
        case CHAR_Urk1:
            return IMG_LTHUMB_URK1;
        case CHAR_Urk2:
            return IMG_LTHUMB_URK2;
        case CHAR_Urk3:
            return IMG_LTHUMB_URK3;
        case CHAR_Urk4:
            return IMG_LTHUMB_URK4;
        case CHAR_Gor1:
            return IMG_LTHUMB_GOR1;
        case CHAR_Gor2:
            return IMG_LTHUMB_GOR2;
        case CHAR_Gor3:
            return IMG_LTHUMB_GOR3;
        case CHAR_Gor4:
            return IMG_LTHUMB_GOR4;
        default:
            break;
    }

    return IMG_LTHUMB_UNKNOWN;
}

bool Player::intro_seen() {
    return _intro_seen;
}

void Player::set_intro_seen() {
    _intro_seen = true;
}

bool Player::intro_comm_seen() {
    return _intro_comm_seen;
}

void Player::set_intro_comm_seen() {
    _intro_comm_seen = true;
}

void Player::give_mc(int extra_mc, MCReason reason) {
    if (extra_mc < 0) {
        L.fatal("Attempt to grant invalid MC amount %d", extra_mc);
    }

    adjust_mc(extra_mc, reason);
}

bool Player::can_afford(int cost) {
    if (cost < 0) {
        L.warn("Calling can_afford() with -ve value %d", cost);
    }
    return get_mc() >= (int)cost;
}

bool Player::attempt_spend(int cost, MCReason reason) {
    return attempt_spend_with_remaining(cost, 0, reason);
}

bool Player::attempt_spend_with_remaining(int cost, int remaining, MCReason reason) {
    if (cost < 0) {
        L.warn("Calling attempt_spend_with_remaining() with -ve value %d", cost);
    }
    if (remaining < 0) {
        L.warn("Calling attempt_spend_with_remaining() with -ve remaining value %d", cost);
    }

    if (can_afford(cost + remaining)) {
        adjust_mc(-cost, reason);
        return true;
    }

    return false;
}

bool Player::attempt_spend_allowing_writeoff(int cost, int lim, MCReason reason) {
    if (cost <= get_mc()) {
        return attempt_spend(cost, reason);
    }

    // We're trying to overspend, but within the accepted limit
    // Zero MC and write off difference
    if (cost <= get_mc()+lim) {
        set_mc(0, reason);
        return true;
    }

    return false;
}

bool Player::attempt_spend_force(int cost, MCReason reason) {
    adjust_mc(-cost, reason);

    if (get_mc() < 0) {
        L.info("%s now has negative MC: %d", get_full_name(), get_mc());
    }

    return true;
}

bool Player::attempt_spend_cpuforce(int cost, MCReason reason) {
    if (is_human()) {
        return attempt_spend(cost, reason);
    }

    return attempt_spend_force(cost, reason);
}

int Player::remove_all_mc(MCReason reason) {
    if (get_mc() < 0) {
        return 0;
    }

    int mc_removed = get_mc();
    set_mc(0, reason);

    L.debug("[%s]: Funds zeroed (was %dMC)", get_full_name(), mc_removed);

    return mc_removed;
}

void Player::set_mc(int target_mc, MCReason reason) {
    adjust_mc(-(get_mc()), reason);
}

void Player::cpu_write_off_debt() {
    if (!is_human()) {
        L.error("Attempt to write off debt of human player, who should never be in debt");
        return;
    }

    if (get_mc() < 0) {
        L.info("[%s] - Writing off debt (%d)", get_full_name(), get_mc());
        set_mc(0, MC_CPUWriteOffDebt);
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

    if (is_human() && reputation >= 6) {
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
    if (get_race() != RACE_Human) {
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
    if (FEATURE(EF_COMBINE_SS_COUNSELLOR) && (off == OFF_Counsellor)) {
        off = OFF_Secret;
    }
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
        if (FEATURE(EF_COMBINE_SS_COUNSELLOR) && ((Officer)i == OFF_Counsellor)) {
            continue;
        }
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

    officer_status_reset(off, off_q);

    bool all_good = true;

    for (int i = 0; i < (int)OFFICER_MAX; ++i) {
        if (get_officer((Officer)i) != OFFQ_Good) {
            all_good = false;
            break;
        }
    }

    if (all_good) {
        achievement_manager.unlock(ACH_AllGoodOfficers);
    }
}

void Player::register_officer_fired_nopay(Officer off, OfficerQuality offq) {
    L.debug("Officer fired: %d %d", off, offq);
    officers_fired_nopay[officer_idx(off, offq)] = true;
}

void Player::register_officer_quit(Officer off, OfficerQuality offq) {
    L.debug("Officer quit: %d %d", off, offq);
    officers_quit[officer_idx(off, offq)] = true;
}

void Player::register_officer_killed(Officer off, OfficerQuality offq) {
    L.debug("Officer killed: %d %d", off, offq);
    officers_deceased[officer_idx(off, offq)] = true;
}

bool Player::officer_fired_nopay(Officer off, OfficerQuality offq) {
    return officers_fired_nopay[officer_idx(off, offq)];
}

bool Player::officer_quit(Officer off, OfficerQuality offq) {
    return officers_quit[officer_idx(off, offq)];
}

bool Player::officer_killed(Officer off, OfficerQuality offq) {
    return officers_deceased[officer_idx(off, offq)];
}
void Player::officer_status_reset(Officer off, OfficerQuality offq) {
    officers_fired_nopay[officer_idx(off, offq)] = false;
    officers_quit[officer_idx(off, offq)] = false;
    officers_deceased[officer_idx(off, offq)] = false;
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

void Player::save_mc_month_end() {
    int m = exostate().get_orig_month();
    L.debug("SAVE MC RECORDS: Month %d", m);

    mc_at_end_of_previous_month = mc;
    mc_history[m-1] = mc;

    for (int i = 0; i < MAX_MC_CATEGORIES; ++i) {
        mc_gains_lifetime[i] += mc_gains_this_month[i];
        mc_losses_lifetime[i] += mc_losses_this_month[i];
        mc_gains_last_month[i] = mc_gains_this_month[i];
        mc_losses_last_month[i] = mc_losses_this_month[i];
        mc_gains_this_month[i] = 0;
        mc_losses_this_month[i] = 0;
    }
}

void Player::save_mc_month_start() {
    mc_at_start_of_current_month = mc;

    for (int i = 0; i < MAX_MC_CATEGORIES; ++i) {
        mc_gains_lifetime[i] += mc_gains_this_month[i];
        mc_losses_lifetime[i] += mc_losses_this_month[i];
        mc_gains_over_last_month[i] = mc_gains_this_month[i];
        mc_losses_over_last_month[i] = mc_losses_this_month[i];
        mc_gains_this_month[i] = 0;
        mc_losses_this_month[i] = 0;
    }
}

void Player::adjust_mc(int amount, MCReason reason) {
    mc += amount;

    if (amount > 0) {
        L.debug("REGISTER GAIN: %dMC", amount);
        mc_gains_this_month[(int)reason] += amount;
    }

    if (amount < 0) {
        L.debug("REGISTER LOSS: %dMC", abs(amount));
        mc_losses_this_month[(int)reason] += abs(amount);
    }
}

int Player::officer_idx(Officer off, OfficerQuality offq) {
    return ((OFFQ_MAX * (int)off) + (int)offq);
}

const char* Player::get_officer_title(Officer officer) {
    return get_officer_character_title(officer, get_officer(officer));
}

const char* Player::get_officer_name(Officer officer) {
    return get_officer_character_name(officer, get_officer(officer));
}

const char* Player::get_officer_title_and_name(Officer officer) {
    return get_officer_character_title_and_name(officer, get_officer(officer));
}

const char* Player::get_officer_character_desc(Officer o) {
    if ((o == OFF_Science))    return O_SC;
    if ((o == OFF_Fleet))      return O_FL;
    if ((o == OFF_Battle))     return O_BA;
    if ((o == OFF_Secret))     return O_SS;
    if ((o == OFF_Counsellor)) return O_CO;
    L.error("Unknown officer");
    return "<NONE>";
}

const char* Player::get_officer_character_title(Officer o, OfficerQuality q) {
    if ((o == OFF_Science)    && (q == OFFQ_Poor))    return OT_SC_P;
    if ((o == OFF_Science)    && (q == OFFQ_Average)) return OT_SC_A;
    if ((o == OFF_Science)    && (q == OFFQ_Good))    return OT_SC_G;
    if ((o == OFF_Fleet)      && (q == OFFQ_Poor))    return OT_FL_P;
    if ((o == OFF_Fleet)      && (q == OFFQ_Average)) return OT_FL_A;
    if ((o == OFF_Fleet)      && (q == OFFQ_Good))    return OT_FL_G;
    if ((o == OFF_Battle)     && (q == OFFQ_Poor))    return OT_BA_P;
    if ((o == OFF_Battle)     && (q == OFFQ_Average)) return OT_BA_A;
    if ((o == OFF_Battle)     && (q == OFFQ_Good))    return OT_BA_G;
    if ((o == OFF_Secret)     && (q == OFFQ_Poor))    return OT_SS_P;
    if ((o == OFF_Secret)     && (q == OFFQ_Average)) return OT_SS_A;
    if ((o == OFF_Secret)     && (q == OFFQ_Good))    return OT_SS_G;
    if ((o == OFF_Counsellor) && (q == OFFQ_Poor))    return OT_CO_P;
    if ((o == OFF_Counsellor) && (q == OFFQ_Average)) return OT_CO_A;
    if ((o == OFF_Counsellor) && (q == OFFQ_Good))    return OT_CO_G;
    L.error("Unknown officer");
    return "<NONE>";
}

const char* Player::get_officer_character_name(Officer o, OfficerQuality q) {
    if ((o == OFF_Science)    && (q == OFFQ_Poor))    return ON_SC_P;
    if ((o == OFF_Science)    && (q == OFFQ_Average)) return ON_SC_A;
    if ((o == OFF_Science)    && (q == OFFQ_Good))    return ON_SC_G;
    if ((o == OFF_Fleet)      && (q == OFFQ_Poor))    return ON_FL_P;
    if ((o == OFF_Fleet)      && (q == OFFQ_Average)) return ON_FL_A;
    if ((o == OFF_Fleet)      && (q == OFFQ_Good))    return ON_FL_G;
    if ((o == OFF_Battle)     && (q == OFFQ_Poor))    return ON_BA_P;
    if ((o == OFF_Battle)     && (q == OFFQ_Average)) return ON_BA_A;
    if ((o == OFF_Battle)     && (q == OFFQ_Good))    return ON_BA_G;
    if ((o == OFF_Secret)     && (q == OFFQ_Poor))    return ON_SS_P;
    if ((o == OFF_Secret)     && (q == OFFQ_Average)) return ON_SS_A;
    if ((o == OFF_Secret)     && (q == OFFQ_Good))    return ON_SS_G;
    if ((o == OFF_Counsellor) && (q == OFFQ_Poor))    return ON_CO_P;
    if ((o == OFF_Counsellor) && (q == OFFQ_Average)) return ON_CO_A;
    if ((o == OFF_Counsellor) && (q == OFFQ_Good))    return ON_CO_G;
    L.error("Unknown officer");;
    return "<NONE>";
}

const char* Player::get_officer_character_title_and_name(Officer o, OfficerQuality q) {
    if ((o == OFF_Science)    && (q == OFFQ_Poor))    return OT_SC_P " " ON_SC_P;
    if ((o == OFF_Science)    && (q == OFFQ_Average)) return OT_SC_A " " ON_SC_A;
    if ((o == OFF_Science)    && (q == OFFQ_Good))    return OT_SC_G " " ON_SC_G;
    if ((o == OFF_Fleet)      && (q == OFFQ_Poor))    return OT_FL_P " " ON_FL_P;
    if ((o == OFF_Fleet)      && (q == OFFQ_Average)) return OT_FL_A " " ON_FL_A;
    if ((o == OFF_Fleet)      && (q == OFFQ_Good))    return OT_FL_G " " ON_FL_G;
    if ((o == OFF_Battle)     && (q == OFFQ_Poor))    return OT_BA_P " " ON_BA_P;
    if ((o == OFF_Battle)     && (q == OFFQ_Average)) return OT_BA_A " " ON_BA_A;
    if ((o == OFF_Battle)     && (q == OFFQ_Good))    return OT_BA_G " " ON_BA_G;
    if ((o == OFF_Secret)     && (q == OFFQ_Poor))    return OT_SS_P " " ON_SS_P;
    if ((o == OFF_Secret)     && (q == OFFQ_Average)) return OT_SS_A " " ON_SS_A;
    if ((o == OFF_Secret)     && (q == OFFQ_Good))    return OT_SS_G " " ON_SS_G;
    if ((o == OFF_Counsellor) && (q == OFFQ_Poor))    return OT_CO_P " " ON_CO_P;
    if ((o == OFF_Counsellor) && (q == OFFQ_Average)) return OT_CO_A " " ON_CO_A;
    if ((o == OFF_Counsellor) && (q == OFFQ_Good))    return OT_CO_G " " ON_CO_G;
    L.error("Unknown officer");
    return "<NONE>";
}

void Player::save(cJSON* j) const
{
    SAVE_NUM(j, nopirates);
    SAVE_ARRAY_OF_NUM(j, officers_fired_nopay);
    SAVE_ARRAY_OF_NUM(j, officers_quit);
    SAVE_ARRAY_OF_NUM(j, officers_deceased);
    SAVE_BOOL(j, invalid_placement_seen);
    SAVE_BOOL(j, advanced_report_unlocked);
    SAVE_BOOL(j, advanced_galmap_unlocked);
    SAVE_ENUM(j, character);
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
    SAVE_NUM(j, mc_at_end_of_previous_month);
    SAVE_NUM(j, mc_at_start_of_current_month);
    SAVE_BOOL(j, _intro_seen);
    SAVE_BOOL(j, _intro_comm_seen);
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
    SAVE_ARRAY_OF_NUM(j, mc_gains_lifetime);
    SAVE_ARRAY_OF_NUM(j, mc_losses_lifetime);
    SAVE_ARRAY_OF_NUM(j, mc_gains_last_month);
    SAVE_ARRAY_OF_NUM(j, mc_losses_last_month);
    SAVE_ARRAY_OF_NUM(j, mc_gains_over_last_month);
    SAVE_ARRAY_OF_NUM(j, mc_losses_over_last_month);
    SAVE_ARRAY_OF_NUM(j, mc_gains_this_month);
    SAVE_ARRAY_OF_NUM(j, mc_losses_this_month);
    SAVE_ARRAY_OF_NUM(j, mc_history);
}

void Player::load(cJSON* j)
{
    LOAD_NUM(j, nopirates);
    LOAD_ARRAY_OF_NUM(j, officers_fired_nopay);
    LOAD_ARRAY_OF_NUM(j, officers_quit);
    LOAD_ARRAY_OF_NUM(j, officers_deceased);
    LOAD_BOOL(j, invalid_placement_seen);
    LOAD_BOOL(j, advanced_report_unlocked);
    LOAD_BOOL(j, advanced_galmap_unlocked);
    LOAD_ENUM(j, character);
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
    LOAD_NUM(j, mc_at_end_of_previous_month);
    LOAD_NUM(j, mc_at_start_of_current_month);
    LOAD_BOOL(j, _intro_seen);
    LOAD_BOOL(j, _intro_comm_seen);
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
    LOAD_ARRAY_OF_NUM(j, mc_gains_lifetime);
    LOAD_ARRAY_OF_NUM(j, mc_losses_lifetime);
    LOAD_ARRAY_OF_NUM(j, mc_gains_last_month);
    LOAD_ARRAY_OF_NUM(j, mc_losses_last_month);
    SAVE_ARRAY_OF_NUM(j, mc_gains_over_last_month);
    SAVE_ARRAY_OF_NUM(j, mc_losses_over_last_month);
    LOAD_ARRAY_OF_NUM(j, mc_gains_this_month);
    LOAD_ARRAY_OF_NUM(j, mc_losses_this_month);
    LOAD_ARRAY_OF_NUM(j, mc_history);
}
