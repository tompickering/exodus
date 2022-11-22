#ifndef GUARD_EPHEMERAL_STATE_H
#define GUARD_EPHEMERAL_STATE_H

/*
 * This represents game state which:
 * * Needs to be persisted over mode transitions (allows modes to communicate), and
 * * Applies over an 'atomic' series of events - I.E. not applicable to game save
 */

#include "exodus.h"

#include "exodus_state.h"
#include "galaxy/planet.h"

#include "exodus_debug.h"

extern ExodusState exostate;

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
    int total() {
        return inf+gli+art;
    }
} UnitCount;

typedef struct {
    AggressorType agg_type;
    // FIXME: These two would be better as a BattleResolution enum, since retreated => !won
    bool aggressor_won;
    bool aggressor_retreated;
    bool rebel_peace;
    UnitCount agg_init; // \ Units that were taken into battle.
    UnitCount def_init; // /
    UnitCount agg_lost; // \ Units that were destroyed in battle.
    UnitCount def_lost; // /
    UnitCount agg_surf; // \ Units that are still on the surface and
    UnitCount def_surf; // / may need to be moved to planet or fleet.
    void clear() {
        aggressor_won = false;
        aggressor_retreated = false;
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
    StoneSet tgt_stones;
    int n_strikes;
    bool enable_explosions;
    bool enable_explosion_anims;
    bool draw;
    bool irradiated;
    bool show_target;
    // If -1 then blameless destruction (meteor etc)
    // If >=0 then this player's attacking
    int destroyer_idx;
    bool terror;
    bool nuke;
} PlanetDestruction;

typedef struct {
    bool done;
    int cost;
} Research;

typedef struct {
    int loc;
    int months;
} FlyPlan;

enum PostPlanetAction : uint8_t {
    PPA_BadLaws,
    PPA_NoEssentials,
    PPA_Festival,
};

// Multiple actions might be needed simultaneously - need 1 bit per reason
typedef uint8_t PostPlanetFlags;

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
    EPH_ProbePlanet,
    EPH_ScoutPlanet,
    EPH_Destruction,
    EPH_ResearchCheck,
    EPH_AlienResearch,
    EPH_ResumeFly,
    EPH_PostPlanet,
    EPH_SelectPlanet,
    EPH_MovePlanet,
    EPH_GameOver,
};

enum SelectPlanetReason : uint8_t {
    SPR_None,
    SPR_PlanetSurface,
    SPR_MAX
};

class EphemeralState {
    public:
        EphemeralState();
        EphState get_ephemeral_state();
        void set_ephemeral_state(EphState);
        void clear_ephemeral_state();
        bool ephemeral_state_set();
        ExodusMode get_appropriate_mode();

        void set_postplanet(PostPlanetAction);
        bool consume_postplanet(PostPlanetAction);

        // These MUST be set going into EPH_SelectPlanet
        // This is where the results are stored
        // The state should be done via select_planet()
        int* selectplanet_star;
        int* selectplanet_planet;
        int selectplanet_mc;
        Trace selectplanet_trace;
        MissionType selectplanet_mission;
        // Use this form if you just want to store values to use later
        void select_planet(int& st, int& pl) {
            selectplanet_star = &st;
            selectplanet_planet = &pl;
            selectplanet_mc = 0;
            selectplanet_trace = TRACE_None;
            selectplanet_mission = MT_None;
            set_ephemeral_state(EPH_SelectPlanet);
        }
        // Parameterless select_planet will use internal value storage
        // Use this form if you want to respond immediately
        SelectPlanetReason selectplanet_reason = SPR_None;
        int selectplanet_star_internal;
        int selectplanet_planet_internal;
        void select_planet(SelectPlanetReason reason) {
            selectplanet_reason = reason;
            select_planet(selectplanet_star_internal, selectplanet_planet_internal);
        }
        ExodusMode selectplanet_resolve() {
            clear_ephemeral_state();
            SelectPlanetReason reason = selectplanet_reason;
            selectplanet_reason = SPR_None;
            switch (reason) {
                case SPR_PlanetSurface:
                    exostate.set_active_flytarget(exostate.loc2tgt(selectplanet_star_internal));
                    exostate.set_active_planet(selectplanet_planet_internal);
                    set_ephemeral_state(EPH_ScoutPlanet);
                    return get_appropriate_mode();
                default:
                    break;
            }

            return ExodusMode::MODE_GalaxyMap;
        }

        LunarBattleParams lunar_battle;
        LunarBattleReport lunar_battle_report;
        SpaceBattleParams space_battle;
        PlanetDestruction destruction;
        Research research;
        FlyPlan fly_plan;
        GameOverReason game_over_reason;

        const char* default_music;

        bool galaxymap_pixelswap;
    private:
        EphState eph_state;

        PostPlanetFlags postplanet_flags;

    friend class ExodusDebug;
};


#endif
