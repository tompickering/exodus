#ifndef GUARD_INPUT_SDL_H
#define GUARD_INPUT_SDL_H

#include "input.h"

class InputManagerSDL : public InputManager {
    public:
        virtual bool update() override;
        virtual unsigned int read_numbers() override;
        virtual bool is_num_held(unsigned char) override;
        virtual void start_text_input() override;
        virtual void stop_text_input() override;
};

#endif
