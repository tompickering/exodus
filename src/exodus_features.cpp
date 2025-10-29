#include "exodus_features.h"

#include "shared.h"

// Planets in the star map are rendered at scale according to size
static bool MULTISIZE_PLANETS = false;

// Planets in the colonise mode are rendered at scale according to size
static bool MULTISIZE_PLANETS_COLONISE = false;

// Planets in the panel are rendered at scale according to size
static bool MULTISIZE_PLANETS_PANEL = false;

// In lunar battles, downed gliders and broken artillery serve as cover
static bool WRECKAGE_COVER = false;

// Only allow one alliance request per player per alliance type per month
static bool ALLIANCE_LIMIT = false;

// Only allow one festival per planet per month
// (N.B. 'Large festival' makes no sense without this!)
static bool FESTIVAL_LIMIT = false;

// Only allow festivals if the planet has at least one city
static bool FESTIVAL_REQUIRES_CITY = false;

// Enables a few extra counsellor messages
static bool COUNSELLOR_EXTRA = false;

// Display the name of an unpaid officer in bulletin
static bool UNPAID_OFFICER_NAMED = false;

// Display the name of an unpaid officer in bulletin
static bool OFFICER_COST_INFO = false;

// In orig, toggle buttons show current state rather than effect-when-clicked, which seems unnatural
static bool FLIP_BUTTONS = false;

// CPU accounts for volcano plutonium bonus when building reactors
static bool CPU_EXPLOITS_VOLCANO = false;

// Humans don't lose allies randomly
static bool HUMAN_NO_RANDOM_ALLY_LOSS = false;

// 'Money' aim ignores army funds - the player could win without this by manually adjusting all planets
static bool AIM_MONEY_EXCLUDE_ARMY = false;

// Randomise certain loops-over-planets in the game to mitigate order bias
//static bool PLANET_ITERATION_RANDOMISE = false;

// There is a chance rebels won't adopt a new ruler after a successful revolution
static bool REBEL_REPUBLIC_CHANCE = false;

// Prevents AI from moving battle units against the direction they've just moved
static bool PREVENT_AI_BATTLE_BACKTRACK = false;

// Only allows one bombing mission per planet for human players
static bool BOMBING_LIMIT_HUMAN = false;

// Recallable, navigable planet summaries
static bool PLANET_RECALLABLE_SUMMARIES = false;

// Display owned planets on the galaxy map
static bool GALAXY_MAP_PLANET_MARKERS = false;

// Attempt to purchase more expensive units first, to optimise MC
static bool OPTIMISE_ARMY_PRODUCTION_PRIORITIES = false;

// Fixes bug meaning some AI terrorist actions are never taken
static bool FIX_AI_TERROR = false;


static bool DISABLE_MULTIPLAYER = true;

static bool mode_set = false;

void exodus_set_game_mode(ExodusGameMode game_mode) {
    mode_set = true;

    if (game_mode == EXODUSGAMEMODE_Classic) {
        MULTISIZE_PLANETS = false;
        MULTISIZE_PLANETS_COLONISE = false;
        MULTISIZE_PLANETS_PANEL = false;
        WRECKAGE_COVER = false;
        ALLIANCE_LIMIT = false;
        FESTIVAL_LIMIT = false;
        FESTIVAL_REQUIRES_CITY = false;
        COUNSELLOR_EXTRA = false;
        UNPAID_OFFICER_NAMED = false;
        OFFICER_COST_INFO = false;
        FLIP_BUTTONS = false;
        CPU_EXPLOITS_VOLCANO = false;
        HUMAN_NO_RANDOM_ALLY_LOSS = false;
        AIM_MONEY_EXCLUDE_ARMY = false;
        //PLANET_ITERATION_RANDOMISE = false;
        REBEL_REPUBLIC_CHANCE = false;
        PREVENT_AI_BATTLE_BACKTRACK = false;
        BOMBING_LIMIT_HUMAN = false;
        PLANET_RECALLABLE_SUMMARIES = false;
        GALAXY_MAP_PLANET_MARKERS = false;
        OPTIMISE_ARMY_PRODUCTION_PRIORITIES = false;
        FIX_AI_TERROR = false;
        DISABLE_MULTIPLAYER = true;
    }

    if (game_mode == EXODUSGAMEMODE_Enhanced) {
        MULTISIZE_PLANETS = true;
        MULTISIZE_PLANETS_COLONISE = true;
        MULTISIZE_PLANETS_PANEL = true;
        WRECKAGE_COVER = true;
        ALLIANCE_LIMIT = true;
        FESTIVAL_LIMIT = true;
        FESTIVAL_REQUIRES_CITY = true;
        COUNSELLOR_EXTRA = true;
        UNPAID_OFFICER_NAMED = true;
        OFFICER_COST_INFO = true;
        FLIP_BUTTONS = true;
        CPU_EXPLOITS_VOLCANO = true;
        HUMAN_NO_RANDOM_ALLY_LOSS = true;
        AIM_MONEY_EXCLUDE_ARMY = true;
        //PLANET_ITERATION_RANDOMISE = true;
        REBEL_REPUBLIC_CHANCE = true;
        PREVENT_AI_BATTLE_BACKTRACK = true;
        BOMBING_LIMIT_HUMAN = true;
        PLANET_RECALLABLE_SUMMARIES = true;
        GALAXY_MAP_PLANET_MARKERS = true;
        OPTIMISE_ARMY_PRODUCTION_PRIORITIES = true;
        FIX_AI_TERROR = true;
        DISABLE_MULTIPLAYER = true;
    }
}

bool FEATURE(ExodusFeature f) {
    if (!mode_set) {
        L.fatal("Attempt to query feature before initialising features");
    }

    if (f == EF_MULTISIZE_PLANETS) return MULTISIZE_PLANETS;
    if (f == EF_MULTISIZE_PLANETS_COLONISE) return MULTISIZE_PLANETS_COLONISE;
    if (f == EF_MULTISIZE_PLANETS_PANEL) return MULTISIZE_PLANETS_PANEL;
    if (f == EF_WRECKAGE_COVER) return WRECKAGE_COVER;
    if (f == EF_ALLIANCE_LIMIT) return ALLIANCE_LIMIT;
    if (f == EF_FESTIVAL_LIMIT) return FESTIVAL_LIMIT;
    if (f == EF_FESTIVAL_REQUIRES_CITY) return FESTIVAL_REQUIRES_CITY;
    if (f == EF_COUNSELLOR_EXTRA) return COUNSELLOR_EXTRA;
    if (f == EF_UNPAID_OFFICER_NAMED) return UNPAID_OFFICER_NAMED;
    if (f == EF_OFFICER_COST_INFO) return OFFICER_COST_INFO;
    if (f == EF_FLIP_BUTTONS) return FLIP_BUTTONS;
    if (f == EF_CPU_EXPLOITS_VOLCANO) return CPU_EXPLOITS_VOLCANO;
    if (f == EF_HUMAN_NO_RANDOM_ALLY_LOSS) return HUMAN_NO_RANDOM_ALLY_LOSS;
    if (f == EF_AIM_MONEY_EXCLUDE_ARMY) return AIM_MONEY_EXCLUDE_ARMY;
    //if (f == EF_PLANET_ITERATION_RANDOMISE) return PLANET_ITERATION_RANDOMISE;
    if (f == EF_REBEL_REPUBLIC_CHANCE) return REBEL_REPUBLIC_CHANCE;
    if (f == EF_PREVENT_AI_BATTLE_BACKTRACK) return PREVENT_AI_BATTLE_BACKTRACK;
    if (f == EF_BOMBING_LIMIT_HUMAN) return BOMBING_LIMIT_HUMAN;
    if (f == EF_PLANET_RECALLABLE_SUMMARIES) return PLANET_RECALLABLE_SUMMARIES;
    if (f == EF_GALAXY_MAP_PLANET_MARKERS) return GALAXY_MAP_PLANET_MARKERS;
    if (f == EF_OPTIMISE_ARMY_PRODUCTION_PRIORITIES) return OPTIMISE_ARMY_PRODUCTION_PRIORITIES;
    if (f == EF_FIX_AI_TERROR) return FIX_AI_TERROR;
    if (f == EF_DISABLE_MULTIPLAYER) return DISABLE_MULTIPLAYER;

    L.fatal("Unhandled feature %d", (int)f);
    return false;
}
