#ifndef GUARD_INPUT_SDL_H
#define GUARD_INPUT_SDL_H

#include "input.h"

class InputManagerSDL : public InputManager {
    public:
        virtual bool update() override;
        virtual unsigned int read_numbers();
};

#endif
