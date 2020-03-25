#include "galaxy_drawer.h"

#include "galaxy/galaxy.h"
#include "galaxy/star.h"

#include "draw/draw.h"

#include "assetpaths.h"

#define PAD_X 10
#define PAD_Y 10

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

const int GDRAW_W = RES_X - PAD_X * 2;
const int GDRAW_H = RES_Y - PAD_Y * 2;
const int SEP_X = GDRAW_W / GALAXY_COLS;
const int SEP_Y = GDRAW_H / GALAXY_ROWS;

GalaxyDrawer::GalaxyDrawer() {
}

void GalaxyDrawer::draw_galaxy(bool pixelswap) {
    DrawTarget tgt = pixelswap ? TGT_Secondary : TGT_Primary;

    draw_manager.draw(tgt, IMG_BG_STARS1);
    unsigned int n_stars;
    Galaxy *gal = exostate.get_galaxy();
    const Star *stars = gal->get_stars(n_stars);
    for (unsigned int i = 0; i < n_stars; ++i) {
        const Star *s = &stars[i];
        const char* spr = STAR_SPRITES[s->get_size()];
        int x = PAD_X + SEP_X * s->x;
        int y = PAD_Y + SEP_Y * s->y;
        draw_manager.draw(tgt, spr, {x, y, 0.5, 0.5, 1, 1});
    }
}
