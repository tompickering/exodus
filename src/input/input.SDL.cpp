#include "input.SDL.h"

#include <SDL2/SDL.h>

#include "shared.h"

// Must have 1:1 correspondence with Input
int sdl_input_key[] = {
    SDLK_SPACE,
    SDLK_ESCAPE,
    SDLK_RETURN,
    SDLK_BACKSPACE,
    SDLK_UP,
    SDLK_DOWN,
    SDLK_LEFT,
    SDLK_RIGHT,
    SDLK_LSHIFT,
    SDLK_RSHIFT,
    SDLK_F1,
    SDLK_F2,
    SDLK_F3,
    SDLK_F4,
    SDLK_F5,
    SDLK_F6,
    SDLK_F7,
    SDLK_F8,
    SDLK_F9,
    SDLK_F10,
    SDLK_F11,
    SDLK_F12,

    SDLK_a,
    SDLK_b,
    SDLK_c,
    SDLK_d,
    SDLK_e,
    SDLK_f,
    SDLK_g,
    SDLK_h,
    SDLK_i,
    SDLK_j,
    SDLK_k,
    SDLK_l,
    SDLK_m,
    SDLK_n,
    SDLK_o,
    SDLK_p,
    SDLK_q,
    SDLK_r,
    SDLK_s,
    SDLK_t,
    SDLK_u,
    SDLK_v,
    SDLK_w,
    SDLK_x,
    SDLK_y,
    SDLK_z,
};

bool InputManagerSDL::update(float delta) {
    if (!InputManager::update(delta)) {
        return false;
    }

    SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            return false;
        } else if (e.type == SDL_KEYDOWN && !e.key.repeat) {
            int input_idx = 0;
            int input_bit = 0;
            for (int i = 0; i < Input::K_END; ++i) {
                if (e.key.keysym.sym == sdl_input_key[i]) {
                    input_idx = i / 32;
                    input_bit = i % 32;
                    input[input_idx] |= 1 << input_bit;
                }
            }
        } else if (e.type == SDL_KEYUP) {
            int input_idx = 0;
            int input_bit = 0;
            for (int i = 0; i < Input::K_END; ++i) {
                if (e.key.keysym.sym == sdl_input_key[i]) {
                    input_idx = i / 32;
                    input_bit = i % 32;
                    input[input_idx] &= ~(1 << input_bit);
                }
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            SDL_GetMouseState(&click_pos.x, &click_pos.y);
            last_click_pos = click_pos;
            click_held = true;
        } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
            clear_click_held_state();
        } else if (e.type == SDL_MOUSEMOTION) {
            clear_click_held_state();
        } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT) {
            SDL_GetMouseState(&click_pos_r.x, &click_pos_r.y);
        } else if (e.type == SDL_TEXTINPUT) {
            if (!SDL_IsTextInputActive())
                continue;
            int len = strnlen(text, INPUT_MAX_TEXT);
            if (len >= INPUT_MAX_TEXT)
                continue;
            strcat(text, e.text.text);
        }
    }

    return true;
}

unsigned int InputManagerSDL::read_numbers() {
    SDL_PumpEvents();
    unsigned int numbers = 0;
    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    for (int i = 0; i <= 9; ++i) {
        if (state[SDL_SCANCODE_1 + ((i + 9) % 10)]) {
            numbers |= (1 << i);
        }
    }
    return numbers;
}

bool InputManagerSDL::is_num_held(unsigned char n) {
    return (bool)((read_numbers() >> n) & 1);

}

void InputManagerSDL::start_text_input() {
    for (int i = 0; i <= INPUT_MAX_TEXT; ++i) {
        text[i] = '\0';
    }
    SDL_StartTextInput();
}

void InputManagerSDL::stop_text_input() {
    SDL_StopTextInput();
}
