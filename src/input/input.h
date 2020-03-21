#ifndef GUARD_INPUT_H
#define GUARD_INPUT_H

#define INPUT_MAX_TEXT 20

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
        const char* get_input_text();
        virtual unsigned int read_numbers() = 0;
        virtual bool is_num_held(unsigned char) = 0;
        virtual void start_text_input() = 0;
        virtual void stop_text_input() = 0;
    protected:
        bool space;
        bool escape;
        MousePos mouse_pos;
        MousePos click_pos;
        virtual bool update() = 0;
        char text[INPUT_MAX_TEXT + 1];
    private:
        bool _read(Input input, bool reset);
};

#ifdef SDL
#include "input.SDL.h"
#endif

#endif
