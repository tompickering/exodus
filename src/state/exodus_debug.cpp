#include "exodus_debug.h"

#include "exodus_state.h"
#include "ephemeral_state.h"

#include "player/player.h"

extern ExodusState exostate;

void ExodusDebug::add_mc(int mc) {
    Player* p = exostate.get_active_player();
    if (p) {
        p->give_mc(mc);
    }
}
