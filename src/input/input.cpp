#include "input.h"

#include <cstring>
#include "util/value.h"

#define CLICK_REPEAT_DELAY .4f
#define CLICK_REPEAT_DELAY2 1.2f
#define CLICK_REPEAT_INTERVAL .05f
#define CLICK_REPEAT_INTERVAL2 .01f

InputManager::InputManager() {
    mouse_pos.x = -1;
    mouse_pos.y = -1;
    click_pos.x = -1;
    click_pos.y = -1;
    click_pos_r.x = -1;
    click_pos_r.y = -1;
    text[INPUT_MAX_TEXT] = '\0';
    enable_repeating_clicks(false);
    text_input_is_active = false;
}

bool InputManager::init(const InputManagerOptions& input_options) {
    options = input_options;
    return true;
}

bool InputManager::update(float delta) {
    if (click_held) {
        click_held_time += delta;
    } else {
        click_held_time = 0;
        click_repeat_timeout = 0;
    }

    if (repeating_clicks) {
        // Holding mouse button simulates clicks
        if (click_held_time > CLICK_REPEAT_DELAY) {
            if (click_repeat_timeout <= 0) {
                click_pos = last_click_pos;
                if (click_held_time > CLICK_REPEAT_DELAY2) {
                    click_repeat_timeout = CLICK_REPEAT_INTERVAL2;
                } else {
                    click_repeat_timeout = CLICK_REPEAT_INTERVAL;
                }
            } else {
                click_repeat_timeout -= delta;
            }
        }
    }

    return true;
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

void InputManager::input_text_overfill(int overfill) {
    int len = strnlen(text, INPUT_MAX_TEXT);
    text[len - overfill] = '\0';
}

void InputManager::set_input_text(const char* text_to_set) {
    text[INPUT_MAX_TEXT] = '\0';
    strncpy(text, text_to_set, INPUT_MAX_TEXT);
}

void InputManager::append_input_text(const char* text_to_append) {
    text[INPUT_MAX_TEXT] = '\0';

    int len_current = strnlen(text, INPUT_MAX_TEXT);

    if (len_current >= INPUT_MAX_TEXT) {
        return;
    }

    int len_append_max = INPUT_MAX_TEXT - len_current;
    int len_append = strnlen(text_to_append, INPUT_MAX_TEXT);

    int bytes_to_append = min(len_append, len_append_max);

    if (bytes_to_append < 0) {
        return;
    }

    strncat(text, text_to_append, bytes_to_append);

    int len_new = strnlen(text, INPUT_MAX_TEXT);

    /*
     * FIXME: This only assesses the most recent byte.
     *        As it stands this is the only way in which input
     *        arrives, but this should really be fixed.
     */

    if (len_new >= 2) {
        //JK: Check for UTF-8 two-byte identifier and, if found, remove and
        //shift (+64) the following char into corresponding Latin characters of extended ASCII
        //N.B. Check exactly for signed char -61 ('Atilde') or we'll undo shifted chars in the next pass
        char next_to_last_char = text[len_new - 2];
        if (next_to_last_char==-61) {
            char last_char = text[len_new - 1];
            char replacement_char[2];
            replacement_char[1]=0;
            if (last_char < -64) replacement_char[0] = last_char+64;
            char workstring[INPUT_MAX_TEXT];
            // Remove last 2 (UTF-8 two-byte) characters
            strncpy(workstring, text, strlen(text) - 2);
            // Add Latin special character within extended ASCII range
            strcat(workstring, replacement_char);
            set_input_text(workstring);
        }
    }
}

void InputManager::backspace() {
    int i = 0;
    for (; i < INPUT_MAX_TEXT; ++i) {
        if (text[i] == '\0')
            break;
    }

    if (i > 0) text[i-1] = '\0';
}

void InputManager::enable_repeating_clicks(bool enable) {
    clear_click_held_state();
    repeating_clicks = enable;
    L.verb("Repeating clicks %sABLED", repeating_clicks?"EN":"DIS");
}

void InputManager::clear_click_held_state() {
    click_held = false;
    last_click_pos.reset();
    click_held_time = 0;
    click_repeat_timeout = 0;
}

bool InputManager::is_click_held() {
    return click_held;
}

bool InputManager::click_held_for(float t) {
    return click_held && (click_held_time >= t);
}
