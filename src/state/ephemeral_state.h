#ifndef GUARD_EPHEMERAL_STATE_H
#define GUARD_EPHEMERAL_STATE_H

/*
 * This represents game state which:
 * * Needs to be persisted over mode transitions (allows modes to communicate), and
 * * Applies over an 'atomic' series of events - I.E. not applicable to game save
 */

#include "exodus.h"

#include "galaxy/planet.h"

enum DestructionType {
    DESTROY_NStones,
    DESTROY_NRandom,
};

typedef struct {
    DestructionType type;
    Stone tgt_stone;
    int n_strikes;
    bool enable_explosions;
    bool draw;
    bool irradiated;
    bool show_target;
} PlanetDestruction;

enum EphState {
    EPH_None,
    EPH_Destruction,
};

class EphemeralState {
    public:
        EphemeralState();
        EphState get_ephemeral_state();
        void set_ephemeral_state(EphState);
        void clear_ephemeral_state();
        ExodusMode get_appropriate_mode();

        PlanetDestruction destruction;
    private:
        EphState eph_state;
};


#endif
