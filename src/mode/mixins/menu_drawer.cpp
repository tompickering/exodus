#include "menu_drawer.h"

#include "state/exodus_state.h"
#include "save/save.h"
#include "input/input.h"
#include "util/iter.h"

#include "shared.h"
#include "assetpaths.h"

#define OFF_PAY_MC 50

extern ExodusState exostate;
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

static const int ART_COST = 1000;

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

    for (int i = 0; i < MENU_LINES; ++i) {
        strncpy(menu_text[i], "", 1);
        draw_manager.unset_selectable(id_menu_lines[i]);
    }

    if (!_menu_is_open) {
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

void MenuDrawer::menu_update(float delta) {
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
    draw_manager.set_selectable(id_menu_lines[idx]);
}

void MenuDrawer::menu_open_specific_mode() {
    Player *p = exostate.get_active_player();
    bool art_ok = true;
    first_update = true;

    if (!p->can_afford(ART_COST)) art_ok = false;
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
            if (art_ok) {
                menu_set_opt(6, "Build Artificial Planet (%dMC)", ART_COST);
            } else {
                menu_set_txt(6, COL_TEXT_GREYED, "Build Artificial Planet (%dMC)", ART_COST);
            }
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
                if (p->can_afford(OFF_PAY_MC)) {
                    menu_set_opt(2, "Pay %dMC for their work", OFF_PAY_MC);
                } else {
                    menu_set_txt(2, COL_TEXT_GREYED, "Pay %dMC for their work", OFF_PAY_MC);
                }
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
            menu_set_opt(5, "A Personal File (100 MC)");
            menu_set_opt(6, "Terrorist Attacks (50+ MC)");
            menu_set_opt(7, "Orbital Bomb Attacks (500+ MC)");
            menu_set_opt(14, "Exit Menu");
            break;
        case MM_SecInfo:
            {
                menu_set_txt(0, COL_TEXT2, "You surely wish to see...");
                // FIXME Get rid of all the horrible hard-coded costs and string duplication
                if (p->can_afford(20)) {
                    menu_set_opt(3, "... a lord's amount of money (20MC)");
                    menu_set_opt(4, "... a lord's allies (20MC)");
                    menu_set_opt(5, "... a lord's number of planets (20MC)");
                } else {
                    menu_set_txt(3, COL_TEXT_GREYED, "... a lord's amount of money (20MC)");
                    menu_set_txt(4, COL_TEXT_GREYED, "... a lord's allies (20MC)");
                    menu_set_txt(5, COL_TEXT_GREYED, "... a lord's number of planets (20MC)");
                }

                if (p->can_afford(50)) {
                    menu_set_opt(6, "... a lord's inventions (50MC)");
                } else {
                    menu_set_txt(6, COL_TEXT_GREYED, "... a lord's inventions (50MC)");
                }

                if (p->can_afford(100)) {
                    menu_set_opt(7, "... a planet's surface (100MC)");
                } else {
                    menu_set_txt(7, COL_TEXT_GREYED, "... a planet's surface (100MC)");
                }

                menu_set_opt(14, "Exit Menu");
            }
            break;
        case MM_PlayerChoice:
            menu_print_other_players();
            break;
        case MM_SecInfoMC:
            // TODO: Print information
            menu_set_txt(0, COL_TEXT2, "INFO MC");
            break;
        case MM_SecInfoAllies:
            // TODO: Print information
            menu_set_txt(0, COL_TEXT2, "INFO ALLIES");
            break;
        case MM_SecInfoPlanets:
            // TODO: Print information
            menu_set_txt(0, COL_TEXT2, "INFO PLANETS");
            break;
        case MM_SecInfoInventions:
            // TODO: Print information
            menu_set_txt(0, COL_TEXT2, "INFO INVENTIONS");
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
            break;
        case MM_FleetInfo:
            break;
        case MM_News:
            break;
        case MM_Planets:
            break;
        case MM_Inventions:
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
            if (draw_manager.query_click(id_menu_lines[2]).id) {
                menu_open(MM_OfficersAndTaxes);
                return true;
            }
            // 3: Secret Service
            if (draw_manager.query_click(id_menu_lines[3]).id) {
                menu_open(MM_SecretService);
                return true;
            }
            // 4: Set / Replace Star Markers
            // 5: Equip Starship
            if (draw_manager.query_click(id_menu_lines[5]).id) {
                menu_action = MA_EquipShip;
            }
            // 6: Build Artificial Planet
            if (draw_manager.query_click(id_menu_lines[6]).id) {
                Planet *planet = exostate.get_planet_under_construction(player_idx);
                if (planet) {
                    // TODO: Dialogue here
                    L.debug("Advancing artificial planet");
                    if (p->attempt_spend(ART_COST)) {
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
                    if (p->attempt_spend(ART_COST)) {
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
            if (draw_manager.query_click(id_menu_lines[11]).id) {
                // Re-open save menu to refresh metadata
                menu_open(MM_Save);
                return true;
            }
            // 12: Quit Game
            // 14: Exit Menu
            if (draw_manager.query_click(id_menu_lines[14]).id) {
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

                if (draw_manager.query_click(id_menu_lines[14]).id) {
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
                if (draw_manager.query_click(id_menu_lines[2]).id) {
                    if (p->attempt_spend(OFF_PAY_MC)) {
                        p->adjust_reputation(1);
                        menu_open(MM_OldOfficerPaid);
                        return true;
                    }
                }

                if (draw_manager.query_click(id_menu_lines[3]).id) {
                    p->adjust_reputation(-11);
                    menu_open(MM_OldOfficerDismissed);
                    return true;
                }

                if (draw_manager.query_click(id_menu_lines[4]).id) {
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
            if (draw_manager.query_click(id_menu_lines[4]).id) {
                menu_open(MM_SecInfo);
                return true;
            }

            // 5: A Personal File (100 MC)
            // 6: Terrorist Attacks (50+ MC)
            // 7: Orbital Bomb Attacks (500+ MC)
            // 14: Exit Menu
            if (draw_manager.query_click(id_menu_lines[14]).id) {
                menu_open(MM_Ctrl);
                return true;
            }
            break;
        case MM_SecInfo:
            // FIXME: Get rid of hard-coded costs

            // 3: Amount of money
            if (draw_manager.query_click(id_menu_lines[3]).id) {
                if (p->attempt_spend(20)) {
                    menu_open_player_select(MM_SecInfoMC);
                    return true;
                }
            }

            // 4: Allies (20MC)
            if (draw_manager.query_click(id_menu_lines[4]).id) {
                if (p->attempt_spend(20)) {
                    menu_open_player_select(MM_SecInfoAllies);
                    return true;
                }
            }

            // 5: Number of planets (20MC)
            if (draw_manager.query_click(id_menu_lines[5]).id) {
                if (p->attempt_spend(20)) {
                    menu_open_player_select(MM_SecInfoPlanets);
                    return true;
                }
            }

            // 6: Inventions (50MC)
            if (draw_manager.query_click(id_menu_lines[6]).id) {
                if (p->attempt_spend(50)) {
                    menu_open_player_select(MM_SecInfoInventions);
                    return true;
                }
            }

            // 7: Planet surface (100MC)
            if (draw_manager.query_click(id_menu_lines[7]).id) {
                if (p->attempt_spend(100)) {
                    // TODO
                    return true;
                }
            }

            // 14: Exit Menu
            if (draw_manager.query_click(id_menu_lines[14]).id) {
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
            if (draw_manager.clicked()) {
                menu_open(MM_SecretService);
                return true;
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
                if (draw_manager.query_click(id_menu_lines[i+2]).id) {
                    save_manager.save(i);
                    menu_open(MM_Save);
                    return true;
                }
            }
            if (draw_manager.query_click(id_menu_lines[14]).id) {
                menu_open(MM_Ctrl);
                return true;
            }
            break;
        case MM_Stat:
            // 2: General Information
            // 3: Fleet Information
            // 4: Recall Latest News
            // 5: List Planets
            // 6: List Inventions
            // 7: Relationship to Lords
            // 9: Starship Status
            // 10: Starship Damage
            // 12: Statistics
            // 14: Exit Menu
            if (draw_manager.query_click(id_menu_lines[14]).id) {
                menu_action = MA_Close;
            }
            break;
        case MM_GenInfo:
            break;
        case MM_FleetInfo:
            break;
        case MM_News:
            break;
        case MM_Planets:
            break;
        case MM_Inventions:
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
        if (tgt->is_participating()) {
            menu_set_opt(row, tgt->get_full_name());
        } else {
            menu_set_txt(row, COL_TEXT_GREYED, tgt->get_full_name());
        }
        row++;
    }
}

bool MenuDrawer::menu_player_selected() {
    // Skip the first row as this is text
    for (int row = 1; row < MENU_LINES; ++row) {
        if (draw_manager.query_click(id_menu_lines[row]).id) {
            menu_selected_player = menu_line_players[row];
            return true;
        }
    }
    return false;
}
