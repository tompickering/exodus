#ifndef GUARD_EPHEMERAL_STATE_H
#define GUARD_EPHEMERAL_STATE_H

/*
 * This represents game state which:
 * * Needs to be persisted over mode transitions (allows modes to communicate), and
 * * Applies over an 'atomic' series of events - I.E. not applicable to game save
 */

#include "exodus.h"

#include "galaxy/planet.h"

#include "exodus_debug.h"

enum AggressorType {
    AGG_Player,
    AGG_Rebels,
    AGG_Aliens,
};

typedef struct {
    AggressorType aggressor_type;
    int aggressor_idx;
    bool human_battle;
    bool human_attacking;  // Separate to support
    bool human_defending;  // multiplayer PvP.
    bool auto_battle;
    bool aggressor_manual_placement;
    bool defender_manual_placement;
    int aggressor_group_size;
    int defender_group_size;
    int aggressor_inf;
    int aggressor_gli;
    int aggressor_art;
    int defender_inf;
    int defender_gli;
    int defender_art;
    int defender_mines;
} LunarBattleParams;

typedef struct {
    int aggressor_units_lost;
    int defender_units_lost;
    bool aggressor_won;
} LunarBattleReport;

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

typedef struct {
    bool done;
    int cost;
} Research;

enum EphState {
    EPH_None,
    EPH_LunarBattlePrep,
    EPH_LunarBattle,
    EPH_LunarBattleReport,
    EPH_Destruction,
    EPH_ResearchCheck,
};

class EphemeralState {
    public:
        EphemeralState();
        EphState get_ephemeral_state();
        void set_ephemeral_state(EphState);
        void clear_ephemeral_state();
        ExodusMode get_appropriate_mode();

        LunarBattleParams lunar_battle;
        LunarBattleReport lunar_battle_report;
        PlanetDestruction destruction;
        Research research;
    private:
        EphState eph_state;

    friend class ExodusDebug;
};


#endif
