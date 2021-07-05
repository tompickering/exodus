#ifndef GUARD_INPUT_H
#define GUARD_INPUT_H

#define INPUT_MAX_TEXT 20

#include "shared.h"

enum Input {
    K_Space,
    K_Escape,
    K_Enter,
    K_Backspace,
    K_Up,
    K_Down,
    K_Left,
    K_Right,
    K_LShift,
    K_RShift,
    K_F1,
    K_F2,
    K_F3,
    K_F4,
    K_F5,
    K_F6,
    K_F7,
    K_F8,
    K_F9,
    K_F10,
    K_F11,
    K_F12,

    K_END,
};

class InputManager {
    public:
        InputManager();
        bool read(Input);
        bool consume(Input);
        MousePos get_mouse_pos();
        MousePos read_click();
        MousePos read_click_r();
        const char* get_input_text(int);
        virtual unsigned int read_numbers() = 0;
        virtual bool is_num_held(unsigned char) = 0;
        virtual void start_text_input() = 0;
        virtual void stop_text_input() = 0;
        void set_input_text(const char*);
        void backspace();
    protected:
        bool space;
        bool escape;
        bool enter;
        MousePos mouse_pos;
        MousePos click_pos;
        MousePos click_pos_r;
        virtual bool update() = 0;
        uint32_t input[1 + K_END / 32];
        char text[INPUT_MAX_TEXT + 1];
    private:
        bool _read(Input input, bool reset);
};

#ifdef SDL
#include "input.SDL.h"
#endif

#endif
