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
    int aggressor_tele;
    int defender_inf;
    int defender_gli;
    int defender_art;
    int defender_mines;
} LunarBattleParams;

typedef struct {
    int inf;
    int gli;
    int art;
    int base;
    void clear() {
        inf = 0;
        gli = 0;
        art = 0;
        base = 0;
    }
} UnitCount;

typedef struct {
    bool aggressor_won;
    bool rebel_peace;
    UnitCount agg_init; // \ Units that were taken into battle.
    UnitCount def_init; // /
    UnitCount agg_lost; // \ Units that were destroyed in battle.
    UnitCount def_lost; // /
    UnitCount agg_surf; // \ Units that are still on the surface and
    UnitCount def_surf; // / may need to be moved to planet or fleet.
    void clear() {
        rebel_peace = false;
        agg_init.clear();
        def_init.clear();
        agg_lost.clear();
        def_lost.clear();
        agg_surf.clear();
        def_surf.clear();
    }
} LunarBattleReport;

typedef struct {
    int enemy_ships;
    int enemy_cargo;
    int enemy_scouts;
} SpaceBattleParams;

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

typedef struct {
    int loc;
    int months;
} FlyPlan;

enum GameOverReason {
    GAMEOVER_Failed,
    GAMEOVER_Dead,
    GAMEOVER_AllHumansDead,
};

enum EphState {
    EPH_None,
    EPH_LunarBattlePrep,
    EPH_LunarBattle,
    EPH_LunarBattleReport,
    EPH_Destruction,
    EPH_ResearchCheck,
    EPH_ResumeFly,
    EPH_GameOver,
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
        SpaceBattleParams space_battle;
        PlanetDestruction destruction;
        Research research;
        FlyPlan fly_plan;
        GameOverReason game_over_reason;
    private:
        EphState eph_state;

    friend class ExodusDebug;
};


#endif
