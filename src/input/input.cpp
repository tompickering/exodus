#include "input.h"

#include <cstring>

InputManager::InputManager() {
    mouse_pos.x = -1;
    mouse_pos.y = -1;
    click_pos.x = -1;
    click_pos.y = -1;
    click_pos_r.x = -1;
    click_pos_r.y = -1;
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
    return mouse_pos;
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

MousePos InputManager::read_click_r() {
    MousePos result;
    result = click_pos_r;
    click_pos_r.x = -1;
    click_pos_r.y = -1;
    return result;
}

const char* InputManager::get_input_text(int max_len) {
    if (max_len > 0 && max_len <= INPUT_MAX_TEXT)
        text[max_len] = '\0';
    return (const char*) text;
}

void InputManager::set_input_text(const char* text_to_set) {
    text[INPUT_MAX_TEXT] = '\0';
    strncpy(text, text_to_set, INPUT_MAX_TEXT);
}

void InputManager::backspace() {
    int i = 0;
    for (; i < INPUT_MAX_TEXT; ++i) {
        if (text[i] == '\0')
            break;
    }

    if (i > 0) text[i-1] = '\0';
}
