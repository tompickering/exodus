#include "menu.h"

#include "../assetpaths.h"

Menu::Menu() : ModeBase("Menu") {
}

void Menu::enter() {
    ModeBase::enter();
    draw_manager.pixelswap_draw(IMG_BG_STARS2);
    draw_manager.pixelswap_draw_text(Font::Large, "Please select.", Justify::Centre, RES_X/2, 135, {0xEE, 0xEE, 0xAA});
    draw_manager.pixelswap_draw_text(Font::Large, "Conquer the stars", Justify::Centre, RES_X/2, 205, {0xFF, 0xFF, 0xFF});
    draw_manager.pixelswap_draw_text(Font::Large, "Load a saved game", Justify::Centre, RES_X/2, 250, {0xFF, 0xFF, 0xFF});
    draw_manager.pixelswap_start();
    pixelswap_done = false;
}

void Menu::exit() {
    ModeBase::exit();
}

ExodusMode Menu::update(float delta) {
    if (draw_manager.pixelswap_active()) {
        return ExodusMode::MODE_None;
    }

    if (!pixelswap_done) {
        pixelswap_done = true;
        draw_manager.save_background();
        draw_manager.show_cursor(true);
    }

    return ExodusMode::MODE_None;
}
