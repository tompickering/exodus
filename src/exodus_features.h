#ifndef GUARD_EXODUS_FEATURES_H
#define GUARD_EXODUS_FEATURES_H


// Planets in the star map are rendered at scale according to size
#define FEATURE_MULTISIZE_PLANETS 1

// Planets in the colonise mode are rendered at scale according to size
#define FEATURE_MULTISIZE_PLANETS_COLONISE 1

// Planets in the panel are rendered at scale according to size
#define FEATURE_MULTISIZE_PLANETS_PANEL 1

// In lunar battles, downed gliders and broken artillery serve as cover
#define FEATURE_WRECKAGE_COVER 1

// Only allow one alliance request per player per alliance type per month
#define FEATURE_ALLIANCE_LIMIT 1

// Only allow one festival per planet per month
// (N.B. 'Large festival' makes no sense without this!)
#define FEATURE_FESTIVAL_LIMIT 1

// Only allow festivals if the planet has at least one city
#define FEATURE_FESTIVAL_REQUIRES_CITY 1

// Enables a few extra counsellor messages
#define FEATURE_COUNSELLOR_EXTRA 1

// Display the name of an unpaid officer in bulletin
#define FEATURE_UNPAID_OFFICER_NAMED 1

// Display the name of an unpaid officer in bulletin
#define FEATURE_OFFICER_COST_INFO 1

// In orig, toggle buttons show current state rather than effect-when-clicked, which seems unnatural
#define FEATURE_FLIP_BUTTONS 1

// CPU accounts for volcano plutonium bonus when building reactors
#define FEATURE_CPU_EXPLOITS_VOLCANO 1

// Humans don't lose allies randomly
#define FEATURE_HUMAN_NO_RANDOM_ALLY_LOSS 1

// 'Money' aim ignores army funds - the player could win without this by manually adjusting all planets
#define FEATURE_AIM_MONEY_EXCLUDE_ARMY 1

// Randomise certain loops-over-planets in the game to mitigate order bias
#define FEATURE_PLANET_ITERATION_RANDOMISE 1

// There is a chance rebels won't adopt a new ruler after a successful revolution
#define FEATURE_REBEL_REPUBLIC_CHANCE 1

// Prevents AI from moving battle units against the direction they've just moved
#define FEATURE_PREVENT_AI_BATTLE_BACKTRACK 1

// Only allows one bombing mission per planet for human players
#define FEATURE_BOMBING_LIMIT_HUMAN 1

// Recallable, navigable planet summaries
#define FEATURE_PLANET_RECALLABLE_SUMMARIES 1

// Fixes bug meaning some AI terrorist actions are never taken
#define FIX_AI_TERROR 1

#define DISABLE_MULTIPLAYER 1

#endif
