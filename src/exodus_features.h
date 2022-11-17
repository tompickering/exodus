#ifndef GUARD_EXODUS_FEATURES_H
#define GUARD_EXODUS_FEATURES_H


// Planets in the star map are rendered at scale according to size
#define FEATURE_MULTISIZE_PLANETS 1

// In lunar battles, downed gliders and broken artillery serve as cover
#define FEATURE_WRECKAGE_COVER 1

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

// Fixes bug meaning some AI terrorist actions are never taken
#define FIX_AI_TERROR 1

#endif
