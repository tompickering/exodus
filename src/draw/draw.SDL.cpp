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

    font = TTF_OpenFont(PATH_FONT_AUDIOWIDE, 14);

    if (!font) {
        L.error("Could not load font Audiowide");
        return false;
    }

    SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, 0x0, 0x0, 0x0));

    return true;
}

void DrawManagerSDL::load_resources() {
    for (unsigned int i = 0;; ++i) {
        const char* img_path = ASSETS_IMG[i];
        if (img_path[0] == '\0')
            break;
        L.debug("Loading %s", img_path);
        sprite_data[img_path] = (void*) IMG_Load(img_path);
    }
}

void DrawManagerSDL::update(MousePos mouse_pos, MousePos click_pos) {
    SDL_UpdateWindowSurface(win);
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
