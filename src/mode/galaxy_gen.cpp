#include "galaxy_gen.h"

#include "anim.h"
#include "mixins/panel_drawer.h"

#include "assetpaths.h"

enum ID {
    BG,
    TEXT,
    YESORNO,
    END,
};

Anim anim_ynfade(
    3,
    IMG_BR15_FADE1,
    IMG_BR15_FADE2,
    IMG_BR15_FADE3
);

GalaxyGen::GalaxyGen() : ModeBase("GalaxyGen"), GalaxyDrawer() {
}

void GalaxyGen::enter() {
    ModeBase::enter(ID::END);
    stage = GGSTG_Gen;
    fade_interp = 0;
    accepted = false;
    draw_manager.show_cursor(true);
}

const float FADE_SPEED = 5.f;

ExodusMode GalaxyGen::update(float delta) {
    SpriteClick click;

    if (draw_manager.pixelswap_active()) {
        return ExodusMode::MODE_None;
    }

    unsigned char c;

    switch (stage) {
        case GGSTG_Gen:
            exostate().generate_galaxy();
            draw_galaxy(true);
            draw_manager.fill(
                TGT_Secondary,
                galaxy_panel_area,
                {0, 0, 0});
            draw_manager.pixelswap_start();
            stage = GGSTG_SwapIn;
            break;
        case GGSTG_SwapIn:
            draw_manager.fill(
                ID::BG,
                galaxy_panel_area,
                {0, 0, 0});
            stage = GGSTG_FadeIn;
            break;
        case GGSTG_FadeIn:
            fade_interp += delta * FADE_SPEED;
            if (fade_interp > 1) fade_interp = 1;
            c = (int)(fade_interp * (float)0xFF);
            draw_manager.draw(
                id(ID::YESORNO),
                anim_ynfade.frame(fade_interp),
                {RES_X/2, 480, 0.5, 0.5, 1, 1});
            draw_manager.draw_text(
                id(ID::TEXT),
                "Choose this galaxy?",
                Justify::Centre,
                RES_X/2, 434,
                {c, c, c});
            if (fade_interp == 1) {
                draw_manager.save_background();
                stage = GGSTG_Wait;
            }
            break;
        case GGSTG_Wait:
            click = draw_manager.query_click(id(ID::YESORNO));
            if (click.id) {
                accepted = click.x < 0.5;
                stage = GGSTG_FadeOut;
            }
            break;
        case GGSTG_FadeOut:
            fade_interp -= delta * FADE_SPEED;
            if (fade_interp < 0) fade_interp = 0;
            c = (int)(fade_interp * (float)0xFF);
            draw_manager.draw(
                id(ID::YESORNO),
                anim_ynfade.frame(fade_interp),
                {RES_X/2, 480, 0.5, 0.5, 1, 1});
            draw_manager.draw_text(
                id(ID::TEXT),
                "Choose this galaxy?",
                Justify::Centre,
                RES_X/2, 434,
                {c, c, c});
            if (fade_interp == 0) {
                draw_manager.fill(
                    ID::BG,
                    galaxy_panel_area,
                    {0, 0, 0});
                if (accepted) {
                    exostate().finalise_galaxy();
                    stage = GGSTG_ToGame;
                } else {
                    stage = GGSTG_Gen;
                }
            }
            break;
        case GGSTG_ToGame:
            ephstate.galaxymap_pixelswap = true;
            return ExodusMode::MODE_GalaxyMap;
            break;
        default:
            break;
    }

    return ExodusMode::MODE_None;
}
