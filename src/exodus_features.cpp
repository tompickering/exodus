#include "exodus_features.h"

#include "shared.h"

// New COMM message when starting the game
static bool INTRO_COMM = false;

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

// Determines galaxy-star transition time
static bool FAST_FADES = false;

// Diplays hits on enemy or enemy HP depending on general
static bool LUNAR_BATTLE_ADVANCED_HP_INFO = false;

// Diplays hits on enemy or enemy HP depending on general
static bool LUNAR_BATTLE_INF_DISCOVER_MINES = false;

// Units gain a power bonus after killing an enemy
static bool LUNAR_BATTLE_PROMOTION = false;

// Officers may be upgraded for a single lunar battle
static bool LUNAR_BATTLE_OFFICER_UPGRADE = false;

// Mines are louder than normal explosions
static bool LUNAR_BATTLE_LOUD_MINES = false;

// New-style panels
static bool LUNAR_BATTLE_NEW_PANELS = false;

// Battle fire markers
static bool LUNAR_BATTLE_FIRE_MARKERS = false;

// Science section in monthly report changes based on funding
static bool REFINED_SCIENCE_REPORTS = false;

// Adds an Amber possibility for food and plutonium
static bool SMARTER_STATUS_LIGHTS = false;

// Unlockable advanced star map
static bool ADVANCED_GALAXY_MAP = false;

// Award MC from space battles
static bool SPACE_BATTLE_MC = false;

// AI characters
static bool CHARACTERS = false;

// Finance menu (replaces News)
static bool FINANCE_MENU = false;

// Skip more bulletins
static bool SKIP_MORE_BULLETINS = false;

// Make random climate change less likely
static bool CLIMATE_CHANGE_LESS_LIKELY = false;

// Enables some MC to be gained from clearing cities if you're in that system
static bool RESETTLING = false;

// Lunar base can be built from COMM menu
static bool LUNAR_BASE_FROM_COMM_MENU;

// Enables Academies - N.B. this replaces Lunar Base, so use with LUNAR_BASE_FROM_COMM_MENU
static bool ACADEMIES = false;

// Lords are known only when met for the first time
static bool LORD_DISCOVERY = false;

// Officer characters
static bool OFFICER_CHARACTERS = false;

// Incorporate Counsellor officer into Secret Service
static bool COMBINE_SS_COUNSELLOR = false;

// Space battle button layout
static bool SPACE_BATTLE_LAYOUT = false;

// Fixes bug meaning some AI terrorist actions are never taken
static bool FIX_AI_TERROR = false;

// Improved Fleet Admiral was never implemented in the original!
static bool FIX_FLEET_ADMIRAL = false;

// Fix issue where units never move into range
static bool FIX_LUNAR_BATTLE_STALEMATE = false;

// Fix MC going to player in attack payoff event
static bool FIX_ATTACKPAYOFF_MC = false;


static bool DISABLE_MULTIPLAYER = true;

static ExodusGameMode mode = EXODUSGAMEMODE_Classic;
static bool mode_set = false;

void exodus_set_game_mode(ExodusGameMode game_mode) {
    mode = game_mode;
    mode_set = true;

    if (game_mode == EXODUSGAMEMODE_Classic) {
        INTRO_COMM = false;
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
        FAST_FADES = false;
        LUNAR_BATTLE_ADVANCED_HP_INFO = false;
        LUNAR_BATTLE_INF_DISCOVER_MINES = false;
        LUNAR_BATTLE_PROMOTION = false;
        LUNAR_BATTLE_OFFICER_UPGRADE = false;
        LUNAR_BATTLE_LOUD_MINES = false;
        LUNAR_BATTLE_NEW_PANELS = false;
        LUNAR_BATTLE_FIRE_MARKERS = false;
        REFINED_SCIENCE_REPORTS = false;
        SMARTER_STATUS_LIGHTS = false;
        ADVANCED_GALAXY_MAP = false;
        SPACE_BATTLE_MC = false;
        CHARACTERS = false;
        FINANCE_MENU = false;
        SKIP_MORE_BULLETINS = false;
        CLIMATE_CHANGE_LESS_LIKELY = false;
        RESETTLING = false;
        LUNAR_BASE_FROM_COMM_MENU = false;
        ACADEMIES = false;
        LORD_DISCOVERY = false;
        OFFICER_CHARACTERS = false;
        COMBINE_SS_COUNSELLOR = false;
        SPACE_BATTLE_LAYOUT = false;
        FIX_AI_TERROR = false;
        FIX_FLEET_ADMIRAL = false;
        FIX_LUNAR_BATTLE_STALEMATE = true;
        FIX_ATTACKPAYOFF_MC = false;
        DISABLE_MULTIPLAYER = true;
    }

    if (game_mode == EXODUSGAMEMODE_Enhanced) {
        INTRO_COMM = true;
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
        FAST_FADES = true;
        LUNAR_BATTLE_ADVANCED_HP_INFO = true;
        LUNAR_BATTLE_INF_DISCOVER_MINES = true;
        LUNAR_BATTLE_PROMOTION = true;
        LUNAR_BATTLE_OFFICER_UPGRADE = true;
        LUNAR_BATTLE_LOUD_MINES = true;
        LUNAR_BATTLE_NEW_PANELS = true;
        LUNAR_BATTLE_FIRE_MARKERS = true;
        REFINED_SCIENCE_REPORTS = true;
        SMARTER_STATUS_LIGHTS = true;
        ADVANCED_GALAXY_MAP = true;
        SPACE_BATTLE_MC = true;
        CHARACTERS = true;
        FINANCE_MENU = true;
        SKIP_MORE_BULLETINS = true;
        CLIMATE_CHANGE_LESS_LIKELY = true;
        RESETTLING = true;
        LUNAR_BASE_FROM_COMM_MENU = true;
        ACADEMIES = true;
        LORD_DISCOVERY = true;
        OFFICER_CHARACTERS = true;
        COMBINE_SS_COUNSELLOR = true;
        SPACE_BATTLE_LAYOUT = true;
        FIX_AI_TERROR = true;
        FIX_FLEET_ADMIRAL = true;
        FIX_LUNAR_BATTLE_STALEMATE = true;
        FIX_ATTACKPAYOFF_MC = true;
        DISABLE_MULTIPLAYER = true;
    }
}

bool ENHANCED() {
    if (!mode_set) {
        L.fatal("Attempt to query enhanced mode before initialising features");
    }

    return (mode == ExodusGameMode::EXODUSGAMEMODE_Enhanced);
}

bool FEATURE(ExodusFeature f) {
    if (!mode_set) {
        L.fatal("Attempt to query feature before initialising features");
    }

    if (f == EF_INTRO_COMM) return INTRO_COMM;
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
    if (f == EF_FAST_FADES) return FAST_FADES;
    if (f == EF_LUNAR_BATTLE_ADVANCED_HP_INFO) return LUNAR_BATTLE_ADVANCED_HP_INFO;
    if (f == EF_LUNAR_BATTLE_INF_DISCOVER_MINES) return LUNAR_BATTLE_INF_DISCOVER_MINES;
    if (f == EF_LUNAR_BATTLE_PROMOTION) return LUNAR_BATTLE_PROMOTION;
    if (f == EF_LUNAR_BATTLE_OFFICER_UPGRADE) return LUNAR_BATTLE_OFFICER_UPGRADE;
    if (f == EF_LUNAR_BATTLE_LOUD_MINES) return LUNAR_BATTLE_LOUD_MINES;
    if (f == EF_LUNAR_BATTLE_NEW_PANELS) return LUNAR_BATTLE_NEW_PANELS;
    if (f == EF_LUNAR_BATTLE_FIRE_MARKERS) return LUNAR_BATTLE_FIRE_MARKERS;
    if (f == EF_REFINED_SCIENCE_REPORTS) return REFINED_SCIENCE_REPORTS;
    if (f == EF_SMARTER_STATUS_LIGHTS) return SMARTER_STATUS_LIGHTS;
    if (f == EF_ADVANCED_GALAXY_MAP) return ADVANCED_GALAXY_MAP;
    if (f == EF_SPACE_BATTLE_MC) return SPACE_BATTLE_MC;
    if (f == EF_CHARACTERS) return CHARACTERS;
    if (f == EF_FINANCE_MENU) return FINANCE_MENU;
    if (f == EF_SKIP_MORE_BULLETINS) return SKIP_MORE_BULLETINS;
    if (f == EF_CLIMATE_CHANGE_LESS_LIKELY) return CLIMATE_CHANGE_LESS_LIKELY;
    if (f == EF_RESETTLING) return RESETTLING;
    if (f == EF_LUNAR_BASE_FROM_COMM_MENU) return LUNAR_BASE_FROM_COMM_MENU;
    if (f == EF_ACADEMIES) return ACADEMIES;
    if (f == EF_LORD_DISCOVERY) return LORD_DISCOVERY;
    if (f == EF_OFFICER_CHARACTERS) return OFFICER_CHARACTERS;
    if (f == EF_COMBINE_SS_COUNSELLOR) return COMBINE_SS_COUNSELLOR;
    if (f == EF_FIX_AI_TERROR) return FIX_AI_TERROR;
    if (f == EF_FIX_FLEET_ADMIRAL) return FIX_FLEET_ADMIRAL;
    if (f == EF_FIX_LUNAR_BATTLE_STALEMATE) return FIX_LUNAR_BATTLE_STALEMATE;
    if (f == EF_FIX_ATTACKPAYOFF_MC) return FIX_ATTACKPAYOFF_MC;
    if (f == EF_DISABLE_MULTIPLAYER) return DISABLE_MULTIPLAYER;

    L.fatal("Unhandled feature %d", (int)f);
    return false;
}
