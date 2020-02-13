#include "menu.h"

#include "../assetpaths.h"

Menu::Menu() : ModeBase("Menu") {
}

void Menu::enter() {
    ModeBase::enter();
    draw_manager.pixelswap_draw(IMG_MENU_BG_INITIAL);
    draw_manager.pixelswap_start();
}

void Menu::exit() {
    ModeBase::exit();
}

ExodusMode Menu::update(float delta) {
    if (draw_manager.pixelswap_active()) {
        return ExodusMode::MODE_None;
    }

    return ExodusMode::MODE_None;
}
