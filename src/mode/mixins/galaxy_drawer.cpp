#include "galaxy_drawer.h"

#include "galaxy/galaxy.h"
#include "galaxy/star.h"

#include "draw/draw.h"

#include "assetpaths.h"

#include "util/iter.h"

#include "shared.h"

#define PAD_X 30
#define PAD_Y 30
#define DRAW_W RES_X
#define DRAW_H 400

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
    guild_id = ID_NONE;

    for (int i = 0; i < GALAXY_MAX_STARS; ++i) {
        star_ids[i] = ID_NONE;
    }

    for (int i = 0; i < N_MARKERS; ++i) {
        marker_ids[i] = ID_NONE;
        marker_text_ids[i] = ID_NONE;
    }
}

FlyTarget* GalaxyDrawer::get_clicked_flytarget() {
    SpriteClick click;
    Galaxy *gal = exostate().get_galaxy();

    for (int i = 0; i < GALAXY_MAX_STARS; ++i) {
        if (!star_ids[i])
            break;
        click = draw_manager.query_click(star_ids[i]);
        if (click.id) {
            return &(gal->get_stars()[i]);
        }
    }

    click = draw_manager.query_click(guild_id);
    if (click.id) {
        return gal->get_guild();
    }

    return nullptr;
}

FlyTarget* GalaxyDrawer::get_mouseover_flytarget() {
    SpriteClick click;
    Galaxy *gal = exostate().get_galaxy();

    for (int i = 0; i < GALAXY_MAX_STARS; ++i) {
        if (!star_ids[i])
            break;
        click = draw_manager.query_mouseover(star_ids[i]);
        if (click.id) {
            return &(gal->get_stars()[i]);
        }
    }

    click = draw_manager.query_click(guild_id);
    if (click.id) {
        return gal->get_guild();
    }

    return nullptr;
}

void GalaxyDrawer::get_draw_position(const FlyTarget* ft, int& draw_x, int& draw_y) {
    draw_x = PAD_X + SEP_X * ft->x;
    draw_y = PAD_Y + SEP_Y * ft->y;
}

void GalaxyDrawer::draw_galaxy(bool pixelswap) {
    int x, y;
    const char *spr;

    DrawTarget tgt = pixelswap ? TGT_Secondary : TGT_Primary;

    draw_manager.clear_sprite_ids();
    draw_manager.draw(tgt, BG_SPR);
    for (StarIterator siter; !siter.complete(); ++siter) {
        Star *s = siter.get();
        spr = STAR_SPRITES[s->get_size()];
        get_draw_position(s, x, y);
        star_ids[siter.get_idx()] = draw_manager.new_sprite_id();
        draw_manager.draw(tgt, star_ids[siter.get_idx()], spr, {x, y, 0.5, 0.5, 1, 1});

#if FEATURE_GALAXY_MAP_PLANET_MARKERS
        if (!exostate().multiplayer()) {
            int drawn = 0;
            for (int i = 0; i < STAR_MAX_PLANETS; ++i) {
                Planet *p = s->get_planet(i);
                if (p && p->exists() && p->is_owned()) {
                    Player *o = exostate().get_player(p->get_owner());
                    if (o && o->is_human()) {
                        draw_manager.draw(
                            tgt,
                            p->sprites()->marker,
                            {x+10+6*drawn, y-10,
                             0.5, 0.5, 1, 1});
                        ++drawn;
                    }
                }
            }
        }
#endif
    }

    Galaxy *gal = exostate().get_galaxy();
    Guild *guild = gal->get_guild();
    spr = GUILD_SPRITE;
    get_draw_position(guild, x, y);
    guild_id = draw_manager.new_sprite_id();
    draw_manager.draw(tgt, guild_id, spr, {x, y, 0.5, 0.5, 1, 1});

    //draw_manager.fill({0, PAD_Y + DRAW_H, RES_X, RES_Y - PAD_Y - DRAW_H}, {0, 0, 0});

    for (int i = 0; i < N_MARKERS; ++i) {
        marker_ids[i] = draw_manager.new_sprite_id();
    }

    for (int i = 0; i < N_MARKERS; ++i) {
        marker_text_ids[i] = draw_manager.new_sprite_id();
    }

    draw_markers(pixelswap, false);
}

void GalaxyDrawer::draw_markers(bool pixelswap, bool names_only) {
    int x, y;
    Player *p = exostate().get_active_player();

    DrawTarget tgt = pixelswap ? TGT_Secondary : TGT_Primary;

    int n_stars;
    Galaxy *gal = exostate().get_galaxy();
    const Star *stars = gal->get_stars(n_stars);

    if (!names_only) {
        for (int i = 0; i < N_MARKERS; ++i) {
            const StarMarker* marker = p->get_marker(i);
            if (marker->valid()) {
                const Star* s = &stars[marker->idx];
                get_draw_position(s, x, y);
                draw_manager.draw(tgt, marker_ids[i], IMG_TS1_MK2, {x, y, 0.5, 0.5, 1, 1});
            }
        }
    }

    for (int i = 0; i < N_MARKERS; ++i) {
        const StarMarker* marker = p->get_marker(i);
        if (marker->valid()) {
            const Star* s = &stars[marker->idx];
            get_draw_position(s, x, y);
            draw_manager.draw_text(
                tgt,
                marker_text_ids[i],
                Font::Tiny,
                marker->tag,
                Justify::Centre,
                x, y+14,
                {0x33, 0x77, 0xFF});
        }
    }
}
