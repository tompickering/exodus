#include "input.h"

InputManager::InputManager() {
    mouse_pos.x = -1;
    mouse_pos.y = -1;
    click_pos.x = -1;
    click_pos.y = -1;
}

bool InputManager::consume(Input input) {
    return _read(input, true);
}

bool InputManager::read(Input input) {
    return _read(input, false);
}

bool InputManager::_read(Input input, bool reset) {
    bool result;
    switch (input) {
        case Space:
            result = space;
            space = space && !reset;
            break;
        case Escape:
            result = escape;
            escape = escape && !reset;
            break;
    }
    return result;
}

MousePos InputManager::get_mouse_pos() {
    return {(int)((float)mouse_pos.x / UPSCALE_X),
            (int)((float)mouse_pos.y / UPSCALE_Y)};
}

MousePos InputManager::read_click() {
    MousePos result;
    if (click_pos.x > 0 && click_pos.y > 0) {
        result.x =(int)((float)click_pos.x / UPSCALE_X);
        result.y =(int)((float)click_pos.y / UPSCALE_Y);
    } else {
        result.x = -1;
        result.y = -1;
    }
    click_pos.x = -1;
    click_pos.y = -1;
    return result;
}
