#include "newsitem.h"

#include "exodus_state.h"

extern ExodusState exostate;

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

static char b[128];

// Orig registers with PROCdonotice
const char* NewsItem::get_string() const {
    Player *p0, *p1 = nullptr;
    if (player_0 >= 0) {
        p0 = exostate.get_player(player_0);
    }
    if (player_1 >= 0) {
        p1 = exostate.get_player(player_1);
    }

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
             return "A city has expanded";
        case NI_SuccessfulRevolution:
             return "Successful revolution";
        case NI_Revolution:
             return "Revolution";
        case NI_AlienAttack:
             return "Attack of alien pirates";
        case NI_PlanetTakeover:
             snprintf(b, sizeof(b), "%s has taken over %s's planet", p0->get_name(), p1->get_name());
             return b;
        case NI_FailedTakeover:
             snprintf(b, sizeof(b), "%s tried to take over %s's planet", p0->get_name(), p1->get_name());
             return b;
        case NI_AssassinCaptured:
             return "Assassin captured";
        case NI_TerroristAttack:
             return "Terrorist attack";
        case NI_Meteor:
             return "Meteor has hit the planet";
        case NI_LeftGalaxy:
             snprintf(b, sizeof(b), "%s has left the galaxy", p0->get_name());
             return b;
        case NI_NewAnimal:
             return "New animal discovered";
        case NI_NewPlant:
             return "New plant discovered";
        case NI_NewInvention:
             snprintf(b, sizeof(b), "New invention: %s", p0->get_invention_str(inv));
             return b;
        case NI_Plague:
             return "Plague";
        case NI_PlantsPoisoned:
             return "Plants poisoned";
        case NI_Epidemic:
             return "Epidemic";
        case NI_BattleMachinesDestructed:
             return "Battle machines have been destructed";
        case NI_GeologicalChange:
             return "Geological change of surface";
        case NI_BombAttack:
             return "Bomb attack";
        case NI_Debug:
            return "TEST NEWS ITEM";
    }

    return "";
}
