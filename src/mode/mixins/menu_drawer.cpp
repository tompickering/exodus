#include "menu_drawer.h"

#include "state/exodus_state.h"
#include "state/ephemeral_state.h"
#include "save/save.h"
#include "input/input.h"
#include "util/iter.h"

#include "exodus_features.h"

#include "shared.h"
#include "assetpaths.h"


extern EphemeralState ephstate;
extern SAVEMANAGER save_manager;
extern INPUTMANAGER input_manager;

const int MENU_BORDER = 6;
const int MENU_W = 436 + (MENU_BORDER) * 2;
const int MENU_H = 306 + (MENU_BORDER) * 2;
const int MENU_X = (RES_X / 2) - (MENU_W / 2);
const int MENU_Y = (RES_Y / 2) - (MENU_H / 2) - 12;
const int MENU_TEXT_X = MENU_X + MENU_BORDER * 2 + 2;
const int MENU_FLAG_BG_X = (RES_X / 2) - 48 - MENU_BORDER;
const int MENU_FLAG_BG_Y = MENU_Y - 2 - 56 - MENU_BORDER*2;
const int MENU_FLAG_BG_W = 96 + MENU_BORDER*2;
const int MENU_FLAG_BG_H = 56 + MENU_BORDER*2;
const int MENU_BG_X = MENU_X + MENU_BORDER;
const int MENU_BG_Y = MENU_Y + MENU_BORDER;

const int MARKER_ENTRY_W = 120;
const int MARKER_ENTRY_H = 20;

static const int OFF_DESC_LINES = 5;

static const char* off_desc_poor[] = {
    "The poorly-rated officer has no special",
    "abilities.",
    "",
    "However, you might choose them because",
    "they demand no special salary.",
};

static const char* off_desc_avg_sci[] = {
    "The scientists will work better and",
    "faster under the leadership of this",
    "officer.",
    "",
    "",
};

static const char* off_desc_good_sci[] = {
    "The scientists will do their best to",
    "create new inventions and make new",
    "biological discoveries under the",
    "leadership of this officer.",
    "",
};

static const char* off_desc_avg_flt[] = {
    "This admiral will train your space pilots",
    "to be more accurate and effective",
    "during space combat.",
    "",
    "",
};

static const char* off_desc_good_flt[] = {
    "This admiral will give your space pilots",
    "very effective training which will improve",
    "their fighting abilities as much as",
    "possible.",
    "",
};

static const char* off_desc_avg_btl[] = {
    "Your battle units will be more effective",
    "under the control of this general.",
    "",
    "",
    "",
};

static const char* off_desc_good_btl[] = {
    "This battle general promises to give your",
    "battle units much better accuracy in",
    "lunar battles due to a new organisation",
    "within the unit clusters.",
    "",
};

static const char* off_desc_avg_sec[] = {
    "This skilled agent will improve the skills",
    "of your agents and terrorists so that they",
    "can work more accurately.",
    "He also trains his agents to be more watchful",
    "in case of enemy terrorist attacks.",
};

static const char* off_desc_good_sec[] = {
    "This agent will allow all of your agency",
    "staff to work at maximum efficiency.",
    "",
    "",
    "",
};

static const char* off_desc_avg_clr[] = {
    "This counsellor's knowledge also includes",
    "planetary defense systems and other small",
    "details compared to a poorly-rated",
    "counsellor.",
    "",
};

static const char* off_desc_good_clr[] = {
    "This counsellor's knowledge of planetary",
    "defense systems and market prices is",
    "unrivalled.",
    "",
    "",
};


// Officer characters

static const char* off_desc_char_poor[] = {
    "The poorly-rated officer has no special",
    "abilities.",
    "",
    "",
    "",
};

static const char* off_desc_char_avg_sci[] = {
    "The scientists will work better and faster",
    "under the leadership of this officer.",
    "",
    "",
    "",
};

static const char* off_desc_char_good_sci[] = {
    "The scientists will do their best to create new",
    "inventions and make new biological discoveries",
    "under the leadership of this officer.",
    "",
    "",
};

static const char* off_desc_char_avg_flt[] = {
    "This commander will train your space pilots to",
    "be more accurate and effective during space.",
    "combat.",
    "",
    "",
};

static const char* off_desc_char_good_flt[] = {
    "This admiral will give your space pilots very",
    "effective training which will improve their",
    "fighting abilities as much as possible.",
    "",
    "",
};

static const char* off_desc_char_avg_btl[] = {
    "Your battle units will be more effective.",
    "Infantry can spot mines and you can see hits",
    "scored on enemy units. Killing any superior",
    "unit will grant unit promotion.",
    "",
};

static const char* off_desc_char_good_btl[] = {
    "Your battle units gain deadly precision. You can",
    "see the exact HP of enemy units. Discovered",
    "mines remain visible, and like-for-like kills grant",
    "promotions.",
    "",
};

static const char* off_desc_char_avg_sec[] = {
    "This skilled leader will improve the skills of",
    "your agents and terrorists so that they can",
    "work more accurately. He understands",
    "planetary defense systems.",
    "",
};

static const char* off_desc_char_good_sec[] = {
    "This agent will allow all of your agency staff to",
    "work at maximum efficiency.",
    "",
    "",
    "",
};

static const char* off_desc_char_avg_clr[] = {
    "This counsellor's knowledge also includes",
    "planetary defense systems and other small",
    "details compared to a poorly-rated",
    "counsellor.",
    "",
};

static const char* off_desc_char_good_clr[] = {
    "This counsellor's knowledge of planetary",
    "defense systems and market prices is",
    "unrivalled.",
    "",
    "",
};

MenuDrawer::MenuDrawer()
    : _menu_is_open(false)
    , menu_new_officer((Officer)0)
    , menu_mode(MM_Ctrl)
    , menu_new_mode(ExodusMode::MODE_None)
    , menu_action(MA_None)
    , menu_new_officer_quality(OFFQ_Poor)
    , menu_clickable_rows(0)
    , menu_selected_player(nullptr)
    , menu_player_select_mode(MM_SecPersonalFile)
    , first_update(true)
    , menu_inv(INV_MassProduction)
    , menu_art_planet_phase(0)
    , menu_art_planet_named(false)
    , menu_planets_start(0)
    , menu_planets_more(false)
    , marker_being_set(0) {
}

bool MenuDrawer::menu_is_open() {
    return _menu_is_open;
}

MenuAction MenuDrawer::menu_get_action() {
    return menu_action;
}

void MenuDrawer::menu_open(MenuMode mode) {
    if (FEATURE(EF_OFFICER_CHARACTERS)) {
        if (mode == MM_NewOfficer) mode = MM_NewOfficerCharacter;
    }

    menu_mode = mode;
    menu_action = MA_None;
    menu_clickable_rows = 0;

    for (int i = 0; i < MENU_LINES; ++i) {
        strncpy(menu_text[i], "", 1);
        draw_manager.unset_selectable(id_menu_lines[i]);
    }

    if (_menu_is_open) {
        for (int i = 0; i < MENU_LINES; ++i) {
            draw_manager.draw(id_menu_lines[i], nullptr);
        }
    } else {
        id_menu_header_flag_bg      = draw_manager.new_sprite_id();
        id_menu_header_flag         = draw_manager.new_sprite_id();
        id_menu_header_l            = draw_manager.new_sprite_id();
        id_menu_header_r            = draw_manager.new_sprite_id();
        id_menu_panel               = draw_manager.new_sprite_id();
        id_menu_bg                  = draw_manager.new_sprite_id();
        id_menu_sci                 = draw_manager.new_sprite_id();
        id_menu_tax                 = draw_manager.new_sprite_id();
        id_menu_scimore             = draw_manager.new_sprite_id();
        id_menu_taxmore             = draw_manager.new_sprite_id();
        id_menu_newoff_opt          = draw_manager.new_sprite_id();
        id_menu_artplanname         = draw_manager.new_sprite_id();
        id_menu_marker_entry_border = draw_manager.new_sprite_id();
        id_menu_marker_entry        = draw_manager.new_sprite_id();
        id_menu_marker_entry_text   = draw_manager.new_sprite_id();

        for (int i = 0; i < MENU_N_OFFICERS; ++i) {
            id_menu_newoff[i] = draw_manager.new_sprite_id();
        }

        for (int i = 0; i < MENU_LINES; ++i) {
            id_menu_lines[i] = draw_manager.new_sprite_id();
        }

        draw_manager.fill(
            id_menu_panel,
            {MENU_X, MENU_Y,
             MENU_W, MENU_H},
            COL_BORDERS);
        draw_manager.fill(
            FILL_3D_Out_Hollow,
            {MENU_X, MENU_Y,
             MENU_W, MENU_H},
            COL_BORDERS);

        // Draw header flag and background
        draw_manager.fill(
            id_menu_header_flag_bg,
            {MENU_FLAG_BG_X, MENU_FLAG_BG_Y,
             MENU_FLAG_BG_W, MENU_FLAG_BG_H},
             COL_BORDERS);
        draw_manager.fill(
            FILL_3D_Out_Hollow,
            {MENU_FLAG_BG_X, MENU_FLAG_BG_Y,
             MENU_FLAG_BG_W, MENU_FLAG_BG_H},
             COL_BORDERS);

        draw_manager.draw(
            id_menu_header_flag,
            flags[exostate().get_active_player()->get_flag_idx()],
            {MENU_FLAG_BG_X + MENU_BORDER,
             MENU_FLAG_BG_Y + MENU_BORDER,
             0, 0, 1, 1});

        draw_manager.draw(
            id_menu_header_l,
            ENHANCED() ? IMG_HEADER_L : IMG_TS1_HEADER,
            {MENU_FLAG_BG_X - 2,
             MENU_Y - 2,
             1, 1, 1, 1});

        draw_manager.draw(
            id_menu_header_r,
            ENHANCED() ? IMG_HEADER_R : IMG_TS1_HEADER,
            {MENU_FLAG_BG_X + MENU_FLAG_BG_W + 2,
             MENU_Y - 2,
             0, 1, 1, 1});
    }

    draw_manager.draw(
        id_menu_bg,
        menu_get_bg(),
        {MENU_X + MENU_BORDER,
         MENU_Y + MENU_BORDER,
         0, 0, 1, 1});

    input_manager.enable_repeating_clicks(false);

    menu_open_specific_mode();

    _menu_is_open = true;
}

void MenuDrawer::menu_open_invention(Invention inv) {
    menu_inv = inv;
    menu_open(MM_Invention);
}

void MenuDrawer::menu_close() {
    menu_action = MA_None;

    for (int i = 0; i < MENU_LINES; ++i) {
        draw_manager.draw(id_menu_lines[i], nullptr);
        draw_manager.release_sprite_id(id_menu_lines[i]);
    }

    draw_manager.draw(id_menu_marker_entry_text,   nullptr);
    draw_manager.draw(id_menu_marker_entry,        nullptr);
    draw_manager.draw(id_menu_marker_entry_border, nullptr);
    draw_manager.draw(id_menu_artplanname,         nullptr);
    draw_manager.draw(id_menu_newoff_opt,          nullptr);
    draw_manager.draw(id_menu_sci,                 nullptr);
    draw_manager.draw(id_menu_tax,                 nullptr);
    draw_manager.draw(id_menu_scimore,             nullptr);
    draw_manager.draw(id_menu_taxmore,             nullptr);
    draw_manager.draw(id_menu_header_flag_bg,      nullptr);
    draw_manager.draw(id_menu_header_flag,         nullptr);
    draw_manager.draw(id_menu_header_l,            nullptr);
    draw_manager.draw(id_menu_header_r,            nullptr);
    draw_manager.draw(id_menu_panel,               nullptr);
    draw_manager.draw(id_menu_bg,                  nullptr);

    for (int i = 0; i < MENU_N_OFFICERS; ++i) {
        draw_manager.draw(id_menu_newoff[i], nullptr);
        draw_manager.release_sprite_id(id_menu_newoff[i]);
    }

    draw_manager.release_sprite_id(id_menu_header_flag_bg);
    draw_manager.release_sprite_id(id_menu_header_flag);
    draw_manager.release_sprite_id(id_menu_header_l);
    draw_manager.release_sprite_id(id_menu_header_r);
    draw_manager.release_sprite_id(id_menu_panel);
    draw_manager.release_sprite_id(id_menu_bg);
    draw_manager.release_sprite_id(id_menu_sci);
    draw_manager.release_sprite_id(id_menu_tax);
    draw_manager.release_sprite_id(id_menu_scimore);
    draw_manager.release_sprite_id(id_menu_taxmore);
    draw_manager.release_sprite_id(id_menu_newoff_opt);
    draw_manager.release_sprite_id(id_menu_artplanname);
    draw_manager.release_sprite_id(id_menu_marker_entry_border);
    draw_manager.release_sprite_id(id_menu_marker_entry);
    draw_manager.release_sprite_id(id_menu_marker_entry_text);

    input_manager.enable_repeating_clicks(false);

    _menu_is_open = false;
}

int MenuDrawer::menu_get_y(int row) {
    return MENU_Y + MENU_BORDER + 2 + row * 20;
}

ExodusMode MenuDrawer::menu_update(float delta) {
    while (true) {
        menu_action = MA_None;

        for (int i = 0; i < MENU_LINES; ++i) {
            draw_manager.draw_text(
                id_menu_lines[i],
                menu_text[i],
                Justify::Left,
                MENU_TEXT_X,
                menu_get_y(i),
                menu_text_col[i]);
        }

        MousePos pos = draw_manager.get_click();
        if (pos.valid()) {
            if (pos.x < MENU_X || pos.x > MENU_X + MENU_W + 2*MENU_BORDER ||
                pos.y < MENU_Y || pos.y > MENU_Y + MENU_H + 2*MENU_BORDER) {
                menu_action = MA_Close;
                return ExodusMode::MODE_None;
            }
        }

        if (input_manager.consume(Input::K_Escape)) {
            menu_action = MA_Close;
            return ExodusMode::MODE_None;
        }

        menu_new_mode = MODE_None;

        if (!menu_specific_update()) {
            break;
        } else if (menu_new_mode != MODE_None) {
            ExodusMode new_mode = menu_new_mode;
            menu_new_mode = ExodusMode::MODE_None;
            return new_mode;
        } else {
            draw_manager.consume_click();
        }
    }

    if (ephstate.ephemeral_state_set()) {
        return ephstate.get_appropriate_mode();
    }

    return ExodusMode::MODE_None;
}

const char* MenuDrawer::menu_get_bg() {
    switch (menu_mode) {
        case MM_Save:
            return ENHANCED() ? IMG_ME2_MENU_NEW : IMG_ME2_MENU;
        default:
            return ENHANCED() ? IMG_ME1_MENU_NEW : IMG_ME1_MENU;
    }
    return ENHANCED() ? IMG_ME1_MENU_NEW : IMG_ME1_MENU;
}

void MenuDrawer::menu_set_txt(int idx, RGB col, const char* in_text, ...) {
    if (idx >= MENU_LINES) {
        L.error("Tried to set invalid menu text index %d to %s", idx, in_text);
        return;
    }

    va_list args;
    va_start(args, in_text);
    vsnprintf(menu_text[idx], MENU_MAX_TEXT - 1, in_text, args);
    va_end(args);

    menu_text_col[idx] = col;
}

void MenuDrawer::menu_set_opt(int idx, const char* in_text, ...) {
    if (idx >= MENU_LINES) {
        L.error("Tried to set invalid menu option index %d to %s", idx, in_text);
        return;
    }

    va_list args;
    va_start(args, in_text);
    vsnprintf(menu_text[idx], MENU_MAX_TEXT - 1, in_text, args);
    va_end(args);

    menu_text_col[idx] = COL_TEXT;
    menu_clickable_rows |= (1<<idx);
    draw_manager.set_selectable(id_menu_lines[idx]);
}

void MenuDrawer::menu_set_opt(int idx, const char* in_text, bool enabled) {
    if (enabled) {
        menu_set_opt(idx, in_text);
    } else {
        menu_set_txt(idx, COL_TEXT_GREYED, in_text);
    }
}

void MenuDrawer::menu_open_specific_mode() {
    Player *p = exostate().get_active_player();
    int p_idx = exostate().get_player_idx(p);
    bool art_ok = true;
    first_update = true;

    if (!p->can_afford(COST_ART)) art_ok = false;
    if (!p->has_invention(INV_OrbitalMassConstruction)) art_ok = false;

    Planet *art = exostate().get_planet_under_construction(p_idx);
    if (art && art->get_construction_phase() >= 3) {
        // Artificial planet is awaiting finalisation
        art_ok = false;
    }

    bool service_ok = exostate().get_n_active_cpu_players() > 0;

    switch(menu_mode) {
        case MM_Ctrl:
            menu_set_txt(0, COL_TEXT2, "Please select, %s", p->get_ref());
            menu_set_opt(2, "Change Officers & Taxes");
            menu_set_opt(3, "Secret Service", service_ok);
            menu_set_opt(4, "Set / Replace Star Markers (M)");
            menu_set_opt(5, "Equip Starship (E)");
            menu_set_opt(6, "Build Artificial Planet (" STR(COST_ART) "MC)", art_ok);
            menu_set_opt(8, "Wait One Month (Spc)");
            menu_set_opt(10, "Show Distances (D)");
            menu_set_opt(11, "Save Game (S for last)");
            menu_set_opt(12, "Quit Game");
            menu_set_opt(14, "Exit Menu");
            break;
        case MM_OfficersAndTaxes:
            {
                menu_set_txt(0,  COL_TEXT2, "Please select:");
                menu_set_txt(2,  COL_TEXT,  "Science:");
                menu_set_txt(3,  COL_TEXT,  "Taxes:");
                menu_set_txt(6,  COL_TEXT,  "Leading Officers");
                if (FEATURE(EF_OFFICER_CHARACTERS)) {
                    menu_set_txt(8,  COL_TEXT, Player::get_officer_character_desc(OFF_Science));
                    menu_set_txt(9,  COL_TEXT, Player::get_officer_character_desc(OFF_Fleet));
                    menu_set_txt(10, COL_TEXT, Player::get_officer_character_desc(OFF_Battle));
                    menu_set_txt(11, COL_TEXT, Player::get_officer_character_desc(OFF_Secret));
                    if (!FEATURE(EF_COMBINE_SS_COUNSELLOR)) {
                        menu_set_txt(12, COL_TEXT, Player::get_officer_character_desc(OFF_Counsellor));
                    }
                } else {
                    menu_set_txt(8,  COL_TEXT, "Science Officer");
                    menu_set_txt(9,  COL_TEXT, "Fleet Admiral");
                    menu_set_txt(10, COL_TEXT, "Battle General");
                    menu_set_txt(11, COL_TEXT, "Secret Service Leader");
                    if (!FEATURE(EF_COMBINE_SS_COUNSELLOR)) {
                        menu_set_txt(12, COL_TEXT, "Ship Counsellor");
                    }
                }
                menu_set_opt(14, "Exit");
                input_manager.enable_repeating_clicks(true);
            }
            break;
        case MM_NewOfficer:
            {
                const char* off_str = "Science Officer";
                const char** off_desc = &off_desc_poor[0];
                const OfficerQuality &q = menu_new_officer_quality;

                switch (menu_new_officer) {
                    case OFF_Science:
                        off_str = "Science Officer";
                        if (q==OFFQ_Average) off_desc = off_desc_avg_sci;
                        if (q==OFFQ_Good)    off_desc = off_desc_good_sci;
                        break;
                    case OFF_Fleet:
                        off_str = "Fleet Admiral";
                        if (q==OFFQ_Average) off_desc = off_desc_avg_flt;
                        if (q==OFFQ_Good)    off_desc = off_desc_good_flt;
                        break;
                    case OFF_Battle:
                        off_str = "Battle General";
                        if (q==OFFQ_Average) off_desc = off_desc_avg_btl;
                        if (q==OFFQ_Good)    off_desc = off_desc_good_btl;
                        break;
                    case OFF_Secret:
                        off_str = "Secret Service Leader";
                        if (q==OFFQ_Average) off_desc = off_desc_avg_sec;
                        if (q==OFFQ_Good)    off_desc = off_desc_good_sec;
                        break;
                    case OFF_Counsellor:
                        off_str = "Ship Counsellor";
                        if (q==OFFQ_Average) off_desc = off_desc_avg_clr;
                        if (q==OFFQ_Good)    off_desc = off_desc_good_clr;
                        break;
                    default:
                        L.error("Unknown officer: %d", (int)menu_new_officer);
                        break;
                }

                menu_set_txt(0,  COL_TEXT2, "New %s", off_str);
                menu_set_txt(2,  COL_TEXT,  "Classification:");
                menu_set_txt(3,  COL_TEXT,  "Engagement cost:");
                menu_set_txt(4,  COL_TEXT,  "Monthly cost:");

                for (int i = 0; i < OFF_DESC_LINES; ++i) {
                    menu_set_txt(6+i,  COL_TEXT, off_desc[i]);
                }
            }
            break;
        case MM_OldOfficer:
            {
                if (FEATURE(EF_OFFICER_CHARACTERS)) {
                    const char* tn = Player::get_officer_character_title_and_name(menu_new_officer, menu_old_officer_quality);
                    menu_set_txt(0, COL_TEXT, "What do you want to do with %s?", tn);
                } else {
                    menu_set_txt(0, COL_TEXT, "What do you want to do with the old officer?");
                }
                menu_set_opt(2, "Pay " STR(OFF_PAY_MC) "MC for their work", p->can_afford(OFF_PAY_MC));

                if (FEATURE(EF_OFFICER_CHARACTERS)) {
                    menu_set_opt(3, "Dismiss Without Honours");
                    menu_set_opt(4, "Incarcerate");
                } else {
                    menu_set_opt(3, "Dismiss");
                    menu_set_opt(4, "Kill");
                }
            }
            break;
        case MM_OldOfficerPaid:
            {
                menu_set_txt(0, COL_TEXT, "You have shown that you are a fair");
                menu_set_txt(1, COL_TEXT, "and generous sovereign.");
            }
            break;
        case MM_OldOfficerDismissed:
            {
                p->register_officer_fired_nopay(menu_new_officer, menu_old_officer_quality);
                if (FEATURE(EF_OFFICER_CHARACTERS)) {
                    const char* tn = Player::get_officer_character_title_and_name(menu_new_officer, menu_old_officer_quality);
                    menu_set_txt(0, COL_TEXT, "%s has been dismissed without", tn);
                } else {
                    menu_set_txt(0, COL_TEXT, "The old officer has been dismissed without");
                }
                menu_set_txt(1, COL_TEXT, "any payment for their work.");
            }
            break;
        case MM_OldOfficerKilled:
            {
                p->register_officer_killed(menu_new_officer, menu_old_officer_quality);
                if (FEATURE(EF_OFFICER_CHARACTERS)) {
                    const char* tn = Player::get_officer_character_name(menu_new_officer, menu_old_officer_quality);
                    menu_set_txt(0, COL_TEXT, "%s has been sent to a penal colony.", tn);
                } else {
                    menu_set_txt(0, COL_TEXT, "The old officer has been shot.");
                }
                menu_set_txt(1, COL_TEXT, "Will this be good for your reputation?");
            }
            break;
        case MM_NewOfficerCharacter:
            {
                const char* off_str = "Science Officer";
                const char** off_desc = &off_desc_char_poor[0];
                const OfficerQuality &q = menu_new_officer_quality;

                switch (menu_new_officer) {
                    case OFF_Science:
                        off_str = Player::get_officer_character_desc(OFF_Science);
                        if (q==OFFQ_Average) off_desc = off_desc_char_avg_sci;
                        if (q==OFFQ_Good)    off_desc = off_desc_char_good_sci;
                        break;
                    case OFF_Fleet:
                        off_str = Player::get_officer_character_desc(OFF_Fleet);
                        if (q==OFFQ_Average) off_desc = off_desc_char_avg_flt;
                        if (q==OFFQ_Good)    off_desc = off_desc_char_good_flt;
                        break;
                    case OFF_Battle:
                        off_str = Player::get_officer_character_desc(OFF_Battle);
                        if (q==OFFQ_Average) off_desc = off_desc_char_avg_btl;
                        if (q==OFFQ_Good)    off_desc = off_desc_char_good_btl;
                        break;
                    case OFF_Secret:
                        off_str = Player::get_officer_character_desc(OFF_Secret);
                        if (q==OFFQ_Average) off_desc = off_desc_char_avg_sec;
                        if (q==OFFQ_Good)    off_desc = off_desc_char_good_sec;
                        break;
                    case OFF_Counsellor:
                        off_str = Player::get_officer_character_desc(OFF_Counsellor);
                        if (q==OFFQ_Average) off_desc = off_desc_char_avg_clr;
                        if (q==OFFQ_Good)    off_desc = off_desc_char_good_clr;
                        break;
                    default:
                        L.error("Unknown officer: %d", (int)menu_new_officer);
                        break;
                }

                menu_set_txt(0,  COL_TEXT2, "New %s", off_str);
                menu_set_txt(2,  COL_TEXT,  "Rank:");
                menu_set_txt(3,  COL_TEXT,  "Name:");
                menu_set_txt(4,  COL_TEXT,  "Classification:");
                menu_set_txt(5,  COL_TEXT,  "Engagement cost:");
                menu_set_txt(6,  COL_TEXT,  "Monthly cost:");
                menu_set_txt(7,  COL_TEXT,  "Status:");

                for (int i = 0; i < OFF_DESC_LINES; ++i) {
                    menu_set_txt(9+i,  COL_TEXT, off_desc[i]);
                }
            }
            break;
        case MM_SecretService:
            menu_set_txt(0, COL_TEXT2, "Secret Service");
            menu_set_txt(2, COL_TEXT, "Please choose:");
            menu_set_opt(4, "Information Services (20+ MC)");
            menu_set_opt(5, "A Personal File (" STR(COST_FILE) " MC)");
            menu_set_opt(6, "Terrorist Attacks (50+ MC)");
            menu_set_opt(7, "Orbital Bomb Attacks (500+ MC)");
            menu_set_opt(14, "Exit Menu");
            break;
        case MM_SecInfo:
            {
                menu_set_txt(0, COL_TEXT2, "You surely wish to see...");
                menu_set_opt(3, "... a lord's amount of money (" STR(COST_INF_MC) "MC)",
                        p->can_afford(COST_INF_MC));
                menu_set_opt(4, "... a lord's allies (" STR(COST_INF_ALLIES) "MC)",
                        p->can_afford(COST_INF_ALLIES));
                menu_set_opt(5, "... a lord's number of planets (" STR(COST_INF_NPLANS) "MC)",
                        p->can_afford(COST_INF_NPLANS));
                menu_set_opt(6, "... a lord's inventions (" STR(COST_INF_INV) "MC)",
                        p->can_afford(COST_INF_INV));
                menu_set_opt(7, "... a planet's surface (" STR(COST_INF_SURF) "MC)",
                        p->can_afford(COST_INF_SURF));
                menu_set_opt(14, "Exit Menu");
            }
            break;
        case MM_PlayerChoice:
            menu_print_other_players();
            break;
        case MM_SecInfoMC:
            {
                Player *pl = menu_selected_player;
                menu_set_txt(0, COL_TEXT2, "Information about %s", pl->get_full_name());
                menu_set_txt(2, COL_TEXT, "This is the desired information:");
                // N.B. CPU players CAN be in debt (-ve MC)!
                menu_set_txt(3, COL_TEXT, "%s owns %dMCredits.", pl->get_full_name(), pl->get_mc());
            }
            break;
        case MM_SecInfoAllies:
            {
                Player *pl = menu_selected_player;
                menu_set_txt(0, COL_TEXT2, "The allies of %s are:", pl->get_full_name());
                int row = 2;
                menu_set_txt(row, COL_TEXT, "None");
                int pl_idx = exostate().get_player_idx(pl);
                for (int i = 0; i < N_PLAYERS; ++i) {
                    if (i == pl_idx) continue;
                    if (exostate().is_allied(i, pl_idx)) {
                        Player *ally = exostate().get_player(i);
                        menu_set_txt(row++, COL_TEXT, ally->get_full_name());
                    }
                }
            }
            break;
        case MM_SecInfoPlanets:
            {
                Player *pl = menu_selected_player;
                menu_set_txt(0, COL_TEXT2, "Information about %s", pl->get_full_name());
                menu_set_txt(2, COL_TEXT, "This is the desired information:");
                int n = exostate().get_n_planets(pl);
                menu_set_txt(3, COL_TEXT, "%s owns %d planets.", pl->get_full_name(), n);
            }
            break;
        case MM_SecInfoInventions:
            {
                Player *pl = menu_selected_player;
                menu_set_txt(0, COL_TEXT2, "%s has the following inventions:", pl->get_full_name());

                int row = 2;
                menu_set_txt(row, COL_TEXT, "None");
                for (Invention i = (Invention)0; i < INV_MAX; i = (Invention)((int)i+1)) {
                    if (pl->has_invention(i)) {
                        menu_set_txt(row++, COL_TEXT, pl->get_invention_str(i));
                    }
                }
            }
            break;
        case MM_SecPersonalFile:
            {
                Player *pl = menu_selected_player;

                menu_set_txt(0, COL_TEXT2, "PERSONAL FILE");

                menu_set_txt(2, COL_TEXT, "Name:");

                draw_manager.draw_text(
                    pl->get_name(),
                    Justify::Left,
                    MENU_X+120, menu_get_y(2),
                    COL_TEXT2);

                menu_set_txt(4, COL_TEXT, "Race:");
                draw_manager.draw_text(
                    pl->get_race_str(),
                    Justify::Left,
                    MENU_X+120, menu_get_y(4),
                    COL_TEXT2);

                // TODO: Check this value
                GuildTitle t = pl->get_guild_title();
                menu_set_txt(5, COL_TEXT, "Status:");
                draw_manager.draw_text(
                    t == GUILDTITLE_None ? "None" : pl->get_guild_title_str(),
                    Justify::Left,
                    MENU_X+120, menu_get_y(5),
                    COL_TEXT2);

                int nplans = 0;
                int total_unrest = 0;

                for (PlanetIterator pi(exostate().get_player_idx(pl)); !pi.complete(); ++pi) {
                    nplans++;
                    total_unrest += pi.get()->get_unrest();
                }

                char nplans_str[16];
                snprintf(nplans_str, sizeof(nplans_str), "%d", nplans);
                menu_set_txt(6, COL_TEXT, "Planets:");
                draw_manager.draw_text(
                    nplans_str,
                    Justify::Left,
                    MENU_X+120, menu_get_y(6),
                    COL_TEXT2);

                menu_set_txt(8, COL_TEXT, "Profile:");

                char line[128];
                const char* desc = "%s";

                switch (pl->get_race()) {
                    case RACE_Human:
                        L.warn("Personal file unexpectedly called on human player");
                        desc = "%s has a volatile nature.";
                        break;
                    case RACE_Yokon:
                        desc = "%s has a restless character.";
                        break;
                    case RACE_Teri:
                        desc = "%s is a peaceful monarch.";
                        break;
                    case RACE_Urkash:
                        desc = "%s lives for the war.";
                        break;
                    case RACE_Gordoon:
                        desc = "%s is a fair and silent monarch.";
                        break;
                }

                snprintf(line, sizeof(line), desc, pl->get_full_name());
                menu_set_txt(10, COL_TEXT, line);

                const char* a = "His";
                const char* b = "him";

                if (pl->get_gender() == GENDER_Female) {
                    a = "Her";
                    b = "her";
                }

                if (pl->get_gender() == GENDER_Other) {
                    a = "Their";
                    b = "them";
                }

                const char* c = "adore ";
                if (total_unrest > 8) {
                    c = "hate ";
                } else if (total_unrest > 7) {
                    c = "do not like ";
                } else if (total_unrest > 5) {
                    b = "";
                    c = "are not pleased";
                } else if (total_unrest > 3) {
                    c = "accept ";
                } else if (total_unrest > 1) {
                    b = "";
                    c = "are content";
                } else if (total_unrest > 0) {
                    c = "love ";
                }

                snprintf(line, sizeof(line), "%s people %s%s. Amongst the", a, c, b);
                menu_set_txt(11, COL_TEXT, line);

                int rep = pl->get_reputation();

                a = "he has";

                if (pl->get_gender() == GENDER_Female) {
                    a = "she has";
                }

                if (pl->get_gender() == GENDER_Other) {
                    a = "they have";
                }

                const char* rep_str = "a very bad";

                if (rep > 3) {
                    rep_str = "a good";
                } else if (rep > 2) {
                    rep_str = "no special";
                } else if (rep > 1) {
                    rep_str = "a bad";
                }

                snprintf(line, sizeof(line), "lords, %s %s reputation.", a, rep_str);
                menu_set_txt(12, COL_TEXT, line);

                menu_set_txt(14, COL_TEXT2, "End of personal file.");
            }
            break;
        case MM_SecAttack:
            {
                menu_set_txt(0, COL_TEXT2, "You surely wish to attack a planet's...");

                menu_set_opt(4, "... command station (" STR(COST_ATT_CMD) "MC)",
                        p->can_afford(COST_ATT_CMD));
                menu_set_opt(5, "... cultivated area (" STR(COST_ATT_AGRI) "MC)",
                        p->can_afford(COST_ATT_AGRI));
                menu_set_opt(6, "... plutonium production (" STR(COST_ATT_PLU) "MC)",
                        p->can_afford(COST_ATT_PLU));
                menu_set_opt(7, "... army production (" STR(COST_ATT_ARMY) "MC)",
                        p->can_afford(COST_ATT_ARMY));
                menu_set_opt(8, "... spaceport (" STR(COST_ATT_PORT) "MC)",
                        p->can_afford(COST_ATT_PORT));
                menu_set_opt(9, "... trading centre (" STR(COST_ATT_TRADE) "MC)",
                        p->can_afford(COST_ATT_TRADE));
                menu_set_opt(10, "... mining (" STR(COST_ATT_MINE) "MC)",
                        p->can_afford(COST_ATT_MINE));

                menu_set_opt(14, "Exit Menu");
            }
            break;
        case MM_SecAttackAlreadyPlanned:
            {
                menu_set_txt(1, COL_TEXT, "We are already working out a mission.");
                menu_set_txt(2, COL_TEXT, "Sorry.");
            }
            break;
        case MM_SecBombing:
            {
                menu_set_txt(0, COL_TEXT2, "You surely wish...");

                menu_set_opt(4, "... random orbital bombing (" STR(COST_ATT_BOMB) "MC)",
                        p->can_afford(COST_ATT_BOMB));
                menu_set_opt(5, "... nuclear extermination (" STR(COST_ATT_NUKE) "MC)",
                        p->can_afford(COST_ATT_NUKE));

                menu_set_opt(14, "Exit Menu");
            }
            break;
        case MM_SecBombingNoTech:
            {
                menu_set_txt(1, COL_TEXT, "We haven't yet invented a system to");
                menu_set_txt(2, COL_TEXT, "realize orbital attacks. Sorry.");
            }
            break;
        case MM_StarMarker:
            {
                menu_set_txt(0, COL_TEXT2, "Set/replace a marker");

                for (int i = 0; i < N_MARKERS; ++i) {
                    // FIXME: Formatting in menu_set_opt is ideal but causes numbers to be off - why?
                    char t[4];
                    snprintf(t, sizeof(t), "M%d", i+1);
                    menu_set_opt(i+2, t);

                    const StarMarker *m = p->get_marker(i);
                    if (m->tag[0] != '\0') {
                        draw_manager.draw_text(
                            m->tag,
                            Justify::Left,
                            MENU_TEXT_X + 50,
                            menu_get_y(i+2),
                            {0xFF, 0, 0});
                    }
                }

                marker_being_set = -1;
            }
            break;
        case MM_EquipShip:
            break;
        case MM_ArtificialWorld:
            {
                int player_idx = exostate().get_active_player_idx();
                Planet *planet = exostate().get_planet_under_construction(player_idx);
                if (planet) {
                    L.debug("Advancing artificial planet");
                    menu_art_planet_phase = planet->get_construction_phase();
                    menu_art_planet_named = true;
                    if (planet->advance_construction_phase()) {
                        if (menu_art_planet_phase == 1) {
                            achievement_manager.unlock(ACH_ArtPlanetP2);
                        }

                        if (menu_art_planet_phase == 2) {
                            achievement_manager.unlock(ACH_ArtPlanetP3);
                        }
                    } else {
                        L.error("Attempt to advance non-advanceable planet");
                    }
                    /*
                     * On completion, the planet will actually appear the following month,
                     * and a bulletin will show. This is done with finalise_construction().
                     */
                } else {
                    L.debug("Constructing artificial planet");

                    achievement_manager.unlock(ACH_ArtPlanetP1);

                    menu_action = MA_BuildArtificialWorld;
                }

                menu_set_txt(0, COL_TEXT2, "Artificial Planet Construction");

                const char* img = IMG_PP1_CONST;

                const char* t0 = "We are starting phase 1";
                const char* t1 = "Phase: Planning";

                const char* t2 = "2 further phases are required.";

                const char* t3 = "Planet's name:";
                const char* t4 = "";

                if (menu_art_planet_phase == 1) {
                    img = IMG_PP2_CONST;

                    t0 = "We are starting phase 2";
                    t1 = "Phase: Construction";

                    t2 = "1 further phase is required.";

                    t3 = "We are now going to use the";
                    t4 = "Orbital Mass Construction System.";
                }

                if (menu_art_planet_phase >= 2) {
                    img = IMG_PP3_CONST;

                    t0 = "We are starting phase 3";
                    t1 = "Phase: Animation";

                    t2 = "No further phases are required.";

                    t3 = "We are now creating the artificial";
                    t4 = "atmosphere and the ice moon.";
                }

                draw_manager.draw(img, {MENU_X+40, menu_get_y(1)+20, 0, 0, 1, 1});

                draw_manager.draw_text(
                    t0,
                    Justify::Left,
                    MENU_X+150, menu_get_y(3),
                    COL_TEXT);
                draw_manager.draw_text(
                    t1,
                    Justify::Left,
                    MENU_X+150, menu_get_y(4),
                    COL_TEXT);

                draw_manager.draw_text(
                    t2,
                    Justify::Left,
                    MENU_X+40, menu_get_y(7),
                    COL_TEXT);

                draw_manager.draw_text(
                    t3,
                    Justify::Left,
                    MENU_X+40, menu_get_y(9),
                    COL_TEXT);
                draw_manager.draw_text(
                    t4,
                    Justify::Left,
                    MENU_X+40, menu_get_y(10),
                    COL_TEXT);

                if (!menu_art_planet_named) {
                    input_manager.start_text_input();
                    draw_manager.enable_text_cursor(id_menu_artplanname);
                    input_manager.set_input_text("Genesis");
                }
            }
            break;
        case MM_Save:
            {
                const SaveMeta *meta = save_manager.get_all_meta(true);
                menu_set_txt(0, COL_TEXT2, "Save game in slot:");
                char n[4];
                for (int i = 1; i < MAX_SLOTS; ++i) {
                    if (meta[i].exists) {
                        menu_set_opt(i+1, "%d: %s / Month %d / Planets %d",
                                     i, meta[i].name, meta[i].month, meta[i].planets);
                    } else {
                        // FIXME: The following line should work, but values end up corrupt
                        //menu_set_opt(i+2, "%d", i+1);
                        snprintf(n, sizeof(n), "%d", i);
                        menu_set_opt(i+1, n);
                    }
                }
                menu_set_opt(14, "Exit Menu");
            }
            break;
        case MM_Stat:
            menu_set_txt(0, COL_TEXT2, "Please select, %s", p->get_ref());
            menu_set_opt(2, "General Information");
            menu_set_opt(3, "Fleet Information");
            if (FEATURE(EF_FINANCE_MENU)) {
                menu_set_opt(4, "Finance Information (F)");
            } else {
                menu_set_opt(4, "Recall Latest News (R)");
            }
            menu_set_opt(5, "List Planets");
            menu_set_opt(6, "List Inventions");
            menu_set_opt(7, "Relationship to Lords");
            menu_set_opt(9, "Starship Status");
            menu_set_opt(10, "Starship Damage");
            menu_set_opt(12, "Statistics");
            menu_set_opt(14, "Exit Menu");
            break;
        case MM_GenInfo:
            {
                char txt[64 + FT_MAX_NAME];

                menu_set_txt(0, COL_TEXT2, "Status file, Month %d", exostate().get_month());

                menu_set_txt(2, COL_TEXT, "Money:");
                snprintf(txt, sizeof(txt), "%dMC", p->get_mc());
                draw_manager.draw_text(
                    txt,
                    Justify::Left,
                    MENU_X+120, menu_get_y(2),
                    COL_TEXT2);

                menu_set_txt(4, COL_TEXT2, "Monthly:");

                int taxes = 0;
                int army = 0;
                int sci = 0;
                int planets = 0;

                for (PlanetIterator piter(p_idx); !piter.complete(); ++piter) {
                    int planet_net = piter.get()->get_net_income();
                    // FIXME: Deduplicate this with the logic when income is calculated
                    // FIXME: This doesn't account for the special case where we keep 1MC
                    //        if income is 1 and sci tax < 50%
                    int kept = (planet_net * (100 - p->get_tax())) / 100;
                    taxes += kept;
                    sci += (planet_net - kept);
                    army += piter.get()->get_army_funding();
                    planets++;
                }

                menu_set_txt(5, COL_TEXT, "Taxes:");
                snprintf(txt, sizeof(txt), "%dMC", taxes);
                draw_manager.draw_text(
                    txt,
                    Justify::Left,
                    MENU_X+120, menu_get_y(5),
                    COL_TEXT2);

                menu_set_txt(6, COL_TEXT, "Science:");
                snprintf(txt, sizeof(txt), "%dMC", sci);
                draw_manager.draw_text(
                    txt,
                    Justify::Left,
                    MENU_X+120, menu_get_y(6),
                    COL_TEXT2);

                menu_set_txt(7, COL_TEXT, "Army:");
                snprintf(txt, sizeof(txt), "%dMC", army);
                draw_manager.draw_text(
                    txt,
                    Justify::Left,
                    MENU_X+120, menu_get_y(7),
                    COL_TEXT2);

                int row = 8;

                if (FEATURE(EF_OFFICER_COST_INFO)) {
                    menu_set_txt(row, COL_TEXT, "Officers:");
                    snprintf(txt, sizeof(txt), "%dMC", p->get_total_officer_costs());
                    draw_manager.draw_text(
                        txt,
                        Justify::Left,
                        MENU_X+120, menu_get_y(row),
                        COL_TEXT2);
                    row++;
                }

                row++;

                menu_set_txt(row, COL_TEXT, "Planets");
                snprintf(txt, sizeof(txt), "%d", planets);
                draw_manager.draw_text(
                    txt,
                    Justify::Left,
                    MENU_X+120, menu_get_y(row),
                    COL_TEXT2);

                row++;

                const Fleet &f = p->get_fleet();
                menu_set_txt(row, COL_TEXT, "Fleet size:");
                snprintf(txt, sizeof(txt), "%d", f.size());
                draw_manager.draw_text(
                    txt,
                    Justify::Left,
                    MENU_X+120, menu_get_y(row),
                    COL_TEXT2);

                row += 2;

                if (p->get_location().in_flight()) {
                    int tgt = p->get_location().get_target();
                    FlyTarget *ft = exostate().loc2tgt(tgt);
                    bool star = (ft != exostate().get_galaxy()->get_guild());
                    snprintf(txt, sizeof(txt),
                             "The fleet is flying to the %s%s.",
                             (star?"star ":""),
                             ft->name);
                    menu_set_txt(row, COL_TEXT, txt);
                }
            }
            break;
        case MM_FleetInfo:
            {
                char txt[32];

                menu_set_txt(0, COL_TEXT2, "Fleet information");

                const Fleet &f = p->get_fleet();

                menu_set_txt(2, COL_TEXT, "Scouts:");
                snprintf(txt, sizeof(txt), "%d", f.scouts);
                draw_manager.draw_text(txt, Justify::Left, MENU_X+150, menu_get_y(2), COL_TEXT2);

                menu_set_txt(3, COL_TEXT, "Transporters:");
                snprintf(txt, sizeof(txt), "%d", f.transporters);
                draw_manager.draw_text(txt, Justify::Left, MENU_X+150, menu_get_y(3), COL_TEXT2);
                snprintf(txt, sizeof(txt), "Free: %d units", p->get_freight_capacity());
                draw_manager.draw_text(txt, Justify::Left, MENU_X+240, menu_get_y(3), COL_TEXT);

                menu_set_txt(4, COL_TEXT, "Warships:");
                snprintf(txt, sizeof(txt), "%d", f.warships);
                draw_manager.draw_text(txt, Justify::Left, MENU_X+150, menu_get_y(4), COL_TEXT2);

                menu_set_txt(5, COL_TEXT, "Bombers:");
                snprintf(txt, sizeof(txt), "%d", f.bombers);
                draw_manager.draw_text(txt, Justify::Left, MENU_X+150, menu_get_y(5), COL_TEXT2);

                menu_set_txt(7, COL_TEXT2, "Cargo:");

                const Freight& fr = f.freight;

                menu_set_txt(9, COL_TEXT, "Minerals:");
                snprintf(txt, sizeof(txt), "%d", fr.minerals);
                draw_manager.draw_text(txt, Justify::Left, MENU_X+150, menu_get_y(9), COL_TEXT2);

                menu_set_txt(10, COL_TEXT, "Food:");
                snprintf(txt, sizeof(txt), "%d", fr.food);
                draw_manager.draw_text(txt, Justify::Left, MENU_X+150, menu_get_y(10), COL_TEXT2);

                menu_set_txt(11, COL_TEXT, "Plutonium:");
                snprintf(txt, sizeof(txt), "%d", fr.plutonium);
                draw_manager.draw_text(txt, Justify::Left, MENU_X+150, menu_get_y(11), COL_TEXT2);

                menu_set_txt(12, COL_TEXT, "Robots:");
                snprintf(txt, sizeof(txt), "%d", fr.robots);
                draw_manager.draw_text(txt, Justify::Left, MENU_X+150, menu_get_y(12), COL_TEXT2);

                draw_manager.draw_text("Infantry:", Justify::Left, MENU_X+240, menu_get_y(9), COL_TEXT);
                snprintf(txt, sizeof(txt), "%d", fr.infantry);
                draw_manager.draw_text(txt, Justify::Left, MENU_X+360, menu_get_y(9), COL_TEXT2);

                draw_manager.draw_text("Gliders:", Justify::Left, MENU_X+240, menu_get_y(10), COL_TEXT);
                snprintf(txt, sizeof(txt), "%d", fr.gliders);
                draw_manager.draw_text(txt, Justify::Left, MENU_X+360, menu_get_y(10), COL_TEXT2);

                draw_manager.draw_text("Artillery:", Justify::Left, MENU_X+240, menu_get_y(11), COL_TEXT);
                snprintf(txt, sizeof(txt), "%d", fr.artillery);
                draw_manager.draw_text(txt, Justify::Left, MENU_X+360, menu_get_y(11), COL_TEXT2);
            }
            break;
        case MM_News:
            break;
        case MM_Planets:
            {
                menu_planets_more = false;

                int x_inf = MENU_X + 300;
                int x_gli = x_inf + 46;
                int x_art = x_gli + 46;

                menu_set_txt(0, COL_TEXT2, "List of planets");
                draw_manager.draw_text("Inf", Justify::Centre, x_inf, menu_get_y(0), COL_TEXT2);
                draw_manager.draw_text("Gli", Justify::Centre, x_gli, menu_get_y(0), COL_TEXT2);
                draw_manager.draw_text("Art", Justify::Centre, x_art, menu_get_y(0), COL_TEXT2);

                char n[8];

                bool any = false;
                int ctr = 0;
                for (PlanetIterator piter(p_idx); !piter.complete(); ++piter) {
                    ctr++;

                    if (ctr <= menu_planets_start) {
                        continue;
                    }

                    any = true;

                    Star *st = piter.get_star();
                    Planet *pl = piter.get();

                    int r = 1 + (ctr - menu_planets_start);
                    menu_set_txt(r, COL_TEXT, "%s at %s", pl->get_name(), st->name);

                    int inf, gli, art;
                    pl->get_army(inf, gli, art);

                    snprintf(n, sizeof(n), "%d", inf);
                    draw_manager.draw_text(n, Justify::Centre, x_inf, menu_get_y(r), COL_TEXT);
                    snprintf(n, sizeof(n), "%d", gli);
                    draw_manager.draw_text(n, Justify::Centre, x_gli, menu_get_y(r), COL_TEXT);
                    snprintf(n, sizeof(n), "%d", art);
                    draw_manager.draw_text(n, Justify::Centre, x_art, menu_get_y(r), COL_TEXT);

                    if (r >= 12) {
                        // This is the last row available - check if more are needed
                        ++piter;
                        if (!piter.complete()) {
                            // There are more planets after this one
                            menu_planets_start = ctr;
                            menu_planets_more = true;
                            draw_manager.draw_text("More...", Justify::Right, MENU_X + MENU_W - 8, menu_get_y(14), COL_TEXT);
                            break;
                        }
                    }
                }

                if (!any) {
                    menu_set_txt(2, COL_TEXT, "You have no planets yet.");
                }

            }
            break;
        case MM_Inventions:
            {
                menu_set_txt(0, COL_TEXT2, "Inventions");
                for (int i = 0; i < INV_MAX; ++i) {
                    Invention inv = (Invention)i;
                    RGB col = COL_TEXT;
                    if (!p->has_invention(inv)) {
                        col = COL_TEXT_GREYED;
                    }
                    menu_set_txt(i+1, col, p->get_invention_str(inv));
                }
            }
            break;
        case MM_Invention:
            {
                menu_set_txt(0, COL_TEXT2, "%s Research", p->get_invention_type_str(menu_inv));
                menu_set_txt(1, COL_TEXT2, "Invention: %s", p->get_invention_str(menu_inv));

                if (p->has_invention(menu_inv)) {
                    menu_set_txt(2, COL_TEXT, "(Already invented)");
                } else {
                    menu_set_txt(2, COL_TEXT, "(Not invented yet)");
                }

                const char *t0, *t1, *t2;
                p->get_invention_description(menu_inv, t0, t1, t2);
                menu_set_txt(4, COL_TEXT, t0);
                menu_set_txt(5, COL_TEXT, t1);
                menu_set_txt(6, COL_TEXT, t2);

                uint32_t pre = p->get_invention_prerequisites(menu_inv);

                if (pre == 0) {
                    menu_set_txt(8, COL_TEXT2, "This is a basic invention.");
                } else {
                    menu_set_txt(8, COL_TEXT2, "Previous inventions needed:");
                    int l = 10;
                    for (int i = 0; i < INV_MAX; ++i) {
                        if (pre & (1<<i)) {
                            menu_set_txt(l++, COL_TEXT, p->get_invention_str((Invention)i));
                        }
                    }
                }
            }
            break;
        case MM_Relations:
            {
                char rel[32];

                menu_set_txt(0, COL_TEXT2, "Relationship to other lords");

                int line_idx = 1;

                for (int other_idx = 0; other_idx < N_PLAYERS; ++other_idx) {
                    if (other_idx == p_idx)
                        continue;

                    Player *other = exostate().get_player(other_idx);

                    RGB lordcol = COL_TEXT;
                    RGB relcol = COL_TEXT;
                    const char* display_name = "";

                    if (FEATURE(EF_LORD_DISCOVERY) && !exostate().check_lord_discovery(p_idx, other_idx)) {
                        display_name = "?????";
                        snprintf(rel, sizeof(rel), " ");
                        lordcol = COL_TEXT_GREYED;
                        relcol = COL_TEXT_GREYED;
                    } else {
                        display_name = other->get_full_name();

                        snprintf(rel, sizeof(rel), "None");

                        if (other->is_hostile_to(p_idx)) {
                            snprintf(rel, sizeof(rel), "Hostile");
                            relcol = COL_TEXT_BAD;
                        } else {
                            const char* sep = "";

                            int relidx = 0;
                            if (exostate().has_alliance(p_idx, other_idx, ALLY_Trade)) {
                                relidx += snprintf(rel+relidx, sizeof(rel)-relidx, "%sTrade", sep);
                                relcol = COL_TEXT2;
                                sep = " / ";
                            }
                            if (exostate().has_alliance(p_idx, other_idx, ALLY_NonAttack)) {
                                relidx += snprintf(rel+relidx, sizeof(rel)-relidx, "%sNo Att", sep);
                                relcol = COL_TEXT2;
                                sep = " / ";
                            }
                            if (exostate().has_alliance(p_idx, other_idx, ALLY_War)) {
                                relidx += snprintf(rel+relidx, sizeof(rel)-relidx, "%sWar Ally", sep);
                                relcol = COL_TEXT2;
                                sep = " / ";
                            }
                        }

                        if (!other->is_participating()) {
                            lordcol = COL_TEXT_GREYED;
                            relcol = COL_TEXT_GREYED;
                        }
                    }

                    // TODO_MP: Multiplayer relations here - may be 'friendly' as per 'verhalten'

                    menu_set_txt(line_idx, lordcol, display_name);
                    draw_manager.draw_text(rel, Justify::Left, MENU_X+200, menu_get_y(line_idx), relcol);

                    line_idx++;
                }
            }
            break;
        case MM_ShipStatus:
            {
                draw_manager.draw(IMG_SP1_SHIP1, {MENU_X+248, menu_get_y(1), 0, 0, 2, 2});

                const Starship& s = p->get_starship();

                menu_set_txt(0, COL_TEXT2, "Ship status");
                menu_set_txt(2, COL_TEXT, "Condition: %s", s.damaged() ? "Damaged" : "OK");

                menu_set_txt(4, COL_TEXT2, "Systems:");

                int x = MENU_X + 190;
                char n[8];

                menu_set_txt(6, COL_TEXT, "Shield generators");
                snprintf(n, sizeof(n), "%d", s.shield_generators);
                draw_manager.draw_text(n, Justify::Left, x, menu_get_y(6), COL_TEXT2);

                menu_set_txt(7, COL_TEXT, "Laser guns");
                snprintf(n, sizeof(n), "%d", s.laser_guns);
                draw_manager.draw_text(n, Justify::Left, x, menu_get_y(7), COL_TEXT2);

                menu_set_txt(8, COL_TEXT, "Missile launchers");
                snprintf(n, sizeof(n), "%d", s.missile_launchers);
                draw_manager.draw_text(n, Justify::Left, x, menu_get_y(8), COL_TEXT2);

                menu_set_txt(9, COL_TEXT, "Crew");
                snprintf(n, sizeof(n), "%d", s.crew);
                draw_manager.draw_text(n, Justify::Left, x, menu_get_y(9), COL_TEXT2);

                menu_set_txt(10, COL_TEXT, "Bionic probes");
                snprintf(n, sizeof(n), "%d", s.bionic_probes);
                draw_manager.draw_text(n, Justify::Left, x, menu_get_y(10), COL_TEXT2);

                menu_set_txt(11, COL_TEXT, "Escape capsule");
                draw_manager.draw_text(s.escape_capsule ? "Yes" : "No",
                        Justify::Left, x, menu_get_y(11), COL_TEXT2);

                menu_set_txt(12, COL_TEXT, "Repair hangar");
                draw_manager.draw_text(s.repair_hangar ? "Yes" : "No",
                        Justify::Left, x, menu_get_y(12), COL_TEXT2);
            }
            break;
        case MM_ShipDamage:
            {
                draw_manager.draw(IMG_SP1_SHIP1, {MENU_X+248, menu_get_y(1), 0, 0, 2, 2});

                const Starship& s = p->get_starship();

                menu_set_txt(0, COL_TEXT2, "Ship status");
                menu_set_txt(2, COL_TEXT, "Condition: %s", s.damaged() ? "Damaged" : "OK");

                menu_set_txt(4, COL_TEXT2, "Internal systems:");

                int x = MENU_X + 190;
                char n[16];

                menu_set_txt(6, COL_TEXT, "Thrust engines");
                snprintf(n, sizeof(n), "%d%%", 100 - s.pct_damage_thrust);
                draw_manager.draw_text(n, Justify::Left, x, menu_get_y(6), COL_TEXT2);

                menu_set_txt(7, COL_TEXT, "Comm systems");
                snprintf(n, sizeof(n), "%d%%", 100 - s.pct_damage_comms);
                draw_manager.draw_text(n, Justify::Left, x, menu_get_y(7), COL_TEXT2);

                menu_set_txt(8, COL_TEXT, "Structure");
                snprintf(n, sizeof(n), "%d%%", 100 - s.pct_damage_struct);
                draw_manager.draw_text(n, Justify::Left, x, menu_get_y(8), COL_TEXT2);
            }
            break;
        case MM_Stats:
            {
                menu_set_txt(0, COL_TEXT2, "Statistics about the lords");

                Player* most_planets = 0;
                int most_planets_ctr = 0;

                Player* most_mc = p;
                int most_mc_ctr = 0;

                Player* most_armies = p;
                int most_armies_ctr = 0;

                Player* most_alliances = p;
                int most_alliances_ctr = 0;

                Player* most_inventions = p;
                int most_inventions_ctr = 0;

                Player* most_ships = p;
                int most_ships_ctr = 0;

                for (int i = 0; i < N_PLAYERS; ++i) {
                    Player *pl = exostate().get_player(i);

                    if (!pl->is_participating())
                        continue;

                    int planets = 0;
                    int mc = 0;
                    int armies = 0;
                    int alliances = 0;
                    int inventions = 0;
                    int ships = 0;

                    for (PlanetIterator piter(i); !piter.complete(); ++piter) {
                        ++planets;
                        armies += piter.get()->get_army_size();
                    }

                    mc = pl->get_mc();

                    for (int j = 0; j < N_PLAYERS; ++j) {
                        if (i == j)
                            continue;
                        if (!exostate().get_player(j)->is_participating())
                            continue;
                        if (exostate().has_alliance(i, j, ALLY_Trade))     ++alliances;
                        if (exostate().has_alliance(i, j, ALLY_NonAttack)) ++alliances;
                        if (exostate().has_alliance(i, j, ALLY_War))       ++alliances;
                    }

                    for (int inv = 0; inv < INV_MAX; ++inv) {
                        if (pl->has_invention((Invention)inv)) {
                            ++inventions;
                        }
                    }

                    ships = pl->get_fleet().size();



                    if (planets > most_planets_ctr) {
                        most_planets = pl;
                        most_planets_ctr = planets;
                    }

                    if (mc > most_mc_ctr) {
                        most_mc = pl;
                        most_mc_ctr = mc;
                    }

                    if (armies > most_armies_ctr) {
                        most_armies = pl;
                        most_armies_ctr = armies;
                    }

                    if (alliances > most_alliances_ctr) {
                        most_alliances = pl;
                        most_alliances_ctr = alliances;
                    }

                    if (inventions > most_inventions_ctr) {
                        most_inventions = pl;
                        most_inventions_ctr = inventions;
                    }

                    if (ships > most_ships_ctr) {
                        most_ships = pl;
                        most_ships_ctr = ships;
                    }
                }

                Player *most_awards = exostate().get_player(0);
                int most_awards_ctr = 0;

                for (int i = 0; i < N_PLAYERS; ++i) {
                    Player *pl = exostate().get_player(i);
                    int awards = 0;
                    if (pl == most_planets)    ++awards;
                    if (pl == most_mc)         ++awards;
                    if (pl == most_armies)     ++awards;
                    if (pl == most_alliances)  ++awards;
                    if (pl == most_inventions) ++awards;
                    if (pl == most_ships)      ++awards;

                    if (awards > most_awards_ctr) {
                        most_awards = pl;
                        most_awards_ctr = awards;
                    }
                }

                menu_set_txt( 3, COL_TEXT , "Most Planets:");
                draw_manager.draw_text(most_planets->get_full_name(), Justify::Left, MENU_X+260, menu_get_y(3), COL_TEXT);
                menu_set_txt( 4, COL_TEXT , "Most MC:");
                draw_manager.draw_text(most_mc->get_full_name(), Justify::Left, MENU_X+260, menu_get_y(4), COL_TEXT);
                menu_set_txt( 5, COL_TEXT , "Most Armies:");
                draw_manager.draw_text(most_armies->get_full_name(), Justify::Left, MENU_X+260, menu_get_y(5), COL_TEXT);
                menu_set_txt( 6, COL_TEXT , "Most Alliances:");
                draw_manager.draw_text(most_alliances->get_full_name(), Justify::Left, MENU_X+260, menu_get_y(6), COL_TEXT);
                menu_set_txt( 7, COL_TEXT , "Most Inventions:");
                draw_manager.draw_text(most_inventions->get_full_name(), Justify::Left, MENU_X+260, menu_get_y(7), COL_TEXT);
                menu_set_txt( 8, COL_TEXT , "Most Ships:");
                draw_manager.draw_text(most_ships->get_full_name(), Justify::Left, MENU_X+260, menu_get_y(8), COL_TEXT);
                menu_set_txt(10, COL_TEXT2, "Superior:");
                draw_manager.draw_text(most_awards->get_full_name(), Justify::Left, MENU_X+260, menu_get_y(10), COL_TEXT2);
            }

            break;
    }
}

// Return true if there was a user interaction and we need to redraw
bool MenuDrawer::menu_specific_update() {
    Player *p = exostate().get_active_player();
    int player_idx = exostate().get_player_idx(p);

    switch(menu_mode) {
        case MM_Ctrl:
            // 2: Change Officers & Taxes
            if (menu_row_clicked(2)) {
                menu_open(MM_OfficersAndTaxes);
                return true;
            }
            // 3: Secret Service
            if (menu_row_clicked(3)) {
                menu_open(MM_SecretService);
                return true;
            }
            // 4: Set / Replace Star Markers
            if (menu_row_clicked(4)) {
                menu_open(MM_StarMarker);
                return true;
            }
            // 5: Equip Starship
            if (menu_row_clicked(5)) {
                menu_action = MA_EquipShip;
            }
            // 6: Build Artificial Planet
            if (menu_row_clicked(6)) {
                // Cost is refunded on cancel
                if (p->attempt_spend(COST_ART, MC_ArtPlanet)) {
                    menu_art_planet_phase = 0;
                    menu_art_planet_named = false;
                    menu_open(MM_ArtificialWorld);
                    return true;
                } else {
                    L.error("Should not have offered unaffordable option");
                }
            }
            // 8: Wait One Month
            if (menu_row_clicked(8)) {
                menu_action = MA_WaitOneMonth;
            }
            // 10: Show Distances
            if (menu_row_clicked(10)) {
                menu_new_mode = MODE_Distances;
                return true;
            }
            // 11: Save Game
            if (menu_row_clicked(11)) {
                // Re-open save menu to refresh metadata
                menu_open(MM_Save);
                return true;
            }
            // 12: Quit Game
            if (menu_row_clicked(12)) {
                menu_action = MA_Quit;
            }
            // 14: Exit Menu
            if (menu_row_clicked(14)) {
                menu_action = MA_Close;
            }
            break;
        case MM_OfficersAndTaxes:
            {
                if (first_update) {
                    draw_manager.draw_text(
                        id_menu_scimore,
                        "More",
                        Justify::Left,
                        MENU_X+180, menu_get_y(2),
                        COL_TEXT2);
                    draw_manager.draw_text(
                        id_menu_taxmore,
                        "More",
                        Justify::Left,
                        MENU_X+180, menu_get_y(3),
                        COL_TEXT2);

                    draw_manager.draw_text(
                        "Rating",
                        Justify::Centre,
                        MENU_X+254, menu_get_y(6),
                        COL_TEXT);
                    draw_manager.draw_text(
                        "Cost",
                        Justify::Centre,
                        MENU_X+326, menu_get_y(6),
                        COL_TEXT);

                    for (int i = 0; i < MENU_N_OFFICERS; ++i) {
                        if (FEATURE(EF_COMBINE_SS_COUNSELLOR)) {
                            if ((Officer)i == OFF_Counsellor) {
                                continue;
                            }
                        }
                        OfficerQuality q = p->get_officer((Officer)i);
                        const char* s = "poor";
                        if (q == OFFQ_Average) s = "average";
                        if (q == OFFQ_Good) s = "good";
                        draw_manager.draw_text(
                            s,
                            Justify::Centre,
                            MENU_X+254, menu_get_y(8+i),
                            COL_TEXT);

                        int cost = p->get_officer_cost((Officer)i);
                        char str[10];
                        snprintf(str, sizeof(str), "%d", cost);
                        draw_manager.draw_text(
                            str,
                            Justify::Centre,
                            MENU_X+326, menu_get_y(8+i),
                            COL_TEXT);

                        draw_manager.draw_text(
                            id_menu_newoff[i],
                            "New",
                            Justify::Centre,
                            MENU_X+400, menu_get_y(8+i),
                            COL_TEXT2);
                    }
                }

                int t = p->get_tax();
                bool redraw_scitax = first_update;
                // FIXME: The Player concept of 'tax' is actually reversed
                // from the meaning here. On Player, tax=MC reserved for sci
                // Here, tax = MC% receive from income.
                if (draw_manager.query_click(id_menu_taxmore).id) {
                    redraw_scitax = true;
                    p->set_tax(t-1);
                }
                if (draw_manager.query_click(id_menu_scimore).id) {
                    redraw_scitax = true;
                    p->set_tax(t+1);
                }

                if (redraw_scitax) {
                    t = p->get_tax();
                    char pct[16];
                    snprintf(pct, sizeof(pct), "%d%%", t);
                    draw_manager.draw_text(
                        id_menu_sci,
                        pct,
                        Justify::Left,
                        MENU_X+100, menu_get_y(2),
                        COL_TEXT);
                    snprintf(pct, sizeof(pct), "%d%%", 100-t);
                    draw_manager.draw_text(
                        id_menu_tax,
                        pct,
                        Justify::Left,
                        MENU_X+100, menu_get_y(3),
                        COL_TEXT);
                }

                for (int i = 0; i < MENU_N_OFFICERS; ++i) {
                    if (draw_manager.query_click(id_menu_newoff[i]).id) {
                        menu_new_officer = (Officer)i;
                        menu_new_officer_quality = OFFQ_Poor;
                        menu_open(MM_NewOfficer);
                        return true;
                    }
                }

                if (menu_row_clicked(14)) {
                    menu_open(MM_Ctrl);
                    return true;
                }
            }
            break;
        case MM_NewOfficer:
            {
                OfficerQuality q = menu_new_officer_quality;
                int engagement_cost = p->get_officer_initial_cost(q);

                if (first_update) {
                    const char* q_str = "poor";
                    if (q == OFFQ_Average) q_str = "average";
                    if (q == OFFQ_Good) q_str = "good";
                    draw_manager.draw_text(
                        q_str,
                        Justify::Left,
                        MENU_X + 340, menu_get_y(2),
                        COL_TEXT2);

                    char cost_str[8];
                    snprintf(cost_str, sizeof(cost_str), "%d", engagement_cost);
                    draw_manager.draw_text(
                        cost_str,
                        Justify::Left,
                        MENU_X + 340, menu_get_y(3),
                        COL_TEXT2);

                    int cost = p->get_officer_cost(q);
                    snprintf(cost_str, sizeof(cost_str), "%d", cost);
                    draw_manager.draw_text(
                        cost_str,
                        Justify::Left,
                        MENU_X + 340, menu_get_y(4),
                        COL_TEXT2);

                    draw_manager.draw(
                        id_menu_newoff_opt,
                        IMG_BR8_EXPORT,
                        {RES_X/2, MENU_Y + MENU_H-MENU_BORDER-2,
                         .5, 1, 1, 1});
                    draw_manager.fill(
                        FILL_3D_Out,
                        {MENU_X+MENU_BORDER+2, MENU_Y+MENU_H-MENU_BORDER-28, 35, 26},
                        COL_BORDERS);
                    draw_manager.fill(
                        FILL_3D_Out,
                        {MENU_X+MENU_W-MENU_BORDER-37, MENU_Y+MENU_H-MENU_BORDER-28, 35, 26},
                        COL_BORDERS);
                }

                SpriteClick clk = draw_manager.query_click(id_menu_newoff_opt);
                if (clk.id) {
                    if (clk.x < .33f) {
                        if (p->attempt_spend(engagement_cost, MC_NewOfficer)) {
                            menu_old_officer_quality = p->get_officer(menu_new_officer);
                            draw_manager.draw(id_menu_newoff_opt, nullptr);
                            p->set_officer(menu_new_officer, menu_new_officer_quality);
                            menu_open(MM_OldOfficer);
                            return true;
                        }
                    } else if (clk.x < .66f) {
                        q = (OfficerQuality)(((int)q + 1) % (int)OFFQ_MAX);
                        menu_new_officer_quality = q;
                        menu_open(MM_NewOfficer);
                        return true;
                    } else {
                        menu_open(MM_OfficersAndTaxes);
                        return true;
                    }
                }
            }
            break;
        case MM_OldOfficer:
            {
                if (menu_row_clicked(2)) {
                    if (p->attempt_spend(OFF_PAY_MC, MC_OldOfficer)) {
                        p->adjust_reputation(1);
                        menu_open(MM_OldOfficerPaid);
                        return true;
                    }
                }

                if (menu_row_clicked(3)) {
                    p->adjust_reputation(-1);
                    menu_open(MM_OldOfficerDismissed);
                    return true;
                }

                if (menu_row_clicked(4)) {
                    p->adjust_reputation(-1);
                    for (PlanetIterator piter(player_idx); !piter.complete(); ++piter) {
                        piter.get()->adjust_unrest(1);
                    }
                    menu_open(MM_OldOfficerKilled);
                    return true;
                }
            }
            break;
        case MM_OldOfficerPaid:
        case MM_OldOfficerDismissed:
        case MM_OldOfficerKilled:
            if (draw_manager.clicked()) {
                menu_open(MM_OfficersAndTaxes);
                return true;
            }
            break;
        case MM_NewOfficerCharacter:
            {
                OfficerQuality q = menu_new_officer_quality;
                int engagement_cost = p->get_officer_initial_cost(q);

                if (first_update) {
                    draw_manager.draw_text(
                        Player::get_officer_character_title(menu_new_officer, q),
                        Justify::Left,
                        MENU_X + 300, menu_get_y(2),
                        COL_TEXT);

                    draw_manager.draw_text(
                        Player::get_officer_character_name(menu_new_officer, q),
                        Justify::Left,
                        MENU_X + 300, menu_get_y(3),
                        COL_TEXT);

                    const char* q_str = "poor";
                    if (q == OFFQ_Average) q_str = "average";
                    if (q == OFFQ_Good) q_str = "good";
                    draw_manager.draw_text(
                        q_str,
                        Justify::Left,
                        MENU_X + 300, menu_get_y(4),
                        COL_TEXT2);

                    char cost_str[8];
                    snprintf(cost_str, sizeof(cost_str), "%d", engagement_cost);
                    draw_manager.draw_text(
                        cost_str,
                        Justify::Left,
                        MENU_X + 300, menu_get_y(5),
                        COL_TEXT2);

                    int cost = p->get_officer_cost(q);
                    snprintf(cost_str, sizeof(cost_str), "%d", cost);
                    draw_manager.draw_text(
                        cost_str,
                        Justify::Left,
                        MENU_X + 300, menu_get_y(6),
                        COL_TEXT2);

                    const char* off_status = "Alive";
                    RGB off_status_col = {0, 0xFF, 0};

                    if (p->officer_fired_nopay(menu_new_officer, menu_new_officer_quality)) {
                        off_status = "Fired W/O Pay";
                        off_status_col = COL_TEXT;
                    }

                    if (p->officer_quit(menu_new_officer, menu_new_officer_quality)) {
                        off_status = "Quit";
                        off_status_col = COL_TEXT;
                    }

                    if (p->officer_killed(menu_new_officer, menu_new_officer_quality)) {
                        off_status = "Incarcerated";
                        off_status_col = COL_TEXT_BAD;
                    }

                    if (p->get_officer(menu_new_officer) == menu_new_officer_quality) {
                        off_status = "Employed";
                        off_status_col = {0, 0xFF, 0};
                    }

                    draw_manager.draw_text(
                        off_status,
                        Justify::Left,
                        MENU_X + 300, menu_get_y(7),
                        off_status_col);

                    draw_manager.draw(
                        id_menu_newoff_opt,
                        IMG_BR8_EXPORT,
                        {RES_X/2, MENU_Y + MENU_H-MENU_BORDER-2,
                         .5, 1, 1, 1});
                    draw_manager.fill(
                        FILL_3D_Out,
                        {MENU_X+MENU_BORDER+2, MENU_Y+MENU_H-MENU_BORDER-28, 35, 26},
                        COL_BORDERS);
                    draw_manager.fill(
                        FILL_3D_Out,
                        {MENU_X+MENU_W-MENU_BORDER-37, MENU_Y+MENU_H-MENU_BORDER-28, 35, 26},
                        COL_BORDERS);
                }

                SpriteClick clk = draw_manager.query_click(id_menu_newoff_opt);
                if (clk.id) {
                    if (clk.x < .33f) {
                        bool ok = true;

                        const char* tn = Player::get_officer_character_title_and_name(menu_new_officer, menu_new_officer_quality);
                        if (p->get_officer(menu_new_officer) == menu_new_officer_quality) {
                            menu_set_txt(9,  COL_TEXT, "%s in already in your employ,", tn);
                            menu_set_txt(10,  COL_TEXT, "%s.", p->get_ref());
                            menu_set_txt(11,  COL_TEXT, "");
                            menu_set_txt(12,  COL_TEXT, "");
                            ok = false;
                        } else if (p->officer_fired_nopay(menu_new_officer, menu_new_officer_quality)) {
                            if ((exostate().get_month() % 2) == 0) {
                                menu_set_txt(9,  COL_TEXT, "%s is angry about having been", tn);
                                menu_set_txt(10,  COL_TEXT, "dismissed without pay, and has refused");
                                menu_set_txt(11,  COL_TEXT, "your offer. Perhaps another time...");
                                menu_set_txt(12,  COL_TEXT, "");
                                ok = false;
                            }
                        } else if (p->officer_quit(menu_new_officer, menu_new_officer_quality)) {
                            if ((exostate().get_month() % 2) == 0) {
                                menu_set_txt(9,  COL_TEXT, "%s is angry about having been", tn);
                                menu_set_txt(10,  COL_TEXT, "unpaid, and has refused your offer.");
                                menu_set_txt(11,  COL_TEXT, "Perhaps another time...");
                                menu_set_txt(12,  COL_TEXT, "");
                                ok = false;
                            }
                        }

                        if (ok && !p->attempt_spend(engagement_cost, MC_NewOfficer)) {
                            ok = false;
                            menu_set_txt(9,  COL_TEXT, "We cannot afford %s, %s.", tn, p->get_ref());
                            menu_set_txt(10,  COL_TEXT, "");
                            menu_set_txt(11,  COL_TEXT, "");
                            menu_set_txt(12,  COL_TEXT, "");
                        }

                        if (ok) {
                            menu_old_officer_quality = p->get_officer(menu_new_officer);
                            draw_manager.draw(id_menu_newoff_opt, nullptr);
                            p->set_officer(menu_new_officer, menu_new_officer_quality);
                            menu_open(MM_OldOfficer);
                            return true;
                        }
                    } else if (clk.x < .66f) {
                        q = (OfficerQuality)(((int)q + 1) % (int)OFFQ_MAX);
                        menu_new_officer_quality = q;
                        menu_open(MM_NewOfficer);
                        return true;
                    } else {
                        menu_open(MM_OfficersAndTaxes);
                        return true;
                    }
                }
            }
            break;
        case MM_SecretService:
            // 4: Information Services (20+ MC)
            if (menu_row_clicked(4)) {
                menu_open(MM_SecInfo);
                return true;
            }

            // 5: A Personal File
            if (menu_row_clicked(5)) {
                if (p->attempt_spend(COST_FILE, MC_SecretService)) {
                    menu_open_player_select(MM_SecPersonalFile);
                    return true;
                }
            }

            // 6: Terrorist Attacks
            if (menu_row_clicked(6)) {
                if (p->has_mission()) {
                    menu_open(MM_SecAttackAlreadyPlanned);
                } else {
                    menu_open(MM_SecAttack);
                }
                return true;
            }

            // 7: Orbital Bomb Attacks
            if (menu_row_clicked(7)) {
                if (p->has_invention(INV_OrbitalBombs)) {
                    if (p->has_mission()) {
                        menu_open(MM_SecAttackAlreadyPlanned);
                    } else {
                        menu_open(MM_SecBombing);
                    }
                } else {
                    menu_open(MM_SecBombingNoTech);
                }
                return true;
            }

            // 14: Exit Menu
            if (menu_row_clicked(14)) {
                menu_open(MM_Ctrl);
                return true;
            }
            break;
        case MM_SecInfo:
            // 3: Amount of money
            if (menu_row_clicked(3)) {
                if (p->attempt_spend(COST_INF_MC, MC_SecretService)) {
                    menu_open_player_select(MM_SecInfoMC);
                    return true;
                }
            }

            // 4: Allies
            if (menu_row_clicked(4)) {
                if (p->attempt_spend(COST_INF_ALLIES, MC_SecretService)) {
                    menu_open_player_select(MM_SecInfoAllies);
                    return true;
                }
            }

            // 5: Number of planets
            if (menu_row_clicked(5)) {
                if (p->attempt_spend(COST_INF_NPLANS, MC_SecretService)) {
                    menu_open_player_select(MM_SecInfoPlanets);
                    return true;
                }
            }

            // 6: Inventions
            if (menu_row_clicked(6)) {
                if (p->attempt_spend(COST_INF_INV, MC_SecretService)) {
                    menu_open_player_select(MM_SecInfoInventions);
                    return true;
                }
            }

            // 7: Planet surface
            if (menu_row_clicked(7)) {
                if (p->can_afford(COST_INF_SURF)) {
                    ephstate.select_planet(SPR_PlanetSurface);
                    ephstate.selectplanet_mc = COST_INF_SURF;
                    ephstate.selectplanet_mc_reason = MC_SecretService;
                    menu_new_mode = ephstate.get_appropriate_mode();
                    return true;
                }
            }

            // 14: Exit Menu
            if (menu_row_clicked(14)) {
                menu_open(MM_SecretService);
                return true;
            }
            break;
        case MM_PlayerChoice:
            if (menu_player_selected()) {
                menu_open(menu_player_select_mode);
                return true;
            }
            break;
        case MM_SecInfoMC:
        case MM_SecInfoAllies:
        case MM_SecInfoPlanets:
        case MM_SecInfoInventions:
        case MM_SecPersonalFile:
            if (draw_manager.clicked()) {
                menu_open(MM_SecretService);
                return true;
            }
            break;
        case MM_SecAttack:
            {
                MissionType mission = MT_None;

                int &star_idx = p->get_mission_star_ref();
                int &planet_idx = p->get_mission_planet_ref();

                int cost = 0;

                //  4: command station
                if (mission == MT_None && menu_row_clicked(4)) {
                    cost = COST_ATT_CMD;
                    mission = MT_TerrorComm;
                }

                //  5: cultivated area
                if (mission == MT_None && menu_row_clicked(5)) {
                    cost = COST_ATT_AGRI;
                    mission = MT_TerrorAgri;
                }

                //  6: plutonium production
                if (mission == MT_None && menu_row_clicked(6)) {
                    cost = COST_ATT_PLU;
                    mission = MT_TerrorPlu;
                }

                //  7: army production
                if (mission == MT_None && menu_row_clicked(7)) {
                    cost = COST_ATT_ARMY;
                    mission = MT_TerrorArmy;
                }

                //  8: spaceport
                if (mission == MT_None && menu_row_clicked(8)) {
                    cost = COST_ATT_PORT;
                    mission = MT_TerrorPort;
                }

                //  9: trading centre
                if (mission == MT_None && menu_row_clicked(9)) {
                    cost = COST_ATT_TRADE;
                    mission = MT_TerrorTrade;
                }

                // 10: mining
                if (mission == MT_None && menu_row_clicked(10)) {
                    cost = COST_ATT_MINE;
                    mission = MT_TerrorMine;
                }

                if (mission != MT_None && !p->can_afford(cost)) {
                    mission = MT_None;
                    cost = 0;
                }

                if (mission != MT_None) {
                    ephstate.select_planet(&star_idx, &planet_idx);
                    ephstate.selectplanet_mc = cost;
                    ephstate.selectplanet_mc_reason = MC_Missions;
                    ephstate.selectplanet_mission = mission;
                    return false;
                }

                // 14: Exit Menu
                if (menu_row_clicked(14)) {
                    menu_open(MM_SecretService);
                    return true;
                }
            }
            break;
        case MM_SecAttackAlreadyPlanned:
            {
                if (draw_manager.clicked()) {
                    menu_open(MM_SecretService);
                    return true;
                }
            }
            break;
        case MM_SecBombing:
            {
                int &star_idx = p->get_mission_star_ref();
                int &planet_idx = p->get_mission_planet_ref();

                //  4: random bombing
                if (menu_row_clicked(4)) {
                    if (p->can_afford(COST_ATT_BOMB)) {
                        ephstate.select_planet(&star_idx, &planet_idx);
                        ephstate.selectplanet_mc = COST_ATT_BOMB;
                        ephstate.selectplanet_mc_reason = MC_Missions;
                        ephstate.selectplanet_trace = TRACE_PlanetsBombed;
                        ephstate.selectplanet_mission = MT_RandomBomb;
                        return false;
                    }
                }

                //  5: nuclear extermination
                if (menu_row_clicked(5)) {
                    if (p->can_afford(COST_ATT_NUKE)) {
                        ephstate.select_planet(&star_idx, &planet_idx);
                        ephstate.selectplanet_mc = COST_ATT_NUKE;
                        ephstate.selectplanet_mc_reason = MC_Missions;
                        ephstate.selectplanet_trace = TRACE_PlanetsNuked;
                        ephstate.selectplanet_mission = MT_Nuclear;
                        return false;
                    }
                }

                // 14: Exit Menu
                if (menu_row_clicked(14)) {
                    menu_open(MM_SecretService);
                    return true;
                }
            }
            break;
        case MM_SecBombingNoTech:
            {
                if (draw_manager.clicked()) {
                    menu_open(MM_SecretService);
                    return true;
                }
            }
            break;
        case MM_StarMarker:
            if (marker_being_set < 0) {
                for (int i = 0; i < N_MARKERS; ++i) {
                    if (menu_row_clicked(i+2)) {
                        marker_being_set = i;
                        const StarMarker *m = p->get_marker(i);
                        input_manager.start_text_input();
                        draw_manager.enable_text_cursor(id_menu_marker_entry_text);

                        if (m) {
                            input_manager.set_input_text(m->tag);
                        }

                        int y = menu_get_y(i+2);
                        draw_manager.fill(
                            id_menu_marker_entry_border,
                            {MENU_TEXT_X + 50-BORDER, y-BORDER,
                             MARKER_ENTRY_W+2*BORDER, MARKER_ENTRY_H+2*BORDER},
                            COL_BORDERS);
                        draw_manager.fill(
                            FILL_3D_In_Hollow,
                            {MENU_TEXT_X + 50-BORDER, y-BORDER,
                             MARKER_ENTRY_W+2*BORDER, MARKER_ENTRY_H+2*BORDER},
                            COL_BORDERS);
                        draw_manager.fill(
                            id_menu_marker_entry,
                            {MENU_TEXT_X + 50, y,
                             MARKER_ENTRY_W, MARKER_ENTRY_H},
                             {0, 0, 0});
                    }
                }
            } else {
                int y = menu_get_y(marker_being_set+2);

                draw_manager.draw_text(
                    id_menu_marker_entry_text,
                    input_manager.get_input_text(MAX_MARKER),
                    Justify::Left,
                    MENU_TEXT_X + 54, y,
                    COL_TEXT);

                if (input_manager.consume(K_Backspace)) {
                    input_manager.backspace();
                }

                if (input_manager.consume(K_Enter)) {
                    const char* new_marker = input_manager.get_input_text(MAX_MARKER);
                    if (new_marker[0] != '\0') {
                        p->set_marker_tag(marker_being_set, new_marker);
                        input_manager.stop_text_input();
                        draw_manager.disable_text_cursor();
                        StarMarker *marker = p->get_marker(marker_being_set);
                        ephstate.select_planet(SPR_MarkStar, &marker->idx, nullptr);
                        menu_new_mode = ephstate.get_appropriate_mode();
                        return true;
                    }
                }
            }
            break;
        case MM_EquipShip:
            break;
        case MM_ArtificialWorld:
            if (!menu_art_planet_named) {
                if (input_manager.consume(K_Backspace)) {
                    input_manager.backspace();
                }

                const char* name = input_manager.get_input_text(PLANET_MAX_NAME);

                draw_manager.draw_text(
                    id_menu_artplanname,
                    name,
                    Justify::Left,
                    MENU_X+200, menu_get_y(9),
                    COL_TEXT);

                if (input_manager.consume(K_Enter) && strnlen(name, 1)) {
                    input_manager.stop_text_input();
                    draw_manager.disable_text_cursor();
                    menu_art_planet_named = true;

                    draw_manager.draw_text(
                        "Now please select the planet's position.",
                        Justify::Left,
                        MENU_X+40, menu_get_y(12),
                        COL_TEXT);
                }
            } else if (draw_manager.clicked()) {
                if (menu_art_planet_phase == 0) {
                    // New planet needs placing
                    menu_action = MA_BuildArtificialWorld;
                } else {
                    // Planet was advanced
                    menu_open(MM_Ctrl);
                    return true;
                }
            }
            break;
        case MM_Save:
            for (int i = 1; i < MAX_SLOTS; ++i) {
                if (menu_row_clicked(i+1)) {
                    save_manager.save(i);
                    QUICKSAVE_SLOT = i;
                    menu_open(MM_Save);
                    return true;
                }
            }
            if (menu_row_clicked(14)) {
                menu_open(MM_Ctrl);
                return true;
            }
            break;
        case MM_Stat:
            // 2: General Information
            if (menu_row_clicked(2)) {
                menu_open(MM_GenInfo);
                return true;
            }
            // 3: Fleet Information
            if (menu_row_clicked(3)) {
                menu_open(MM_FleetInfo);
                return true;
            }
            // 4: Recall Latest News
            if (menu_row_clicked(4)) {
                if (FEATURE(EF_FINANCE_MENU)) {
                    menu_new_mode = MODE_Finance;
                } else {
                    menu_new_mode = MODE_News;
                }
                return true;
            }
            // 5: List Planets
            if (menu_row_clicked(5)) {
                menu_planets_start = 0;
                menu_open(MM_Planets);
                return true;
            }
            // 6: List Inventions
            if (menu_row_clicked(6)) {
                menu_open(MM_Inventions);
                return true;
            }
            // 7: Relationship to Lords
            if (menu_row_clicked(7)) {
                menu_open(MM_Relations);
                return true;
            }
            // 9: Starship Status
            if (menu_row_clicked(9)) {
                menu_open(MM_ShipStatus);
                return true;
            }
            // 10: Starship Damage
            if (menu_row_clicked(10)) {
                menu_open(MM_ShipDamage);
                return true;
            }
            // 12: Statistics
            if (menu_row_clicked(12)) {
                menu_open(MM_Stats);
                return true;
            }
            // 14: Exit Menu
            if (menu_row_clicked(14)) {
                menu_action = MA_Close;
            }
            break;
        case MM_GenInfo:
            if (draw_manager.clicked()) {
                menu_open(MM_Stat);
                return true;
            }
            break;
        case MM_FleetInfo:
            if (draw_manager.clicked()) {
                menu_open(MM_Stat);
                return true;
            }
            break;
        case MM_News:
            break;
        case MM_Planets:
            if (draw_manager.clicked()) {
                if (menu_planets_more) {
                    menu_open(MM_Planets);
                    return true;
                }
                menu_open(MM_Stat);
                return true;
            }
            break;
        case MM_Inventions:
            for (int i = 0; i < INV_MAX; ++i) {
                if (draw_manager.query_click(id_menu_lines[i+1]).id) {
                    menu_open_invention((Invention)i);
                    return true;
                }
            }
            if (draw_manager.clicked()) {
                menu_open(MM_Stat);
                return true;
            }
            break;
        case MM_Invention:
            if (draw_manager.clicked()) {
                menu_open(MM_Inventions);
                return true;
            }
            break;
        case MM_Relations:
            if (draw_manager.clicked()) {
                menu_open(MM_Stat);
                return true;
            }
            break;
        case MM_ShipStatus:
            if (draw_manager.clicked()) {
                menu_open(MM_Stat);
                return true;
            }
            break;
        case MM_ShipDamage:
            if (draw_manager.clicked()) {
                menu_open(MM_Stat);
                return true;
            }
            break;
        case MM_Stats:
            if (draw_manager.clicked()) {
                menu_open(MM_Stat);
                return true;
            }
            break;
    }

    first_update = false;

    return false;
}

bool MenuDrawer::menu_row_clicked(int row) {
    if ((menu_clickable_rows & (1<<row)) == 0) {
        return false;
    }
    return draw_manager.query_click(id_menu_lines[row]).id;
}

void MenuDrawer::menu_open_player_select(MenuMode mode) {
    menu_player_select_mode = mode;
    menu_open(MM_PlayerChoice);
}

void MenuDrawer::menu_print_other_players() {
    int player_idx = exostate().get_active_player_idx();
    menu_set_txt(0, COL_TEXT2, "Please select the desired lord.");
    int row = 1;
    for (int i = 0; i < N_PLAYERS; ++i) {
        if (i == player_idx) continue;
        Player *tgt = exostate().get_player(i);
        menu_line_players[row] = i;
        menu_set_opt(row, tgt->get_full_name(), tgt->is_participating());
        row++;
    }
}

bool MenuDrawer::menu_player_selected() {
    // Skip the first row as this is text
    // FIXME: Need a way to go back if no options available
    for (int row = 1; row < MENU_LINES; ++row) {
        if (draw_manager.query_click(id_menu_lines[row]).id) {
            Player *pl = exostate().get_player(menu_line_players[row]);
            if (pl->is_participating()) {
                menu_selected_player = pl;
                return true;
            }
        }
    }
    return false;
}
