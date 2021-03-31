#include "arrive.h"

#include "galaxy/flytarget.h"

#include "anim.h"
#include "util/value.h"

#include "assetpaths.h"

static const float SPACEPORT_TIME = 2.5f;
static const float HANGAR_TIME = 3.f;

enum ID {
    SPACEPORT_SHIP,
    END,
};

static Anim anim_hangar(19,
                    IMG_LP1_LAND2,  IMG_LP1_LAND3,  IMG_LP1_LAND4,  IMG_LP1_LAND5,
    IMG_LP1_LAND6,  IMG_LP1_LAND7,  IMG_LP1_LAND8,  IMG_LP1_LAND9,  IMG_LP1_LAND10,
    IMG_LP1_LAND11, IMG_LP1_LAND12, IMG_LP1_LAND13, IMG_LP1_LAND14, IMG_LP1_LAND15,
    IMG_LP1_LAND16, IMG_LP1_LAND17, IMG_LP1_LAND18, IMG_LP1_LAND19, IMG_LP1_LAND20);

Arrive::Arrive() : ModeBase("Arrive"), PanelDrawer(PNL_Galaxy), FrameDrawer() {
    time = 0;
    stage = ARR_Spaceport;
}

void Arrive::enter() {
    ModeBase::enter(ID::END);
    time = 0;
    stage = ARR_Spaceport;
    player = exostate.get_active_player();
    planet = exostate.get_active_planet();

    if (!(planet && planet->exists() && planet->is_owned())) {
        L.fatal("Entered arrival mode for invalid planet");
    }

    owner = exostate.get_player(planet->get_owner());

    base_draw(planet->sprites()->spaceport);
}

void Arrive::base_draw(const char* bg) {
    FlyTarget *tgt = exostate.loc2tgt(player->get_location().get_target());

    draw_manager.draw(bg);

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
        10, 380,
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

    float interp;

    switch (stage) {
        case ARR_Spaceport:
            {
                interp = time / SPACEPORT_TIME;
                if (interp > 1) interp = 1;

                if (interp < 1) {
                    int x = RES_X / 2;
                    int y = ilerp(80, 130, interp);
                    float sc = lerp(.8f, .5f, interp);
                    draw_manager.draw(
                        id(ID::SPACEPORT_SHIP),
                        IMG_AP1_APPROACH,
                        {x, y, .5f, .5f, sc, sc});
                } else {
                    draw_manager.draw(id(ID::SPACEPORT_SHIP), nullptr);
                    base_draw(IMG_LORD_ARRIVE);
                    time = 0;
                    stage = ARR_Hangar;
                }
            }
            break;
        case ARR_Hangar:
            {
                interp = time / HANGAR_TIME;
                if (interp > 1) interp = 1;

                if (interp < 1) {
                    int f = anim_hangar.interp_frame(interp);
                    draw_manager.draw(
                        anim_hangar.frame(f),
                        {260, 272, 0, 1, 1, 1});
                } else {
                    return ExodusMode::MODE_Pop;
                }
            }
            break;
    }

    return ExodusMode::MODE_None;
}
