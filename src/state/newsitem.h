#ifndef GUARD_NEWSITEM_H
#define GUARD_NEWSITEM_H

#include "player/player.h"

#define MAX_NEWSITEMS 20

enum NewsItemType {
    NI_None,
    NI_NativesOfferPresents,
    NI_NotEnoughFood,
    NI_ControlLost,
    NI_SurfChangeCultivation,
    NI_SurfChangeClearing,
    NI_CityExpanded,
    NI_SuccessfulRevolution,
    NI_Revolution,
    NI_AlienAttack,
    NI_PlanetTakeover,
    NI_FailedTakeover,
    NI_AssassinCaptured,
    NI_TerroristAttack,
    NI_Meteor,
    NI_LeftGalaxy,
    NI_NewAnimal,
    NI_NewPlant,
    NI_NewInvention,
    NI_Plague,
    NI_PlantsPoisoned,
    NI_Epidemic,
    NI_BattleMachinesDestructed,
    NI_GeologicalChange,
    NI_BombAttack,
    NI_Debug
};

class NewsItem {
    public:
        NewsItem();
        NewsItem(int, int, NewsItemType);
        const NewsItemType type;
        const int star_idx;
        const int planet_idx;
        const char* get_string() const;

        bool player_owned;

        int player_0;
        int player_1;
        Invention inv;
};

#endif
