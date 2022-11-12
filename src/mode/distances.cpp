#include "distances.h"

#include "draw/draw.h"
#include "state/exodus_state.h"

extern DRAWMANAGER draw_manager;
extern ExodusState exostate;

const static int DIST_RINGS = 5;
const static int DIST_STEP = 150;

Distances::Distances() : ModeBase("Distances"), GalaxyDrawer(), PanelDrawer(PNL_Galaxy) {
}

void Distances::enter() {
    draw_galaxy(false);

    Player *p = exostate.get_active_player();

    const PlayerLocation& loc = p->get_location();
    const FlyTarget *ft = exostate.loc2tgt(loc.get_target());

    int cx, cy;
    get_draw_position(ft, cx, cy);

    char n[2];

    for (int i = 1; i < DIST_RINGS; ++i)
    {
        int r = i*DIST_STEP;
        draw_manager.draw_circle(cx, cy, r, COL_TEXT2);
        snprintf(n, sizeof(n), "%d", i+1);
        int off = r+4;
        draw_manager.draw_text(
            n,
            Justify::Right,
            cx - off, cy,
            COL_TEXT2);
        draw_manager.draw_text(
            n,
            Justify::Left,
            cx + off, cy,
            COL_TEXT2);
    }

    draw_panel_bg(TGT_Primary);
    update_panel_info_ft(TGT_Primary, p, exostate.get_active_flytarget());
    update_panel_info_player(TGT_Primary, p);
}

ExodusMode Distances::update(float delta) {
    if (draw_manager.clicked() || input_manager.consume(K_D)) {
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
