#include "galaxygen.h"

#include "anim.h"

#include "assetpaths.h"

enum ID {
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
    accepted = false;
    draw_manager.show_cursor(false);
}

const float FADE_SPEED = 10.f;

ExodusMode GalaxyGen::update(float delta) {
    SpriteClick click;

    if (draw_manager.pixelswap_active()) {
        return ExodusMode::MODE_None;
    }

    switch (stage) {
        case GGSTG_Gen:
            exostate.generate_galaxy();
            draw_galaxy(true);
            draw_manager.fill(
                TGT_Secondary,
                {0, 430, RES_X, RES_Y - 430},
                {0, 0, 0});
            draw_manager.pixelswap_start();
            stage = GGSTG_SwapIn;
            break;
        case GGSTG_SwapIn:
            stage = GGSTG_FadeIn;
            break;
        case GGSTG_FadeIn:
            fade_interp += delta * FADE_SPEED;
            if (fade_interp > 1) fade_interp = 1;
            draw_manager.draw(
                id(ID::YESORNO),
                anim_ynfade.frame(fade_interp),
                {RES_X/2, 460, 0.5, 0.5, 1, 1});
            if (fade_interp == 1) {
                draw_manager.save_background();
                draw_manager.show_cursor(true);
                stage = GGSTG_Wait;
            }
            break;
        case GGSTG_Wait:
            click = draw_manager.query_click(id(ID::YESORNO));
            if (click.id) {
                accepted = click.x < 0.5;
                stage = GGSTG_FadeOut;
                draw_manager.show_cursor(false);
            }
            break;
        case GGSTG_FadeOut:
            fade_interp -= delta * FADE_SPEED;
            if (fade_interp < 0) fade_interp = 0;
            draw_manager.draw(
                id(ID::YESORNO),
                anim_ynfade.frame(fade_interp),
                {RES_X/2, 460, 0.5, 0.5, 1, 1});
            if (fade_interp == 0) {
                draw_manager.fill(
                    {0, 430, RES_X, RES_Y - 430},
                    {0, 0, 0});
                if (accepted) {
                    stage = GGSTG_ToGame;
                } else {
                    stage = GGSTG_Gen;
                }
            }
            break;
        default:
            break;
    }

    return ExodusMode::MODE_None;
}
