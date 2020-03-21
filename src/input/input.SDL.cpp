#include "input.SDL.h"

#include <SDL2/SDL.h>

#include "../shared.h"

bool InputManagerSDL::update() {
    SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            return false;
        } else if (e.type == SDL_KEYDOWN && !e.key.repeat) {
            switch(e.key.keysym.sym) {
                case SDLK_SPACE:
                    space = true;
                    break;
                case SDLK_ESCAPE:
                    escape = true;
                    break;
                case SDLK_RETURN:
                    enter = true;
                    break;
            }
        } else if (e.type == SDL_KEYUP) {
            switch(e.key.keysym.sym) {
                case SDLK_SPACE:
                    space = false;
                    break;
                case SDLK_ESCAPE:
                    escape = false;
                    break;
                case SDLK_RETURN:
                    enter = false;
                    break;
            }
        } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            SDL_GetMouseState(&click_pos.x, &click_pos.y);
            L.debug("Click: %d,%d", click_pos.x, click_pos.y);
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
