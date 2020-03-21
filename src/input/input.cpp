#include "input.h"

InputManager::InputManager() {
    mouse_pos.x = -1;
    mouse_pos.y = -1;
    click_pos.x = -1;
    click_pos.y = -1;
    text[INPUT_MAX_TEXT] = '\0';
}

bool InputManager::consume(Input input) {
    return _read(input, true);
}

bool InputManager::read(Input input) {
    return _read(input, false);
}

bool InputManager::_read(Input key, bool reset) {
    bool result = false;
    int input_idx = 0;
    int input_bit = 0;
    for (int i = 0; i < Input::K_END; ++i) {
        if ((Input)i == key) {
            input_idx = i / 32;
            input_bit = i % 32;
            result = (bool)(input[input_idx] & (1 << input_bit));
            if (result) {
                if (reset) {
                    input[input_idx] &= (input[input_idx] & ~(1 << input_bit));
                }
                break;
            }
        }
    }

    return result;
}

MousePos InputManager::get_mouse_pos() {
    return {(int)((float)mouse_pos.x / UPSCALE_X),
            (int)((float)mouse_pos.y / UPSCALE_Y)};
}

/*
 * Read MousePos describing click location, or
 * {-1, -1} if no click has been recorded.
 * Resets the click state, so no two things may
 * read the same click information.
 * Returns in screen-space - not resolution-space.
 */
MousePos InputManager::read_click() {
    MousePos result;
    result = click_pos;
    click_pos.x = -1;
    click_pos.y = -1;
    return result;
}

const char* InputManager::get_input_text() {
    return (const char*) text;
}
