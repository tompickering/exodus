#include "gamemode_select.h"

#include "exodus_features.h"

enum ID {
    ENHANCED_TXT,
    CLASSIC_TXT,
    END
};

GameModeSelect::GameModeSelect() : ModeBase("GameModeSelect") {
}

void GameModeSelect::enter() {
    ModeBase::enter(ID::END);

    draw_manager.draw(IMG_BG_STARS0);

    draw_manager.draw_text(
        "Please Select",
        Justify::Centre,
        RES_X/2, 40,
        COL_TEXT2);

    draw_manager.draw_text(
        "The ultimate experience",
        Justify::Centre,
        RES_X/2, 160,
        COL_TEXT2);

    draw_manager.draw_text(
        id(ENHANCED_TXT),
        Font::Large,
        "Enhanced",
        Justify::Centre,
        RES_X/2, 190,
        COL_TEXT);

    draw_manager.draw_text(
        "Minimal deviations from the 1997 release",
        Justify::Centre,
        RES_X/2, 320,
        COL_TEXT2);

    draw_manager.draw_text(
        id(CLASSIC_TXT),
        Font::Large,
        "Classic",
        Justify::Centre,
        RES_X/2, 350,
        COL_TEXT);

    draw_manager.show_cursor(true);
}

void GameModeSelect::exit() {
    ModeBase::exit();
}

ExodusMode GameModeSelect::update(float delta) {
    if (draw_manager.query_click(id(ENHANCED_TXT)).id) {
        exodus_set_game_mode(EXODUSGAMEMODE_Enhanced);
        return ExodusMode::MODE_Intro;
    }

    if (draw_manager.query_click(id(CLASSIC_TXT)).id) {
        exodus_set_game_mode(EXODUSGAMEMODE_Classic);
        return ExodusMode::MODE_Intro;
    }

    return ExodusMode::MODE_None;
}
