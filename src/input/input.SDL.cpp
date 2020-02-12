#include "input.SDL.h"

#include <SDL2/SDL.h>

#include "../shared.h"

bool InputManagerSDL::update() {
    SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            return false;
        } else if (e.type == SDL_KEYDOWN && !e.key.repeat) {
            switch(e.key.keysym.sym) {
                case SDLK_SPACE:
                    space = true;
                    break;
                case SDLK_ESCAPE:
                    escape = true;
                    break;
            }
        } else if (e.type == SDL_KEYUP) {
            switch(e.key.keysym.sym) {
                case SDLK_SPACE:
                    space = false;
                    break;
                case SDLK_ESCAPE:
                    escape = false;
                    break;
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            SDL_GetMouseState(&click_pos.x, &click_pos.y);
            L.debug("Click: %d,%d", click_pos.x, click_pos.y);
        }
    }

    return true;
}
