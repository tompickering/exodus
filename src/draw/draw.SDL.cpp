#ifndef GUARD_DRAW_SDL_CPP
#define GUARD_DRAW_SDL_CPP

#include "draw.SDL.h"

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "../shared.h"
#include "../assetpaths.h"

using std::string;

bool DrawManagerSDL::init() {
    L.info("DrawManager Init...");
    win = SDL_CreateWindow(PROG_NAME,
                           SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED,
                           SCREEN_WIDTH,
                           SCREEN_HEIGHT,
                           SDL_WINDOW_SHOWN);
    if (!win) {
        L.error("Could not create SDL window");
        return false;
    }

    surf = SDL_GetWindowSurface(win);

    if (!surf) {
        L.error("Could not create SDL surface");
        return false;
    }

    font = TTF_OpenFont(PATH_FONT_AUDIOWIDE, 20);
    if (!font) {
        L.error("Could not load font Audiowide");
        return false;
    }

    SDL_FillRect(surf, nullptr, SDL_MapRGB(surf->format, 0x0, 0x0, 0x0));

    return true;
}

void DrawManagerSDL::update() {
    SDL_UpdateWindowSurface(win);
}

void* DrawManagerSDL::get_sprite_data(string img_path) {
    if (img_path != "") {
        if (!sprite_data.count(img_path)) {
            sprite_data[img_path] = (void*) IMG_Load(img_path.c_str());
            return sprite_data[img_path];
        }
        return sprite_data[img_path];
    }
    return nullptr;
}

#endif
