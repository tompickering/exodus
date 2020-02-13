#ifndef GUARD_INPUT_H
#define GUARD_INPUT_H

#include "../shared.h"

enum Input {
    Space,
    Escape,
};

class InputManager {
    public:
        InputManager();
        bool read(Input);
        bool consume(Input);
        MousePos get_mouse_pos();
        MousePos read_click();
        virtual unsigned int read_numbers() = 0;
        virtual bool is_num_held(unsigned char) = 0;
    protected:
        bool space;
        bool escape;
        MousePos mouse_pos;
        MousePos click_pos;
        virtual bool update() = 0;
    private:
        bool _read(Input input, bool reset);
};

#ifdef SDL
#include "input.SDL.h"
#endif

#endif
