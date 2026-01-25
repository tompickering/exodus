#ifndef GUARD_DRAW_SDL_CPP
#define GUARD_DRAW_SDL_CPP

#include "draw.SDL.h"

#include <cstring>
#include <cstdlib>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "shared.h"
#include "assetpaths.h"

#include "util/str.h"
#include "util/value.h"

#define PROPORTIONAL_FADE 0

#ifdef CONTINUOUS_UPSCALING
const float UPSCALE_X = (float)SCREEN_WIDTH  / (float)RES_X;
const float UPSCALE_Y = (float)SCREEN_HEIGHT / (float)RES_Y;
#else
const float UPSCALE_X = 1;
const float UPSCALE_Y = 1;
#endif

DrawManagerSDL::DrawManagerSDL() {
    cursor_area = {0, 0, 0, 0};
    cursor_underlay = nullptr;
    hardware_rendering = false;
}

bool DrawManagerSDL::init(const DrawManagerOptions& options) {
    L.info("DrawManager Init...");

    fullscreen = options.fullscreen;
    hardware_rendering = options.use_hardware_rendering;

    renderer = nullptr;
    texture = nullptr;

    win = SDL_CreateWindow(PROG_NAME,
                           SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED,
                           SCREEN_WIDTH,
                           SCREEN_HEIGHT,
                           SDL_WINDOW_SHOWN | (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0));

    if (hardware_rendering) {
        renderer = SDL_CreateRenderer(
                       win, -1,
                       SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        SDL_RenderSetLogicalSize(renderer, RES_X, RES_Y);

        texture = SDL_CreateTexture(
                      renderer,
                      SDL_PIXELFORMAT_ARGB8888,
                      SDL_TEXTUREACCESS_STREAMING,
                      RES_X, RES_Y);
    }

    if (!win) {
        L.error("Could not create SDL window");
        return false;
    }

    SDL_GetWindowSize(win, &win_w, &win_h);

    int cx = win_w >> 1;
    int cy = win_h >> 1;

    float scale_x = (float)win_w / SCREEN_WIDTH;
    float scale_y = (float)win_h / SCREEN_HEIGHT;
    float scale = (scale_x < scale_y) ? scale_x : scale_y;

    int fullscreen_w = (int)((float)SCREEN_WIDTH * scale);
    int fullscreen_h = (int)((float)SCREEN_HEIGHT * scale);
    fullscreen_area = {cx - fullscreen_w/2, cy - fullscreen_h/2,
                       fullscreen_w, fullscreen_h};

    // Draw operations are to 'surf'.
    // If we're upscaling each operation, 'surf' *is* the window surface.
    // If we're upscaling at the end, 'surf' is a RES_X x RES_Y intermediary.
#ifdef CONTINUOUS_UPSCALING
    surf = SDL_GetWindowSurface(win);

    if (!surf) {
        L.error("Could not create SDL window surface");
        return false;
    }
#else
    win_surf = SDL_GetWindowSurface(win);

    if (!win_surf) {
        L.error("Could not create SDL window surface");
        return false;
    }

    surf = SDL_CreateRGBSurface(0, RES_X, RES_Y, 32, 0, 0, 0, 0);

    if (!surf) {
        L.error("Could not create SDL render surface");
        return false;
    }
#endif

    SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, 0x0, 0x0, 0x0));

    background = SDL_CreateRGBSurface(0, surf->w, surf->h, 32, 0, 0, 0, 0);
    if (!background) {
        L.error("Could not create background surface");
        return false;
    }

    src_surf_0 = SDL_CreateRGBSurface(0, surf->w, surf->h, 32, 0, 0, 0, 0);
    if (!src_surf_0) {
        L.error("Could not create pixelswap source surface");
        return false;
    }

    src_surf_1 = SDL_CreateRGBSurface(0, surf->w, surf->h, 32, 0, 0, 0, 0);
    if (!src_surf_1) {
        L.error("Could not create fade target surface");
        return false;
    }

    pattern = SDL_CreateRGBSurface(0, surf->w, surf->h, 32, 0, 0, 0, 0);
    if (!pattern) {
        L.error("Could not create pattern surface");
        return false;
    }

    SDL_Surface *cursor = load_normalised_image(CURSOR_IMG);
    cursor_area.w = cursor->w * UPSCALE_X;
    cursor_area.h = cursor->h * UPSCALE_Y;
    sprite_data[CURSOR_IMG] = cursor;
    cursor_underlay = SDL_CreateRGBSurface(0, cursor_area.w, cursor_area.h, 32, 0, 0, 0, 0);
    if (!cursor_underlay) {
        L.error("Could not create cursor underlay surface");
        return false;
    }

    SDL_ShowCursor(false);

    if (fullscreen) {
        SDL_SetWindowGrab(win, SDL_TRUE);
    }

    return true;
}

SDL_Surface* DrawManagerSDL::load_normalised_image(const char* path) {
    char img_path[ASSET_PATH_LEN_MAX+1];
    SDL_Surface *load_surf;
    SDL_Surface *normalised;
    strncpy(img_path, path, ASSET_PATH_LEN_MAX);
    strcat(img_path, ".png");
    load_surf = IMG_Load(img_path);
    // Ensure image is normalised to RGBA8888
    // This helps to ensure that we don't try to use
    // unsupported operations like scaled blits between
    // certain format combinations, which will fail.
    normalised = SDL_ConvertSurfaceFormat(load_surf, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(load_surf);
    if (normalised) {
        L.verb("Loaded %s", img_path);
    } else {
        L.warn("Could not load %s", img_path);
    }
    return normalised;
}

void DrawManagerSDL::draw_init_image() {
    sprite_data[INIT_IMG] = load_normalised_image(INIT_IMG);
    clear();
    draw(INIT_IMG, {RES_X/2, RES_Y/2, 0.5, 0.5, 1, 1});
#ifndef CONTINUOUS_UPSCALING
    blit_surf_to_window();
#endif
    if (!hardware_rendering) {
        SDL_UpdateWindowSurface(win);
    }
}

void DrawManagerSDL::load_resources() {
    font_data[Font::Default] = (void*)TTF_OpenFont(FONT_AUDIOWIDE, 16 * UPSCALE_X);
    font_data[Font::_3x5] = (void*)TTF_OpenFont(FONT_3X5, 10 * UPSCALE_X);
    font_data[Font::Cybattler] = (void*)TTF_OpenFont(FONT_CYBATTLER, 10 * UPSCALE_X);
    font_data[Font::PixelForce] = (void*)TTF_OpenFont(FONT_PIXELFORCE, 10 * UPSCALE_X);
    font_data[Font::RussoOne] = (void*)TTF_OpenFont(FONT_RUSSO_ONE, 11 * UPSCALE_X);
    font_data[Font::Tiny] = (void*)TTF_OpenFont(FONT_AUDIOWIDE, 10 * UPSCALE_X);
    font_data[Font::Small] = (void*)TTF_OpenFont(FONT_AUDIOWIDE, 12 * UPSCALE_X);
    font_data[Font::Large] = (void*)TTF_OpenFont(FONT_AUDIOWIDE, 28 * UPSCALE_X);

    for (unsigned int i = 0;; ++i) {
        if (ASSETS_IMG[i][0] == '\0')
            break;
        sprite_data[ASSETS_IMG[i]] = load_normalised_image(ASSETS_IMG[i]);
    }

    for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 4; ++i) {
            DrawArea area = {i*172, j*172, 172, 172};
            draw(DrawTarget::TGT_Pattern, IMG_PT1_PATTERN, &area, nullptr);
        }
    }
}

void DrawManagerSDL::update(float delta, MousePos mouse_pos, MousePos new_click_pos, MousePos new_click_pos_r, bool click_held) {
    SDL_Rect ca;
    SDL_Rect ca0;

    DrawManager::update(delta, mouse_pos, new_click_pos, new_click_pos_r, click_held);
    if (pixelswap_active()) {
        pixelswap_update();
    }
    if (fade_active()) {
        float fade_progress = fade_time / (float) fade_seconds;
        if (fade_progress < 1) {
            int this_fade_stage = (int)(fade_progress * (float)fade_stages);
            if (this_fade_stage != fade_stage) {
                // Intial values
                uint32_t *px0 = ((uint32_t*)(src_surf_1->pixels));
                // Target values
                uint32_t *px1 = ((uint32_t*)(src_surf_0->pixels));
                fade_stage = this_fade_stage;
                for (int i = 0; i < surf->w * surf->h; ++i) {
                    uint32_t new_col = 0;
                    uint8_t r0 = (px0[i] & 0x00FF0000) >> 16;
                    uint8_t g0 = (px0[i] & 0x0000FF00) >> 8;
                    uint8_t b0 = (px0[i] & 0x000000FF);
                    uint8_t r1 = (px1[i] & 0x00FF0000) >> 16;
                    uint8_t g1 = (px1[i] & 0x0000FF00) >> 8;
                    uint8_t b1 = (px1[i] & 0x000000FF);
#if PROPORTIONAL_FADE
                    uint8_t new_r = (uint8_t)((int16_t)r0 + (int16_t)(fade_progress * ((float)r1 - (float)r0)));
                    uint8_t new_g = (uint8_t)((int16_t)g0 + (int16_t)(fade_progress * ((float)g1 - (float)g0)));
                    uint8_t new_b = (uint8_t)((int16_t)b0 + (int16_t)(fade_progress * ((float)b1 - (float)b0)));
#else
                    uint8_t step = fade_progress * 0xFF;
                    float rs = r1 > r0 ? 1 : -1;
                    float gs = g1 > g0 ? 1 : -1;
                    float bs = b1 > b0 ? 1 : -1;
                    uint8_t new_r;
                    uint8_t new_g;
                    uint8_t new_b;
                    if (abs((int)r0-(int)r1) < step) {
                        new_r = r1;
                    } else {
                        new_r = r0 + step*rs;
                    }
                    if (abs((int)g0-(int)g1) < step) {
                        new_g = g1;
                    } else {
                        new_g = g0 + step*gs;
                    }
                    if (abs((int)b0-(int)b1) < step) {
                        new_b = b1;
                    } else {
                        new_b = b0 + step*bs;
                    }
#endif
                    new_col |= new_r;
                    new_col <<= 8;
                    new_col |= new_g;
                    new_col <<= 8;
                    new_col |= new_b;
                    ((uint32_t*)(surf->pixels))[i] = new_col;
                }
            }
        } else {
            fade_stages = 0;
            SDL_BlitSurface(src_surf_0, nullptr, surf, nullptr);
        }
    }

    draw_flag_vfx();
    draw_button_vfx();

    bool render_cursor = draw_cursor;

    if (!fullscreen) {
        render_cursor = render_cursor && mouse_pos.x > 0 && mouse_pos.y > 0;
    }

    if (render_cursor) {
        // To ensure that cursor drawing plays nicely with anything
        // else that might be animating on the main game screen,
        // we draw it at the very last minute, saving whatever's
        // underneath and restoring it immediately after displaying
        // the surface on the screen.
        //
        // In this way, the cursor sprite is only ever present on
        // the primary surface for the brief period in which we
        // update the window.

        if (!click_held) {
            cursor_area.x = mouse_pos.x - cursor_area.w / 2;
            cursor_area.y = mouse_pos.y - cursor_area.h / 2;
        }

        if (fullscreen) {
            cursor_area.x = clamp(cursor_area.x, 0, RES_X-cursor_area.w);
            cursor_area.y = clamp(cursor_area.y, 0, RES_Y-cursor_area.h);
        }

        ca = cursor_area;
        ca0 = cursor_area; ca0.x = 0; ca0.y = 0;
        SDL_BlitSurface(surf, &ca, cursor_underlay, &ca0);

        ca = cursor_area;
        ca0 = cursor_area; ca0.x = 0; ca0.y = 0;
        SDL_BlitScaled(
            (SDL_Surface*)sprite_data[CURSOR_IMG],
            nullptr, surf, &ca);
    }

    // If we're not upscaling continuously, we upscale here, at the
    // very end of the draw pipeline.
#ifndef CONTINUOUS_UPSCALING
    blit_surf_to_window();
#endif

    if (!hardware_rendering) {
        SDL_UpdateWindowSurface(win);
    }

    if (render_cursor) {
        // Repair the surface where we rendered the cursor.
        ca = cursor_area;
        ca0 = cursor_area; ca0.x = 0; ca0.y = 0;
        SDL_BlitSurface(cursor_underlay, &ca0, surf, &ca);
    }
}

void DrawManagerSDL::pixelswap_update() {
    if (pixelswap_time > PIXELSWAP_STEP_TIME) {
        if (!pixelswap_stage || !--pixelswap_stage) {
            return;
        }

        const int PX_W = (int)(.5f + 2.f * UPSCALE_X);
        const int PX_H = (int)(.5f + 2.f * UPSCALE_Y);
        for (int y = 0; y < pixelswap_region.h / PX_H; ++y) {
            for (int x = 0; x < pixelswap_region.w / PX_W; ++x) {
                bool swap = (rand() % pixelswap_stage) == 0;
                if (swap) {
                    SDL_Rect r = {pixelswap_region.x + (x * PX_W),
                                  pixelswap_region.y + (y * PX_H), PX_W, PX_H};
                    SDL_BlitSurface(src_surf_0, &r, surf, &r);
                }
            }
        }

        pixelswap_time = 0;
    }

}

void DrawManagerSDL::clear() {
    clear(DrawTarget::TGT_Primary);
}

void DrawManagerSDL::clear(DrawTarget tgt) {
    DrawManager::clear(tgt);
    SDL_Surface *tgt_surf = get_target(tgt);
    SDL_FillRect(tgt_surf, nullptr, SDL_MapRGB(tgt_surf->format, 0x0, 0x0, 0x0));
}

void DrawManagerSDL::save_background() {
    L.verb("Saving background");
    if (SDL_BlitSurface(surf, nullptr, background, nullptr)) {
        L.warn("Background blit failed");
    }
}

void DrawManagerSDL::save_background(DrawArea area) {
    L.verb("Saving background at %d %d (%dx%d)", area.x, area.y, area.w, area.h);

    area.x *= UPSCALE_X;
    area.y *= UPSCALE_Y;
    area.w *= UPSCALE_X;
    area.h *= UPSCALE_Y;

    SDL_Rect rect = {area.x, area.y, area.w, area.h};

    if (SDL_BlitSurface(surf, &rect, background, &rect)) {
        L.warn("Background area blit failed");
    }
}

void DrawManagerSDL::restore_background(DrawArea area) {
    area.x *= UPSCALE_X;
    area.y *= UPSCALE_Y;
    area.w *= UPSCALE_X;
    area.h *= UPSCALE_Y;

    SDL_Rect rect = {area.x, area.y, area.w, area.h};

    if (SDL_BlitSurface(background, &rect, surf, &rect)) {
        L.warn("Background area blit failed");
    }
}

void* DrawManagerSDL::get_sprite_data(const char* img_path) {
    if (strnlen(img_path, 1)) {
        if (!sprite_data.count(img_path)) {
            sprite_data[img_path] = (void*) IMG_Load(img_path);
            return sprite_data[img_path];
        }
        return sprite_data[img_path];
    }
    return nullptr;
}

void DrawManagerSDL::draw_line(int x0, int y0, int x1, int y1, RGB rgb) {
    /*
     * As we are drawing with surfaces and blitting rather than a renderer
     * and SDL_RenderCopy, we are not able to take advantage of SDL's
     * geometry rendering, such as SDL_RenderDrawLine.
     *
     * As such, implement Bresenham's line drawing algorithm (with thickness)
     */

    /*
    SDL_RenderDrawLine(renderer, rgb.r, rgb.g, rgb.b, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
    */

    int xdiff = x1 - x0;
    int ydiff = y1 - y0;

    int xstep = xdiff > 0 ? 1 : -1;
    int ystep = ydiff > 0 ? 1 : -1;

    int dx = abs(xdiff);
    int dy = abs(ydiff);

    int xhead = x0;
    int yhead = y0;

    uint32_t col = (rgb.r << 16)
                 | (rgb.g <<  8)
                 | (rgb.b);

    int w = surf->w;
    uint32_t *pixels = ((uint32_t*)(surf->pixels));

    if (dx >= dy) {
        int e = (2*dy) - dx;
        while (xhead != x1) {
            if (e < 0) {
                e += 2*dy;
            } else {
                e += 2*(dy-dx);
                yhead += ystep;
            }
            xhead += xstep;
            pixels[yhead*w + xhead] = col;
        }
    } else {
        int e = (2*dx) - dy;
        while (yhead != y1) {
            if (e < 0) {
                e += 2*dx;
            } else {
                e += 2*(dx-dy);
                xhead += xstep;
            }
            yhead += ystep;
            pixels[yhead*w + xhead] = col;
        }
    }
}

void DrawManagerSDL::draw_line(int x0, int y0, int x1, int y1, int thickness, RGB rgb) {
    float m = (float)(y1-y0)/(float)(x1-x0);

    int wx = (thickness-1)*sqrt(pow(y1-y0, 2)+pow(y1-y0, 2))/(2*fabs(y1-y0));
    int wy = (thickness-1)*sqrt(pow(x1-x0, 2)+pow(y1-y0, 2))/(2*fabs(x1-x0));

    if (m < 1) {
        for (int i = 0; i < wy; ++i) {
            draw_line(x0, y0+i, x1, y1+i, rgb);
            draw_line(x0, y0-i, x1, y1-i, rgb);
        }
    } else {
        for (int i = 0; i < wx; ++i) {
            draw_line(x0+i, y0, x1+i, y1, rgb);
            draw_line(x0-i, y0, x1-i, y1, rgb);
        }
    }
}

void DrawManagerSDL::draw_rect(DrawArea area, float thickness, RGB rgb) {
    int tx = (int)(thickness * UPSCALE_X);
    int ty = (int)(thickness * UPSCALE_Y);
    fill(TGT_Primary, {area.x, area.y, tx, area.h + ty}, rgb);
    fill(TGT_Primary, {area.x + area.w, area.y, tx, area.h + ty}, rgb);
    fill(TGT_Primary, {area.x, area.y, area.w + tx, ty}, rgb);
    fill(TGT_Primary, {area.x, area.y + area.h, area.w + tx, ty}, rgb);
}

void DrawManagerSDL::draw_circle(int cx, int cy, int r, RGB rgb) {
    /*
     * As we are drawing with surfaces and blitting rather than a renderer
     * and SDL_RenderCopy, we are not able to take advantage of SDL's
     * geometry rendering, such as SDL_RenderDrawLine.
     *
     * As such, implement circle drawing
     */

    uint32_t col = (rgb.r << 16)
                 | (rgb.g <<  8)
                 | (rgb.b);

    int w = surf->w;
    int h = surf->h;

    uint32_t *pixels = ((uint32_t*)(surf->pixels));

    int r2 = pow(r, 2);

    for (int y = max(cy-r, 0); y < min(cy+r, h); ++y) {
        int y2 = pow(y-cy, 2);
        int dx2 = r2 - y2;
        int dx = (int)sqrt(dx2);
        int x0 = cx - dx;
        int x1 = cx + dx;

        if (x0 >= 0 && x0 < w) {
            pixels[y*w + x0] = col;
        }

        if (x1 >= 0 && x1 < w) {
            pixels[y*w + x1] = col;
        }
    }

    for (int x = max(cx-r, 0); x < min(cx+r, w); ++x) {
        int x2 = pow(x-cx, 2);
        int dy2 = r2 - x2;
        int dy = (int)sqrt(dy2);
        int y0 = cy - dy;
        int y1 = cy + dy;

        if (y0 >= 0 && y0 < h) {
            pixels[y0*w + x] = col;
        }

        if (y1 >= 0 && y1 < h) {
            pixels[y1*w + x] = col;
        }
    }
}

void DrawManagerSDL::draw(const char* spr_key) {
    draw(DrawTarget::TGT_Primary, spr_key, nullptr, nullptr);
}

void DrawManagerSDL::draw(const char* spr_key, DrawArea area) {
    draw(DrawTarget::TGT_Primary, spr_key, &area, nullptr);
}

void DrawManagerSDL::draw(const char* spr_key, DrawTransform t) {
    draw(DrawTarget::TGT_Primary, spr_key, t, nullptr);
}

void DrawManagerSDL::draw(SprID id, const char* spr_key) {
    draw(DrawTarget::TGT_Primary, spr_key, nullptr, &id);
}

void DrawManagerSDL::draw(SprID id, const char* spr_key, DrawArea area) {
    draw(DrawTarget::TGT_Primary, spr_key, &area, &id);
}

void DrawManagerSDL::draw(SprID id, const char* spr_key, DrawTransform t) {
    draw(DrawTarget::TGT_Primary, spr_key, t, &id);
}

void DrawManagerSDL::draw(DrawTarget tgt, const char* spr_key) {
    draw(tgt, spr_key, nullptr, nullptr);
}

void DrawManagerSDL::draw(DrawTarget tgt, const char* spr_key, DrawArea area) {
    draw(tgt, spr_key, &area, nullptr);
}

void DrawManagerSDL::draw(DrawTarget tgt, const char* spr_key, DrawTransform t) {
    draw(tgt, spr_key, t, nullptr);
}

void DrawManagerSDL::draw(DrawTarget tgt, SprID id, const char* spr_key) {
    draw(tgt, spr_key, nullptr, &id);
}

void DrawManagerSDL::draw(DrawTarget tgt, SprID id, const char* spr_key, DrawTransform t) {
    draw(tgt, spr_key, t, &id);
}

void DrawManagerSDL::repair_dirty_area(SprID id) {
    repair_dirty_area(id, REPAIR_Below, 0);
}

void DrawManagerSDL::repair_dirty_area(SprID id, int mode, int extend) {
    for (std::vector<SprID>::size_type i = 0; i < id_repair_exclusions.size(); ++i) {
        if (id_repair_exclusions[i] == id) {
            return;
        }
    }

    DrawArea *dirty_area = get_drawn_area(id);
    if (dirty_area) {
        // We know where this sprite was drawn previously.

        if (mode & REPAIR_Below) {
            // If we're repairing below, wipe that area with the background.
            SDL_Rect r = {dirty_area->x, dirty_area->y, dirty_area->w, dirty_area->h};
            r.x -= extend; r.w += extend*2;
            r.y -= extend; r.h += extend*2;
            SDL_BlitSurface(background, &r, surf, &r);
        }

        Repair current = REPAIR_Below;
        // Now iterate over all recorded draw information
        for (std::vector<DrawnSprite>::size_type i = 0; i < drawn_spr_info.size(); ++i) {
            DrawnSprite *info = &drawn_spr_info[i];

            if (current == REPAIR_This) {
                current = REPAIR_Above;
            }
            if (info->id == id) {
                current = REPAIR_This;
            }

            if (current == REPAIR_Below && !(mode & REPAIR_Below)) {
                continue;
            }

            if (current == REPAIR_This && !(mode & REPAIR_This)) {
                continue;
            }

            if (current == REPAIR_Above && !(mode & REPAIR_Above)) {
                continue;
            }

            if (info->area.overlaps(*dirty_area)) {
                // Determine the on-screen area which is overlapping
                DrawArea overlap_area;
                overlap_area.x = max(dirty_area->x, info->area.x);
                overlap_area.y = max(dirty_area->y, info->area.y);
                int overlap_area_br_x = min(dirty_area->x + dirty_area->w,
                                            info->area.x + info->area.w);
                int overlap_area_br_y = min(dirty_area->y + dirty_area->h,
                                            info->area.y + info->area.h);
                overlap_area.w = overlap_area_br_x - overlap_area.x;
                overlap_area.h = overlap_area_br_y - overlap_area.y;

                // This needs redrawing
                switch (info->type) {
                    case DRAWTYPE_Sprite:
                        {
                            if (!info->sprite) {
                                L.error("Drawn sprite record has no sprite reference");
                                continue;
                            }

                            // TODO: Deduplicate with draw()
                            // TODO: Only repair the area inside the overlap

                            SDL_Surface *spr = (SDL_Surface*)get_sprite_data(info->sprite);
                            if (!spr) {
                                L.fatal("Unknown sprite during repair: %s", info->sprite);
                                return;
                            }

                            // Determine the area of the source sprite which we need to draw.
                            // During repair, as we're only drawing the overlapping region,
                            // it may be that we only want to draw part of the image even
                            // if there's no registered source region. For clarity, even if
                            // we don't find one, set the source region explicitly based
                            // on the sprite's width and height.
                            DrawArea *src_area = get_source_region(info->id);
                            SDL_Rect entire_src_rect;
                            if (src_area) {
                                entire_src_rect = {src_area->x, src_area->y, src_area->w, src_area->h};
                            } else {
                                /*
                                 * FIXME: This doesn't work for scaled sprites.
                                 * spr->w and spr->h are the size that the sprite
                                 * occupies on screen post-scaling, and are therefore
                                 * not the correct value to select the source region.
                                 */
                                entire_src_rect = {0, 0, spr->w, spr->h};
                            }

                            // entire_src_rect now contains the area of the source sprite
                            // that occupies info->area. We now want to determine the
                            // (non-strict) sub-region which occupies overlapping_area.
                            // We first want to find the proportions of info->area that
                            // occupy overlapping area - and then index into entire_src_rect
                            // with those same proportions - to get the source region that
                            // we need to draw for repair.
                            float prop_x = (float)(overlap_area.x - info->area.x) / info->area.w;
                            float prop_y = (float)(overlap_area.y - info->area.y) / info->area.h;
                            float prop_w = (float)(overlap_area.w) / info->area.w;
                            float prop_h = (float)(overlap_area.h) / info->area.h;

                            SDL_Rect overlap_src_rect = {entire_src_rect.x + (int)(.5f + prop_x * entire_src_rect.w),
                                                         entire_src_rect.y + (int)(.5f + prop_y * entire_src_rect.h),
                                                         (int)(.5f + entire_src_rect.w * prop_w),
                                                         (int)(.5f + entire_src_rect.h * prop_h)};

                            SDL_Rect dst_rect = {overlap_area.x,
                                                 overlap_area.y,
                                                 overlap_area.w,
                                                 overlap_area.h};

                            /*
                             * FIXME: Note that this may not work perfectly when
                             * repairing scaled sprites, and is liable to result
                             * in a 'jitter' distortion. This happens because it's
                             * possible that there is no integer-specifiable region
                             * in the source sprite that maps to the overlap region
                             * being repaired.
                             *
                             * One solution may be to extend the repair region to consider
                             * all sprites occupying the space, however it may not be
                             * possible to find a suitable region to satisfy them all,
                             * depending on the scales.
                             *
                             * Being able to blit with a fractional position and dimensions
                             * would work, thereby determining the output pixel from
                             * exactly the same blitter state as if it had been drawing
                             * the entire sprite, however I am not aware of an interface
                             * for this in SDL.
                             *
                             * Note that as I proof-of-concept, I was able to demonstrate
                             * the effect disappeared completely when setting the overlap
                             * region to the full screen:
                             *
                             * overlap_area = {0, 0, RES_X, RES_Y};
                             *
                             * This of course defeats the object of efficient blitting,
                             * however extending the region out to incorporate all scaled
                             * sprites may be a viable workaround for some use cases.
                             *
                             */
                            if (SDL_BlitScaled(spr, &overlap_src_rect, surf, &dst_rect)) {
                                L.debug("Repair blit unsuccessful: %s", info->sprite);
                            }
                        }
                        break;
                    case DRAWTYPE_Fill:
                        fill(overlap_area, info->colour);
                        break;
                    case DRAWTYPE_Pattern:
                        fill_pattern(overlap_area);
                        break;
                    default:
                        //L.warn("Can't repair draw of type %d", info->type);
                        break;
                }
            }
        }
    }
}

DrawnSprite* DrawManagerSDL::update_dirty_area(SprID id, DrawArea area) {
    DrawnSprite *dirty_area_info = get_drawn_info(id);
    DrawArea *dirty_area = dirty_area_info ? &(dirty_area_info->area) : nullptr;
    if (dirty_area) {
        // Update the old area to be redrawn with this draw's area
        *dirty_area = area;
    } else {
        // We don't have an existing record for this draw - create a new one
        DrawnSprite new_drawn_area_info;
        new_drawn_area_info.id = id;
        new_drawn_area_info.area = area;
        new_drawn_area_info.type = DRAWTYPE_Unknown;
        new_drawn_area_info.sprite = nullptr;
        new_drawn_area_info.colour = {0, 0, 0};
        drawn_spr_info.push_back(new_drawn_area_info);
        dirty_area_info = &drawn_spr_info.back();
    }
    return dirty_area_info;
}

void DrawManagerSDL::draw(DrawTarget tgt, const char* spr_key, DrawTransform t, SprID* id) {
    if (!spr_key) {
        // We want to remove a sprite from the screen, if drawn
        draw(tgt, spr_key, nullptr, id);
        return;
    }

    SDL_Surface *spr = (SDL_Surface*)get_sprite_data(spr_key);
    if (!spr) {
        L.warn("Unknown sprite: %s", spr_key);
        return;
    }

    int spr_w = spr->w;
    int spr_h = spr->h;

    DrawArea *src_area = nullptr;
    if (id) {
        src_area = get_source_region(*id);
        if (src_area) {
            spr_w = src_area->w;
            spr_h = src_area->h;
        }
    }

    DrawArea area;
    area.x = t.x - (t.anchor_x * spr_w * t.scale_x);
    area.y = t.y - (t.anchor_y * spr_h * t.scale_y);
    area.w = spr_w * t.scale_x;
    area.h = spr_h * t.scale_y;

    draw(tgt, spr_key, &area, id);
}

void DrawManagerSDL::draw(DrawTarget tgt, const char* spr_key, DrawArea* area, SprID* id) {
#ifdef CONTINUOUS_UPSCALING
    DrawArea default_area = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
#else
    DrawArea default_area = {0, 0, RES_X, RES_Y};
#endif

    SDL_Surface *tgt_surf = get_target(tgt);

    if (area) {
        area->x *= UPSCALE_X;
        area->y *= UPSCALE_Y;
        area->w *= UPSCALE_X;
        area->h *= UPSCALE_Y;
    } else {
        area = &default_area;
    }

    if (id) {
        if (tgt_surf == surf) {
            repair_dirty_area(*id);
        }
        if (spr_key) {
            DrawnSprite *drawn_info = update_dirty_area(*id, *area);
            if (!drawn_info) {
                L.fatal("Could not attain draw info for %s", spr_key);
            }
            drawn_info->type = DRAWTYPE_Sprite;
            drawn_info->sprite = spr_key;
        } else {
            release_sprite_id(*id);
        }
    }

    if (!spr_key)
        return;

    if (id && flag_vfx_ids.count(*id)) {
        /*
         * This is a special case - we are going to create some VFX
         * based on this, and we don't actually want to draw it.
         */
        return;
    }

    SDL_Surface *spr = (SDL_Surface*)get_sprite_data(spr_key);
    if (!spr) {
        L.warn("Unknown sprite: %s", spr_key);
        return;
    }

    DrawArea *src_area = nullptr;
    if (id) {
        src_area = get_source_region(*id);
    }

    SDL_Rect src_rect;
    SDL_Rect *src_rect_ptr = nullptr;
    if (src_area) {
        src_rect = {src_area->x, src_area->y, src_area->w, src_area->h};
        src_rect_ptr = &src_rect;
    }

    SDL_Rect dst_rect = {area->x, area->y, area->w, area->h};
    if (SDL_BlitScaled(spr, src_rect_ptr, tgt_surf, &dst_rect)) {
        L.debug("Blit unsuccessful: %s", spr_key);
    }

    // FIXME: If we want to be able to move sprites below other sprites,
    // we need an equivalent of repair_dirty_area() to carry out draws
    // *after* the original SprID here.
}

void DrawManagerSDL::draw_text(const char* text, Justify jst, int x, int y, RGB rgb) {
    draw_text(DrawTarget::TGT_Primary, ID_NONE, Font::Default, text, jst, x, y, &rgb, nullptr);
}

void DrawManagerSDL::draw_text(const char* text, Justify jst, int x, int y, RGB rgb, RGB bg_rgb) {
    draw_text(DrawTarget::TGT_Primary, ID_NONE, Font::Default, text, jst, x, y, &rgb, &bg_rgb);
}

void DrawManagerSDL::draw_text(Font font, const char* text, Justify jst, int x, int y, RGB rgb) {
    draw_text(DrawTarget::TGT_Primary, ID_NONE, font, text, jst, x, y, &rgb, nullptr);
}

void DrawManagerSDL::draw_text(Font font, SprID id, const char* text, Justify jst, int x, int y, RGB rgb) {
    draw_text(DrawTarget::TGT_Primary, id, font, text, jst, x, y, &rgb, nullptr);
}

void DrawManagerSDL::draw_text(Font font, const char* text, Justify jst, int x, int y, RGB rgb, RGB bg_rgb) {
    draw_text(DrawTarget::TGT_Primary, ID_NONE, font, text, jst, x, y, &rgb, &bg_rgb);
}

void DrawManagerSDL::draw_text(SprID id, const char* text, Justify jst, int x, int y, RGB rgb) {
    draw_text(DrawTarget::TGT_Primary, id, Font::Default, text, jst, x, y, &rgb, nullptr);
}

void DrawManagerSDL::draw_text(SprID id, Font font, const char* text, Justify jst, int x, int y, RGB rgb) {
    draw_text(DrawTarget::TGT_Primary, id, font, text, jst, x, y, &rgb, nullptr);
}

void DrawManagerSDL::draw_text(DrawTarget tgt, const char* text, Justify jst, int x, int y, RGB rgb) {
    draw_text(tgt, ID_NONE, Font::Default, text, jst, x, y, &rgb, nullptr);
}

void DrawManagerSDL::draw_text(DrawTarget tgt, const char* text, Justify jst, int x, int y, RGB rgb, RGB bg_rgb) {
    draw_text(tgt, ID_NONE, Font::Default, text, jst, x, y, &rgb, &bg_rgb);
}

void DrawManagerSDL::draw_text(DrawTarget tgt, Font font, const char* text, Justify jst, int x, int y, RGB rgb) {
    draw_text(tgt, ID_NONE, font, text, jst, x, y, &rgb, nullptr);
}

void DrawManagerSDL::draw_text(DrawTarget tgt, Font font, const char* text, Justify jst, int x, int y, RGB rgb, RGB bg_rgb) {
    draw_text(tgt, ID_NONE, font, text, jst, x, y, &rgb, &bg_rgb);
}

void DrawManagerSDL::draw_text(DrawTarget tgt, SprID id, const char* text, Justify jst, int x, int y, RGB rgb) {
    draw_text(tgt, id, Font::Default, text, jst, x, y, &rgb, nullptr);
}

void DrawManagerSDL::draw_text(DrawTarget tgt, SprID id, Font font, const char* text, Justify jst, int x, int y, RGB rgb) {
    draw_text(tgt, id, font, text, jst, x, y, &rgb, nullptr);
}

void DrawManagerSDL::draw_text(DrawTarget tgt, SprID id, Font font, const char* _text, Justify jst, int x, int y, RGB* rgb, RGB* bg_rgb) {
    RGB adjustable_col = *rgb;
    adjust_selectable_text_col(id, adjustable_col);

    SDL_Color colour = {adjustable_col.r, adjustable_col.g, adjustable_col.b};
    SDL_Surface *msg_surf;
    SDL_Surface *tgt_surf = get_target(tgt);

    const char* text = _text;

    if (font == Font::_3x5 || font == Font::RussoOne) {
        text = tmp_caps(_text);
    }

    if (bg_rgb) {
        SDL_Color bg_colour = {bg_rgb->r, bg_rgb->g, bg_rgb->b};
        msg_surf = TTF_RenderText_Shaded((TTF_Font*)font_data[font], text, colour, bg_colour);
    } else {
        msg_surf = TTF_RenderText_Blended((TTF_Font*)font_data[font], text, colour);
    }

    SDL_Rect msg_rect;
    msg_rect.x = UPSCALE_X * x;
    msg_rect.y = UPSCALE_Y * y;

    int render_w, render_h;
    TTF_SizeText((TTF_Font*)font_data[font], text, &render_w, &render_h);

    if (jst != Justify::Left) {
        msg_rect.x -= (jst == Justify::Right) ? render_w : render_w / 2;
    }

    if (text_cursor_enabled()) {
        if (id == text_cursor_text_id) {
            bool draw_text_cursor = text_cursor_cycle < (TEXT_CURSOR_PERIOD/2.f);

            int len = strlen(_text);
            int len_changed = len != text_cursor_text_len;
            text_cursor_text_len = len;

            if (len_changed || !draw_text_cursor) {
                draw(text_cursor_id, nullptr);
            }

            if (draw_text_cursor) {
                refresh_sprite_id(text_cursor_id);
                fill(text_cursor_id, {msg_rect.x + render_w + 2, msg_rect.y+2, 2, render_h-4}, COL_TEXT2);
            }
        }
    }

    if (id) {
        DrawArea area;
        area.x = msg_rect.x;
        area.y = msg_rect.y;
        area.w = render_w;
        area.h = render_h;
        if (tgt_surf == surf) {
            repair_dirty_area(id);
        }
        DrawnSprite *drawn_info = update_dirty_area(id, area);
        if (!drawn_info) {
            L.fatal("Could not attain draw info for text %s", text);
        }
        drawn_info->type = DRAWTYPE_Text;
        // FIXME: We don't store the text info here, so we can't use this to repair later
        // It would look something like strncpy(drawn_info->text, MAX, text)
    }

    SDL_BlitSurface(msg_surf, nullptr, tgt_surf, &msg_rect);

    SDL_FreeSurface(msg_surf);

    // FIXME: If we want to be able to move sprites below other sprites,
    // we need an equivalent of repair_dirty_area() to carry out draws
    // *after* the original SprID here.
}

void DrawManagerSDL::pixelswap_start() {
    pixelswap_start(nullptr);
}

void DrawManagerSDL::pixelswap_start(const DrawArea* area) {
    // Cancel fade, if there's on in progress
    fade_stages = 0;

    pixelswap_stage = 20;
    if (area) {
        pixelswap_region = *area;
        pixelswap_region.x *= UPSCALE_X;
        pixelswap_region.y *= UPSCALE_Y;
        pixelswap_region.w *= UPSCALE_X;
        pixelswap_region.h *= UPSCALE_Y;
    } else {
        pixelswap_region = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    }
    pixelswap_time = 0;
}

bool DrawManagerSDL::pixelswap_active() {
    return pixelswap_stage > 0;
}

void DrawManagerSDL::fill(DrawArea area, RGB col) {
    fill(TGT_Primary, area, col);
}

void DrawManagerSDL::fill(SprID id, DrawArea area, RGB col) {
    fill(TGT_Primary, id, area, col);
}

void DrawManagerSDL::fill(DrawTarget tgt, SprID id, DrawArea area, RGB col) {
    DrawArea transformed_area = {
        (int)((float)area.x * UPSCALE_X),
        (int)((float)area.y * UPSCALE_Y),
        (int)((float)area.w * UPSCALE_X),
        (int)((float)area.h * UPSCALE_Y)};

    if (tgt == TGT_Primary) {
        repair_dirty_area(id);
    }

    DrawnSprite *drawn_info = update_dirty_area(id, transformed_area);
    if (!drawn_info) {
        L.fatal("Could not attain draw info for fill");
    }
    drawn_info->type = DRAWTYPE_Fill;
    drawn_info->colour = col;
    fill(tgt, area, col);

    // FIXME: If we want to be able to move sprites below other sprites,
    // we need an equivalent of repair_dirty_area() to carry out draws
    // *after* the original SprID here.
}

void DrawManagerSDL::fill(DrawTarget tgt, DrawArea area, RGB col) {
    fill(FILL_None, tgt, area, col);
}

void DrawManagerSDL::fill(FillEffect effect, DrawArea area, RGB col) {
    fill(effect, TGT_Primary, area, col);
}

void DrawManagerSDL::fill(FillEffect effect, DrawTarget tgt, DrawArea area, RGB col) {
    SDL_Rect R;
    SDL_Surface *tgt_surf = get_target(tgt);
    R.x = area.x * UPSCALE_X;
    R.y = area.y * UPSCALE_Y;
    R.w = area.w * UPSCALE_X;
    R.h = area.h * UPSCALE_Y;

    if (effect != FILL_3D_Out_Hollow && effect != FILL_3D_In_Hollow) {
        SDL_FillRect(tgt_surf, &R, SDL_MapRGB(surf->format, col.r, col.g, col.b));
    }

    if (effect == FILL_3D_Out_Hollow) effect = FILL_3D_Out;
    if (effect == FILL_3D_In_Hollow) effect = FILL_3D_In;

    if (effect == FILL_3D_Out || effect == FILL_3D_In) {
        int effect_mag = (effect == FILL_3D_Out) ? 1 : -1;
        int effect_mag_positive = (effect_mag >= 0) ? effect_mag : -effect_mag;
        int m0 = 2 * effect_mag_positive;
        int m1 = 3 * m0;

        SDL_Rect t, l, b, r, tl, tr, bl, br, tl_h3, tl_v3, br_h3, br_v3;

        t.x = R.x; t.y = R.y; t.w = R.w; t.h = m0;
        l.x = R.x; l.y = R.y; l.w = m0; l.h = R.h;
        b.x = R.x; b.y = R.y+R.h-m0; b.w = R.w; b.h = m0;
        r.x = R.x+R.w-m0; r.y = R.y; r.w = m0; r.h = R.h;

        tl.x = R.x; tl.y = R.y; tl.w = m0; tl.h = m0;
        tr.x = R.x+R.w-m0; tr.y = R.y; tr.w = m0; tr.h = m0;
        bl.x = R.x; bl.y = R.y+R.h-m0; bl.w = m0; bl.h = m0;
        br.x = R.x+R.w-m0; br.y = R.y+R.h-m0; br.w = m0; br.h = m0;

        tl_h3 = t; tl_h3.w = m1;
        tl_v3 = l; tl_v3.h = m1;
        br_h3 = br; br_h3.x -= (m1-m0); br_h3.w = m1; //JK: adjusted
        br_v3 = br; br_v3.y -= (m1-m0); br_v3.h = m1; //JK: adjusted

        // FIXME: Derive these from col
        RGB l0 = {255, 255, 255};
        RGB l1 = {204, 204, 204};
        RGB l2 = {187, 187, 187};
        RGB md = {153, 153, 153};
        RGB d0 = {136, 136, 130};

        if (effect_mag > 0) {
            SDL_FillRect(tgt_surf, &t, SDL_MapRGB(surf->format, l2.r, l2.g, l2.b));
            SDL_FillRect(tgt_surf, &l, SDL_MapRGB(surf->format, l2.r, l2.g, l2.b));
            SDL_FillRect(tgt_surf, &b, SDL_MapRGB(surf->format, d0.r, d0.g, d0.b));
            SDL_FillRect(tgt_surf, &r, SDL_MapRGB(surf->format, d0.r, d0.g, d0.b));

            SDL_FillRect(tgt_surf, &tl_h3, SDL_MapRGB(surf->format, l1.r, l1.g, l1.b));
            SDL_FillRect(tgt_surf, &tl_v3, SDL_MapRGB(surf->format, l1.r, l1.g, l1.b));
            SDL_FillRect(tgt_surf, &tl, SDL_MapRGB(surf->format, l0.r, l0.g, l0.b));

            SDL_FillRect(tgt_surf, &tr, SDL_MapRGB(surf->format, md.r, md.g, md.b));
            SDL_FillRect(tgt_surf, &bl, SDL_MapRGB(surf->format, md.r, md.g, md.b));
        } else {
            SDL_FillRect(tgt_surf, &t, SDL_MapRGB(surf->format, d0.r, d0.g, d0.b));
            SDL_FillRect(tgt_surf, &l, SDL_MapRGB(surf->format, d0.r, d0.g, d0.b));
            SDL_FillRect(tgt_surf, &b, SDL_MapRGB(surf->format, l2.r, l2.g, l2.b));
            SDL_FillRect(tgt_surf, &r, SDL_MapRGB(surf->format, l2.r, l2.g, l2.b));

            SDL_FillRect(tgt_surf, &br_h3, SDL_MapRGB(surf->format, l1.r, l1.g, l1.b));
            SDL_FillRect(tgt_surf, &br_v3, SDL_MapRGB(surf->format, l1.r, l1.g, l1.b));
            SDL_FillRect(tgt_surf, &br, SDL_MapRGB(surf->format, l0.r, l0.g, l0.b));

            SDL_FillRect(tgt_surf, &tr, SDL_MapRGB(surf->format, md.r, md.g, md.b));
            SDL_FillRect(tgt_surf, &bl, SDL_MapRGB(surf->format, md.r, md.g, md.b));
        }
    }
}

void DrawManagerSDL::fill_pattern(DrawArea area) {
    fill_pattern(TGT_Primary, area);
}

void DrawManagerSDL::fill_pattern(SprID id, DrawArea area) {
    DrawArea transformed_area = {
        (int)((float)area.x * UPSCALE_X),
        (int)((float)area.y * UPSCALE_Y),
        (int)((float)area.w * UPSCALE_X),
        (int)((float)area.h * UPSCALE_Y)};
    DrawnSprite *drawn_info = update_dirty_area(id, transformed_area);
    if (!drawn_info) {
        L.fatal("Could not attain draw info for pattern fill");
    }
    drawn_info->type = DRAWTYPE_Pattern;
    fill_pattern(TGT_Primary, area);

    // FIXME: If we want to be able to move sprites below other sprites,
    // we need an equivalent of repair_dirty_area() to carry out draws
    // *after* the original SprID here.
}

void DrawManagerSDL::fill_pattern(DrawTarget tgt, DrawArea area) {
    SDL_Surface *tgt_surf = get_target(tgt);
    SDL_Rect r;
    r.x = area.x * UPSCALE_X;
    r.y = area.y * UPSCALE_Y;
    r.w = area.w * UPSCALE_X;
    r.h = area.h * UPSCALE_Y;
    SDL_BlitSurface(pattern, &r, tgt_surf, &r);
}

void DrawManagerSDL::fade_start(float seconds, int stages) {
    // Save source image (current screen contents)
    SDL_BlitSurface(surf, nullptr, src_surf_1, nullptr);
    fade_seconds = seconds;
    fade_stage = 0;
    fade_stages = stages;
    fade_time = 0;
}

void DrawManagerSDL::fade_black(float seconds, int stages) {
    // Fill target image with black
    SDL_FillRect(src_surf_0, nullptr, SDL_MapRGB(surf->format, 0x0, 0x0, 0x0));
    fade_start(seconds, stages);
}

void DrawManagerSDL::fade_white(float seconds, int stages) {
    // Fill target image with white
    SDL_FillRect(src_surf_0, nullptr, SDL_MapRGB(surf->format, 0xFF, 0xFF, 0xFF));
    fade_start(seconds, stages);
}

bool DrawManagerSDL::fade_active() {
    return fade_stages > 0;
}

SDL_Surface* DrawManagerSDL::get_target(DrawTarget tgt) {
    switch (tgt) {
        case DrawTarget::TGT_Primary:
            return surf;
        case DrawTarget::TGT_Secondary:
            return src_surf_0;
        case DrawTarget::TGT_Pattern:
            return pattern;
        default:
            break;
    }

    L.fatal("Invalid draw target: %d", tgt);
    return nullptr;
}

void DrawManagerSDL::get_upscale(float& up_x, float& up_y) {
    up_x = UPSCALE_X; up_y = UPSCALE_Y;
}

void DrawManagerSDL::cancel_transitions() {
    pixelswap_stage = 0;
    fade_stages = 0;
}

int DrawManagerSDL::calc_text_overfill(int width, Font font, const char* text) {
    int extent = 0;
    int can_be_rendered = 0;

    TTF_MeasureText((TTF_Font*)font_data[font], text, width, &extent, &can_be_rendered);

    int chars = strlen(text);

    return max((chars - can_be_rendered), 0);
}

void DrawManagerSDL::draw_flag_vfx() {
    for (auto it = flag_vfx_ids.begin(); it != flag_vfx_ids.end(); ++it) {
        SprID id = it->first;
        DrawnSprite *spr = get_drawn_info(id);
        if (!spr) {
            continue;
        }

        repair_dirty_area(id, REPAIR_Below, 10);

        float interp = fmin(1.0, it->second / FLAGVFX_TIME);
        uint32_t r4_max = (uint8_t)((float)0xFF * interp);

        uint32_t *screen = ((uint32_t*)(surf->pixels));

        SDL_Surface *flag_surf = (SDL_Surface*)get_sprite_data(spr->sprite);
        if (!flag_surf) {
            L.warn("Unknown flag VFX sprite: %s", spr->sprite);
            continue;
        }

        uint32_t *flag_px = ((uint32_t*)(flag_surf->pixels));

        /*
         * This is a hack; a single draw iteration leaves gaps of not-filled-in pixels,
         * so if we repair the full background each frame, then these holes are visible.
         * However, if we don't repair at all, then the edges of the flag contain stale
         * pixel data and leave an 'imprint' where the edge of the flag has since moved
         * away. We avoid this by performing a repair, but rendering the last few prior
         * iterations.
         */
        for (int prior = 4; prior >= 0; --prior) {
            uint32_t r4 = (r4_max + 0x100 - prior) % 0x100;

            uint32_t r7 = 0;

            //L.debug("%d %d", spr->area.w/2, spr->area.h/2);
            // 48, 28

            // Our stored flag sprite is twice the size of the original - correct for this
            for (int r9 = spr->area.h/2; r9 > 0; r9--) {
                uint32_t r6 = 0;
                for (int r8 = spr->area.w/2; r8 > 0; r8--) {
                    uint32_t r11 = r4 + (r8 << 1);
                    r11 += r9 << 2;
                    r11 = r11 & 0xFF;
                    r11 = r11 << 3;

                    /*
                     * Orig's index into flag_motion is bytewise - ours is
                     * int32_t-wise, so we need to divide by 4.
                     */
                    r11 >>= 2;

                    uint32_t r0 = flag_motion[r11];
                    uint32_t r1 = flag_motion[r11+1];

                    int fx = (2*(r8-1));
                    int fy = (2*(r9-1));
                    fy = spr->area.h - fy - 2;
                    uint32_t r2 = flag_px[fy*spr->area.w + fx];

                    // flag_px seems to be RGBA and we want RGB
                    r2 >>= 8;

                    r0 += r6;
                    r1 += r7;

                    ++r6;

                    r0 <<= 1;
                    r1 <<= 1;

                    r0 += r1 * RES_X;

                    r0 += spr->area.x;
                    r0 += spr->area.y * RES_X;

                    int _r0 = r0;//our_y*RES_X + our_x;
                    for (int _j = 0; _j < 2; ++_j) {
                        for (int _i = 0; _i < 2; ++_i) {
                            screen[_r0 + _j*RES_X + _i] = r2;
                        }
                    }

                }
                ++r7;
            }
        }
    }
}

void DrawManagerSDL::draw_button_vfx() {
    if (active_button_press) {
        ButtonPress& press = *active_button_press;
        SprID id = press.id;
        DrawnSprite *info = get_drawn_info(id);

        if (!info) {
            L.error("Unknown ID during button update: %d", id);
            return;
        }

        if (press.t < 0 && press.drawn) {
            repair_dirty_area(id, REPAIR_Below | REPAIR_This | REPAIR_Above, 0);
        } else if (!press.drawn) {
            uint32_t *screen = ((uint32_t*)(surf->pixels));
            const DrawArea *a = &(press.area);
            if (a->x < 0 || a->y < 0) {
                // Whole sprite is a button
                a = &(info->area);
            }

            const int shift = 2;
            for (int j = a->h-1; j >= 0; --j) {
                for (int i = a->w-1; i >= 0; --i) {
                    int x = (a->x+i);
                    int y = (a->y+j);
                    //int sx = max(a->x, x - shift);
                    //int sy = max(a->y, y - shift);
                    int sx = max(0, x - shift);
                    int sy = max(0, y - shift);
                    //screen[y*RES_X + x] = screen[sy*RES_X + sx];
                    uint32_t col = screen[sy*RES_X + sx];
                    uint32_t r = (col >> 16) & 0xFF;
                    uint32_t g = (col >> 8) & 0xFF;
                    uint32_t b = col & 0xFF;
                    r *= 0.95;
                    g *= 0.95;
                    b *= 0.95;
                    screen[y*RES_X + x] = (r<<16) | (g<<8) | b;
                }
            }
            press.drawn = true;
        }
    }
}

void DrawManagerSDL::blit_surf_to_window()
{
    if (hardware_rendering) {
        void* pixels = nullptr;
        int pitch = 0;
        SDL_LockTexture(texture, nullptr, &pixels, &pitch);

        uint8_t *src = (uint8_t*)(surf->pixels);
        uint8_t *dst = (uint8_t*)(pixels);

        for (int y = 0; y < RES_Y; ++y) {
            memcpy(dst + y*pitch,
                   src + y*surf->pitch,
                   RES_X*4);
        }

        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
        return;
    }

    if (fullscreen)
    {
        SDL_BlitScaled(surf, nullptr, win_surf, &fullscreen_area);
    }
    else
    {
        SDL_BlitScaled(surf, nullptr, win_surf, nullptr);
    }
}

#endif
