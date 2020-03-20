#include "menu.h"

#include "../assetpaths.h"

enum ID {
    NEWGAME_TXT,
    LOADGAME_TXT,
    END,
};

Menu::Menu() : ModeBase("Menu") {
}

void Menu::enter() {
    ModeBase::enter(ID::END);
    draw_manager.pixelswap_draw(IMG_BG_STARS2);
    draw_manager.pixelswap_draw_text(Font::Large, "Please select.", Justify::Centre, RES_X/2, 135, {0xEE, 0xEE, 0xAA});
    draw_manager.pixelswap_draw_text(id(NEWGAME_TXT), Font::Large, "Conquer the stars", Justify::Centre, RES_X/2, 205, {0xFF, 0xFF, 0xFF});
    draw_manager.pixelswap_draw_text(id(LOADGAME_TXT), Font::Large, "Load a saved game", Justify::Centre, RES_X/2, 250, {0xFF, 0xFF, 0xFF});
    draw_manager.pixelswap_start();
    pixelswap_done = false;
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

    if (draw_manager.query_click(id(NEWGAME_TXT)).id) {
        L.debug("New game");
    }

    if (draw_manager.query_click(id(LOADGAME_TXT)).id) {
        L.debug("Load game");
    }

    return ExodusMode::MODE_None;
}
