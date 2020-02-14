#ifndef GUARD_DRAW_SDL_CPP
#define GUARD_DRAW_SDL_CPP

#include "draw.SDL.h"

#include <cstring>
#include <cstdlib>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "../shared.h"
#include "../assetpaths.h"

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

    surf = SDL_GetWindowSurface(win);

    if (!surf) {
        L.error("Could not create SDL surface");
        return false;
    }

    font = TTF_OpenFont(FONT_AUDIOWIDE, 14 * UPSCALE_X);

    if (!font) {
        L.error("Could not load font Audiowide");
        return false;
    }

    SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, 0x0, 0x0, 0x0));

    background = SDL_CreateRGBSurface(0, surf->w, surf->h, 32, 0, 0, 0, 0);
    if (!background) {
        L.error("Could not create background surface");
        return false;
    }

    temp_surf = SDL_CreateRGBSurface(0, surf->w, surf->h, 32, 0, 0, 0, 0);
    if (!temp_surf) {
        L.error("Could not create pixelswap source surface");
        return false;
    }

    SDL_ShowCursor(false);

    return true;
}

void DrawManagerSDL::load_resources() {
    char img_path[ASSET_PATH_LEN_MAX];
    SDL_Surface *load_surf;
    for (unsigned int i = 0;; ++i) {
        if (ASSETS_IMG[i][0] == '\0')
            break;
        strncpy(img_path, ASSETS_IMG[i], ASSET_PATH_LEN_MAX);
        strcat(img_path, ".png");
        load_surf = IMG_Load(img_path);
        // Ensure image is normalised to RGBA8888
        // This helps to ensure that we don't try to use
        // unsupported operations like scaled blits between
        // certain format combinations, which will fail.
        sprite_data[ASSETS_IMG[i]] =
            SDL_ConvertSurfaceFormat(
                    load_surf, SDL_PIXELFORMAT_RGBA8888, 0);
        SDL_FreeSurface(load_surf);
        if (sprite_data[ASSETS_IMG[i]]) {
            L.debug("Loaded %s", img_path);
        } else {
            L.warn("Could not load %s", img_path);
        }
    }
}

void DrawManagerSDL::update(MousePos mouse_pos, MousePos new_click_pos) {
    DrawManager::update(mouse_pos, new_click_pos);
    if (pixelswap_active()) {
        pixelswap_update();
    }
    if (fade_active()) {
        float fade_progress = fade_timer.get_delta() / (float) fade_seconds;
        if (fade_progress < 1) {
            int this_fade_stage = (int)(fade_progress * (float)fade_stages);
            if (this_fade_stage != fade_stage) {
                uint32_t *px = ((uint32_t*)(temp_surf->pixels));
                fade_stage = this_fade_stage;
                for (int i = 0; i < surf->w * surf->h; ++i) {
                    uint32_t new_col = 0;
                    uint8_t r = (px[i] & 0x00FF0000) >> 16;
                    uint8_t g = (px[i] & 0x0000FF00) >> 8;
                    uint8_t b = (px[i] & 0x000000FF);
                    uint8_t new_r = uint8_t((float)r * (1.f - fade_progress));
                    uint8_t new_g = uint8_t((float)g * (1.f - fade_progress));
                    uint8_t new_b = uint8_t((float)b * (1.f - fade_progress));
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
            clear();
        }
    }

    if (draw_cursor) {
        draw(ID_CURSOR, IMG_INTRO_SH4_BLINK1, {mouse_pos.x, mouse_pos.y, 0.5, 0.5, 1, 1});
    }

    SDL_UpdateWindowSurface(win);
}

void DrawManagerSDL::pixelswap_update() {
    if (pixelswap_timer.get_delta() > 0.01) {
        if (!pixelswap_stage || !--pixelswap_stage) {
            return;
        }

        const int PX_W = (int)(.5f + 2.f * UPSCALE_X);
        const int PX_H = (int)(.5f + 2.f * UPSCALE_Y);
        for (int y = 0; y < SCREEN_HEIGHT / PX_H; ++y) {
            for (int x = 0; x < SCREEN_WIDTH / PX_W; ++x) {
                bool swap = (random() % pixelswap_stage) == 0;
                if (swap) {
                    SDL_Rect r = {x * PX_W, y * PX_H, PX_W, PX_H};
                    SDL_BlitSurface(temp_surf, &r, surf, &r);
                }
            }
        }

        pixelswap_timer.start();
    }

}

void DrawManagerSDL::clear() {
    DrawManager::clear();
    SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, 0x0, 0x0, 0x0));
}

void DrawManagerSDL::pixelswap_clear() {
    SDL_FillRect(temp_surf, nullptr, SDL_MapRGB(surf->format, 0x0, 0x0, 0x0));
}

void DrawManagerSDL::save_background() {
    L.debug("Saving background");
    if (SDL_BlitSurface(surf, nullptr, background, nullptr)) {
        L.warn("Background blit failed");
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

void DrawManagerSDL::draw(const char* spr_key) {
    draw(surf, spr_key, nullptr, nullptr);
}

void DrawManagerSDL::draw(const char* spr_key, DrawArea area) {
    draw(surf, spr_key, &area, nullptr);
}

void DrawManagerSDL::draw(const char* spr_key, DrawTransform t) {
    draw(surf, spr_key, t, nullptr);
}

void DrawManagerSDL::draw(SprID id, const char* spr_key) {
    draw(surf, spr_key, nullptr, &id);
}

void DrawManagerSDL::draw(SprID id, const char* spr_key, DrawTransform t) {
    draw(surf, spr_key, t, &id);
}

void DrawManagerSDL::pixelswap_draw(const char* spr_key) {
    draw(temp_surf, spr_key, nullptr, nullptr);
}

void DrawManagerSDL::pixelswap_draw(const char* spr_key, DrawArea area) {
    draw(temp_surf, spr_key, &area, nullptr);
}

void DrawManagerSDL::pixelswap_draw(const char* spr_key, DrawTransform t) {
    draw(temp_surf, spr_key, t, nullptr);
}

void DrawManagerSDL::pixelswap_draw(SprID id, const char* spr_key) {
    draw(temp_surf, spr_key, nullptr, &id);
}

void DrawManagerSDL::pixelswap_draw(SprID id, const char* spr_key, DrawTransform t) {
    draw(temp_surf, spr_key, t, &id);
}

void DrawManagerSDL::draw(SDL_Surface* tgt, const char* spr_key, DrawArea* area, SprID* id) {
    DrawArea *dirty_area;
    DrawArea default_area = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    if (area) {
        area->x *= UPSCALE_X;
        area->y *= UPSCALE_Y;
        area->w *= UPSCALE_X;
        area->h *= UPSCALE_Y;
    } else {
        area = &default_area;
    }

    if (id) {
        dirty_area = get_drawn_area(*id);
        if (dirty_area) {
            // We know where this sprite was drawn previously.
            // Wipe that area with the background.
            SDL_Rect r = {dirty_area->x, dirty_area->y, dirty_area->w, dirty_area->h};
            SDL_BlitSurface(background, &r, tgt, &r);
            // Update the old area to be redrawn with this draw's area
            *dirty_area = *area;
        } else {
            if (area) {
                drawn_spr_info.push_back({*id, *area});
            } else {
                L.warn("ID image draw without specific area - not saving");
            }
        }
    }

    SDL_Surface *spr = (SDL_Surface*)get_sprite_data(spr_key);
    if (!spr) {
        L.warn("Unknown sprite: %s", spr_key);
        return;
    }
    if (area) {
        SDL_Rect dst_rect = {area->x, area->y, area->w, area->h};
        if (SDL_BlitScaled(spr, nullptr, tgt, &dst_rect)) {
            L.debug("Blit unsuccessful: %s", spr_key);
        }
    } else {
        if (SDL_BlitSurface(spr, nullptr, tgt, nullptr)) {
            L.debug("Blit unsuccessful: %s", spr_key);
        }
    }
}

void DrawManagerSDL::draw(SDL_Surface* tgt, const char* spr_key, DrawTransform t, SprID* id) {
    SDL_Surface *spr = (SDL_Surface*)get_sprite_data(spr_key);
    if (!spr) {
        L.warn("Unknown sprite: %s", spr_key);
        return;
    }

    int spr_w = spr->w;
    int spr_h = spr->h;
    DrawArea area;
    area.x = t.x - (t.anchor_x * spr_w * t.scale_x);
    area.y = t.y - (t.anchor_y * spr_h * t.scale_y);
    area.w = spr_w * t.scale_x;
    area.h = spr_h * t.scale_y;

    draw(tgt, spr_key, &area, id);
}

void DrawManagerSDL::draw_text(const char* text, Justify jst, int x, int y, RGB rgb) {
    draw_text(text, jst, x, y, &rgb, nullptr);
}

void DrawManagerSDL::draw_text(const char* text, Justify jst, int x, int y, RGB rgb, RGB bg_rgb) {
    draw_text(text, jst, x, y, &rgb, &bg_rgb);
}

void DrawManagerSDL::draw_text(const char* text, Justify jst, int x, int y, RGB* rgb, RGB* bg_rgb) {
    SDL_Color colour = {rgb->r, rgb->g, rgb->b};
    SDL_Surface *msg_surf;

    if (bg_rgb) {
        SDL_Color bg_colour = {bg_rgb->r, bg_rgb->g, bg_rgb->b};
        msg_surf = TTF_RenderText_Shaded((TTF_Font*)font, text, colour, bg_colour);
    } else {
        msg_surf = TTF_RenderText_Blended((TTF_Font*)font, text, colour);
    }

    SDL_Rect msg_rect;
    msg_rect.x = UPSCALE_X * x;
    msg_rect.y = UPSCALE_Y * y;

    if (jst != Justify::Left) {
        int render_w, render_h;
        TTF_SizeText((TTF_Font*)font, text, &render_w, &render_h);
        msg_rect.x -= (jst == Justify::Right) ? render_w : render_w / 2;
    }

    SDL_BlitSurface(msg_surf, nullptr, surf, &msg_rect);
}

void DrawManagerSDL::pixelswap_start() {
    pixelswap_start(nullptr);
}

void DrawManagerSDL::pixelswap_start(DrawArea* area) {
    pixelswap_stage = 20;
    if (area) {
        pixelswap_region = *area;
    } else {
        pixelswap_region = {0, 0, 0, 0};
    }
    pixelswap_timer.start();
}

bool DrawManagerSDL::pixelswap_active() {
    return pixelswap_stage > 0;
}

void DrawManagerSDL::fade_black(float seconds, int stages) {
    SDL_BlitSurface(surf, nullptr, temp_surf, nullptr);
    fade_seconds = seconds;
    fade_stage = 0;
    fade_stages = stages;
    fade_timer.start();
}

bool DrawManagerSDL::fade_active() {
    return fade_stages > 0;
}

#endif
