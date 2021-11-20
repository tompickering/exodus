#include "menu_drawer.h"

#include "state/exodus_state.h"
#include "state/ephemeral_state.h"
#include "save/save.h"
#include "input/input.h"
#include "util/iter.h"

#include "shared.h"
#include "assetpaths.h"

#define COST_INF_MC       20
#define COST_INF_ALLIES   20
#define COST_INF_NPLANS   20
#define COST_INF_INV      50
#define COST_INF_SURF    100

#define COST_FILE        100

#define COST_ATT_CMD     200
#define COST_ATT_AGRI    200
#define COST_ATT_PLU     150
#define COST_ATT_ARMY     70
#define COST_ATT_PORT    100
#define COST_ATT_TRADE   100
#define COST_ATT_MINE     50

#define COST_ATT_BOMB    500
#define COST_ATT_NUKE   1000

#define OFF_PAY_MC        50

#define COST_ART        1000

extern ExodusState exostate;
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

MenuDrawer::MenuDrawer() {
    _menu_is_open = false;
    menu_new_officer = (Officer)0;
}

bool MenuDrawer::menu_is_open() {
    return _menu_is_open;
}

MenuAction MenuDrawer::menu_get_action() {
    return menu_action;
}

void MenuDrawer::menu_open(MenuMode mode) {
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
        id_menu_header_flag_bg = draw_manager.new_sprite_id();
        id_menu_header_flag    = draw_manager.new_sprite_id();
        id_menu_header_l       = draw_manager.new_sprite_id();
        id_menu_header_r       = draw_manager.new_sprite_id();
        id_menu_panel          = draw_manager.new_sprite_id();
        id_menu_bg             = draw_manager.new_sprite_id();
        id_menu_sci            = draw_manager.new_sprite_id();
        id_menu_tax            = draw_manager.new_sprite_id();
        id_menu_scimore        = draw_manager.new_sprite_id();
        id_menu_taxmore        = draw_manager.new_sprite_id();
        id_menu_newoff_opt     = draw_manager.new_sprite_id();

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

        // Draw header flag and background
        draw_manager.fill(
            id_menu_header_flag_bg,
            {MENU_FLAG_BG_X, MENU_FLAG_BG_Y,
             MENU_FLAG_BG_W, MENU_FLAG_BG_H},
             COL_BORDERS);

        draw_manager.draw(
            id_menu_header_flag,
            flags[exostate.get_active_player()->get_flag_idx()],
            {MENU_FLAG_BG_X + MENU_BORDER,
             MENU_FLAG_BG_Y + MENU_BORDER,
             0, 0, 1, 1});

        draw_manager.draw(
            id_menu_header_l,
            IMG_TS1_HEADER,
            {MENU_FLAG_BG_X - 2,
             MENU_Y - 2,
             1, 1, 1, 1});

        draw_manager.draw(
            id_menu_header_r,
            IMG_TS1_HEADER,
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

void MenuDrawer::menu_close() {
    menu_action = MA_None;

    for (int i = 0; i < MENU_LINES; ++i) {
        draw_manager.draw(id_menu_lines[i], nullptr);
        draw_manager.release_sprite_id(id_menu_lines[i]);
    }

    draw_manager.draw(id_menu_newoff_opt, nullptr);
    draw_manager.draw(id_menu_sci, nullptr);
    draw_manager.draw(id_menu_tax, nullptr);
    draw_manager.draw(id_menu_scimore, nullptr);
    draw_manager.draw(id_menu_taxmore, nullptr);
    draw_manager.draw(id_menu_header_flag_bg, nullptr);
    draw_manager.draw(id_menu_header_flag, nullptr);
    draw_manager.draw(id_menu_header_l, nullptr);
    draw_manager.draw(id_menu_header_r, nullptr);
    draw_manager.draw(id_menu_panel, nullptr);
    draw_manager.draw(id_menu_bg, nullptr);

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

        if (!menu_specific_update()) {
            break;
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
            return IMG_ME2_MENU;
        default:
            return IMG_ME1_MENU;
    }
    return IMG_ME1_MENU;
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
    Player *p = exostate.get_active_player();
    int p_idx = exostate.get_player_idx(p);
    bool art_ok = true;
    first_update = true;

    if (!p->can_afford(COST_ART)) art_ok = false;
    if (!p->has_invention(INV_OrbitalMassConstruction)) art_ok = false;
    // TODO: Also check that the player doesn't have an artificial planet
    // (or rather, whatever pphase != 3 implies).

    switch(menu_mode) {
        case MM_Ctrl:
            menu_set_txt(0, COL_TEXT2, "Please select, %s", p->get_ref());
            menu_set_opt(2, "Change Officers & Taxes");
            menu_set_opt(3, "Secret Service");
            // TODO: Hotkey
            menu_set_opt(4, "Set / Replace Star Markers (M)");
            // TODO: Hotkey
            menu_set_opt(5, "Equip Starship (E)");
            menu_set_opt(6, "Build Artificial Planet (" STR(COST_ART) "MC)", art_ok);
            menu_set_opt(8, "Wait One Month (Spc)");
            // TODO: Hotkey
            menu_set_opt(10, "Show Distances (D)");
            // TODO: Hotkey
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
                menu_set_txt(8,  COL_TEXT,  "Science Officer");
                menu_set_txt(9,  COL_TEXT,  "Fleet Admiral");
                menu_set_txt(10, COL_TEXT,  "Battle General");
                menu_set_txt(11, COL_TEXT,  "Secret Service Leader");
                menu_set_txt(12, COL_TEXT,  "Ship Counsellor");
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
                menu_set_txt(0, COL_TEXT, "What do you want to do with the old officer?");
                menu_set_opt(2, "Pay " STR(OFF_PAY_MC) "MC for their work", p->can_afford(OFF_PAY_MC));
                menu_set_opt(3, "Dismiss");
                menu_set_opt(4, "Kill");
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
                menu_set_txt(0, COL_TEXT, "The old officer has been dismissed without");
                menu_set_txt(1, COL_TEXT, "any payment for their work.");
            }
            break;
        case MM_OldOfficerKilled:
            {
                menu_set_txt(0, COL_TEXT, "The old officer has been shot.");
                menu_set_txt(1, COL_TEXT, "Will this be good for your reputation?");
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
                int pl_idx = exostate.get_player_idx(pl);
                for (int i = 0; i < N_PLAYERS; ++i) {
                    if (i == pl_idx) continue;
                    if (exostate.is_allied(i, pl_idx)) {
                        Player *ally = exostate.get_player(i);
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
                int n = exostate.get_n_planets(pl);
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
                // TODO
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
        case MM_StarMarker:
            break;
        case MM_EquipShip:
            break;
        case MM_ArtificialWorld:
            break;
        case MM_Save: {
            const SaveMeta *meta = save_manager.get_all_meta(true);
            menu_set_txt(0, COL_TEXT2, "Save game in slot:");
            for (int i = 0; i < MAX_SLOTS; ++i) {
                if (meta[i].exists) {
                    // TODO: Position these elements nicely
                    menu_set_opt(i+2, "%d: %s / Month %d / Planets %d",
                                 i+1, meta[i].name, meta[i].month, meta[i].planets);
                } else {
                    menu_set_opt(i+2, "%d", i+1);
                }
            }
            menu_set_opt(14, "Exit Menu");
            break;
            }
        case MM_Stat:
            menu_set_txt(0, COL_TEXT2, "Please select, %s", p->get_ref());
            menu_set_opt(2, "General Information");
            menu_set_opt(3, "Fleet Information");
            // TODO: Hotkey
            menu_set_opt(4, "Recall Latest News (R)");
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

                menu_set_txt(0, COL_TEXT2, "Status file, Month %d", exostate.get_month());

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

                menu_set_txt(9, COL_TEXT, "Planets");
                snprintf(txt, sizeof(txt), "%d", planets);
                draw_manager.draw_text(
                    txt,
                    Justify::Left,
                    MENU_X+120, menu_get_y(9),
                    COL_TEXT2);

                const Fleet &f = p->get_fleet();
                menu_set_txt(10, COL_TEXT, "Fleet size:");
                snprintf(txt, sizeof(txt), "%d", f.size());
                draw_manager.draw_text(
                    txt,
                    Justify::Left,
                    MENU_X+120, menu_get_y(10),
                    COL_TEXT2);

                if (p->get_location().in_flight()) {
                    int tgt = p->get_location().get_target();
                    FlyTarget *ft = exostate.loc2tgt(tgt);
                    bool star = (ft != exostate.get_galaxy()->get_guild());
                    snprintf(txt, sizeof(txt),
                             "The fleet is flying to the %s%s.",
                             (star?"star ":""),
                             ft->name);
                    menu_set_txt(12, COL_TEXT, txt);
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
        case MM_Relations:
            {
                char rel[32];

                menu_set_txt(0, COL_TEXT2, "Relationship to other lords");

                int line_idx = 1;

                for (int other_idx = 0; other_idx < N_PLAYERS; ++other_idx) {
                    if (other_idx == p_idx)
                        continue;

                    Player *other = exostate.get_player(other_idx);

                    RGB lordcol = COL_TEXT;
                    RGB relcol = COL_TEXT;

                    snprintf(rel, sizeof(rel), "None");

                    if (other->is_hostile_to(p_idx)) {
                        snprintf(rel, sizeof(rel), "Hostile");
                        relcol = COL_TEXT_BAD;
                    } else {
                        const char* sep = "";

                        int relidx = 0;
                        if (exostate.has_alliance(p_idx, other_idx, ALLY_Trade)) {
                            relidx += snprintf(rel+relidx, sizeof(rel)-relidx, "%sTrade", sep);
                            relcol = COL_TEXT2;
                            sep = " / ";
                        }
                        if (exostate.has_alliance(p_idx, other_idx, ALLY_NonAttack)) {
                            relidx += snprintf(rel+relidx, sizeof(rel)-relidx, "%sNo Att", sep);
                            relcol = COL_TEXT2;
                            sep = " / ";
                        }
                        if (exostate.has_alliance(p_idx, other_idx, ALLY_War)) {
                            relidx += snprintf(rel+relidx, sizeof(rel)-relidx, "%sWar Ally", sep);
                            relcol = COL_TEXT2;
                            sep = " / ";
                        }
                    }

                    if (!other->is_participating()) {
                        lordcol = COL_TEXT_GREYED;
                        relcol = COL_TEXT_GREYED;
                    }

                    // TODO: Multiplayer relations here - may be 'friendly' as per 'verhalten'

                    menu_set_txt(line_idx, lordcol, other->get_full_name());
                    draw_manager.draw_text(rel, Justify::Left, MENU_X+200, menu_get_y(line_idx), relcol);

                    line_idx++;
                }
            }
            break;
        case MM_ShipStatus:
            break;
        case MM_ShipDamage:
            break;
        case MM_Stats:
            break;
    }
}

// Return true if there was a user interaction and we need to redraw
bool MenuDrawer::menu_specific_update() {
    Player *p = exostate.get_active_player();
    int player_idx = exostate.get_player_idx(p);

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
            // 5: Equip Starship
            if (menu_row_clicked(5)) {
                menu_action = MA_EquipShip;
            }
            // 6: Build Artificial Planet
            if (menu_row_clicked(6)) {
                Planet *planet = exostate.get_planet_under_construction(player_idx);
                if (planet) {
                    // TODO: Dialogue here
                    L.debug("Advancing artificial planet");
                    if (p->attempt_spend(COST_ART)) {
                        if (!planet->advance_construction_phase()) {
                            L.fatal("get_planet_under_construction() returned non-advanceable planet");
                        }
                        // TODO: On completion, the planet should actually appear the following month,
                        // and this should trigger a bulletin (+ news article?)
                    } else {
                        L.error("Should not have offered unaffordable option");
                    }
                    menu_action = MA_Close;
                } else {
                    // TODO: Check if player can build world (viable star, been visited, not too many etc)
                    // TODO: Dialogue here
                    L.debug("Constructing artificial planet");
                    if (p->attempt_spend(COST_ART)) {
                        menu_action = MA_BuildArtificialWorld;
                    } else {
                        L.error("Should not have offered unaffordable option");
                        menu_action = MA_Close;
                    }
                }
            }
            // 8: Wait One Month
            // 10: Show Distances
            // 11: Save Game
            if (menu_row_clicked(11)) {
                // Re-open save menu to refresh metadata
                menu_open(MM_Save);
                return true;
            }
            // 12: Quit Game
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
                    char pct[5];
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
                        {MENU_X+MENU_BORDER+2, MENU_Y+MENU_H-MENU_BORDER-28, 35, 26},
                        COL_BORDERS);
                    draw_manager.fill(
                        {MENU_X+MENU_W-MENU_BORDER-37, MENU_Y+MENU_H-MENU_BORDER-28, 35, 26},
                        COL_BORDERS);
                }

                SpriteClick clk = draw_manager.query_click(id_menu_newoff_opt);
                if (clk.id) {
                    if (clk.x < .33f) {
                        if (p->attempt_spend(engagement_cost)) {
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
                    if (p->attempt_spend(OFF_PAY_MC)) {
                        p->adjust_reputation(1);
                        menu_open(MM_OldOfficerPaid);
                        return true;
                    }
                }

                if (menu_row_clicked(3)) {
                    p->adjust_reputation(-11);
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
        case MM_SecretService:
            // 4: Information Services (20+ MC)
            if (menu_row_clicked(4)) {
                menu_open(MM_SecInfo);
                return true;
            }

            // 5: A Personal File
            if (menu_row_clicked(5)) {
                if (p->attempt_spend(COST_FILE)) {
                    menu_open_player_select(MM_SecPersonalFile);
                    return true;
                }
            }

            // 6: Terrorist Attacks
            if (menu_row_clicked(6)) {
                // TODO: Check if mission already set
                menu_open(MM_SecAttack);
                return true;
            }

            // 7: Orbital Bomb Attacks
            if (menu_row_clicked(7)) {
                // TODO: Invention check
                // TODO: Check if mission already set
                menu_open(MM_SecBombing);
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
                if (p->attempt_spend(COST_INF_MC)) {
                    menu_open_player_select(MM_SecInfoMC);
                    return true;
                }
            }

            // 4: Allies
            if (menu_row_clicked(4)) {
                if (p->attempt_spend(COST_INF_ALLIES)) {
                    menu_open_player_select(MM_SecInfoAllies);
                    return true;
                }
            }

            // 5: Number of planets
            if (menu_row_clicked(5)) {
                if (p->attempt_spend(COST_INF_NPLANS)) {
                    menu_open_player_select(MM_SecInfoPlanets);
                    return true;
                }
            }

            // 6: Inventions
            if (menu_row_clicked(6)) {
                if (p->attempt_spend(COST_INF_INV)) {
                    menu_open_player_select(MM_SecInfoInventions);
                    return true;
                }
            }

            // 7: Planet surface
            if (menu_row_clicked(7)) {
                if (p->attempt_spend(COST_INF_SURF)) {
                    // TODO
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
                int &star_idx = p->get_mission_star_ref();
                int &planet_idx = p->get_mission_planet_ref();

                //  4: command station
                if (menu_row_clicked(4)) {
                    // TODO: Charge MC only after selection of valid planet
                    if (p->attempt_spend(COST_ATT_CMD)) {
                        p->set_mission_type(MT_TerrorComm);
                        ephstate.select_planet(star_idx, planet_idx);
                        return false;
                    }
                }

                // TODO
                //  5: cultivated area
                //  6: plutonium production
                //  7: army production
                //  8: spaceport
                //  9: trading centre
                // 10: mining

                // 14: Exit Menu
                if (menu_row_clicked(14)) {
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
                    // TODO: Invetion check?
                    // TODO: Charge MC only after selection of valid planet
                    if (p->attempt_spend(COST_ATT_BOMB)) {
                        p->set_mission_type(MT_RandomBomb);
                        ephstate.select_planet(star_idx, planet_idx);
                        return false;
                    }
                }

                //  5: nuclear extermination
                if (menu_row_clicked(5)) {
                    // TODO: Invetion check?
                    // TODO: Charge MC only after selection of valid planet
                    if (p->attempt_spend(COST_ATT_NUKE)) {
                        p->set_mission_type(MT_Nuclear);
                        ephstate.select_planet(star_idx, planet_idx);
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
        case MM_StarMarker:
            break;
        case MM_EquipShip:
            break;
        case MM_ArtificialWorld:
            break;
        case MM_Save:
            for (int i = 0; i < MAX_SLOTS; ++i) {
                if (menu_row_clicked(i+2)) {
                    save_manager.save(i);
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
            // 5: List Planets
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
            // 10: Starship Damage
            // 12: Statistics
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
            break;
        case MM_Inventions:
            for (int i = 0; i < INV_MAX; ++i) {
                if (draw_manager.query_click(id_menu_lines[i+1]).id) {
                    // TODO: Open specific invention
                    return false;
                }
            }
            if (draw_manager.clicked()) {
                menu_open(MM_Stat);
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
            break;
        case MM_ShipDamage:
            break;
        case MM_Stats:
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
    int player_idx = exostate.get_active_player_idx();
    menu_set_txt(0, COL_TEXT2, "Please select the desired lord.");
    int row = 1;
    for (int i = 0; i < N_PLAYERS; ++i) {
        if (i == player_idx) continue;
        Player *tgt = exostate.get_player(i);
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
            Player *pl = exostate.get_player(menu_line_players[row]);
            if (pl->is_participating()) {
                menu_selected_player = pl;
                return true;
            }
        }
    }
    return false;
}
