#ifndef GUARD_DRAW_SDL_CPP
#define GUARD_DRAW_SDL_CPP

#include "draw.SDL.h"

#include <cstring>

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
    background = SDL_CreateRGBSurface(0, surf->w, surf->h, 8, 0, 0, 0, 0);

    if (!background) {
        L.error("Could not create background surface");
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
    SDL_UpdateWindowSurface(win);
}

void DrawManagerSDL::clear() {
    DrawManager::clear();
    SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, 0x0, 0x0, 0x0));
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

void DrawManagerSDL::draw(const char* spr_key, int x, int y, int w, int h) {
    SDL_Surface *spr = (SDL_Surface*)get_sprite_data(spr_key);
    if (!spr) {
        L.warn("Unknown sprite: %s", spr_key);
        return;
    }
    SDL_Rect dst_rect = {x, y, w, h};
    if (SDL_BlitSurface(spr, nullptr, surf, &dst_rect)) {
        L.debug("Blit unsuccessful: %s", spr_key);
    }
}

void DrawManagerSDL::draw(const char* spr_key) {
    SDL_Surface *spr = (SDL_Surface*)get_sprite_data(spr_key);
    if (!spr) {
        L.warn("Unknown sprite: %s", spr_key);
        return;
    }
    if (SDL_BlitSurface(spr, nullptr, surf, nullptr)) {
        L.debug("Blit unsuccessful: %s", spr_key);
    }
}

void DrawManagerSDL::draw_text(const char* text, int x, int y, int w, int h,
                               unsigned char r, unsigned char g, unsigned char b) {

    SDL_Color colour = {r, g, b};
    SDL_Surface *msg_surf = TTF_RenderText_Blended((TTF_Font*)font, text, colour);

    SDL_Rect msg_rect;
    msg_rect.x = x;
    msg_rect.y = y;
    msg_rect.w = w;
    msg_rect.h = h;

    SDL_BlitSurface(msg_surf, nullptr, surf, &msg_rect);
}

#endif
