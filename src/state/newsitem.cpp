#include "newsitem.h"

NewsItem::NewsItem()
    : star_idx(-1), planet_idx(-1), type(NI_None) {
    player_owned = false;
    player_0 = -1;
    player_1 = -1;
    inv = INV_MAX;
}

NewsItem::NewsItem(int star, int planet, NewsItemType _type)
    : star_idx(star), planet_idx(planet), type(_type) {
    player_owned = false;
}

// Orig registers with PROCdonotice
const char* NewsItem::get_string() const {
    switch (type) {
        case NI_None:
            return "";
        case NI_NativesOfferPresents:
             return "Natives offer presents";
        case NI_NotEnoughFood:
             return "Not enough food";
        case NI_ControlLost:
             return "Control lost";
        case NI_SurfChangeCultivation:
             return "Surface change due to cultivation";
        case NI_SurfChangeClearing:
             return "Surface change due to clearing";
        case NI_CityExpanded:
            // TODO: Implement
             return "A city has expanded";
        case NI_SuccessfulRevolution:
            // TODO: Implement
             return "Successful revolution";
        case NI_Revolution:
            // TODO: Implement
             return "Revolution";
        case NI_AlienAttack:
            // TODO: Implement
             return "Attack of alien pirates";
        case NI_PlanetTakeover:
             // TODO: This needs to vary based on planet
            // TODO: Implement
             return "TODO";
        case NI_FailedTakeover:
             // TODO: This needs to vary based on planet
            // TODO: Implement
             return "TODO";
        case NI_AssassinCaptured:
            // TODO: Implement
             return "Assassin captured";
        case NI_TerroristAttack:
            // TODO: Implement
             return "Terrorist attack";
        case NI_Meteor:
            // TODO: Implement
             return "Meteor has hit the planet";
        case NI_LeftGalaxy:
             // TODO: This needs to vary based on player
            // TODO: Implement
             return "TODO";
        case NI_NewAnimal:
            // TODO: Implement
             return "New animal discovered";
        case NI_NewPlant:
            // TODO: Implement
             return "New plant discovered";
        case NI_NewInvention:
             // TODO: This needs to vary based on invention
            // TODO: Implement
             return "TODO";
        case NI_Plague:
            // TODO: Implement
             return "Plague";
        case NI_PlantsPoisoned:
            // TODO: Implement
             return "Plants poisoned";
        case NI_Epidemic:
            // TODO: Implement
             return "Epidemic";
        case NI_BattleMachinesDestructed:
            // TODO: Implement
             return "Battle machines have been destructed";
        case NI_GeologicalChange:
            // TODO: Implement
             return "Geological change of surface";
        case NI_PlayerTakeover:
             // TODO: This needs to vary based on players
            // TODO: Implement
             return "TODO";
        case NI_PlayerFailedTakeover:
             // TODO: This needs to vary based on players
            // TODO: Implement
             return "TODO";
        case NI_BombAttack:
            // TODO: Implement
             return "Bomb attack";
        case NI_Debug:
            return "TEST NEWS ITEM";
    }

    return "";
}
