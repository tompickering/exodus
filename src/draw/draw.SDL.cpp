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
                           SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN);
    if (!win) {
        L.error("Could not create SDL window");
        return false;
    }

    surf = SDL_GetWindowSurface(win);

    if (!surf) {
        L.error("Could not create SDL surface");
        return false;
    }

    font = TTF_OpenFont(FONT_AUDIOWIDE, 14);

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

    pixelswap_src = SDL_CreateRGBSurface(0, surf->w, surf->h, 32, 0, 0, 0, 0);
    if (!pixelswap_src) {
        L.error("Could not create pixelswap source surface");
        return false;
    }

    return true;
}

void DrawManagerSDL::load_resources() {
    char img_path[ASSET_PATH_LEN_MAX];
    for (unsigned int i = 0;; ++i) {
        if (ASSETS_IMG[i][0] == '\0')
            break;
        strncpy(img_path, ASSETS_IMG[i], ASSET_PATH_LEN_MAX);
        strcat(img_path, ".png");
        sprite_data[ASSETS_IMG[i]] = (void*) IMG_Load(img_path);
        if (sprite_data[ASSETS_IMG[i]]) {
            L.debug("Loaded %s", img_path);
        } else {
            L.warn("Could not load %s", img_path);
        }
    }
}

void DrawManagerSDL::update(MousePos mouse_pos, MousePos click_pos) {
    if (pixelswap_active()) {
        pixelswap_update();
    }
    SDL_UpdateWindowSurface(win);
}

void DrawManagerSDL::pixelswap_update() {
    if (pixelswap_timer.get_delta() > 0.01) {
        if (!pixelswap_stage || !--pixelswap_stage) {
            return;
        }

        for (int y = 0; y < SCREEN_HEIGHT / 2; ++y) {
            for (int x = 0; x < SCREEN_WIDTH / 2; ++x) {
                bool swap = (random() % pixelswap_stage) == 0;
                if (swap) {
                    SDL_Rect r = {x * 2, y * 2, 2, 2};
                    SDL_BlitSurface(pixelswap_src, &r, surf, &r);
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
    SDL_FillRect(pixelswap_src, nullptr, SDL_MapRGB(surf->format, 0x0, 0x0, 0x0));
}

void DrawManagerSDL::save_background() {
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
    draw(surf, spr_key, nullptr);
}

void DrawManagerSDL::draw(const char* spr_key, DrawArea area) {
    draw(surf, spr_key, &area);
}

void DrawManagerSDL::draw(const char* spr_key, DrawTransform t) {
    draw(surf, spr_key, t);
}

void DrawManagerSDL::pixelswap_draw(const char* spr_key) {
    draw(pixelswap_src, spr_key, nullptr);
}

void DrawManagerSDL::pixelswap_draw(const char* spr_key, DrawArea area) {
    draw(pixelswap_src, spr_key, &area);
}

void DrawManagerSDL::pixelswap_draw(const char* spr_key, DrawTransform t) {
    draw(pixelswap_src, spr_key, t);
}

void DrawManagerSDL::draw(SDL_Surface* tgt, const char* spr_key, DrawArea* area) {
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

void DrawManagerSDL::draw(SDL_Surface* tgt, const char* spr_key, DrawTransform t) {
    SDL_Surface *spr = (SDL_Surface*)get_sprite_data(spr_key);
    if (!spr) {
        L.warn("Unknown sprite: %s", spr_key);
        return;
    }

    int spr_w = spr->w;
    int spr_h = spr->h;
    DrawArea area;
    area.x = t.x - (t.anchor_x * spr_w * t.scale);
    area.y = t.y - (t.anchor_y * spr_h * t.scale);
    area.w = spr_w * t.scale;
    area.h = spr_h * t.scale;

    draw(tgt, spr_key, &area);
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
    msg_rect.x = x;
    msg_rect.y = y;

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

#endif
