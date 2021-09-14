#include "ephemeral_state.h"

EphemeralState::EphemeralState() {
    clear_ephemeral_state();
    default_music = MUS_ST3_4D; // TODO: Check this - PROCbackmusic doesn't set this up
}

EphState EphemeralState::get_ephemeral_state() {
    return eph_state;
}

void EphemeralState::set_ephemeral_state(EphState new_eph_state) {
    eph_state = new_eph_state;
}

void EphemeralState::clear_ephemeral_state() {
    eph_state = EPH_None;
}

ExodusMode EphemeralState::get_appropriate_mode() {
    switch (eph_state) {
        case EPH_None:
            return ExodusMode::MODE_None;
        case EPH_LunarBattlePrep:
            return ExodusMode::MODE_LunarBattlePrep;
        case EPH_LunarBattle:
            return ExodusMode::MODE_LunarBattle;
        case EPH_LunarBattleReport:
            // This is a little hacky - but battles should always return
            // to the previous mode, which should expect and respond to
            // EPH_LunarBattleReport.
            return ExodusMode::MODE_Pop;
        case EPH_Destruction:
            return ExodusMode::MODE_PlanetMap;
        case EPH_ResearchCheck:
            return ExodusMode::MODE_None;
        case EPH_GameOver:
            return ExodusMode::MODE_GameOver;
    }

    return ExodusMode::MODE_None;
}
