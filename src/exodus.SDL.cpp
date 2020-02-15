#include "exodus.SDL.h"

#include "shared.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

ExodusSDL::ExodusSDL() {
}

ExodusSDL::~ExodusSDL() {
}

bool ExodusSDL::init() {
    bool res = Exodus::init();
    if (!res)
        return res;

    L.info("Initialising SDL...");

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        L.fatal("Could not initialise SDL");
        return false;
    }

    if (TTF_Init() < 0) {
        L.fatal("Could not initialise SDL_ttf");
        return false;
    }

    L.info("SDL initialised.");

    return res;
}
