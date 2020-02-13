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
