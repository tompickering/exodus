#include "input.h"

InputManager::InputManager() {
    mouse_pos.x = -1;
    mouse_pos.y = -1;
    click_pos.x = -1;
    click_pos.y = -1;
}

bool InputManager::read(Input input, bool reset) {
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

MousePos InputManager::read_click() {
    MousePos result = click_pos;
    click_pos.x = -1;
    click_pos.y = -1;
    return result;
}
