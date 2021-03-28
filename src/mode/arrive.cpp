#include "arrive.h"

#include "galaxy/flytarget.h"

#include "anim.h"
#include "assetpaths.h"

enum ID {
    END,
};


Arrive::Arrive() : ModeBase("Arrive"), PanelDrawer(PNL_Galaxy), FrameDrawer() {
    time = 0;
}

void Arrive::enter() {
    ModeBase::enter(ID::END);
    time = 0;
    Player *player = exostate.get_active_player();
    FlyTarget *tgt = exostate.loc2tgt(player->get_location().get_target());
    Planet *planet = exostate.get_active_planet();

    if (!(planet && planet->exists() && planet->is_owned())) {
        L.fatal("Entered arrival mode for invalid planet");
    }

    Player *owner = exostate.get_player(planet->get_owner());

    // TODO: Set proper image
    draw_manager.draw(planet->sprites()->map_bg);

    frame_draw();

    draw_panel_bg(TGT_Primary);
    update_panel_info_player(TGT_Primary, player);
    update_panel_info_ft(TGT_Primary, player, tgt);

    // TODO: Check exact text here, get_name() vs get_full_name() etc
    char text[128];
    snprintf(
        text,
        sizeof(text),
        "%s has arrived at %s to talk with %s.",
        player->get_name(),
        planet->get_name(),
        owner->get_name());

    draw_manager.draw_text(
        text,
        Justify::Left,
        10, 200,
        COL_TEXT);

    draw_manager.save_background();
}

ExodusMode Arrive::update(float delta) {
    /*
    if (input_manager.consume(K_Space)) {
        return ExodusMode::MODE_Pop;
    }
    */

    time += delta;

    if (time > 2) {
        return ExodusMode::MODE_Pop;
    }

    frame_draw();

    return ExodusMode::MODE_None;
}
