#include "menu.h"

#include "../assetpaths.h"

#define OPT_0 {RES_X - 95, 147, 1.0, 0.5, 1, 1}
#define OPT_1 {95, 280, 0.0, 0.5, 1, 1}
#define OPT_2 {RES_X - 95, 413, 1.0, 0.5, 1, 1}

enum ID {
    NEWGAME_TXT,
    LOADGAME_TXT,
    GAL_SZ_SMALL,
    GAL_SZ_MEDIUM,
    GAL_SZ_LARGE,
    END,
};

Menu::Menu() : ModeBase("Menu") {
}

void Menu::enter() {
    ModeBase::enter(ID::END);
    stage = Main;
    draw_manager.pixelswap_draw(IMG_BG_STARS2);
    draw_manager.pixelswap_draw_text(Font::Large, "Please select.", Justify::Centre, RES_X/2, 135, {0xEE, 0xEE, 0xAA});
    draw_manager.pixelswap_draw_text(id(NEWGAME_TXT), Font::Large, "Conquer the stars", Justify::Centre, RES_X/2, 205, {0xFF, 0xFF, 0xFF});
    draw_manager.pixelswap_draw_text(id(LOADGAME_TXT), Font::Large, "Load a saved game", Justify::Centre, RES_X/2, 250, {0xFF, 0xFF, 0xFF});
    draw_manager.pixelswap_start();
    trans_state = Started;
}

/*
 * Main > size > #humans > name/geneder/welcome > flag > aim > other races > confirm
 */
ExodusMode Menu::update(float delta) {
    if (draw_manager.pixelswap_active() || draw_manager.fade_active()) {
        draw_manager.show_cursor(false);
        return ExodusMode::MODE_None;
    }

    switch(stage) {
        case Main:
            if (trans_state == Started) {
                trans_state = Done;
                draw_manager.save_background();
                draw_manager.show_cursor(true);
            }

            if (draw_manager.query_click(id(NEWGAME_TXT)).id) {
                set_stage(Size);
                draw_manager.fade_black(1.2f, 24);
            }

            if (draw_manager.query_click(id(LOADGAME_TXT)).id) {
                L.debug("Load game");
            }
            break;
        case Load:
            break;
        case Size:
            if (trans_state == None) {
                draw_manager.pixelswap_draw(IMG_BG_MENU0);
                draw_manager.pixelswap_draw_text(Font::Large, "Galaxy size", Justify::Left, 20, 30, {0xEE, 0xEE, 0xAA});
                draw_manager.pixelswap_draw(
                        id(GAL_SZ_SMALL),
                        IMG_STARTGR_GAL_S,
                        OPT_0);
                draw_manager.pixelswap_draw(
                        id(GAL_SZ_MEDIUM),
                        IMG_STARTGR_GAL_M,
                        OPT_1);
                draw_manager.pixelswap_draw(
                        id(GAL_SZ_MEDIUM),
                        IMG_STARTGR_GAL_L,
                        OPT_2);
                draw_manager.pixelswap_start();
                trans_state = Started;
            } else if (trans_state == Started) {
                trans_state = Done;
                draw_manager.save_background();
                draw_manager.show_cursor(true);
            } else {
                bool progress = false;
                if (draw_manager.query_click(id(GAL_SZ_SMALL)).id) {
                    config.size = GAL_Small;
                    progress = true;
                } else if (draw_manager.query_click(id(GAL_SZ_MEDIUM)).id) {
                    config.size = GAL_Medium;
                    progress = true;
                } else if (draw_manager.query_click(id(GAL_SZ_LARGE)).id) {
                    config.size = GAL_Large;
                    progress = true;
                }

                if (progress)
                    set_stage(NPlayers);
            }

            break;
        case NPlayers:
            if (trans_state == None) {
                draw_manager.draw(IMG_BG_MENU0);
                draw_manager.draw_text(
                        "How many human players wish to play?",
                        Justify::Centre, RES_X/2, 90, {0xEE, 0xEE, 0xAA});
                trans_state = Done;
            }

            break;
        case Name:
            break;
        case Title:
            break;
        case Flag:
            break;
        case Aim:
            break;
        case Difficulty:
            break;
        case Confirm:
            break;
    }

    return ExodusMode::MODE_None;
}

void Menu::set_stage(Stage new_stage) {
    trans_state = None;
    stage = new_stage;
}
