#include "menu.h"

#include "state/exodus_state.h"

#include "audio/audio.h"

#include "exodus_features.h"

#include "assetpaths.h"

#define OPT_0_X (RES_X - 295)
#define OPT_0_Y 97
#define OPT_1_X 95
#define OPT_1_Y 230
#define OPT_2_X OPT_0_X
#define OPT_2_Y 363

#define OPT_0 {OPT_0_X, OPT_0_Y, 0, 0, 1, 1}
#define OPT_1 {OPT_1_X, OPT_1_Y, 0, 0, 1, 1}
#define OPT_2 {OPT_2_X, OPT_2_Y, 0, 0, 1, 1}

#define TXT_0_X 95
#define TXT_0_Y 117
#define TXT_1_X RES_X - 95
#define TXT_1_Y 250
#define TXT_2_X 95
#define TXT_2_Y 383

extern AUDIOMANAGER audio_manager;

const int LOADFRAME_W = 436;
const int LOADFRAME_H = 306;
const int LOADFRAME_X = RES_X/2 - LOADFRAME_W/2;
const int LOADFRAME_Y = RES_Y/2 - LOADFRAME_H/2;
const int LOADFRAME_STEP = 20;
const int LOADFRAME_MONTH_X = LOADFRAME_X + 240;
const int LOADFRAME_PLANETS_X = LOADFRAME_MONTH_X + 120;

const float DELAY_TIME = 0.4f;

extern SAVEMANAGER save_manager;

enum ID {
    NEWGAME_TXT,
    LOADGAME_TXT,
    CONTINUE_TXT,
    AUTOSAVE_WARN_TXT0,
    AUTOSAVE_WARN_TXT1,
    AUTOSAVE_WARN_TXT2,
    MODE_SELECT,
    MODE_SELECT_TXT,
    LOADFRAME,
    LOADFRAME_INNER,
    LOADFRAME_EXIT,
    BUTTON_BG_0,
    BUTTON_BG_1,
    BUTTON_BG_2,
    BUTTON_BG_3,
    BUTTON_BG_4,
    BUTTON_BG_5,
    BUTTON_BG_6,
    BUTTON_BG_7,
    BUTTON_BG_8,
    BUTTON_BG_9,
    BUTTON_BG_10,
    BUTTON_BG_11,
    BUTTON_BG_12,
    BUTTON_BG_13,
    BUTTON_BG_14,
    GAL_SZ_SMALL,
    GAL_SZ_MEDIUM,
    GAL_SZ_LARGE,
    NPLAYER_LR,
    NPLAYER_OK,
    NPLAYER_TXT,
    PLAYER_NAME,
    PLAYER_TITLE,
    PLAYER_REF,
    GENDER_MALE,
    GENDER_FEMALE,
    GENDER_OTHER,
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
    CONFIRM_FLAG,
    PROCEED,
    RESTART,
    END,
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
    trans_state = None;

    audio_manager.target_music(MUS_INTRO);

    for (int i = 0; i < MAX_SLOTS; ++i) {
        load_game_ids[i] = draw_manager.new_sprite_id();
    }

    savemeta = save_manager.get_all_meta(true);
    autosave_warned = false;
}

void Menu::exit() {
    for (int i = 0; i < MAX_SLOTS; ++i) {
        draw_manager.unset_selectable(load_game_ids[i]);
        draw_manager.draw(load_game_ids[i], nullptr);
        draw_manager.release_sprite_id(load_game_ids[i]);
    }
    ModeBase::exit();
}

SprID id_diff_0;
SprID id_diff_1;
SprID id_diff_2;

/*
 * Main > size > #humans > name/geneder/welcome > flag > aim > other races > confirm
 */
ExodusMode Menu::update(float delta) {
    if (draw_manager.pixelswap_active() || (stage != Main && draw_manager.fade_active())) {
        return ExodusMode::MODE_None;
    }

    switch(stage) {
        case Main:
            if (trans_state == None) {
                config.n_players = 1;
                current_player = 0;

                const char *bg = IMG_MENU_STARS;

                if (!ENHANCED()) {
                    switch (rand() % 8) {
                        case 0:
                            bg = IMG_BG_STARS0;
                            break;
                        case 1:
                            bg = IMG_BG_STARS1;
                            break;
                        case 2:
                            bg = IMG_BG_STARS2;
                            break;
                        case 3:
                            bg = IMG_BG_STARS3;
                            break;
                        case 4:
                            bg = IMG_BG_STARS4;
                            break;
                        case 5:
                            bg = IMG_BG_STARS5;
                            break;
                        case 6:
                            bg = IMG_BG_STARS6;
                            break;
                        case 7:
                            bg = IMG_BG_STARS7;
                            break;
                    }
                }

                draw_manager.draw(TGT_Secondary, bg);

                draw_manager.draw_text(TGT_Secondary,
                        Font::Large,
                        "Please select.",
                        Justify::Centre,
                        RES_X/2, 135,
                        COL_TEXT2);

                draw_manager.draw_text(TGT_Secondary,
                        id(NEWGAME_TXT),
                        Font::Large,
                        "Conquer the stars",
                        Justify::Centre,
                        RES_X/2, 205,
                        COL_TEXT);

                draw_manager.draw_text(TGT_Secondary,
                        id(LOADGAME_TXT),
                        Font::Large,
                        "Load a saved game",
                        Justify::Centre,
                        RES_X/2, 250,
                        COL_TEXT);

                if (savemeta[0].exists) {
                    draw_manager.draw_text(TGT_Secondary,
                            id(CONTINUE_TXT),
                            Font::Large,
                            "Continue",
                            Justify::Centre,
                            RES_X/2, 295,
                            COL_TEXT);
                }

                draw_manager.fill(
                    TGT_Secondary,
                    {0, RES_Y-40,
                     RES_X, 40},
                     {0x08, 0x08, 0x08});

                draw_manager.draw_text(
                        TGT_Secondary,
                        id(MODE_SELECT_TXT),
                        "Enhanced Version",
                        Justify::Centre,
                        RES_X/2 + 12, RES_Y - 31,
                        COL_TEXT2);

                draw_manager.draw(TGT_Secondary,
                        id(MODE_SELECT),
                        ENHANCED() ? IMG_BOX_ON : IMG_BOX_OFF,
                        {RES_X/2 - 90, RES_Y - 20,
                         0.5f, 0.5f, 1.0f, 1.0f});

                draw_manager.pixelswap_start();
                trans_state = Started;
                return ExodusMode::MODE_None;
            }

            if (trans_state == Started) {
                trans_state = Done;
                draw_manager.save_background();
                draw_manager.show_cursor(true);
            }

            if ((draw_manager.query_click(id(MODE_SELECT)).id) || (draw_manager.query_click(id(MODE_SELECT_TXT)).id)) {
                if (ENHANCED()) {
                    exodus_set_game_mode(EXODUSGAMEMODE_Classic);
                } else {
                    exodus_set_game_mode(EXODUSGAMEMODE_Enhanced);
                }

                return ExodusMode::MODE_Reload;
            }

            if (draw_manager.query_click(id(NEWGAME_TXT)).id) {
                if (savemeta[0].exists && !autosave_warned) {
                    autosave_warned = true;

                    draw_manager.draw_text(
                        id(AUTOSAVE_WARN_TXT0),
                        "Warning:",
                        Justify::Centre,
                        RES_X/2, 40,
                        COL_TEXT2);
                    draw_manager.draw_text(
                        id(AUTOSAVE_WARN_TXT1),
                        "This will overwrite the existing autosave.",
                        Justify::Centre,
                        RES_X/2, 60,
                        COL_TEXT2);
                    draw_manager.draw_text(
                        id(AUTOSAVE_WARN_TXT2),
                        "Please click again to confirm.",
                        Justify::Centre,
                        RES_X/2, 80,
                        COL_TEXT2);
                } else {
                    new (&exostate()) ExodusState();
                    new (&ephstate) EphemeralState();
                    set_stage(Size);
                    draw_manager.fade_black(1.2f, 24);
                }
            }

            if (draw_manager.query_click(id(LOADGAME_TXT)).id) {
                draw_manager.fill(
                    id(LOADFRAME),
                    {LOADFRAME_X-4, LOADFRAME_Y-4,
                     LOADFRAME_W+8, LOADFRAME_H+8},
                    COL_BORDERS);

                draw_manager.fill(
                    FILL_3D_Out,
                    {LOADFRAME_X-4, LOADFRAME_Y-4,
                     LOADFRAME_W+8, LOADFRAME_H+8},
                    COL_BORDERS);

                draw_manager.fill_pattern(
                    id(LOADFRAME_INNER),
                    {LOADFRAME_X, LOADFRAME_Y,
                     LOADFRAME_W, LOADFRAME_H});

                draw_manager.draw_text(
                    "Load game:",
                    Justify::Left,
                    LOADFRAME_X+4, LOADFRAME_Y+2,
                    COL_TEXT2);

                draw_manager.draw_text(
                    "Month",
                    Justify::Centre,
                    LOADFRAME_MONTH_X, LOADFRAME_Y+2,
                    COL_TEXT_GREYED);

                draw_manager.draw_text(
                    "Planets",
                    Justify::Centre,
                    LOADFRAME_PLANETS_X, LOADFRAME_Y+2,
                    COL_TEXT_GREYED);

                draw_manager.draw_text(
                    id(LOADFRAME_EXIT),
                    "Exit menu",
                    Justify::Left,
                    LOADFRAME_X+4, LOADFRAME_Y+2+(14*LOADFRAME_STEP),
                    COL_TEXT2);

                char n[8];
                for (int i = 0; i < MAX_SLOTS; ++i) {
                    if (i == 0) {
                        snprintf(n, sizeof(n), "AUTO");
                    } else {
                        snprintf(n, sizeof(n), "%d", i);
                    }

                    draw_manager.draw_text(
                        n,
                        Justify::Left,
                        LOADFRAME_X+4, LOADFRAME_Y+2+((i+2)*LOADFRAME_STEP),
                        COL_TEXT);
                    if (savemeta[i].exists) {
                        snprintf(n, sizeof(n), "%d", savemeta[i].month);
                        draw_manager.draw_text(
                            n,
                            Justify::Centre,
                            LOADFRAME_MONTH_X, LOADFRAME_Y+2+((i+2)*LOADFRAME_STEP),
                            COL_TEXT);
                        snprintf(n, sizeof(n), "%d", savemeta[i].planets);
                        draw_manager.draw_text(
                            n,
                            Justify::Centre,
                            LOADFRAME_PLANETS_X, LOADFRAME_Y+2+((i+2)*LOADFRAME_STEP),
                            COL_TEXT);
                    } else {
                        draw_manager.draw_text(
                            "Empty",
                            Justify::Left,
                            LOADFRAME_X+4+80, LOADFRAME_Y+2+((i+2)*LOADFRAME_STEP),
                            COL_TEXT_GREYED);
                    }
                }

                for (int i = 0; i < MAX_SLOTS; ++i) {
                    draw_manager.set_selectable(load_game_ids[i]);
                }

                stage = Load;
                return ExodusMode::MODE_None;
            }

            if (draw_manager.query_click(id(CONTINUE_TXT)).id) {
                load_slot = 0;
                set_stage(LoadStart);
                return ExodusMode::MODE_None;
            }
            break;
        case Load:
            {
                if (draw_manager.query_click(id(LOADFRAME_EXIT)).id) {
                    set_stage(Main);
                    return ExodusMode::MODE_None;
                }

                for (int i = 0; i < MAX_SLOTS; ++i) {
                    if (savemeta[i].exists) {
                        draw_manager.draw_text(
                            load_game_ids[i],
                            savemeta[i].name,
                            Justify::Left,
                            LOADFRAME_X+4+80, LOADFRAME_Y+2+((i+2)*LOADFRAME_STEP),
                            COL_TEXT);

                        if (draw_manager.query_click(load_game_ids[i]).id) {
                            load_slot = i;
                            set_stage(LoadStart);
                            return ExodusMode::MODE_None;
                        }
                    }
                }
            }
            break;
        case LoadStart:
            {
                int i = load_slot;
                L.debug("LOAD GAME: %s", savemeta[i].name);

                if (i > 0) {
                    QUICKSAVE_SLOT = i;
                }

                if (save_manager.load(i)) {
                    draw_manager.draw(ENHANCED() ? IMG_BG_WELCOME : IMG_BG_STARS2);

                    Player *p = exostate().get_player(0);
                    char welcome[64];
                    snprintf(welcome, sizeof(welcome), "Welcome, %s.", p->get_full_name());
                    draw_manager.draw_text(
                            Font::Large,
                            welcome,
                            Justify::Centre,
                            RES_X/2, 200,
                            COL_TEXT2);

                    set_stage(LoadWelcome);
                    return ExodusMode::MODE_None;
                } else {
                    L.error("Load failed");
                    return ExodusMode::MODE_Reload;
                }
            }
            break;
        case LoadWelcome:
            {
                if (timer > 1.5f) {
                    draw_manager.draw(TGT_Secondary, ENHANCED() ? IMG_BG_WELCOME : IMG_BG_STARS2);
                    draw_manager.pixelswap_start();
                    stage = LoadWelcomeFade;
                    return ExodusMode::MODE_None;
                }

                timer += delta;
            }
            break;
        case LoadWelcomeFade:
            {
                if (!draw_manager.pixelswap_active()) {
                    stage = LoadFinish;
                    return ExodusMode::MODE_None;
                }
            }
            break;
        case LoadFinish:
            {
                ephstate.galaxymap_pixelswap = true;
                return ExodusMode::MODE_GalaxyMap;
            }
            break;
        case Size:
            if (trans_state == None) {
                draw_manager.draw(TGT_Secondary, IMG_BG_MENU0);
                draw_manager.draw_text(TGT_Secondary, Font::Large, "Galaxy size", Justify::Left, 20, 30, COL_TEXT2);

                draw_manager.fill(FILL_3D_Out, TGT_Secondary, {OPT_0_X-6, OPT_0_Y-6, 212, 112}, COL_BORDERS);
                draw_manager.fill(FILL_3D_Out, TGT_Secondary, {OPT_1_X-6, OPT_1_Y-6, 212, 112}, COL_BORDERS);
                draw_manager.fill(FILL_3D_Out, TGT_Secondary, {OPT_2_X-6, OPT_2_Y-6, 212, 112}, COL_BORDERS);

                draw_border_opt(TGT_Secondary, 0, {0, 0, 0});
                draw_border_opt(TGT_Secondary, 1, {0, 0, 0});
                draw_border_opt(TGT_Secondary, 2, {0, 0, 0});

                draw_manager.draw(TGT_Secondary,
                        id(GAL_SZ_SMALL),
                        IMG_STARTGR_GAL_S,
                        OPT_0);
                draw_manager.draw(TGT_Secondary,
                        id(GAL_SZ_MEDIUM),
                        IMG_STARTGR_GAL_M,
                        OPT_1);
                draw_manager.draw(TGT_Secondary,
                        id(GAL_SZ_LARGE),
                        IMG_STARTGR_GAL_L,
                        OPT_2);

                draw_manager.draw_text(TGT_Secondary,
                        "Small",
                        Justify::Left, TXT_0_X, TXT_0_Y, COL_TEXT2);
                draw_manager.draw_text(TGT_Secondary,
                        "Desperate fights for the",
                        Justify::Left, TXT_0_X, TXT_0_Y + 20, COL_TEXT);
                draw_manager.draw_text(TGT_Secondary,
                        "few resources.",
                        Justify::Left, TXT_0_X, TXT_0_Y + 40, COL_TEXT);

                draw_manager.draw_text(TGT_Secondary,
                        "Medium",
                        Justify::Right, TXT_1_X, TXT_1_Y, COL_TEXT2);
                draw_manager.draw_text(TGT_Secondary,
                        "Hardly enough stars to",
                        Justify::Right, TXT_1_X, TXT_1_Y + 20, COL_TEXT);
                draw_manager.draw_text(TGT_Secondary,
                        "live together in peace.",
                        Justify::Right, TXT_1_X, TXT_1_Y + 40, COL_TEXT);

                draw_manager.draw_text(TGT_Secondary,
                        "Large",
                        Justify::Left, TXT_2_X, TXT_2_Y, COL_TEXT2);
                draw_manager.draw_text(TGT_Secondary,
                        "Free expansion",
                        Justify::Left, TXT_2_X, TXT_2_Y + 20, COL_TEXT);
                draw_manager.draw_text(TGT_Secondary,
                        "into space.",
                        Justify::Left, TXT_2_X, TXT_2_Y + 40, COL_TEXT);

                draw_manager.pixelswap_start();
                trans_state = Started;
            } else if (trans_state == Started) {
                trans_state = Done;
                draw_manager.save_background();
                draw_manager.show_cursor(true);
            } else {
                if (draw_manager.query_click(id(GAL_SZ_SMALL)).id) {
                    config.size = GAL_Small;
                    draw_border_opt(0, {0, 0, 0xFF});
                    delay_stage(NPlayers, 0, id(GAL_SZ_SMALL), IMG_STARTGR_GAL_S, OPT_0);
                } else if (draw_manager.query_click(id(GAL_SZ_MEDIUM)).id) {
                    config.size = GAL_Medium;
                    draw_border_opt(1, {0, 0, 0xFF});
                    delay_stage(NPlayers, 1, id(GAL_SZ_MEDIUM), IMG_STARTGR_GAL_M, OPT_1);
                } else if (draw_manager.query_click(id(GAL_SZ_LARGE)).id) {
                    config.size = GAL_Large;
                    draw_border_opt(2, {0, 0, 0xFF});
                    delay_stage(NPlayers, 2, id(GAL_SZ_LARGE), IMG_STARTGR_GAL_L, OPT_2);
                }
            }

            break;
        case NPlayers:
            if (FEATURE(EF_DISABLE_MULTIPLAYER)) {
                config.n_players = 1;
                current_player = 0;
                set_stage(Name);
            } else {
                if (trans_state == None) {
                    draw_manager.draw(IMG_BG_MENU0);
                    draw_manager.draw_text(
                        "How many human players wish to play?",
                        Justify::Centre, RES_X/2, 90, COL_TEXT2);
                    draw_manager.draw(
                        id(ID::NPLAYER_LR),
                        IMG_BR11_LR,
                        {255, 250, 1.0, 0.5, 1, 1});
                    draw_manager.draw(
                        id(ID::NPLAYER_OK),
                        IMG_BR11_OK,
                        {305, 250, 0, 0.5, 1, 1});
                    draw_manager.fill(
                        {264-BORDER, 234-BORDER, 32+2*BORDER, 32+2*BORDER},
                        COL_BORDERS);
                    draw_manager.fill_pattern({264, 234, 32, 32});
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
                        COL_TEXT);
            }

            break;
        case Name:
            if (trans_state == None) {
                draw_manager.show_cursor(false);
                draw_manager.draw(IMG_BG_MENU0);

                const char *txt = nullptr;

                if (FEATURE(EF_DISABLE_MULTIPLAYER)) {
                    txt = "Please identify yourself.";
                } else {
                    switch (current_player) {
                        case 0:
                            txt = "Player One, please identify yourself.";
                            break;
                        case 1:
                            txt = "Player Two, please identify yourself.";
                            break;
                        case 2:
                            txt = "Player Three, please identify yourself.";
                            break;
                        case 3:
                            txt = "Player Four, please identify yourself.";
                            break;
                        case 4:
                            txt = "Player Five, please identify yourself.";
                            break;
                        default:
                            txt = "-";
                            break;
                    }
                }

                draw_manager.draw_text(
                    txt, Justify::Centre, RES_X/2, 90, COL_TEXT);

                draw_manager.fill(
                    FILL_3D_In,
                    {264-BORDER, 234-BORDER, 232+2*BORDER, 28+2*BORDER},
                    COL_BORDERS);
                draw_manager.fill({264, 234, 232, 28}, {0, 0, 0});

                draw_manager.draw_text(
                        "Name:",
                        Justify::Right,
                        255, 238,
                        COL_TEXT2);

                draw_manager.save_background();
                input_manager.start_text_input();
                draw_manager.enable_text_cursor(id(ID::PLAYER_NAME));

                trans_state = Done;
            }

            if (input_manager.consume(K_Backspace)) {
                input_manager.backspace();
                draw_manager.fill(
                    FILL_3D_In,
                    {264-BORDER, 234-BORDER, 232+2*BORDER, 28+2*BORDER},
                    COL_BORDERS);
                draw_manager.fill({264, 234, 232, 28}, {0, 0, 0});
            }

            {
                const char* input_name = input_manager.get_input_text(MAX_PLAYER_NAME);

                draw_manager.draw_text(
                        id(ID::PLAYER_NAME),
                        input_name,
                        Justify::Left,
                        270, 238,
                        COL_TEXT);

                if (input_manager.consume(K_Enter) && strnlen(input_name, 1)) {
                    bool duplicate = false;
                    for (int i = 0; i < current_player; ++i) {
                        if (!strncmp(input_name, config.players[i].get_name(), MAX_PLAYER_NAME)) {
                            duplicate = true;
                            break;
                        }
                    }
                    if (!duplicate) {
                        config.players[current_player].set_name(input_name);
                        input_manager.stop_text_input();
                        draw_manager.disable_text_cursor();
                        set_stage(Gender);
                    } else {
                        L.info("Not allowing duplicate name");
                    }
                }
            }

            break;
        case Gender:
            {
                if (timer == 0) {
                    draw_manager.show_cursor(true);

                    draw_manager.draw_text(
                        "You are...",
                        Justify::Centre,
                        RES_X/2, 300,
                        COL_TEXT);

                    draw_manager.draw(
                        id(ID::GENDER_MALE),
                        IMG_BR13_MALE,
                        {RES_X/3, 340,
                         1, .5, 1, 1});
                    draw_manager.draw(
                        id(ID::GENDER_FEMALE),
                        IMG_BR13_FEMALE,
                        {RES_X/2, 340,
                         .5, .5, 1, 1});
                    draw_manager.draw(
                        id(ID::GENDER_OTHER),
                        IMG_BUTTON_OTHER,
                        {2*RES_X/3, 340,
                         0, .5, 1, 1});
                }

                timer += delta;

                if (draw_manager.query_click(id(ID::GENDER_MALE)).id) {
                    config.players[current_player].set_gender(GENDER_Male);
                    config.players[current_player].set_title("Lord");
                    config.players[current_player].set_ref("Milord");
                    set_stage(Welcome);
                }

                if (draw_manager.query_click(id(ID::GENDER_FEMALE)).id) {
                    config.players[current_player].set_gender(GENDER_Female);
                    config.players[current_player].set_title("Lady");
                    config.players[current_player].set_ref("Milady");
                    set_stage(Welcome);
                }

                if (draw_manager.query_click(id(ID::GENDER_OTHER)).id) {
                    config.players[current_player].set_gender(GENDER_Other);
                    set_stage(CustomTitle);
                }
            }
            break;
        case CustomTitle:
            if (trans_state == None) {
                draw_manager.show_cursor(false);
                draw_manager.draw(IMG_BG_MENU0);

                draw_manager.draw_text(
                    "(E.g. 'Lord', 'Lady'...)",
                    Justify::Centre,
                    RES_X/2, 90,
                    COL_TEXT);

                draw_manager.fill(
                    FILL_3D_In,
                    {264-BORDER, 134-BORDER, 232+2*BORDER, 28+2*BORDER},
                    COL_BORDERS);
                draw_manager.fill({264, 134, 232, 28}, {0, 0, 0});

                draw_manager.draw_text(
                        "Title:",
                        Justify::Right,
                        255, 138,
                        COL_TEXT2);

                draw_manager.save_background();
                input_manager.start_text_input();
                input_manager.set_input_text("Regent");
                draw_manager.enable_text_cursor(id(ID::PLAYER_TITLE));

                trans_state = Done;
            }

            if (input_manager.consume(K_Backspace)) {
                input_manager.backspace();
                draw_manager.fill(
                    FILL_3D_In,
                    {264-BORDER, 134-BORDER, 232+2*BORDER, 28+2*BORDER},
                    COL_BORDERS);
                draw_manager.fill({264, 134, 232, 28}, {0, 0, 0});
            }

            {
                const char* input_title = input_manager.get_input_text(MAX_PLAYER_TITLE);

                draw_manager.draw_text(
                        id(ID::PLAYER_TITLE),
                        input_title,
                        Justify::Left,
                        270, 138,
                        COL_TEXT);

                if (input_manager.consume(K_Enter) && strnlen(input_title, 1)) {
                    config.players[current_player].set_title(input_title);
                    input_manager.stop_text_input();
                    draw_manager.disable_text_cursor();
                    set_stage(CustomRef);
                }
            }
            break;
        case CustomRef:
            if (trans_state == None) {
                draw_manager.show_cursor(false);

                draw_manager.draw_text(
                    "(E.g. 'Milord', 'Milady'...)",
                    Justify::Centre,
                    RES_X/2, 190,
                    COL_TEXT);

                draw_manager.fill(
                    FILL_3D_In,
                    {264-BORDER, 234-BORDER, 232+2*BORDER, 28+2*BORDER},
                    COL_BORDERS);
                draw_manager.fill({264, 234, 232, 28}, {0, 0, 0});

                draw_manager.draw_text(
                        "Reference:",
                        Justify::Right,
                        255, 238,
                        COL_TEXT2);

                draw_manager.save_background();
                input_manager.start_text_input();
                input_manager.set_input_text("My Regent");
                draw_manager.enable_text_cursor(id(ID::PLAYER_REF));

                trans_state = Done;
            }

            if (input_manager.consume(K_Backspace)) {
                input_manager.backspace();
                draw_manager.fill(
                    FILL_3D_In,
                    {264-BORDER, 234-BORDER, 232+2*BORDER, 28+2*BORDER},
                    COL_BORDERS);
                draw_manager.fill({264, 234, 232, 28}, {0, 0, 0});
            }

            {
                const char* input_ref = input_manager.get_input_text(MAX_PLAYER_REFERENCE);

                draw_manager.draw_text(
                        id(ID::PLAYER_REF),
                        input_ref,
                        Justify::Left,
                        270, 238,
                        COL_TEXT);

                if (input_manager.consume(K_Enter) && strnlen(input_ref, 1)) {
                    config.players[current_player].set_ref(input_ref);
                    input_manager.stop_text_input();
                    draw_manager.disable_text_cursor();
                    set_stage(Welcome);
                }
            }
            break;
        case Welcome:
            {
                if (timer == 0) {
                    char t[12 + MAX_PLAYER_REFERENCE];
                    snprintf(t, sizeof(t), "Welcome, %s.",
                             config.players[current_player].get_ref());
                    draw_manager.draw_text(
                        t,
                        Justify::Centre,
                        RES_X/2, 370,
                        COL_TEXT);
                }

                if (timer > 1.4) {
                    set_stage(Flag);
                }

                timer += delta;
            }
            break;
        case Flag:
            if (trans_state == None) {
                draw_manager.draw(IMG_BG_MENU0);
                draw_manager.draw_text(
                    "Please choose one of the flags.",
                    Justify::Centre, RES_X/2, 60,
                    COL_TEXT2);
                for (int j = 0; j < 5; ++j) {
                    for (int i = 0; i < 3; ++i) {
                        int flag_idx = (j*3) + i;

                        int x, y;
                        get_flag_pos(flag_idx, x, y);

                        draw_manager.fill(FILL_3D_Out, {x - 52, y - 32, 104, 64}, COL_BORDERS);
                        draw_border_flag(flag_idx, {0, 0, 0x0});

                        draw_manager.draw(
                            id(ID::FLAG_0 + flag_idx),
                            flags[flag_idx],
                            {x, y, 0.5, 0.5, 1, 1});
                        for (int k = 0; k < current_player; ++k) {
                            if (config.players[k].get_flag_idx() == flag_idx) {
                                draw_manager.draw(
                                    IMG_TS1_FLAG0,
                                    {x, y, 0.5, 0.5, 1, 1});
                            }
                        }
                    }
                }
                draw_manager.save_background();
                draw_manager.show_cursor(true);
                trans_state = Done;
            }

            for (int i = 0; i < 15; ++i) {
                if (draw_manager.query_click(id(ID::FLAG_0 + i)).id) {
                    bool taken = false;
                    for (int j = 0; j < current_player; ++j) {
                        if (config.players[j].get_flag_idx() == i) {
                            taken = true;
                            break;
                        }
                    }

                    if (!taken) {
                        config.players[current_player].set_flag_idx(i);
                        int x, y;
                        get_flag_pos(i, x, y);
                        if (++current_player == config.n_players) {
                            draw_border_flag(i, {0, 0, 0xFF});
                            delay_stage(Aim, 3+i, id(FLAG_0+i), flags[i], {x, y, 0.5f, 0.5f, 1, 1});
                        } else {
                            // Go back to name input for next player
                            draw_border_flag(i, {0, 0, 0xFF});
                            delay_stage(Name, 3+i, id(FLAG_0+i), flags[i], {x, y, 0.5f, 0.5f, 1, 1});
                        }
                    }
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
                    20, 30, COL_TEXT2);

                draw_manager.fill(FILL_3D_Out, {OPT_0_X-6, OPT_0_Y-6, 212, 112}, COL_BORDERS);
                draw_manager.fill(FILL_3D_Out, {OPT_1_X-6, OPT_1_Y-6, 212, 112}, COL_BORDERS);
                draw_manager.fill(FILL_3D_Out, {OPT_2_X-6, OPT_2_Y-6, 212, 112}, COL_BORDERS);

                draw_border_opt(0, {0, 0, 0});
                draw_border_opt(1, {0, 0, 0});
                draw_border_opt(2, {0, 0, 0});

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
                        Justify::Left, TXT_0_X, TXT_0_Y, COL_TEXT2);
                draw_manager.draw_text(
                        "Conquer the galaxy",
                        Justify::Left, TXT_0_X, TXT_0_Y + 20, COL_TEXT);
                draw_manager.draw_text(
                        "within 200 months.",
                        Justify::Left, TXT_0_X, TXT_0_Y + 40, COL_TEXT);

                draw_manager.draw_text(
                        "Money",
                        Justify::Right, TXT_1_X, TXT_1_Y, COL_TEXT2);
                draw_manager.draw_text(
                        "Earn 1000 MC monthly",
                        Justify::Right, TXT_1_X, TXT_1_Y + 20, COL_TEXT);
                draw_manager.draw_text(
                        "after 200 months.",
                        Justify::Right, TXT_1_X, TXT_1_Y + 40, COL_TEXT);

                draw_manager.draw_text(
                        "Civilization",
                        Justify::Left, TXT_2_X, TXT_2_Y, COL_TEXT2);
                draw_manager.draw_text(
                        "Own 30 planets and a",
                        Justify::Left, TXT_2_X, TXT_2_Y + 20, COL_TEXT);
                draw_manager.draw_text(
                        "high technology after",
                        Justify::Left, TXT_2_X, TXT_2_Y + 40, COL_TEXT);
                draw_manager.draw_text(
                        "350 months.",
                        Justify::Left, TXT_2_X, TXT_2_Y + 60, COL_TEXT);

                draw_manager.save_background();
                trans_state = Done;
            }

            if (draw_manager.query_click(id(AIM_MIGHT)).id) {
                config.aim = AIM_Might;
                draw_border_opt(0, {0, 0, 0xFF});
                delay_stage(Difficulty, 0, id(AIM_MIGHT), IMG_STARTGR_AIM_MIGHT, OPT_0);
            } else if (draw_manager.query_click(id(AIM_MONEY)).id) {
                config.aim = AIM_Money;
                draw_border_opt(1, {0, 0, 0xFF});
                delay_stage(Difficulty, 1, id(AIM_MONEY), IMG_STARTGR_AIM_MONEY, OPT_1);
            } else if (draw_manager.query_click(id(AIM_CIV)).id) {
                config.aim = AIM_Civilization;
                draw_border_opt(2, {0, 0, 0xFF});
                delay_stage(Difficulty, 2, id(AIM_CIV), IMG_STARTGR_AIM_CIV, OPT_2);
            }

            break;
        case Difficulty:
            if (trans_state == None) {
                draw_manager.clear_sprite_ids();
                draw_manager.draw(IMG_BG_MENU0);
                draw_manager.draw_text(
                    Font::Large,
                    "Other races",
                    Justify::Left,
                    20, 30, COL_TEXT2);

                const char* txt0_0;
                const char* txt0_1;
                const char* txt0_2;
                const char* txt1_0;
                const char* txt1_1;
                const char* txt1_2;
                const char* txt2_0;
                const char* txt2_1;
                const char* txt2_2;

                if (config.n_players == 1) {
                    id_diff_0 = id(OTHERS_WEAK);
                    id_diff_1 = id(OTHERS_MEDIUM);
                    id_diff_2 = id(OTHERS_STRONG);
                    img_diff_0 = IMG_STARTGR_STR_WEAK;
                    img_diff_1 = IMG_STARTGR_STR_MEDIUM;
                    img_diff_2 = IMG_STARTGR_STR_STRONG;
                    txt0_0 = diff_weak_0;
                    txt0_1 = diff_weak_1;
                    txt0_2 = diff_weak_2;
                    txt1_0 = diff_medium_0;
                    txt1_1 = diff_medium_1;
                    txt1_2 = diff_medium_2;
                    txt2_0 = diff_strong_0;
                    txt2_1 = diff_strong_1;
                    txt2_2 = diff_strong_2;
                } else {
                    id_diff_0 = id(OTHERS_NONE);
                    id_diff_1 = id(OTHERS_WEAK);
                    id_diff_2 = id(OTHERS_MEDIUM);
                    img_diff_0 = IMG_STARTGR_MPSTR_NONE;
                    img_diff_1 = IMG_STARTGR_MPSTR_WEAK;
                    img_diff_2 = IMG_STARTGR_MPSTR_MEDIUM;
                    txt0_0 = diff_none_0;
                    txt0_1 = diff_none_1;
                    txt0_2 = diff_none_2;
                    txt1_0 = diff_weak_0;
                    txt1_1 = diff_weak_1;
                    txt1_2 = diff_weak_2;
                    txt2_0 = diff_medium_0;
                    txt2_1 = diff_medium_1;
                    txt2_2 = diff_medium_2;
                }

                draw_manager.fill(FILL_3D_Out, {OPT_0_X-6, OPT_0_Y-6, 212, 112}, COL_BORDERS);
                draw_manager.fill(FILL_3D_Out, {OPT_1_X-6, OPT_1_Y-6, 212, 112}, COL_BORDERS);
                draw_manager.fill(FILL_3D_Out, {OPT_2_X-6, OPT_2_Y-6, 212, 112}, COL_BORDERS);

                draw_border_opt(0, {0, 0, 0});
                draw_border_opt(1, {0, 0, 0});
                draw_border_opt(2, {0, 0, 0});

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
                        COL_TEXT2);
                draw_manager.draw_text(
                        txt0_1,
                        Justify::Left,
                        TXT_0_X, TXT_0_Y + 20,
                        COL_TEXT);
                draw_manager.draw_text(
                        txt0_2,
                        Justify::Left,
                        TXT_0_X, TXT_0_Y + 40,
                        COL_TEXT);

                draw_manager.draw_text(
                        txt1_0,
                        Justify::Right,
                        TXT_1_X, TXT_1_Y,
                        COL_TEXT2);
                draw_manager.draw_text(
                        txt1_1,
                        Justify::Right,
                        TXT_1_X, TXT_1_Y + 20,
                        COL_TEXT);
                draw_manager.draw_text(
                        txt1_2,
                        Justify::Right,
                        TXT_1_X, TXT_1_Y + 40,
                        COL_TEXT);

                draw_manager.draw_text(
                        txt2_0,
                        Justify::Left,
                        TXT_2_X, TXT_2_Y,
                        COL_TEXT2);
                draw_manager.draw_text(
                        txt2_1,
                        Justify::Left,
                        TXT_2_X, TXT_2_Y + 20,
                        COL_TEXT);
                draw_manager.draw_text(
                        txt2_2,
                        Justify::Left,
                        TXT_2_X, TXT_2_Y + 40,
                        COL_TEXT);

                draw_manager.save_background();
                trans_state = Done;
            }

            if (draw_manager.query_click(id_diff_0).id) {
                config.enemy_start = (config.n_players == 1) ? ENEMY_Weak : ENEMY_None;
                draw_border_opt(0, {0, 0, 0xFF});
                delay_stage(Confirm, 0, id(id_diff_0), img_diff_0, OPT_0);
            } else if (draw_manager.query_click(id_diff_1).id) {
                config.enemy_start = (config.n_players == 1) ? ENEMY_Medium : ENEMY_Weak;
                draw_border_opt(1, {0, 0, 0xFF});
                delay_stage(Confirm, 1, id(id_diff_1), img_diff_1, OPT_1);
            } else if (draw_manager.query_click(id_diff_2).id) {
                config.enemy_start = (config.n_players == 1) ? ENEMY_Strong : ENEMY_Medium;
                draw_border_opt(2, {0, 0, 0xFF});
                delay_stage(Confirm, 2, id(id_diff_2), img_diff_2, OPT_2);
            }
            break;
        case Confirm:
            if (trans_state == None) {
                draw_manager.draw(IMG_BG_STARS0);
                draw_manager.save_background();
                draw_manager.draw_text(
                    "Let me repeat.",
                    Justify::Centre, RES_X/2, 60,
                    COL_TEXT2);

                constexpr int line0_sz = 8 + MAX_PLAYER_TITLE + MAX_PLAYER_NAME;
                char line0[line0_sz];
                Player *player = &(config.players[0]);
                snprintf(line0, line0_sz, "You, %s %s,",
                             player->get_title(), player->get_name());


                char *line1 = (char*)"arrived at a small galaxy,";
                if (config.size == GAL_Medium)
                    line1 = (char*)"arrived at a medium galaxy,";
                if (config.size == GAL_Large)
                    line1 = (char*)"arrived at a large galaxy,";

                char *line2 = (char*)"have the mission to";

                char *line3 = (char*)"show who is the best";
                if (config.aim == AIM_Might)
                    line3 = (char*)"conquer the stars";
                if (config.aim == AIM_Money)
                    line3 = (char*)"gain financial power";
                if (config.aim == AIM_Civilization)
                    line3 = (char*)"build a great civilization";

                char *line4;
                if (config.enemy_start == ENEMY_None)
                    line4 = (char*)"There are no other races.";
                if (config.enemy_start == ENEMY_Weak)
                    line4 = (char*)"Your enemies are weak.";
                if (config.enemy_start == ENEMY_Medium)
                    line4 = (char*)"Your enemies are advanced.";
                if (config.enemy_start == ENEMY_Strong)
                    line4 = (char*)"Your enemies are strong.";

                char *lines[] = {
                    line0,
                    line1,
                    line2,
                    line3,
                    line4,
                };

                for (int i = 0; i < 5; ++i) {
                    draw_manager.draw_text(
                        lines[i],
                        Justify::Centre, RES_X/2, 130 + 30 * i,
                        COL_TEXT);
                }

                draw_manager.draw_text(
                    "Is this correct?",
                    Justify::Centre, RES_X/2, 300,
                    COL_TEXT2);

                draw_manager.set_flag_vfx(id(ID::CONFIRM_FLAG));
                draw_manager.draw(
                    id(ID::CONFIRM_FLAG),
                    flags[config.players[0].get_flag_idx()],
                    {RES_X/2, 400, .5, .5, 1, 1});

                draw_manager.draw(
                        id(PROCEED),
                        IMG_BR7_E1,
                        {180, 400, .5, .5, 1, 1});

                draw_manager.draw(
                        id(RESTART),
                        IMG_BR7_E2,
                        {RES_X-180, 400, .5, .5, 1, 1});

                draw_manager.save_background();
                trans_state = Done;
            }

            if (draw_manager.query_click(id(RESTART)).id) {
                draw_manager.fade_black(1.2f, 24);
                set_stage(Main);
            }

            if (draw_manager.query_click(id(PROCEED)).id) {
                draw_manager.fade_black(1.2f, 24);
                audio_manager.fade_out(1000);
                set_stage(End);
            }

            break;
        case Delay0:
            timer += delta;
            if (timer > DELAY_TIME) {
                if (delay_border_to_reset < 3) {
                    draw_border_opt(delay_border_to_reset, {0, 0, 0});
                } else {
                    draw_border_flag(delay_border_to_reset-3, {0, 0, 0});
                }
                draw_manager.draw(
                        delay_redraw_id,
                        delay_redraw_spr,
                        delay_redraw_transform);
                set_stage(Delay1);
            }
            break;
        case Delay1:
            timer += delta;
            if (timer > DELAY_TIME) {
                set_stage(delayed_stage);
            }
            break;
        case End:
            exostate().init(config);
            return ExodusMode::MODE_GalaxyGen;
    }

    return ExodusMode::MODE_None;
}

void Menu::set_stage(Stage new_stage) {
    draw_manager.draw(id(AUTOSAVE_WARN_TXT0), nullptr);
    draw_manager.draw(id(AUTOSAVE_WARN_TXT1), nullptr);
    draw_manager.draw(id(AUTOSAVE_WARN_TXT2), nullptr);

    trans_state = None;
    stage = new_stage;
    draw_manager.clear_sprite_ids();
    timer = 0;
    autosave_warned = false;
}

void Menu::get_flag_pos(int idx, int& x, int& y) {
    int i = idx % 3;
    int j = idx / 3;
    x = RES_X/4 + i*RES_X/4;
    y = 140 + j*RES_Y/7;

}

void Menu::draw_border_opt(int idx, RGB col) {
    draw_border_opt(TGT_Primary, idx, col);
}

void Menu::draw_border_opt(DrawTarget tgt, int idx, RGB col) {
    int thickness = 2;

    int x = OPT_0_X;
    int y = OPT_0_Y;
    if (idx == 1) {
        x = OPT_1_X;
        y = OPT_1_Y;
    }
    if (idx == 2) {
        x = OPT_2_X;
        y = OPT_2_Y;
    }

    draw_manager.fill(
        tgt,
        id(BUTTON_BG_0+idx),
        {x-thickness, y-thickness, 200+2*thickness, 100+2*thickness},
        col);
}

void Menu::draw_border_flag(int idx, RGB col) {
    int thickness = 1;

    int x, y;
    get_flag_pos(idx, x, y);

    // Flags are drawn centre-anchored - find the top-left corner
    x -= 96/2;
    y -= 56/2;

    draw_manager.fill(
        id(BUTTON_BG_0+idx),
        {x-thickness, y-thickness, 96+2*thickness, 56+2*thickness},
        col);
}

void Menu::delay_stage(Stage next_stage, int border_to_reset, SprID to_redraw, const char* spr, DrawTransform t) {
    delayed_stage = next_stage;

    delay_border_to_reset = border_to_reset;
    delay_redraw_id = to_redraw;
    delay_redraw_spr = spr;
    delay_redraw_transform = t;

    draw_manager.draw(
            delay_redraw_id,
            delay_redraw_spr,
            delay_redraw_transform);

    set_stage(Delay0);
}
