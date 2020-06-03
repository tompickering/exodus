#include "ephemeral_state.h"

EphemeralState::EphemeralState() {
    clear_ephemeral_state();
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
    }

    return ExodusMode::MODE_None;
}
