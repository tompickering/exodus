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
}

bool DrawManagerSDL::init() {
    L.info("DrawManager Init...");
    win = SDL_CreateWindow(PROG_NAME,
                           SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED,
                           SCREEN_WIDTH,
                           SCREEN_HEIGHT,
                           SDL_WINDOW_SHOWN); // | SDL_WINDOW_FULLSCREEN);
    if (!win) {
        L.error("Could not create SDL window");
        return false;
    }

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

    return true;
}

SDL_Surface* DrawManagerSDL::load_normalised_image(const char* path) {
    char img_path[ASSET_PATH_LEN_MAX];
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
        L.debug("Loaded %s", img_path);
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
    SDL_BlitScaled(surf, nullptr, win_surf, nullptr);
#endif
    SDL_UpdateWindowSurface(win);
}

void DrawManagerSDL::load_resources() {
    font_data[Font::Default] = (void*)TTF_OpenFont(FONT_AUDIOWIDE, 16 * UPSCALE_X);
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

void DrawManagerSDL::update(float delta, MousePos mouse_pos, MousePos new_click_pos) {
    SDL_Rect ca;
    SDL_Rect ca0;

    DrawManager::update(delta, mouse_pos, new_click_pos);
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
                    uint8_t new_r = (uint8_t)((int16_t)r0 + (int16_t)(fade_progress * ((float)r1 - (float)r0)));
                    uint8_t new_g = (uint8_t)((int16_t)g0 + (int16_t)(fade_progress * ((float)g1 - (float)g0)));
                    uint8_t new_b = (uint8_t)((int16_t)b0 + (int16_t)(fade_progress * ((float)b1 - (float)b0)));
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

    bool render_cursor = draw_cursor && mouse_pos.x > 0 && mouse_pos.y > 0;

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
        cursor_area.x = mouse_pos.x - cursor_area.w / 2;
        cursor_area.y = mouse_pos.y - cursor_area.h / 2;

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
    SDL_BlitScaled(surf, nullptr, win_surf, nullptr);
#endif

    SDL_UpdateWindowSurface(win);

    if (render_cursor) {
        // Repair the surface where we rendered the cursor.
        ca = cursor_area;
        ca0 = cursor_area; ca0.x = 0; ca0.y = 0;
        SDL_BlitSurface(cursor_underlay, &ca0, surf, &ca);
    }
}

void DrawManagerSDL::pixelswap_update() {
    if (pixelswap_time > 0.01) {
        if (!pixelswap_stage || !--pixelswap_stage) {
            return;
        }

        const int PX_W = (int)(.5f + 2.f * UPSCALE_X);
        const int PX_H = (int)(.5f + 2.f * UPSCALE_Y);
        for (int y = 0; y < pixelswap_region.h / PX_H; ++y) {
            for (int x = 0; x < pixelswap_region.w / PX_W; ++x) {
                bool swap = (random() % pixelswap_stage) == 0;
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
    L.debug("Saving background");
    if (SDL_BlitSurface(surf, nullptr, background, nullptr)) {
        L.warn("Background blit failed");
    }
}

void DrawManagerSDL::save_background(DrawArea area) {
    L.debug("Saving background at %d %d (%dx%d)", area.x, area.y, area.w, area.h);

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

void DrawManagerSDL::draw_rect(DrawArea area, float thickness, RGB rgb) {
    int tx = (int)(thickness * UPSCALE_X);
    int ty = (int)(thickness * UPSCALE_Y);
    fill(TGT_Primary, {area.x, area.y, tx, area.h + ty}, rgb);
    fill(TGT_Primary, {area.x + area.w, area.y, tx, area.h + ty}, rgb);
    fill(TGT_Primary, {area.x, area.y, area.w + tx, ty}, rgb);
    fill(TGT_Primary, {area.x, area.y + area.h, area.w + tx, ty}, rgb);
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
    DrawArea *dirty_area = get_drawn_area(id);
    if (dirty_area) {
        // We know where this sprite was drawn previously.
        // Wipe that area with the background.
        SDL_Rect r = {dirty_area->x, dirty_area->y, dirty_area->w, dirty_area->h};
        SDL_BlitSurface(background, &r, surf, &r);
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

void DrawManagerSDL::draw_text(DrawTarget tgt, SprID id, Font font, const char* text, Justify jst, int x, int y, RGB* rgb, RGB* bg_rgb) {
    RGB adjustable_col = *rgb;
    adjust_selectable_text_col(id, adjustable_col);

    SDL_Color colour = {adjustable_col.r, adjustable_col.g, adjustable_col.b};
    SDL_Surface *msg_surf;
    SDL_Surface *tgt_surf = get_target(tgt);

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
    DrawArea transformed_area = {
        (int)((float)area.x * UPSCALE_X),
        (int)((float)area.y * UPSCALE_Y),
        (int)((float)area.w * UPSCALE_X),
        (int)((float)area.h * UPSCALE_Y)};
    DrawnSprite *drawn_info = update_dirty_area(id, transformed_area);
    if (!drawn_info) {
        L.fatal("Could not attain draw info for fill");
    }
    drawn_info->type = DRAWTYPE_Fill;
    drawn_info->colour = col;
    fill(TGT_Primary, area, col);
}

void DrawManagerSDL::fill(DrawTarget tgt, DrawArea area, RGB col) {
    SDL_Rect r;
    SDL_Surface *tgt_surf = get_target(tgt);
    r.x = area.x * UPSCALE_X;
    r.y = area.y * UPSCALE_Y;
    r.w = area.w * UPSCALE_X;
    r.h = area.h * UPSCALE_Y;
    SDL_FillRect(tgt_surf, &r, SDL_MapRGB(surf->format, col.r, col.g, col.b));
}

void DrawManagerSDL::pattern_fill(DrawArea area) {
    pattern_fill(TGT_Primary, area);
}

void DrawManagerSDL::pattern_fill(DrawTarget tgt, DrawArea area) {
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

#endif
