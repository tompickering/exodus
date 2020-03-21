#include "menu.h"

#include "../assetpaths.h"

#define OPT_0 {RES_X - 95, 147, 1.0, 0.5, 1, 1}
#define OPT_1 {95, 280, 0.0, 0.5, 1, 1}
#define OPT_2 {RES_X - 95, 413, 1.0, 0.5, 1, 1}

#define TXT_0_X 95
#define TXT_0_Y 117
#define TXT_1_X RES_X - 95
#define TXT_1_Y 250
#define TXT_2_X 95
#define TXT_2_Y 383

enum ID {
    NEWGAME_TXT,
    LOADGAME_TXT,
    GAL_SZ_SMALL,
    GAL_SZ_MEDIUM,
    GAL_SZ_LARGE,
    NPLAYER_LR,
    NPLAYER_OK,
    NPLAYER_TXT,
    FLAG_0,
    FLAG_1,
    FLAG_2,
    FLAG_3,
    FLAG_4,
    FLAG_5,
    FLAG_6,
    FLAG_7,
    FLAG_8,
    FLAG_9,
    FLAG_10,
    FLAG_11,
    FLAG_12,
    FLAG_13,
    FLAG_14,
    AIM_MIGHT,
    AIM_MONEY,
    AIM_CIV,
    OTHERS_NONE,
    OTHERS_WEAK,
    OTHERS_MEDIUM,
    OTHERS_STRONG,
    END,
};

const char* flags[] = {
    IMG_TS1_FLAG13,
    IMG_TS1_FLAG14,
    IMG_TS1_FLAG15,
    IMG_TS1_FLAG10,
    IMG_TS1_FLAG11,
    IMG_TS1_FLAG12,
    IMG_TS1_FLAG7,
    IMG_TS1_FLAG8,
    IMG_TS1_FLAG9,
    IMG_TS1_FLAG4,
    IMG_TS1_FLAG5,
    IMG_TS1_FLAG6,
    IMG_TS1_FLAG1,
    IMG_TS1_FLAG2,
    IMG_TS1_FLAG3,
};

const char *diff_none_0 = "None";
const char *diff_none_1 = "The ultimate competition";
const char *diff_none_2 = "of the human race.";

const char *diff_weak_0 = "Weak";
const char *diff_weak_1 = "Your enemies start the";
const char *diff_weak_2 = "same way you do.";

const char *diff_medium_0 = "Advanced";
const char *diff_medium_1 = "Intelligence is needed";
const char *diff_medium_2 = "to stay alive.";

const char *diff_strong_0 = "Strong";
const char *diff_strong_1 = "A real challenge.";
const char *diff_strong_2 = "";

Menu::Menu() : ModeBase("Menu") {
}

void Menu::enter() {
    ModeBase::enter(ID::END);
    stage = Main;

    config.n_players = 1;
    current_player = 0;

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

    SprID id_diff_0;
    SprID id_diff_1;
    SprID id_diff_2;

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
                        id(GAL_SZ_LARGE),
                        IMG_STARTGR_GAL_L,
                        OPT_2);

                draw_manager.pixelswap_draw_text(
                        "Small",
                        Justify::Left, TXT_0_X, TXT_0_Y, {0xFF, 0xFF, 0xFF});
                draw_manager.pixelswap_draw_text(
                        "Desperate fights for the",
                        Justify::Left, TXT_0_X, TXT_0_Y + 20, {0xFF, 0xFF, 0xFF});
                draw_manager.pixelswap_draw_text(
                        "few resources.",
                        Justify::Left, TXT_0_X, TXT_0_Y + 40, {0xFF, 0xFF, 0xFF});

                draw_manager.pixelswap_draw_text(
                        "Medium",
                        Justify::Right, TXT_1_X, TXT_1_Y, {0xFF, 0xFF, 0xFF});
                draw_manager.pixelswap_draw_text(
                        "Hardly enough stars to",
                        Justify::Right, TXT_1_X, TXT_1_Y + 20, {0xFF, 0xFF, 0xFF});
                draw_manager.pixelswap_draw_text(
                        "live together in peace.",
                        Justify::Right, TXT_1_X, TXT_1_Y + 40, {0xFF, 0xFF, 0xFF});

                draw_manager.pixelswap_draw_text(
                        "Large",
                        Justify::Left, TXT_2_X, TXT_2_Y, {0xFF, 0xFF, 0xFF});
                draw_manager.pixelswap_draw_text(
                        "Free expansion",
                        Justify::Left, TXT_2_X, TXT_2_Y + 20, {0xFF, 0xFF, 0xFF});
                draw_manager.pixelswap_draw_text(
                        "into space.",
                        Justify::Left, TXT_2_X, TXT_2_Y + 40, {0xFF, 0xFF, 0xFF});

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
                draw_manager.draw(
                    id(ID::NPLAYER_LR),
                    IMG_BR11_LR,
                    {255, 250, 1.0, 0.5, 1, 1}
                    );
                draw_manager.draw(
                    id(ID::NPLAYER_OK),
                    IMG_BR11_OK,
                    {305, 250, 0, 0.5, 1, 1}
                    );
                draw_manager.pattern_fill({260, 230, 40, 40});
                draw_manager.save_background();
                trans_state = Done;
            }

            SpriteClick click;
            click = draw_manager.query_click(id(ID::NPLAYER_LR));
            if (click.id) {
                config.n_players += click.x > 0.5 ? 1 : -1;
                if (config.n_players < 1)
                    config.n_players = 1;
                if (config.n_players > 5)
                    config.n_players = 5;
            }

            if (draw_manager.query_click(id(ID::NPLAYER_OK)).id) {
                current_player = 0;
                set_stage(Name);
            }

            char n[2];
            n[0] = '1' + (config.n_players - 1);
            n[1] = '\0';
            draw_manager.draw_text(
                    id(ID::NPLAYER_TXT),
                    n,
                    Justify::Centre,
                    280, 240,
                    {0xFF, 0xFF, 0xFF});

            break;
        case Name:
            if (trans_state == None) {
                draw_manager.show_cursor(false);
                draw_manager.draw(IMG_BG_MENU0);

                char *txt;
                switch (current_player) {
                    case 0:
                        txt = (char*) "Player One, please identify yourself.";
                        break;
                    case 1:
                        txt = (char*) "Player Two, please identify yourself.";
                        break;
                    case 2:
                        txt = (char*) "Player Three, please identify yourself.";
                        break;
                    case 3:
                        txt = (char*) "Player Four, please identify yourself.";
                        break;
                    case 4:
                        txt = (char*) "Player Five, please identify yourself.";
                        break;
                    default:
                        txt = (char*) "-";
                        break;
                }

                draw_manager.draw_text(
                    txt, Justify::Centre, RES_X/2, 90, {0xFF, 0xFF, 0xFF});

                draw_manager.fill({260, 230, 240, 36}, {0, 0, 0});

                draw_manager.draw_text(
                        "Name:",
                        Justify::Right,
                        255, 238,
                        {0xEE, 0xEE, 0xAA});

                draw_manager.save_background();
                input_manager.start_text_input();

                trans_state = Done;
            }

            if (input_manager.consume(K_Backspace)) {
                input_manager.backspace();
                draw_manager.fill({260, 230, 240, 36}, {0, 0, 0});
            }

            draw_manager.draw_text(
                    input_manager.get_input_text(12),
                    Justify::Left,
                    265, 238,
                    {0xFF, 0xFF, 0xFF});

            if (input_manager.consume(K_Enter)) {
                set_stage(Title);
            }

            break;
        case Title:
            if (trans_state == None) {
                draw_manager.save_background();
                draw_manager.show_cursor(true);
                trans_state = Done;
            }

            set_stage(Flag);
            break;
        case Flag:
            if (trans_state == None) {
                draw_manager.draw(IMG_BG_MENU0);
                draw_manager.draw_text(
                    "Please choose one of the flags.",
                    Justify::Centre, RES_X/2, 60,
                    {0xEE, 0xEE, 0xAA});
                for (int j = 0; j < 5; ++j) {
                    for (int i = 0; i < 3; ++i) {
                        int flag_idx = (j*3) + i;
                        draw_manager.draw(
                            id(ID::FLAG_0 + flag_idx),
                            flags[flag_idx],
                            {RES_X/4 + i*RES_X/4,
                             140 + j*RES_Y/7,
                             0.5, 0.5, 1, 1});
                    }
                }
                draw_manager.save_background();
                draw_manager.show_cursor(true);
                trans_state = Done;
            }

            for (int i = 0; i < 15; ++i) {
                if (draw_manager.query_click(id(ID::FLAG_0 + i)).id) {
                    L.debug("Chose flag: %d", i);
                    set_stage(Aim);
                }
            }

            break;
        case Aim:
            if (trans_state == None) {
                draw_manager.draw(IMG_BG_MENU0);
                draw_manager.draw_text(
                    Font::Large,
                    "The aim",
                    Justify::Left,
                    20, 30, {0xEE, 0xEE, 0xAA});

                draw_manager.draw(
                        id(AIM_MIGHT),
                        IMG_STARTGR_AIM_MIGHT,
                        OPT_0);
                draw_manager.draw(
                        id(AIM_MONEY),
                        IMG_STARTGR_AIM_MONEY,
                        OPT_1);
                draw_manager.draw(
                        id(AIM_CIV),
                        IMG_STARTGR_AIM_CIV,
                        OPT_2);

                draw_manager.draw_text(
                        "Might",
                        Justify::Left, TXT_0_X, TXT_0_Y, {0xFF, 0xFF, 0xFF});
                draw_manager.draw_text(
                        "Conquer the galaxy",
                        Justify::Left, TXT_0_X, TXT_0_Y + 20, {0xFF, 0xFF, 0xFF});
                draw_manager.draw_text(
                        "within 200 months.",
                        Justify::Left, TXT_0_X, TXT_0_Y + 40, {0xFF, 0xFF, 0xFF});

                draw_manager.draw_text(
                        "Money",
                        Justify::Right, TXT_1_X, TXT_1_Y, {0xFF, 0xFF, 0xFF});
                draw_manager.draw_text(
                        "Earn 1000 MC monthly",
                        Justify::Right, TXT_1_X, TXT_1_Y + 20, {0xFF, 0xFF, 0xFF});
                draw_manager.draw_text(
                        "after 200 months.",
                        Justify::Right, TXT_1_X, TXT_1_Y + 40, {0xFF, 0xFF, 0xFF});

                draw_manager.draw_text(
                        "Civilization",
                        Justify::Left, TXT_2_X, TXT_2_Y, {0xFF, 0xFF, 0xFF});
                draw_manager.draw_text(
                        "Own 30 planets and a high",
                        Justify::Left, TXT_2_X, TXT_2_Y + 20, {0xFF, 0xFF, 0xFF});
                draw_manager.draw_text(
                        "technology after 350 months.",
                        Justify::Left, TXT_2_X, TXT_2_Y + 40, {0xFF, 0xFF, 0xFF});

                draw_manager.save_background();
                trans_state = Done;
            }

            if (draw_manager.query_click(id(AIM_MIGHT)).id) {
                set_stage(Difficulty);
            } else if (draw_manager.query_click(id(AIM_MONEY)).id) {
                set_stage(Difficulty);
            } else if (draw_manager.query_click(id(AIM_CIV)).id) {
                set_stage(Difficulty);
            }

            break;
        case Difficulty:
            if (trans_state == None) {
                draw_manager.draw(IMG_BG_MENU0);
                draw_manager.draw_text(
                    Font::Large,
                    "Other races",
                    Justify::Left,
                    20, 30, {0xEE, 0xEE, 0xAA});

                // TODO: Multiplayer variants
                id_diff_0 = id(OTHERS_WEAK);
                id_diff_1 = id(OTHERS_MEDIUM);
                id_diff_2 = id(OTHERS_STRONG);
                const char* img_diff_0 = IMG_STARTGR_STR_WEAK;
                const char* img_diff_1 = IMG_STARTGR_STR_MEDIUM;
                const char* img_diff_2 = IMG_STARTGR_STR_STRONG;
                const char* txt0_0 = diff_weak_0;
                const char* txt0_1 = diff_weak_1;
                const char* txt0_2 = diff_weak_2;
                const char* txt1_0 = diff_medium_0;
                const char* txt1_1 = diff_medium_1;
                const char* txt1_2 = diff_medium_2;
                const char* txt2_0 = diff_strong_0;
                const char* txt2_1 = diff_strong_1;
                const char* txt2_2 = diff_strong_2;

                draw_manager.draw(
                        id_diff_0,
                        img_diff_0,
                        OPT_0);
                draw_manager.draw(
                        id_diff_1,
                        img_diff_1,
                        OPT_1);
                draw_manager.draw(
                        id_diff_2,
                        img_diff_2,
                        OPT_2);

                draw_manager.draw_text(
                        txt0_0,
                        Justify::Left,
                        TXT_0_X, TXT_0_Y,
                        {0xFF, 0xFF, 0xFF});
                draw_manager.draw_text(
                        txt0_1,
                        Justify::Left,
                        TXT_0_X, TXT_0_Y + 20,
                        {0xFF, 0xFF, 0xFF});
                draw_manager.draw_text(
                        txt0_2,
                        Justify::Left,
                        TXT_0_X, TXT_0_Y + 40,
                        {0xFF, 0xFF, 0xFF});

                draw_manager.draw_text(
                        txt1_0,
                        Justify::Right, TXT_1_X, TXT_1_Y, {0xFF, 0xFF, 0xFF});
                draw_manager.draw_text(
                        txt1_1,
                        Justify::Right,
                        TXT_1_X, TXT_1_Y + 20,
                        {0xFF, 0xFF, 0xFF});
                draw_manager.draw_text(
                        txt1_2,
                        Justify::Right,
                        TXT_1_X, TXT_1_Y + 40,
                        {0xFF, 0xFF, 0xFF});

                draw_manager.draw_text(
                        txt2_0,
                        Justify::Left,
                        TXT_2_X, TXT_2_Y,
                        {0xFF, 0xFF, 0xFF});
                draw_manager.draw_text(
                        txt2_1,
                        Justify::Left,
                        TXT_2_X, TXT_2_Y + 20,
                        {0xFF, 0xFF, 0xFF});
                draw_manager.draw_text(
                        txt2_2,
                        Justify::Left,
                        TXT_2_X, TXT_2_Y + 40,
                        {0xFF, 0xFF, 0xFF});

                draw_manager.save_background();
                trans_state = Done;
            }

            if (draw_manager.query_click(id_diff_0).id) {
                set_stage(Confirm);
            } else if (draw_manager.query_click(id_diff_1).id) {
                set_stage(Confirm);
            } else if (draw_manager.query_click(id_diff_2).id) {
                set_stage(Confirm);
            }
            break;
        case Confirm:
            if (trans_state == None) {
                draw_manager.draw(IMG_BG_STARS0);
                draw_manager.draw_text(
                    "Let me repeat.",
                    Justify::Centre, RES_X/2, 60,
                    {0xEE, 0xEE, 0xAA});

                draw_manager.save_background();
            }
            break;
    }

    return ExodusMode::MODE_None;
}

void Menu::set_stage(Stage new_stage) {
    trans_state = None;
    stage = new_stage;
}
