#include "exodus.SDL.h"

#include "shared.h"

ExodusSDL::ExodusSDL() {
}

ExodusSDL::~ExodusSDL() {
}

bool ExodusSDL::init() {
    bool res = Exodus::init();
    if (!res)
        return res;
    L.info("Initialising SDL...");
    return res;
}
