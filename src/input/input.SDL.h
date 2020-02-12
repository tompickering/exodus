#ifndef GUARD_INPUT_SDL_H
#define GUARD_INPUT_SDL_H

#include "input.h"

class InputManagerSDL : public InputManager {
    public:
        virtual bool update() override;
};

#endif
