#ifndef GUARD_NEWSITEM_H
#define GUARD_NEWSITEM_H

#include "save/saveable.h"

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
    NI_PlanetClaimed,
    NI_Debug
};

class NewsItem : public Saveable {
    public:
        virtual void save(cJSON* j) const override {
            SAVE_ENUM(j, type);
            SAVE_NUM(j, star_idx);
            SAVE_NUM(j, planet_idx);
            SAVE_BOOL(j, player_owned);
            SAVE_NUM(j, player_0);
            SAVE_NUM(j, player_1);
            SAVE_ENUM(j, inv);
        }

        virtual void load(cJSON* j) override {
            LOAD_ENUM(j, type);
            LOAD_NUM(j, star_idx);
            LOAD_NUM(j, planet_idx);
            LOAD_BOOL(j, player_owned);
            LOAD_NUM(j, player_0);
            LOAD_NUM(j, player_1);
            LOAD_ENUM(j, inv);
        }

        NewsItem();
        NewsItem(int, int, NewsItemType);

        const char* get_string() const;

        NewsItemType type;
        int star_idx;
        int planet_idx;

        bool player_owned;

        int player_0;
        int player_1;
        Invention inv;
};

#endif
