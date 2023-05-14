#include "game_over.h"

#include "audio/audio.h"

#include "assetpaths.h"

extern AUDIOMANAGER audio_manager;

enum ID {
    LEAVE,
    NEW_GAME,
    END,
};

GameOver::GameOver() : ModeBase("GameOver") {
}

void GameOver::enter() {
    ModeBase::enter(ID::END);

    draw_manager.draw(IMG_GAMEOVER);
    const char *reason = "";

    switch (ephstate.game_over_reason) {
        case GAMEOVER_Failed:
            reason = "MISSION FAILED";
            break;
        case GAMEOVER_Dead:
            reason = "YOU HAVE DIED";
            break;
        case GAMEOVER_AllHumansDead:
            reason = "ALL HUMANS HAVE FAILED";
            break;
    }

    ephstate.clear_ephemeral_state();

    draw_manager.draw_text(
        Font::Large,
        reason,
        Justify::Centre,
        RES_X/2, 100,
        COL_TEXT2);

    draw_manager.draw_text("You did not reach your aim.",
        Justify::Centre, RES_X/2, 160, COL_TEXT);
    draw_manager.draw_text("Others were stronger than you.",
        Justify::Centre, RES_X/2, 200, COL_TEXT);
    draw_manager.draw_text("Now you are just one of many",
        Justify::Centre, RES_X/2, 240, COL_TEXT);
    draw_manager.draw_text("who tried to gain the absolute power.",
        Justify::Centre, RES_X/2, 260, COL_TEXT);

    // TODO: This isn't the right music, though unused in orig
    audio_manager.target_music(MUS_ST1_10);

    stage = GO_FailMessage;
    quit = false;
}

ExodusMode GameOver::update(float delta) {
    switch (stage) {
        case GO_FailMessage:
            {
                if (draw_manager.clicked()) {
                    // TODO: Fade music
                    draw_manager.draw(TGT_Secondary, IMG_GAMEOVER);
                    draw_manager.draw_text(
                        TGT_Secondary,
                        id(ID::LEAVE),
                        Font::Large,
                        "Leave the game",
                        Justify::Centre,
                        RES_X/2, 200,
                        COL_TEXT);
                    draw_manager.draw_text(
                        TGT_Secondary,
                        id(ID::NEW_GAME),
                        Font::Large,
                        "Try a new game",
                        Justify::Centre,
                        RES_X/2, 260,
                        COL_TEXT);
                    draw_manager.pixelswap_start();
                    stage = GO_AgainOrNot;
                }
            }
            break;
        case GO_AgainOrNot:
            {
                draw_manager.show_cursor(true);
                if (draw_manager.query_click(id(ID::LEAVE)).id) {
                    quit = true;
                    draw_manager.fade_black(1.2f, 24);
                    stage = GO_FadeOut;
                }
                if (draw_manager.query_click(id(ID::NEW_GAME)).id) {
                    quit = false;
                    draw_manager.fade_black(1.2f, 24);
                    stage = GO_FadeOut;
                }
            }
            break;
        case GO_FadeOut:
            {
                if (!draw_manager.fade_active()) {
                    return quit ? ExodusMode::MODE_Quit : ExodusMode::MODE_NewGame;
                }
            }
            break;
    }

    return ExodusMode::MODE_None;
}
