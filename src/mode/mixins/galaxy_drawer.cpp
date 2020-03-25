#include "galaxy_drawer.h"

#include "../../galaxy/star.h"

#include "../../draw/draw.h"

#include "../../assetpaths.h"

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

GalaxyDrawer::GalaxyDrawer() {
}

void GalaxyDrawer::draw_galaxy(bool pixelswap) {
    DrawTarget tgt = pixelswap ? TGT_Secondary : TGT_Primary;

    draw_manager.draw(tgt, IMG_BG_STARS1);
    unsigned int n_stars;
    Galaxy *gal = exostate.get_galaxy();
    const Star *stars = gal->get_stars(n_stars);
    for (unsigned int i = 0; i < n_stars; ++i) {
        draw_manager.draw(tgt, IMG_TS1_SUN4);
    }
}
