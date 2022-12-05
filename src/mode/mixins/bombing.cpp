#include "bombing.h"

#include "util/value.h"

#include "shared.h"

extern ExodusState exostate;
extern EphemeralState ephstate;

Bombing::Bombing() {
}

void Bombing::bomb_planet(StoneSet targets, bool bomb_guns, int& att, int& bombers_killed) {
    att = 0;
    bombers_killed = 0;

    Planet *p = exostate.get_active_planet();

    int player_idx = exostate.get_active_player_idx();
    Player *player = exostate.get_active_player();
    int owner_idx = p->get_owner();
    Player *owner = exostate.get_player(owner_idx);

    if (owner) {
        exostate.unset_alliances(player_idx, owner_idx);
        if (!owner->is_human()) {
            if (owner->get_flag(0) != AI_Lo) {
                owner->set_hostile_to(player_idx);
            }
        }
    }

    if (targets.has(STONE_City)) {
        p->adjust_unrest(2);
    }

    Fleet& fleet = player->get_fleet_nonconst();

    att = p->plan_bomb(fleet.bombers, targets, bombers_killed, bomb_guns);

    if (bomb_guns) {
        p->adjust_airdef_guns(-att);
    }

    fleet.bombers = max(fleet.bombers - bombers_killed, 0);

    ephstate.clear_ephemeral_state();

    ephstate.set_ephemeral_state(EPH_Destruction);
    ephstate.destruction.type = DESTROY_NStones;
    ephstate.destruction.tgt_stones = targets;
    ephstate.destruction.n_strikes = att;
    ephstate.destruction.enable_explosions = true;
    ephstate.destruction.enable_explosion_anims = true;
    ephstate.destruction.irradiated = false;
    ephstate.destruction.show_target = att<3;
    ephstate.destruction.destroyer_idx = player_idx;
    ephstate.destruction.terror = false;
    ephstate.destruction.nuke = false;
    ephstate.destruction.draw = player->is_human() || owner->is_human();
}
