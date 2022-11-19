#include "ephemeral_state.h"

EphemeralState::EphemeralState() {
    clear_ephemeral_state();
    default_music = MUS_ST3_4D; // TODO: Check this - PROCbackmusic doesn't set this up
    galaxymap_pixelswap = false;
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

bool EphemeralState::ephemeral_state_set() {
    return eph_state != EPH_None;
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
        case EPH_ScoutPlanet:
            return ExodusMode::MODE_PlanetMap;
        case EPH_Destruction:
            return ExodusMode::MODE_PlanetMap;
        case EPH_ResearchCheck:
            return ExodusMode::MODE_None;
        case EPH_PostPlanet:
            // FIXME: This assumes exiting planet map will open
            // StarMap. Ideally we would specify StarMap here, but
            // we need to find another way to trigger a mode stack pop.
            return ExodusMode::MODE_Pop;
        case EPH_SelectPlanet:
            return ExodusMode::MODE_GalaxyMap;
        case EPH_GameOver:
            return ExodusMode::MODE_GameOver;
    }

    return ExodusMode::MODE_None;
}

void EphemeralState::set_postplanet(PostPlanetAction ppa) {
    postplanet_flags |= (1 << ppa);
}

bool EphemeralState::consume_postplanet(PostPlanetAction ppa) {
    PostPlanetFlags fmask = (1 << ppa);
    if (postplanet_flags & fmask) {
        postplanet_flags &= ~fmask;
        return true;
    }
    return false;
}
