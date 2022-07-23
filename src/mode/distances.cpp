#include "distances.h"

#include "draw/draw.h"
#include "state/exodus_state.h"

extern DRAWMANAGER draw_manager;
extern ExodusState exostate;

Distances::Distances() : ModeBase("Distances"), GalaxyDrawer(), PanelDrawer(PNL_Galaxy) {
}

void Distances::enter() {
    draw_galaxy(false);
    draw_panel_bg(TGT_Primary);
    update_panel_info_ft(TGT_Primary, exostate.get_active_player(), exostate.get_active_flytarget());
}

ExodusMode Distances::update(float delta) {
    if (draw_manager.clicked()) {
        return ExodusMode::MODE_Pop;
    }
}
