#include "galaxy_drawer.h"

#include "galaxy/galaxy.h"
#include "galaxy/star.h"

#include "draw/draw.h"

#include "assetpaths.h"

#define PAD_X 30
#define PAD_Y 30
#define DRAW_W RES_X
#define DRAW_H 400

const DrawArea galaxy_panel_area = {0, 430, RES_X, RES_Y - 430};

static const char* BG_SPR = IMG_BG_STARS3;

static const char* STAR_SPRITES[] = {
    IMG_TS1_SUN1,
    IMG_TS1_SUN2,
    IMG_TS1_SUN3,
    IMG_TS1_SUN4,
    IMG_TS1_SUN5,
    IMG_TS1_SUN6,
    IMG_TS1_SUN7,
    IMG_TS1_SUN8,
    IMG_TS1_SUN9,
};

static const char* GUILD_SPRITE = IMG_TS1_WORM;

const int GDRAW_W = DRAW_W - PAD_X * 2;
const int GDRAW_H = DRAW_H - PAD_Y * 2;
const int SEP_X = GDRAW_W / GALAXY_COLS;
const int SEP_Y = GDRAW_H / GALAXY_ROWS;

GalaxyDrawer::GalaxyDrawer() {
}

void GalaxyDrawer::draw_galaxy(bool pixelswap) {
    int x, y;
    const char *spr;

    DrawTarget tgt = pixelswap ? TGT_Secondary : TGT_Primary;

    draw_manager.draw(tgt, BG_SPR);
    unsigned int n_stars;
    Galaxy *gal = exostate.get_galaxy();
    const Star *stars = gal->get_stars(n_stars);
    for (unsigned int i = 0; i < n_stars; ++i) {
        const Star *s = &stars[i];
        spr = STAR_SPRITES[s->get_size()];
        x = PAD_X + SEP_X * s->x;
        y = PAD_Y + SEP_Y * s->y;
        draw_manager.draw(tgt, spr, {x, y, 0.5, 0.5, 1, 1});
    }

    Guild *guild = gal->get_guild();
    spr = GUILD_SPRITE;
    x = PAD_X + SEP_X * guild->x;
    y = PAD_Y + SEP_Y * guild->y;
    draw_manager.draw(tgt, spr, {x, y, 0.5, 0.5, 1, 1});

    //draw_manager.fill({0, PAD_Y + DRAW_H, RES_X, RES_Y - PAD_Y - DRAW_H}, {0, 0, 0});
}
