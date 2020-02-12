#ifndef GUARD_INPUT_H
#define GUARD_INPUT_H

enum Input {
    Space,
    Escape,
};

typedef struct {
    int x;
    int y;
} MousePos;

class InputManager {
    public:
        InputManager();
        bool read(Input, bool reset);
        MousePos read_click();
    protected:
        bool space;
        bool escape;
        MousePos mouse_pos;
        MousePos click_pos;
        virtual bool update() = 0;
};

#ifdef SDL
#include "input.SDL.h"
#endif

#endif
