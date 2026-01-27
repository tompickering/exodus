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

    K_A,
    K_B,
    K_C,
    K_D,
    K_E,
    K_F,
    K_G,
    K_H,
    K_I,
    K_J,
    K_K,
    K_L,
    K_M,
    K_N,
    K_O,
    K_P,
    K_Q,
    K_R,
    K_S,
    K_T,
    K_U,
    K_V,
    K_W,
    K_X,
    K_Y,
    K_Z,

    K_END,
};

struct InputManagerOptions {
    int win_w;
    int win_h;
    bool fullscreen = true;
};

class InputManager {
    public:
        InputManager();
        virtual bool init(const InputManagerOptions&);
        bool read(Input);
        bool consume(Input);
        MousePos get_mouse_pos();
        MousePos read_click();
        MousePos read_click_r();
        const char* get_input_text(int);
        virtual void input_text_overfill(int overfill);
        virtual unsigned int read_numbers() = 0;
        virtual bool is_num_held(unsigned char) = 0;
        virtual void start_text_input() { text_input_is_active = true; }
        virtual void stop_text_input() { text_input_is_active = false; }
        bool active_text_input() const { return text_input_is_active; }
        void set_input_text(const char*);
        void backspace();
        void enable_repeating_clicks(bool);
        void clear_click_held_state();
        virtual bool is_click_held();
        virtual bool click_held_for(float);
    protected:
        InputManagerOptions options;
        bool space;
        bool escape;
        bool enter;
        MousePos mouse_pos;
        MousePos click_pos;
        MousePos last_click_pos;
        MousePos click_pos_r;
        virtual bool update(float);
        uint32_t input[1 + K_END / 32];
        char text[INPUT_MAX_TEXT + 1];
        bool click_held;
        float click_held_time;
        float click_repeat_timeout;
    private:
        bool _read(Input input, bool reset);
        bool repeating_clicks;
        bool text_input_is_active;
};

#ifdef SDL
#include "input.SDL.h"
#endif

#endif
