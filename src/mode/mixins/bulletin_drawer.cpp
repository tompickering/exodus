#include "bulletin_drawer.h"

#include "assetpaths.h"
#include "shared.h"

#include "input/input.h"

#define BULLETIN_TRANSITION_RATE 1.4f

const int BULLETIN_BORDER = 6;
const int BULLETIN_W = 436 + (BULLETIN_BORDER) * 2;
const int BULLETIN_H = 306 + (BULLETIN_BORDER) * 2;
const int BULLETIN_X = (RES_X / 2) - (BULLETIN_W / 2);
const int BULLETIN_Y = (RES_Y / 2) - (BULLETIN_H / 2) - 12;
const int BULLETIN_TEXT_X = BULLETIN_X + BULLETIN_BORDER * 2 + 4;
const int BULLETIN_FLAG_BG_X = (RES_X / 2) - 48 - BULLETIN_BORDER;
const int BULLETIN_FLAG_BG_Y = BULLETIN_Y - 2 - 56 - BULLETIN_BORDER*2;
const int BULLETIN_FLAG_BG_W = 96 + BULLETIN_BORDER*2;
const int BULLETIN_FLAG_BG_H = 56 + BULLETIN_BORDER*2;
const int BULLETIN_BG_X = BULLETIN_X + BULLETIN_BORDER;
const int BULLETIN_BG_Y = BULLETIN_Y + BULLETIN_BORDER;

extern INPUTMANAGER input_manager;

BulletinDrawer::BulletinDrawer() {
    for (int i = 0; i < BULLETIN_LINES; ++i) {
        strncpy(bulletin_text[i], "", 1);
    }
    bulletin_mode = BM_Default;
    bulletin_reset_text_cols();
    bulletin_has_been_acknowledged = false;
    bulletin_text_idx = 0;
    bulletin_transition = 0;
    bulletin_redraws_needed = true;
    bulletin_is_yesno = false;
    bulletin_yesno_was_yes = false;
    bulletin_praction = BPR_None;
    bulletin_report = nullptr;
    bulletin_report_summary_page = 0;
    _bulletin_is_open = false;

    bulletin_bg_preserve = nullptr;
}

bool BulletinDrawer::bulletin_start_new(bool transition) {
    return bulletin_start_new(transition, -1);
}

bool BulletinDrawer::bulletin_start_new(bool transition, Star* star) {
    int star_idx = -1;

    if (star) {
        star_idx = exostate().get_star_idx(star);
    }

    return bulletin_start_new(transition, star_idx);
}

bool BulletinDrawer::bulletin_start_new(bool transition, int star_idx) {
    return bulletin_start_new_internal(transition, star_idx, BM_Default);
}

bool BulletinDrawer::bulletin_start_new(bool transition, BulletinMode mode) {
    return bulletin_start_new_internal(transition, -1, mode);
}

bool BulletinDrawer::bulletin_start_manual(BulletinManualPage page) {
    bulletin_manual_page_opened = page;
    return bulletin_continue_manual(page);
}

bool BulletinDrawer::bulletin_continue_manual(BulletinManualPage page) {
    L.debug("MANUAL PAGE %d", (int)page);
    bulletin_manual_page_current = page;
    return bulletin_start_new_internal(false, -1, BM_Manual);
}

bool BulletinDrawer::bulletin_start_new_internal(bool transition, int star_idx, BulletinMode mode) {
    for (int i = 0; i < bulletin_text_idx; ++i) {
        draw_manager.draw(id_bulletin_text[i], nullptr);
    }

    if (star_idx >= 0) {
        if (!exostate().any_human_has_visited(star_idx)) {
            return false;
        }
    }

    bulletin_reset();

    bulletin_mode = mode;

    if (bulletin_mode == BM_Report || bulletin_mode == BM_ReportSummary) {
        bulletin_praction = BPR_None;
    }

    if (!bulletin_is_open())
        bulletin_open();

    bulletin_transition = transition ? 0 : 1;

    bulletin_set_bg(nullptr);

    if (!transition) {
        bulletin_bg_preserve = nullptr;
    }

    if (bulletin_mode == BM_Manual) {
        bulletin_set_flag(bulletin_get_manual_flag());
    } else {
        bulletin_set_active_player_flag();
    }

    return true;
}

void BulletinDrawer::bulletin_update(float dt) {
    bulletin_transition += dt * BULLETIN_TRANSITION_RATE;
    if (bulletin_transition >= 1) {
        bulletin_transition = 1;
        draw_manager.draw(
            id_bulletin_bg_scan,
            nullptr);
    }

    if (bulletin_redraws_needed && !bulletin_has_been_acknowledged) {
        bulletin_update_bg();
        bulletin_draw_events();
        if (bulletin_mode == BM_ReportSummary) {
            bulletin_draw_report_summary();
        }

        if (bulletin_mode == BM_Manual) {
            bulletin_update_manual_page(true);
        }

        bulletin_draw_text();
    }

    bulletin_redraws_needed = bulletin_transition < 1;

    if (bulletin_is_yesno) {
        SpriteClick clk = draw_manager.query_click(id_bulletin_yesno);
        if (clk.id) {
            if (clk.x < 0.5) {
                L.debug("YESNO: YES");
                bulletin_yesno_was_yes = true;
            } else {
                L.debug("YESNO: NO");
            }
            bulletin_has_been_acknowledged = true;
        }
    } else if (bulletin_mode == BM_Report || bulletin_mode == BM_ReportSummary) {
        if (draw_manager.query_click(id_bulletin_zoom).id) {
            bulletin_praction = BPR_Zoom;
        }

        SpriteClick clk = draw_manager.query_click(id_bulletin_prbuttons);
        if (clk.id) {
            if (clk.x <= 0.33) {
                bulletin_praction = BPR_Back;
            } else if (clk.x < 0.67) {
                bulletin_praction = BPR_Forward;
            } else {
                bulletin_praction = BPR_Close;
                bulletin_has_been_acknowledged = true;
            }
        }

        if (bulletin_mode == BM_ReportSummary) {
            for (int i = 0; i < BULLETIN_REPORT_SUMMARY_LINES; ++i) {
                if (draw_manager.query_click(id_bulletin_text[i]).id) {
                    bulletin_report_specific = (bulletin_report_summary_page * BULLETIN_REPORT_SUMMARY_LINES) + i;
                    bulletin_praction = BPR_OpenSpecificReport;
                    break;
                }
            }
        }
    } else if (bulletin_mode == BM_Manual) {
        if (draw_manager.query_click(id_bulletin_contents).id) {
            // Start rather than Continue as we're changing section
            bulletin_start_manual(BMP_Contents);
        }

        SpriteClick clk = draw_manager.query_click(id_bulletin_prbuttons);
        if (clk.id) {
            if (clk.x <= 0.33) {
                if (bulletin_manual_page_current > bulletin_manual_page_opened) {
                    bulletin_continue_manual(BulletinManualPage((int)bulletin_manual_page_current - 1));
                }
            } else if (clk.x < 0.67) {
                if ((bulletin_manual_page_current+1) < bulletin_get_end_page()) {
                    bulletin_continue_manual(BulletinManualPage((int)bulletin_manual_page_current + 1));
                }
            } else {
                bulletin_praction = BPR_Close;
                bulletin_has_been_acknowledged = true;
            }
        }

        bulletin_update_manual_page(false);
    } else if (bulletin_is_war_ally) {
        bulletin_war_ally_update();
    } else if (draw_manager.clicked()) {
        if (bulletin_transition < 1) {
            bulletin_transition = 1;
        } else {
            bulletin_has_been_acknowledged = true;
        }
    }
}

static const char* get_event_icon(PlanetReportEvent event) {
    switch (event) {
        case PRE_Meteor:
            return IMG_PREVENT1;
        case PRE_Defects:
            return IMG_PREVENT2;
        case PRE_Climate:
            return IMG_PREVENT3;
        case PRE_Plague:
            return IMG_PREVENT4;
        case PRE_Aliens:
            return IMG_PREVENT5;
        case PRE_NewSpecies:
            return IMG_PREVENT6;
        case PRE_Epidemic:
            return IMG_PREVENT7;
        case PRE_Rebels:
            return IMG_PREVENT8;
        case PRE_Meltdown:
            return IMG_PREVENT9;
        case PRE_SurfChangeCultivation:
            return IMG_PREVENT10;
        case PRE_SurfChangeClearing:
            return IMG_PREVENT11;
        case PRE_HQDestroyed:
            return IMG_PREVENT12;
        case PRE_ArmyProductionSaved:
            return IMG_PREVENT13;
        case PRE_ArmyProductionStopped:
            return IMG_PREVENT14;
        default:
            L.error("No icon for event %d", (int)event);
    }

    return nullptr;
}

static const char* get_light_spr(PlanetTrafficLight light) {
    if (light == PTL_Red) return IMG_SU1_CTA;
    if (light == PTL_Amber) return IMG_SU1_CTC;
    return IMG_SU1_CTB;
}

void BulletinDrawer::bulletin_draw_events() {
    if (!bulletin_report) {
        return;
    }

    const PlanetReport& rpt = *bulletin_report;

    if (!rpt.finalised) {
        L.error("The report should have been finalised by the time we render it");
    }

    int event_count = 0;

    // Warning icons

    const char* icon_spr = nullptr;

    switch (bulletin_report->get_level()) {
        case PREL_Crit:
            icon_spr = IMG_EXCL_CRIT;
            break;
        case PREL_Warn:
            icon_spr = IMG_EXCL_WARN;
            break;
        case PREL_Good:
            icon_spr = IMG_EXCL_OK;
            break;
        default:
            break;
    }

    if (icon_spr != nullptr) {
        draw_manager.draw(
            icon_spr,
            {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 4,
             BULLETIN_Y + BULLETIN_BORDER + 4 + (28 * event_count),
             1.0f, 0.0f, 1, 1});

        ++event_count;
    }

    // Events

    for (int event_idx = 0; event_idx < (int)PRE_MAX; ++event_idx) {
        PlanetReportEvent event = (PlanetReportEvent)event_idx;

        if (rpt.has_event(event)) {
            const char* icon = get_event_icon(event);

            draw_manager.draw(
                icon,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 4,
                 BULLETIN_Y + BULLETIN_BORDER + 4 + (28 * event_count),
                 1.0f, 0.0f, 1, 1});

            ++event_count;
        }
    }

    // Gauges

    const int st_x = BULLETIN_TEXT_X;
    const int st_y = BULLETIN_Y + BULLETIN_H - BULLETIN_BORDER*2 - 2;

    draw_manager.draw(
        IMG_PL_ST,
        {st_x, st_y,
         0, 1, 1, 1});

    draw_manager.draw(
        get_light_spr(rpt.light_food),
        {st_x+22, st_y,
         0, 1, 1, 1});

    draw_manager.draw(
        get_light_spr(rpt.light_plu),
        {st_x+60, st_y,
         0, 1, 1, 1});

    draw_manager.draw(
        get_light_spr(rpt.light_unrest),
        {st_x+98, st_y,
         0, 1, 1, 1});

    draw_manager.draw(
        get_light_spr(rpt.light_army),
        {st_x+136, st_y,
         0, 1, 1, 1});
}

void BulletinDrawer::bulletin_draw_report_summary() {
    const int first = bulletin_report_summary_page * BULLETIN_REPORT_SUMMARY_LINES;
    const int last = (bulletin_report_summary_page + 1) * BULLETIN_REPORT_SUMMARY_LINES;

    int drawn = 0;

    for (int r_idx = first; r_idx < last; ++r_idx) {
        if (r_idx >= exostate().planet_report_count()) {
            break;
        }

        const PlanetReport& rpt = exostate().get_planet_report(r_idx);

        Planet *p = exostate().get_planet(rpt.star_idx, rpt.planet_idx);
        Star *s = exostate().get_star(rpt.star_idx);

        int y = bulletin_text_y(drawn, 51) - 1;

        const char* icon_spr = nullptr;

        switch (rpt.get_level()) {
            case PREL_Crit:
                icon_spr = IMG_EXCL_CRIT;
                break;
            case PREL_Warn:
                icon_spr = IMG_EXCL_WARN;
                break;
            case PREL_Good:
                icon_spr = IMG_EXCL_OK;
                break;
            default:
                break;
        }

        if (icon_spr) {
            draw_manager.draw(
                icon_spr,
                {BULLETIN_TEXT_X + 8, y + 11,
                 0.5, 0.5, 1, 1});
        }

        float planet_scale = 1.0;

#if FEATURE_MULTISIZE_PLANETS_PANEL
        if (p->get_size() == PLANET_Small) planet_scale = 0.6;
        if (p->get_size() == PLANET_Medium) planet_scale = 0.8;
#endif

        draw_manager.draw(
            p->sprites()->panel_icon,
            {BULLETIN_TEXT_X + 32, y + 11,
             0.5, 0.5, planet_scale, planet_scale});

        char line[BULLETIN_MAX_TEXT];

        snprintf(line, BULLETIN_MAX_TEXT, "%s, %s", p->get_name(), s->name);

        draw_manager.draw_text(
            id_bulletin_text[drawn],
            line,
            Justify::Left,
            BULLETIN_TEXT_X + 48,
            y,
            COL_TEXT);

        snprintf(line, BULLETIN_MAX_TEXT, "Ar: +%d; +%dMC", rpt.prod_inf + rpt.prod_gli + rpt.prod_art, rpt.trade_mc);

        draw_manager.draw_text(
            line,
            Justify::Right,
            BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 4,
            y,
            COL_TEXT);

        const int st_x = BULLETIN_TEXT_X + 48;
        const int st_y = y + 24;

        draw_manager.draw(
            IMG_PL_ST,
            {st_x, st_y,
             0, 0, 1, 1});

        draw_manager.draw(
            get_light_spr(rpt.light_food),
            {st_x+22, st_y,
             0, 0, 1, 1});

        draw_manager.draw(
            get_light_spr(rpt.light_plu),
            {st_x+60, st_y,
             0, 0, 1, 1});

        draw_manager.draw(
            get_light_spr(rpt.light_unrest),
            {st_x+98, st_y,
             0, 0, 1, 1});

        draw_manager.draw(
            get_light_spr(rpt.light_army),
            {st_x+136, st_y,
             0, 0, 1, 1});

        const int max_events = 8;

        int event_total = 0;

        for (int event_idx = 0; event_idx < (int)PRE_MAX; ++event_idx) {
            PlanetReportEvent event = (PlanetReportEvent)event_idx;

            if (rpt.has_event(event)) {
                ++event_total;
            }
        }

        int event_count = 0;

        for (int event_idx = 0; event_idx < (int)PRE_MAX; ++event_idx) {
            PlanetReportEvent event = (PlanetReportEvent)event_idx;

            if (rpt.has_event(event)) {
                const char* icon = get_event_icon(event);

                draw_manager.draw(
                    icon,
                    {st_x + 150 + (28 * event_count),
                     st_y-2,
                     0, 0, 1, 1});

                ++event_count;

                if (event_total > max_events && event_count == (max_events-1)) {
                    draw_manager.draw_text(
                        Font::Large,
                        "...",
                        Justify::Left,
                        st_x + 154 + (28 * event_count),
                        st_y - 14,
                        COL_TEXT);
                    break;
                }
            }
        }

        ++drawn;
    }
}

void BulletinDrawer::bulletin_draw_text() {
    for (int i = 0; i < bulletin_text_idx; ++i) {
        if (bulletin_transition < 1) {
            draw_manager.draw(
                id_bulletin_text[i],
                nullptr);
        } else {
            draw_manager.draw_text(
                id_bulletin_text[i],
                bulletin_text[i],
                Justify::Left,
                BULLETIN_TEXT_X,
                bulletin_text_y(i),
                bulletin_text_col[i]);
        }
    }

    if (bulletin_transition >= 1) {
        if (bulletin_is_yesno) {
            draw_manager.draw(
                id_bulletin_yesno,
                IMG_BR14_EXPORT,
                {RES_X / 2,
                 BULLETIN_Y + BULLETIN_H - BULLETIN_BORDER - 2,
                 0.5f, 1, 1, 1});
        }
        if (bulletin_is_war_ally) {
            bulletin_war_ally_init();
        }
    }
}

void BulletinDrawer::bulletin_war_ally_init() {
    for (int i = 0; i < BULLETIN_WAR_ALLY_IDS; ++i) {
        bulletin_war_ally_ids_army[i] = draw_manager.new_sprite_id();
    }
    bulletin_war_ally_id_exit = draw_manager.new_sprite_id();

    input_manager.enable_repeating_clicks(true);

    draw_manager.draw_text(
        "Planet",
        Justify::Centre,
        BULLETIN_X + 170,
        bulletin_text_y(6) + 6,
        COL_TEXT);
    draw_manager.draw_text(
        "Send",
        Justify::Centre,
        BULLETIN_X + 276,
        bulletin_text_y(6) + 6,
        COL_TEXT);

    for (int i = 0; i < 3; ++i) {
        const char* img = IMG_TD2_TR4;
        if (i == 1) {
            img =  IMG_TD2_TR5;
        } else if (i == 2) {
            img =  IMG_TD2_TR6;
        }

        draw_manager.draw(
            bulletin_war_ally_ids_army[i*5],
            img,
            {BULLETIN_X + 20,
             BULLETIN_Y + 164 + i*44,
             0, 0.5f, 1, 1});

        draw_manager.draw(
            bulletin_war_ally_ids_army[i*5 + 3],
            IMG_BR4_EXPORT3,
            {BULLETIN_X + BULLETIN_W - 20,
             BULLETIN_Y + 164 + i*44,
             1, 0.5f, 1, 1});
    }

    draw_manager.draw(
        bulletin_war_ally_id_exit,
        IMG_BR5_EXPORT4,
        {RES_X / 2,
         BULLETIN_Y + BULLETIN_H - BULLETIN_BORDER - 2,
         0.5f, 1, 1, 1});
}

void BulletinDrawer::bulletin_war_ally_update() {
    if (draw_manager.query_click(bulletin_war_ally_id_exit).id) {
        for (int i = 0; i < BULLETIN_WAR_ALLY_IDS; ++i) {
            draw_manager.draw(bulletin_war_ally_ids_army[i], nullptr);
            draw_manager.release_sprite_id(bulletin_war_ally_ids_army[i]);
        }
        draw_manager.draw(bulletin_war_ally_id_exit, nullptr);
        draw_manager.release_sprite_id(bulletin_war_ally_id_exit);
        input_manager.enable_repeating_clicks(false);
        bulletin_has_been_acknowledged = true;
        return;
    }

    int inf, gli, art;
    bulletin_war_ally_planet->get_army(inf, gli, art);

    char n[8];
    for (int i = 0; i < 3; ++i) {
        int stock = inf;
        int *to_send = &bulletin_war_ally_inf;
        if (i == 1) {
            stock = gli;
            to_send = &bulletin_war_ally_gli;
        } else if (i == 2) {
            stock = art;
            to_send = &bulletin_war_ally_art;
        }

        snprintf(n, sizeof(n), "%d", stock);
        draw_manager.draw_text(
            bulletin_war_ally_ids_army[i*5 + 1],
            n,
            Justify::Centre,
            BULLETIN_X + 170,
            BULLETIN_Y + 152 + i*44,
            COL_TEXT);

        snprintf(n, sizeof(n), "%d", *to_send);
        draw_manager.draw_text(
            bulletin_war_ally_ids_army[i*5 + 2],
            n,
            Justify::Centre,
            BULLETIN_X + 276,
            BULLETIN_Y + 152 + i*44,
            COL_TEXT);

        SpriteClick clk = draw_manager.query_click(bulletin_war_ally_ids_army[i*5 + 3]);

        if (clk.id) {
            if (clk.x < 0.5) {
                if (*to_send > 0) {
                    (*to_send)--;
                    switch (i) {
                        case 0:
                            bulletin_war_ally_planet->adjust_army(1, 0, 0);
                            break;
                        case 1:
                            bulletin_war_ally_planet->adjust_army(0, 1, 0);
                            break;
                        case 2:
                            bulletin_war_ally_planet->adjust_army(0, 0, 1);
                            break;
                    }
                }
            } else {
                if (stock > 0) {
                    switch (i) {
                        case 0:
                            bulletin_war_ally_planet->adjust_army(-1, 0, 0);
                            break;
                        case 1:
                            bulletin_war_ally_planet->adjust_army(0, -1, 0);
                            break;
                        case 2:
                            bulletin_war_ally_planet->adjust_army(0, 0, -1);
                            break;
                    }
                    (*to_send)++;
                }
            }
        }

        if (stock <= 0) {
            draw_manager.draw(
                bulletin_war_ally_ids_army[i*5 + 4],
                IMG_TD2_TR0,
                {BULLETIN_X + 20,
                 BULLETIN_Y + 164 + i*44,
                 0, 0.5f, 1, 1});
        } else {
            draw_manager.draw(bulletin_war_ally_ids_army[i*5 + 4], nullptr);
        }
    }
}

void BulletinDrawer::bulletin_set_next_text(const char* in_text, ...) {
    if (bulletin_text_idx >= BULLETIN_LINES) {
        L.error("Tried to set invalid bulletin text index %d to %s", bulletin_text_idx, in_text);
        return;
    }

    va_list args;
    va_start(args, in_text);
    vsnprintf(bulletin_text[bulletin_text_idx++], BULLETIN_MAX_TEXT - 1, in_text, args);
    va_end(args);
}

void BulletinDrawer::bulletin_set_text_col(RGB col) {
    bulletin_text_col[bulletin_text_idx] = col;
}

void BulletinDrawer::bulletin_write_planet_info(Star* s, Planet* p) {
    if (!s || !p) {
        L.fatal("Attempt to write data for null star / planet");
    }

    Player *owner = nullptr;
    if (p->is_owned()) owner = exostate().get_player(p->get_owner());

    RGB col = COL_TEXT_BAD;
    if (owner && owner->is_human())
        col = COL_TEXT2;

    bulletin_set_text_col(col);
    bulletin_set_next_text("Planet %s, System %s", p->get_name(), s->name);
    bulletin_set_next_text("");
    bulletin_set_text_col(col);
    bulletin_set_next_text("%s", owner ? owner->get_full_name() : "");
    bulletin_set_next_text("");
}

void BulletinDrawer::bulletin_open() {
    id_bulletin_header_flag = draw_manager.new_sprite_id();
    id_bulletin_header_l = draw_manager.new_sprite_id();
    id_bulletin_header_r = draw_manager.new_sprite_id();
    id_bulletin_panel = draw_manager.new_sprite_id();
    id_bulletin_black = draw_manager.new_sprite_id();
    id_bulletin_bg = draw_manager.new_sprite_id();
    id_bulletin_bg_preserve = draw_manager.new_sprite_id();
    id_bulletin_bg_scan = draw_manager.new_sprite_id();
    id_bulletin_yesno = draw_manager.new_sprite_id();
    id_bulletin_prbuttons = draw_manager.new_sprite_id();
    id_bulletin_zoom = draw_manager.new_sprite_id();
    id_bulletin_contents = draw_manager.new_sprite_id();

    bulletin_bg_preserve = nullptr;

    for (int i = 0; i < BULLETIN_LINES; ++i) {
        id_bulletin_text[i] = draw_manager.new_sprite_id();
    }

    draw_manager.fill(
        id_bulletin_panel,
        {BULLETIN_X, BULLETIN_Y,
         BULLETIN_W, BULLETIN_H},
         COL_BORDERS);

    bulletin_set_bg(nullptr);

    draw_manager.fill(
        id_bulletin_black,
        {BULLETIN_X + BULLETIN_BORDER,
         BULLETIN_Y + BULLETIN_BORDER,
         436, 306},
         {0, 0, 0});

    // Draw header flag and background
    draw_manager.fill(
        id_bulletin_header_flag,
        {BULLETIN_FLAG_BG_X, BULLETIN_FLAG_BG_Y,
         BULLETIN_FLAG_BG_W, BULLETIN_FLAG_BG_H},
         COL_BORDERS);

    if (bulletin_mode == BM_Report) {
        draw_manager.draw(
            id_bulletin_zoom,
            IMG_BTNZOOM,
            {BULLETIN_FLAG_BG_X - 2,
             BULLETIN_Y - 2,
             1, 1, 1, 1});
        draw_manager.draw(
            id_bulletin_prbuttons,
            IMG_PRBUTTONS,
            {BULLETIN_FLAG_BG_X + BULLETIN_FLAG_BG_W + 2,
             BULLETIN_Y - 2,
             0, 1, 1, 1});
    } else if (bulletin_mode == BM_ReportSummary) {
        draw_manager.draw(
            id_bulletin_prbuttons,
            IMG_PRBUTTONS,
            {BULLETIN_FLAG_BG_X + BULLETIN_FLAG_BG_W + 2,
             BULLETIN_Y - 2,
             0, 1, 1, 1});
    } else if (bulletin_mode == BM_Manual) {
        draw_manager.draw(
            id_bulletin_contents,
            IMG_BTN_TOP,
            {BULLETIN_FLAG_BG_X - 2,
             BULLETIN_Y - 2,
             1, 1, 1, 1});
        draw_manager.draw(
            id_bulletin_prbuttons,
            IMG_PRBUTTONS,
            {BULLETIN_FLAG_BG_X + BULLETIN_FLAG_BG_W + 2,
             BULLETIN_Y - 2,
             0, 1, 1, 1});
    } else {
        draw_manager.draw(
            id_bulletin_header_l,
            IMG_TS1_HEADER,
            {BULLETIN_FLAG_BG_X - 2,
             BULLETIN_Y - 2,
             1, 1, 1, 1});

        draw_manager.draw(
            id_bulletin_header_r,
            IMG_TS1_HEADER,
            {BULLETIN_FLAG_BG_X + BULLETIN_FLAG_BG_W + 2,
             BULLETIN_Y - 2,
             0, 1, 1, 1});
    }

    _bulletin_is_open = true;
}

void BulletinDrawer::bulletin_close() {
    if (!bulletin_is_open()) {
        L.fatal("Attempt to close bulletin whilst not open");
    }

    for (int i = 0; i < BULLETIN_LINES; ++i) {
        draw_manager.release_sprite_id(id_bulletin_text[i]);
    }

    draw_manager.draw(id_bulletin_panel, nullptr);
    draw_manager.draw(id_bulletin_black, nullptr);
    draw_manager.draw(id_bulletin_header_flag, nullptr);
    draw_manager.draw(id_bulletin_header_l, nullptr);
    draw_manager.draw(id_bulletin_header_r, nullptr);
    draw_manager.draw(id_bulletin_bg, nullptr);
    draw_manager.draw(id_bulletin_bg_preserve, nullptr);
    draw_manager.draw(id_bulletin_bg_scan, nullptr);
    draw_manager.draw(id_bulletin_yesno, nullptr);
    draw_manager.draw(id_bulletin_prbuttons, nullptr);
    draw_manager.draw(id_bulletin_zoom, nullptr);
    draw_manager.draw(id_bulletin_contents, nullptr);

    draw_manager.release_sprite_id(id_bulletin_header_flag);
    draw_manager.release_sprite_id(id_bulletin_header_l);
    draw_manager.release_sprite_id(id_bulletin_header_r);
    draw_manager.release_sprite_id(id_bulletin_panel);
    draw_manager.release_sprite_id(id_bulletin_black);
    draw_manager.release_sprite_id(id_bulletin_bg);
    draw_manager.release_sprite_id(id_bulletin_bg_preserve);
    draw_manager.release_sprite_id(id_bulletin_bg_scan);
    draw_manager.release_sprite_id(id_bulletin_yesno);
    draw_manager.release_sprite_id(id_bulletin_prbuttons);
    draw_manager.release_sprite_id(id_bulletin_zoom);
    draw_manager.release_sprite_id(id_bulletin_contents);


    // Wipe all info
    bulletin_reset();

    _bulletin_is_open = false;
}

void BulletinDrawer::bulletin_reset() {
    for (int i = 0; i < BULLETIN_LINES; ++i) {
        strncpy(bulletin_text[i], "", 1);
    }

    bulletin_mode = BM_Default;
    bulletin_reset_text_cols();
    bulletin_transition = 0;
    bulletin_redraws_needed = true;
    bulletin_has_been_acknowledged = false;
    bulletin_text_idx = 0;
    bulletin_is_yesno = false;
    bulletin_yesno_was_yes = false;
    bulletin_praction = BPR_None;
    bulletin_report = nullptr;
    bulletin_report_summary_page = 0;

    bulletin_is_war_ally = false;

    // N.B. Don't reset war ally vars here as we need to retrieve them later
}

void BulletinDrawer::bulletin_ensure_closed() {
    if (bulletin_is_open()) bulletin_close();
}

bool BulletinDrawer::bulletin_is_open() {
    return _bulletin_is_open;
}

int BulletinDrawer::bulletin_text_y(int idx) {
    return bulletin_text_y(idx, 18);
}

int BulletinDrawer::bulletin_text_y(int idx, int gap) {
    return BULLETIN_Y + BULLETIN_BORDER + 4 + idx * gap;
}

void BulletinDrawer::bulletin_reset_text_cols() {
    for (int i = 0; i < BULLETIN_LINES; ++i) {
        bulletin_text_col[i] = COL_TEXT;
    }
}

bool BulletinDrawer::bulletin_acknowledged() {
    return bulletin_has_been_acknowledged;
}

bool BulletinDrawer::bulletin_was_yesno_yes() {
    return bulletin_yesno_was_yes;
}

void BulletinDrawer::bulletin_set_bg(const char* img) {
    // N.B. Can set nullptr for black
    bulletin_bg = img;
}

void BulletinDrawer::bulletin_set_yesno() {
    bulletin_is_yesno = true;
    bulletin_yesno_was_yes = false;
}

BulletinPRAction BulletinDrawer::bulletin_get_praction() {
    return bulletin_praction;
}

void BulletinDrawer::bulletin_set_report(const PlanetReport *report) {
    bulletin_report = report;
}

void BulletinDrawer::bulletin_set_report_summary_page(int page) {
    bulletin_report_summary_page = page;
}

void BulletinDrawer::bulletin_set_war_ally(Planet* p, int mc) {
    bulletin_is_war_ally = true;
    bulletin_war_ally_planet = p;
    bulletin_war_ally_mc = mc;
    bulletin_war_ally_inf = 0;
    bulletin_war_ally_gli = 0;
    bulletin_war_ally_art = 0;
}

int BulletinDrawer::bulletin_get_war_ally_result(int& i, int& g, int& a) {
    i = bulletin_war_ally_inf;
    g = bulletin_war_ally_gli;
    a = bulletin_war_ally_art;
    return bulletin_war_ally_mc;
}

void BulletinDrawer::bulletin_update_bg() {
    int h = 306;
    if (bulletin_transition < 1) {
        h = (int)(306.f * bulletin_transition);
    } else {
        bulletin_bg_preserve = bulletin_bg;
    }

    if (bulletin_bg) {
        if (bulletin_bg_preserve && bulletin_transition < 1) {
            /* FIXME
             * Without removing bulletin_bg, there is a flicker
             * to black before each next bulletin starts. This
             * is because when bulletin_bg is first drawn for
             * the next bulletin, its source region is small
             * but the draw repairs the larger area that it
             * occupied on the previous bulletin - where the
             * preserved bg has now been drawn - and as it's
             * 'below' the preserved background, this doesn't
             * get restored.
             *
             * We're in an awkward situation whereby the
             * background and preseved background are alternately
             * to be displayed on top of one another. The proper
             * solution would likely be to have repairs also restore
             * sprites above their previous location - however
             * we work around the issue here by removing the
             * bulletin background - thereby getting the repair
             * out of the way - before drawing the preserved
             * background.
             */
            draw_manager.draw(id_bulletin_bg, nullptr);
            draw_manager.draw(
                id_bulletin_bg_preserve,
                bulletin_bg_preserve,
                {BULLETIN_BG_X,
                 BULLETIN_BG_Y,
                 0, 0, 1, 1});
        }
        DrawArea a = {0, 0, 436, h};
        draw_manager.set_source_region(
            id_bulletin_bg,
            &a);
        draw_manager.draw(
            id_bulletin_bg,
            bulletin_bg,
            {BULLETIN_BG_X,
             BULLETIN_BG_Y,
             0, 0, 1, 1});
    } else {
        // nullptr -> black background
        draw_manager.fill(
            id_bulletin_black,
            {BULLETIN_BG_X,
             BULLETIN_BG_Y,
             436, h},
             {0, 0, 0});
    }

    // Ensure we don't draw lower then the bg area
    if (bulletin_transition < 1 && h < 434) {
        draw_manager.fill(
            id_bulletin_bg_scan,
            {BULLETIN_BG_X, BULLETIN_BG_Y + h, 436, 2},
             {0, 0, 0xFF});
    } else {
        draw_manager.draw(
            id_bulletin_bg_scan,
            nullptr);
    }
}

void BulletinDrawer::bulletin_set_flag(const char* img) {
    if (!bulletin_is_open()) {
        return;
    }

    draw_manager.draw(
        img,
        {BULLETIN_FLAG_BG_X + BULLETIN_BORDER,
         BULLETIN_FLAG_BG_Y + BULLETIN_BORDER,
         0, 0, 1, 1});
}

void BulletinDrawer::bulletin_set_player_flag(Player* player) {
    bulletin_set_flag(flags[player->get_flag_idx()]);
}

void BulletinDrawer::bulletin_set_active_player_flag() {
    bulletin_set_player_flag(exostate().get_active_player());
}

BulletinManualPage BulletinDrawer::bulletin_get_end_page() {
    switch (bulletin_manual_page_opened) {
        case BMP_Contents:
            return BMP_Contents_END;
        case BMP_FirstSteps:
            return BMP_FirstSteps_END;
        case BMP_GalaxyMap:
            return BMP_GalaxyMap_END;
        case BMP_Ctrl:
            return BMP_Ctrl_END;
        case BMP_StarMap:
            return BMP_StarMap_END;
        case BMP_PlanetMap:
            return BMP_PlanetMap_END;
        case BMP_Comm:
            return BMP_Comm_END;
        case BMP_LunarBattle:
            return BMP_LunarBattle_END;
        case BMP_Trade:
            return BMP_Trade_END;
        case BMP_Alliances:
            return BMP_Alliances_END;
        case BMP_Fleet:
            return BMP_Fleet_END;
        case BMP_Science:
            return BMP_Science_END;
        case BMP_SpaceCombat:
            return BMP_SpaceCombat_END;
        case BMP_EndingTheGame:
            return BMP_EndingTheGame_END;
        case BMP_Tips:
            return BMP_Tips_END;
        case BMP_FAQ:
            return BMP_FAQ_END;
        default:
            break;
    }

    return (BulletinManualPage)((int)bulletin_manual_page_opened + 1);
}

const char* BulletinDrawer::bulletin_get_manual_flag() {
    switch (bulletin_manual_page_current) {
        case BMP_FirstSteps:
            return IMG_FLAG_START;
        case BMP_GalaxyMap:
        case BMP_GalaxyMap_2:
        case BMP_GalaxyMap_3:
        case BMP_FirstSteps_7:
            return IMG_FLAG_GALMAP;
        case BMP_Ctrl:
        case BMP_Ctrl_2:
        case BMP_Ctrl_3:
            return IMG_FLAG_CTRL;
        case BMP_StarMap:
        case BMP_StarMap_2:
        case BMP_FirstSteps_2:
            return IMG_FLAG_SOLMAP;
        case BMP_PlanetMap:
        case BMP_FirstSteps_3:
        case BMP_FirstSteps_4:
            return IMG_FLAG_SURF;
        case BMP_FirstSteps_5:
            return IMG_FLAG_STARPORT;
        case BMP_PlanetMap_2:
        case BMP_PlanetMap_3:
        case BMP_FirstSteps_6:
            return IMG_FLAG_LIGHTS;
        case BMP_Comm:
        case BMP_Comm_2:
        case BMP_Comm_3:
        case BMP_Comm_4:
            return IMG_FLAG_COMM;
        case BMP_LunarBattle:
        case BMP_LunarBattle_2:
            return IMG_FLAG_BTLPREP;
        case BMP_LunarBattle_3:
        case BMP_LunarBattle_4:
            return IMG_FLAG_BTL;
        case BMP_Trade:
            return IMG_FLAG_TRADE;
        case BMP_Alliances:
        case BMP_Alliances_2:
            return IMG_FLAG_ALLIANCES;
        case BMP_Fleet:
        case BMP_Fleet_2:
            return IMG_FLAG_FLEET;
        case BMP_Science:
            return IMG_FLAG_SCIENCE;
        case BMP_SpaceCombat:
        case BMP_SpaceCombat_2:
            return IMG_FLAG_SHIP;
        case BMP_SpaceCombat_3:
            return IMG_FLAG_SPACEBTL;
        case BMP_EndingTheGame:
            return IMG_FLAG_GUILDHQ;
        default:
            break;
    }

    return IMG_FLAG_MAN;
}

#define LINK(N,P) if (draw_manager.query_click(id_bulletin_text[N]).id) { bulletin_start_manual(P); }

/*
 * Use the same function for draw and update so that the logic of each
 * for each page can be placed together.
 */
void BulletinDrawer::bulletin_update_manual_page(bool draw) {
    BulletinManualPage p = bulletin_manual_page_current;

    if (p == BMP_Contents) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("CONTENTS");
            bulletin_set_next_text("First Steps");
            bulletin_set_next_text("The Galaxy Map");
            bulletin_set_next_text("The CTRL Menu");
            bulletin_set_next_text("The Star Map");
            bulletin_set_next_text("COMM: Settling, Negotiating, Attacking");
            bulletin_set_next_text("The Planet Map");
            bulletin_set_next_text("Lunar Battle");
            bulletin_set_next_text("Trade with Foreign Planets");
            bulletin_set_next_text("Alliances");
            bulletin_set_next_text("Fleet Operations");
            bulletin_set_next_text("Science");
            bulletin_set_next_text("Your Starship and Space Combat");
            bulletin_set_next_text("Ending The Game");
            bulletin_set_next_text("Tips");
            bulletin_set_next_text("FAQs");
        } else {
            LINK(1, BMP_FirstSteps)
            LINK(2, BMP_GalaxyMap)
            LINK(3, BMP_Ctrl)
            LINK(4, BMP_StarMap)
            LINK(5, BMP_Comm)
            LINK(6, BMP_PlanetMap)
            LINK(7, BMP_LunarBattle)
            LINK(8, BMP_Trade)
            LINK(9, BMP_Alliances)
            LINK(10, BMP_Fleet)
            LINK(11, BMP_Science)
            LINK(12, BMP_SpaceCombat)
            LINK(13, BMP_EndingTheGame)
            LINK(14, BMP_Tips)
            LINK(15, BMP_FAQ)
        }
    }

    if (p == BMP_FirstSteps) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("First Steps");
            bulletin_set_next_text("");
            bulletin_set_next_text("Your fleet has just arrived at the centre of a");
            bulletin_set_next_text("new galaxy. What should you do next?");
            bulletin_set_next_text("");
            bulletin_set_next_text("You are stationed at the Space Guild. It may");
            bulletin_set_next_text("be worth exploring it to see what you can learn");
            bulletin_set_next_text("about this unfamiliar world. Perhaps you should");
            bulletin_set_next_text("take a closer look? Select it and click ZOOM.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Our mission demands that we travel the galaxy,");
            bulletin_set_next_text("and there is no time like the present! Select the");
            bulletin_set_next_text("star which piques your interest, and click FLY.");
            bulletin_set_next_text("");
            bulletin_set_next_text("From the CTRL menu, select 'Wait One Month'");
            bulletin_set_next_text("(or press Space) until you arrive...");
        }
    }

    if (p == BMP_FirstSteps_2) {
        if (draw) {
            bulletin_set_next_text("Once you arrive at your destination, select the");
            bulletin_set_next_text("star and click ZOOM to inspect the system.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Planets controlled by rival powers will have a");
            bulletin_set_next_text("name shown in red. Unclaimed planets will");
            bulletin_set_next_text("display no name.");
            bulletin_set_next_text("");
            bulletin_set_next_text("If a rival world is present, you may select their");
            bulletin_set_next_text("planet and use COMM to hail them. However, it's");
            bulletin_set_next_text("unwise to make a new enemy at this time.");
            bulletin_set_next_text("");
            bulletin_set_next_text("If an unclaimed planet is present, consider");
            bulletin_set_next_text("claiming it - click it and select COMM to.");
            bulletin_set_next_text("begin the process.");
            bulletin_set_next_text("");
            bulletin_set_next_text("If there are none, it's likely best to move on...");
        }
    }

    if (p == BMP_FirstSteps_3) {
        if (draw) {
            bulletin_set_next_text("After you claim a planet, use MAP to inspect");
            bulletin_set_next_text("the planet's surface.");
            bulletin_set_next_text("");
            bulletin_set_next_text("You must immediately build a Command Base.");
            bulletin_set_next_text("If a planet ever loses its last Base, you will");
            bulletin_set_next_text("lose control of the planet. Select Command");
            bulletin_set_next_text("Base and click on a clear space to build it.");
            bulletin_set_next_text("");
            bulletin_set_next_text("The Command Base needs to consume one unit");
            bulletin_set_next_text("of food per month. You will also need to");
            bulletin_set_next_text("build one Agricultural facility to provide");
            bulletin_set_next_text("it. A starved Command Base will collapse!");
            bulletin_set_next_text("");
            bulletin_set_next_text("Once you have constructed these two units,");
            bulletin_set_next_text("you must decide how to develop the planet.");
            bulletin_set_next_text("Use the '?' to learn about each construction.");
        }
    }

    if (p == BMP_FirstSteps_4) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("TRADE");
            bulletin_set_next_text("If the planet is to produce trading commodities,");
            bulletin_set_next_text("then it will need a Starport to transfer them to");
            bulletin_set_next_text("your fleet, or a Trading Centre to sell locally.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("MILITARY");
            bulletin_set_next_text("If the planet is to produce battle units, it");
            bulletin_set_next_text("will require production facilities, plutonium");
            bulletin_set_next_text("to power them and army funding. Trasfer of");
            bulletin_set_next_text("units to your fleet also requires a Starport");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("WEALTH");
            bulletin_set_next_text("Citites produce MC.");
            bulletin_set_next_text("");
            bulletin_set_next_text("An entirely defenseless planet invites invasion.");
            bulletin_set_next_text("A Lunar Base is an excellent deterrent.");
        }
    }

    if (p == BMP_FirstSteps_5) {
        if (draw) {
            bulletin_set_next_text("A Starport consists of 3 units; a Control, a");
            bulletin_set_next_text("Landing and a Power Plant.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Starports are required to transfer goods");
            bulletin_set_next_text("and combat units between your fleet and");
            bulletin_set_next_text("the planet. This is done via the COMM option");
            bulletin_set_next_text("in the Star Map.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Starports also support the production of ships");
            bulletin_set_next_text("for your fleet. Some ships may be used for");
            bulletin_set_next_text("military operations on rival worlds; some");
            bulletin_set_next_text("will strengthen your fleet, and some will allow");
            bulletin_set_next_text("you to transport more battle units or goods.");
            bulletin_set_next_text("");
            bulletin_set_next_text("AirDef guns ward off bombing attacks.");
            bulletin_set_next_text("Robots keep an angry population in check.");
        }
    }

    if (p == BMP_FirstSteps_6) {
        if (draw) {
            bulletin_set_next_text("A series of indicators will tell you whether");
            bulletin_set_next_text("the planet has sufficient food and plutonium");
            bulletin_set_next_text("for its operations, along with the contentment");
            bulletin_set_next_text("of the populace. Click these for more detail.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Excess food and plutonium is stored on the");
            bulletin_set_next_text("planet - but note that food is perishable.");
            bulletin_set_next_text("");
            bulletin_set_next_text("The Army Funding control shows you how many");
            bulletin_set_next_text("MC of the total generated by the planet each");
            bulletin_set_next_text("month are being diverted to the military.");
            bulletin_set_next_text("");
            bulletin_set_next_text("This is required for unit production. The");
            bulletin_set_next_text("figure is red if it is insufficient to operate");
            bulletin_set_next_text("all facilities - but any overpayment is lost!");
        }
    }

    if (p == BMP_FirstSteps_7) {
        if (draw) {
            bulletin_set_next_text("");
            bulletin_set_next_text("");
            bulletin_set_next_text("");
            bulletin_set_next_text("");
            bulletin_set_next_text("Your next move is up to you.");
            bulletin_set_next_text("");
            bulletin_set_next_text("");
            bulletin_set_next_text("What sort of ruler will you choose to be?");
            bulletin_set_next_text("");
            bulletin_set_next_text("");
            bulletin_set_next_text("Good Luck!");
        }
    }

    if (p == BMP_GalaxyMap) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("The Galaxy Map");
            bulletin_set_next_text("");
            bulletin_set_next_text("(For advice on your First Steps, click here)");
            bulletin_set_next_text("");
            bulletin_set_next_text("The map shows all the stars your fleet can");
            bulletin_set_next_text("travel to.");
            bulletin_set_next_text("");
            bulletin_set_next_text("At the centre is the Space Guild.");
            bulletin_set_next_text("This is where you begin your journey.");
            bulletin_set_next_text("");
            bulletin_set_next_text("This icon shows the location");
            bulletin_set_next_text("of your fleet.");
            bulletin_set_next_text("");
            bulletin_set_next_text("When it is in flight, this icon");
            bulletin_set_next_text("shows its destination.");

            draw_manager.draw(
                IMG_TS1_WORM,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 48,
                 bulletin_text_y(8),
                 0.5f, 0.5f, 1, 1});

            draw_manager.draw(
                IMG_TS1_ICON1,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 48,
                 bulletin_text_y(11),
                 0.5f, 0.5f, 1, 1});

            draw_manager.draw(
                IMG_TS1_DEST,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 48,
                 bulletin_text_y(14),
                 0.5f, 0.5f, 1, 1});
        } else {
            LINK(2, BMP_FirstSteps)
        }
    }

    if (p == BMP_GalaxyMap_2) {
        if (draw) {
            bulletin_set_next_text("");
            bulletin_set_next_text("Click on a Star (or Guild) to select it.");
            bulletin_set_next_text("");
            bulletin_set_next_text("You can label (visited) stars with 'M'.");
            bulletin_set_next_text("");
            bulletin_set_next_text("The lower-right panel shows a '?'");
            bulletin_set_next_text("if you have not visited this system.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Otherwise, it will display a system's.");
            bulletin_set_next_text("planets. Planets owned by you are");
            bulletin_set_next_text("marked yellow. Planets owned by a");
            bulletin_set_next_text("rival are marked red.");
            bulletin_set_next_text("");
            bulletin_set_next_text("A fleet icon shows that an empire's");
            bulletin_set_next_text("fleet is present.");

            draw_manager.draw(
                IMG_TS1_MK1,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 48,
                 bulletin_text_y(1) - 4,
                 0.5f, 0.f, 1, 1});

            draw_manager.draw(
                IMG_TS1_QMARK,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 48,
                 bulletin_text_y(5)+4,
                 0.5f, 0.f, 1, 1});

            draw_manager.draw(
                IMG_MAN_PLANPANEL,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER-2,
                 bulletin_text_y(10),
                 1.f, 0.5f, 0.5, 0.5});

            draw_manager.draw(
                IMG_TS1_SHICON,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 56,
                 bulletin_text_y(13)+4,
                 0.5f, 0.f, 1, 1});
        }
    }

    if (p == BMP_GalaxyMap_3) {
        if (draw) {
            bulletin_set_next_text("You have the following options:");
            bulletin_set_next_text("");
            bulletin_set_next_text("");
            bulletin_set_next_text("Commence flight to the selected star.");
            bulletin_set_next_text("");
            bulletin_set_next_text("");
            bulletin_set_next_text("Command functions; save/load options.");
            bulletin_set_next_text("Click here to learn more about CTRL.");
            bulletin_set_next_text("");
            bulletin_set_next_text("");
            bulletin_set_next_text("Various status information options.");
            bulletin_set_next_text("");
            bulletin_set_next_text("");
            bulletin_set_next_text("Take a closer look at the selected star");
            bulletin_set_next_text("(if visited) or Guild.");

            draw_manager.draw(
                IMG_MAN_BTN_FLY,
                {BULLETIN_TEXT_X,
                 bulletin_text_y(2),
                 0.f, 0.f, 1, 1});

            draw_manager.draw(
                IMG_MAN_BTN_CTRL,
                {BULLETIN_TEXT_X,
                 bulletin_text_y(5),
                 0.f, 0.f, 1, 1});

            draw_manager.draw(
                IMG_MAN_BTN_STAT,
                {BULLETIN_TEXT_X,
                 bulletin_text_y(9),
                 0.f, 0.f, 1, 1});

            draw_manager.draw(
                IMG_MAN_BTN_ZOOM,
                {BULLETIN_TEXT_X,
                 bulletin_text_y(12),
                 0.f, 0.f, 1, 1});
        } else {
            LINK(7, BMP_Ctrl)
        }
    }

    if (p == BMP_Ctrl) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("The CTRL Menu");
            bulletin_set_next_text("");
            bulletin_set_next_text("Several game features are accessed via CTRL,");
            bulletin_set_next_text("including 'Wait One Month' which is a critical");
            bulletin_set_next_text("aspect of gameplay.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Change Officers & Taxes");
            bulletin_set_next_text("Change the amount of your income which is");
            bulletin_set_next_text("invested in science, or hire new officers.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Science Officer: Impacts speed of research");
            bulletin_set_next_text("Fleet Admiral: Affects accuracy in space combat");
            bulletin_set_next_text("Battle General: Affects accuracy in battle");
            bulletin_set_next_text("Secret Service Leader: Affects mission success");
            bulletin_set_next_text("Ship Counsellor: Offers trade/defence insights");
        }
    }

    if (p == BMP_Ctrl_2) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Secret Service");
            bulletin_set_next_text("Request information services, plan sabotage");
            bulletin_set_next_text("operations or - if you have the technology -");
            bulletin_set_next_text("order orbital bomb attacks.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Set / Replace Star Markers");
            bulletin_set_next_text("Place helpful markers on stars");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Equip Starship");
            bulletin_set_next_text("Purchase ship upgrades and enlist crew.");
            bulletin_set_next_text("Click here for more details.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Build Artificial Planet");
            bulletin_set_next_text("If you have developed the technology, you can");
            bulletin_set_next_text("construct an entire planet. Three phases are");
            bulletin_set_next_text("required, each costing 1000MC.");
        } else {
            LINK(10, BMP_SpaceCombat)
        }
    }

    if (p == BMP_Ctrl_3) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Wait One Month");
            bulletin_set_next_text("Allow one month to pass. This ends your 'turn'.");
            bulletin_set_next_text("Any CPU powers all carry out actions, and your");
            bulletin_set_next_text("planets generate their monthly production.");
            bulletin_set_next_text("Flight from one star to another always requires");
            bulletin_set_next_text("at least one month to pass.");
            bulletin_set_next_text("");
            bulletin_set_next_text("You can press Space to allow a month to pass.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Show Distances");
            bulletin_set_next_text("Display a visual reference showing how long");
            bulletin_set_next_text("travel to other stars will take.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Save Game and Quit Game");
            bulletin_set_next_text("Save your progress, or exit to the menu.");
        }
    }

    if (p == BMP_StarMap) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("The Star Map");
            bulletin_set_next_text("");
            bulletin_set_next_text("This map shows the habitable planets of a star.");
            bulletin_set_next_text("Named planets usually belong to empires.");
            bulletin_set_next_text("Unnamed planets may be claimed via COMM.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Click on a planet to select it.");
            bulletin_set_next_text("Your interactions with the planet will be");
            bulletin_set_next_text("different depending on whether:");
            bulletin_set_next_text("");
            bulletin_set_next_text("* It is owned by you");
            bulletin_set_next_text("* It is owned by a rival");
            bulletin_set_next_text("* It is not owned at all");
        }
    }

    if (p == BMP_StarMap_2) {
        if (draw) {
            bulletin_set_next_text("");
            bulletin_set_next_text("You have the following options:");
            bulletin_set_next_text("");
            bulletin_set_next_text("                  (If the planet is not owned by you)");
            bulletin_set_next_text("Start a Scout or Bombing mission.");
            bulletin_set_next_text("");
            bulletin_set_next_text("                  (If the planet is owned by you)");
            bulletin_set_next_text("Show the surface map for building and");
            bulletin_set_next_text("management.");
            bulletin_set_next_text("");
            bulletin_set_next_text("                  Show information about the planet.");
            bulletin_set_next_text("If the planet is owned by a rival, having more");
            bulletin_set_next_text("Scouts may reveal more detailed information...");
            bulletin_set_next_text("");
            bulletin_set_next_text("                  Click here to learn more about COMM.");

            draw_manager.draw(
                IMG_MAN_BTN_FLEET,
                {BULLETIN_TEXT_X,
                 bulletin_text_y(3) + 2,
                 0.f, 0.f, 1, 1});

            draw_manager.draw(
                IMG_MAN_BTN_MAP,
                {BULLETIN_TEXT_X,
                 bulletin_text_y(6) + 2,
                 0.f, 0.f, 1, 1});

            draw_manager.draw(
                IMG_MAN_BTN_INFO,
                {BULLETIN_TEXT_X,
                 bulletin_text_y(10) + 2,
                 0.f, 0.f, 1, 1});

            draw_manager.draw(
                IMG_MAN_BTN_COMM,
                {BULLETIN_TEXT_X,
                 bulletin_text_y(14) + 2,
                 0.f, 0.f, 1, 1});
        } else {
            LINK(14, BMP_Comm);
        }
    }

    if (p == BMP_Comm) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("The COMM Button");
            bulletin_set_next_text("");
            bulletin_set_next_text("COMM behaves differently depending on");
            bulletin_set_next_text("whethr the selected planet is:");
            bulletin_set_next_text("");
            bulletin_set_next_text("* Unclaimed");
            bulletin_set_next_text("* Owned by you");
            bulletin_set_next_text("* Owned by a rival power");
        } else {
            LINK(5, BMP_Comm_2);
            LINK(6, BMP_Comm_3);
            LINK(7, BMP_Comm_4);
        }
    }

    if (p == BMP_Comm_2) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("The COMM Button: Unclaimed Planets");
            bulletin_set_next_text("");
            bulletin_set_next_text("Consider a planet for colonisation.");
            bulletin_set_next_text("");
            bulletin_set_next_text("You can see slightly more detailed");
            bulletin_set_next_text("information about the planet - along");
            bulletin_set_next_text("with cost of settlement - before deciding");
            bulletin_set_next_text("whether to go ahead and claim the world.");
        }
    }

    if (p == BMP_Comm_3) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("The COMM Button: Your Planets");
            bulletin_set_next_text("");
            bulletin_set_next_text("Contact your own planet.");
            bulletin_set_next_text("");
            bulletin_set_next_text("If you have a Starport, you can transfer");
            bulletin_set_next_text("goods between your fleet and the planet here,");
            bulletin_set_next_text("or commission new ships for your fleet.");
            bulletin_set_next_text("");
            bulletin_set_next_text("If you are sufficiently wealthy and have the");
            bulletin_set_next_text("requisite technology, you can also engineer");
            bulletin_set_next_text("the climate of the planet.");
        }
    }

    if (p == BMP_Comm_4) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("The COMM Button: Rival Planets");
            bulletin_set_next_text("");
            bulletin_set_next_text("Contact the planet owner.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Here, you can gain an audience with the");
            bulletin_set_next_text("planet's ruler. You can request trade,");
            bulletin_set_next_text("propose an alliance, declare an invasion,");
            bulletin_set_next_text("or perhaps apologise and grovel for past");
            bulletin_set_next_text("transgressions.");
        }
    }

    if (p == BMP_PlanetMap) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("The Surface Map");
            bulletin_set_next_text("");
            bulletin_set_next_text("Developing a planet will yield MC, minerals,");
            bulletin_set_next_text("battle units and more.");
            bulletin_set_next_text("");
            bulletin_set_next_text("The right panel shows available constructions.");
            bulletin_set_next_text("Click on a building to select it and click on the");
            bulletin_set_next_text("map to place it. Note that every planet needs a");
            bulletin_set_next_text("working Command Base (plus 1 food per month).");
            bulletin_set_next_text("");
            bulletin_set_next_text("Use the '?' button on the right to learn more");
            bulletin_set_next_text("about each building's function.");
        }
    }

    if (p == BMP_PlanetMap_2) {
        if (draw) {
            bulletin_set_next_text("Red lights indicate insufficient resources or");
            bulletin_set_next_text("an angered population. Click them for details.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Food is produced by Agriculture facilities, and");
            bulletin_set_next_text("plutonium is produced by reactors.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Surplus food, minerals and plutonium is stored");
            bulletin_set_next_text("on the planet until capacity is reached. Note");
            bulletin_set_next_text("that food stored in reserve is perishable! Check");
            bulletin_set_next_text("the INFO button on the Star Map for details.");
            bulletin_set_next_text("");
            bulletin_set_next_text("A planet always requires 1 food per month.");
            bulletin_set_next_text("Cities require 3 food per month in order to");
            bulletin_set_next_text("survive, and might occasionally expand.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Agriculture is sensitive to climate.");
        }
    }

    if (p == BMP_PlanetMap_3) {
        if (draw) {
            bulletin_set_next_text("Battle units require 1 plutonium per factory,");
            bulletin_set_next_text("and a sufficient allocation of funds from the");
            bulletin_set_next_text("planet's monthly product.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Infantry production requires 1MC");
            bulletin_set_next_text("Glider production requires 2MC");
            bulletin_set_next_text("Aetillery production requires 3MC");
            bulletin_set_next_text("");
            bulletin_set_next_text("Beware - plutonium is dangerous!");
            bulletin_set_next_text("");
            bulletin_set_next_text("A Spaceport is required to transfer goods");
            bulletin_set_next_text("and battle units between your fleet and");
            bulletin_set_next_text("the planet surface.");
        }
    }

    if (p == BMP_LunarBattle) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Lunar Battle: Preparation");
            bulletin_set_next_text("");
            bulletin_set_next_text("When you invade a rival world - or are");
            bulletin_set_next_text("yourself invaded - a battle will commence.");
            bulletin_set_next_text("");
            bulletin_set_next_text("To protect civilians and infrastructure,");
            bulletin_set_next_text("battles are fought on the planet's moon.");
            bulletin_set_next_text("");
            bulletin_set_next_text("In either case, you have the choice. Allow");
            bulletin_set_next_text("your officer to handle the battle for you");
            bulletin_set_next_text("and await the news, or assume command");
            bulletin_set_next_text("yourself.");
        }
    }

    if (p == BMP_LunarBattle_2) {
        if (draw) {
            bulletin_set_next_text("On the field of battle, units are organised into");
            bulletin_set_next_text("squads of size 1 to 20. You may decide on the");
            bulletin_set_next_text("squad size to use.");
            bulletin_set_next_text("");
            bulletin_set_next_text("The more units in a squad, the more times it will");
            bulletin_set_next_text("fire on its turn.");
            bulletin_set_next_text("");
            bulletin_set_next_text("You may choose to have your officer deploy");
            bulletin_set_next_text("your units onto the battle field, or take");
            bulletin_set_next_text("charge of this yourself.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Click here for details about combat.");
        } else {
            LINK(11, BMP_LunarBattle);
        }
    }

    if (p == BMP_LunarBattle_3) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Lunar Battle: Units");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Infantry");
            bulletin_set_next_text("Low range and accuracy, but can");
            bulletin_set_next_text("use the surroundings as cover.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Gliders");
            bulletin_set_next_text("Can move further than infantry, but");
            bulletin_set_next_text("are susceptible to hidden mines.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Artillery");
            bulletin_set_next_text("Superior accuracy and range, but");
            bulletin_set_next_text("stationary and may only fire");
            bulletin_set_next_text("forwards.");

            draw_manager.draw(
                IMG_GF4_4,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 48,
                 bulletin_text_y(4),
                 0.5f, 0.5f, 1, 1});

            draw_manager.draw(
                IMG_GF4_5,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 48,
                 bulletin_text_y(8) + 8,
                 0.5f, 0.5f, 1, 1});

            draw_manager.draw(
                IMG_GF4_6,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 48,
                 bulletin_text_y(12),
                 0.5f, 0.5f, 1, 1});
        }
    }

    if (p == BMP_LunarBattle_4) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Lunar Battle: Combat");
            bulletin_set_next_text("");
            bulletin_set_next_text("Squads take turns to move.");
            bulletin_set_next_text("");
            bulletin_set_next_text("On a squad's turn, click the arrows or use");
            bulletin_set_next_text("the cursor keys to move, until the unit");
            bulletin_set_next_text("cannot move further. Finish the movement");
            bulletin_set_next_text("phase early by clicking on the unit itself,");
            bulletin_set_next_text("or by pressing Enter.");
            bulletin_set_next_text("");
            bulletin_set_next_text("If any enemy unit is in range, you will now");
            bulletin_set_next_text("be able to open fire by clicking on the target.");
            bulletin_set_next_text("The number of hits you score depends on the");
            bulletin_set_next_text("power of the unit, the enemy's cover and the");
            bulletin_set_next_text("skill of your general.");
        }
    }

    if (p == BMP_Trade) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Trade with Foreign Planets");
            bulletin_set_next_text("Note: All planetary trading is done with");
            bulletin_set_next_text("the private markets and doesn't affect");
            bulletin_set_next_text("the planet owner's stocks.");
            bulletin_set_next_text("");
            bulletin_set_next_text("In order to trade with a planet, you must");
            bulletin_set_next_text("be stationed in the same system.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Use COMM to trade with a foreign planet.");
        }
    }

    if (p == BMP_Alliances) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Alliances");
            bulletin_set_next_text("");
            bulletin_set_next_text("It can be advantageous to seek alliances with");
            bulletin_set_next_text("rival powers.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Trade Alliances");
            bulletin_set_next_text("You agree to allow free trade with each others'");
            bulletin_set_next_text("empires. You also agree to the sale of arms.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Non-Attack Alliances");
            bulletin_set_next_text("You agree not to invade one another's worlds.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("War Alliances");
            bulletin_set_next_text("You agree to support one another in the event");
            bulletin_set_next_text("of an invasion.");
        }
    }

    if (p == BMP_Alliances_2) {
        if (draw) {
            bulletin_set_next_text("If you want to convince a rival power to");
            bulletin_set_next_text("form an alliance with you, they need to");
            bulletin_set_next_text("have reason to take you seriously.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Remember that just as a power can form an");
            bulletin_set_next_text("alliance with you, so too can they hold a");
            bulletin_set_next_text("grudge.");
        }
    }


    if (p == BMP_Fleet) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Fleet Operations");
            bulletin_set_next_text("");
            bulletin_set_next_text("The FLEET button in the Star Map allows you");
            bulletin_set_next_text("to deploy ships to a planet's surface.");
            bulletin_set_next_text("");
            bulletin_set_next_text("In order to enact fleet operations against");
            bulletin_set_next_text("a planet, it must be stationed in that solar");
            bulletin_set_next_text("system.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Note that if the planet is owned by a rival");
            bulletin_set_next_text("power and has Air Defence guns, they are");
            bulletin_set_next_text("liable to destroy your ships.");
        }
    }

    if (p == BMP_Fleet_2) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Scout Flight");
            bulletin_set_next_text("Fly over the world and reveal its map.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Covered Scout Flight");
            bulletin_set_next_text("The same as a Scout Flight - but send your");
            bulletin_set_next_text("Bombers as escort to attack AirDef guns.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Bomber Attack");
            bulletin_set_next_text("Bombard specific planetary targets.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Ships and AirDef guns can be built by");
            bulletin_set_next_text("any planet which has a Starport.");
        }
    }

    if (p == BMP_Science) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Science");
        }
    }

    if (p == BMP_SpaceCombat) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Your Starship");
            bulletin_set_next_text("");
            bulletin_set_next_text("The STAT menu contains options which");
            bulletin_set_next_text("allow you to review the status of your");
            bulletin_set_next_text("ship.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Ship upgrades can be purchased via an");
            bulletin_set_next_text("option in the CTRL menu.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Shield Generator");
            bulletin_set_next_text("Provides a regenerative force");
            bulletin_set_next_text("field around the ship.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Laser Guns and Missile Launchers");
            bulletin_set_next_text("Allows your ship to fire more");
            bulletin_set_next_text("projectiles in Space Combat.");

            draw_manager.draw(
                IMG_EQ1_TR1,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 60,
                 bulletin_text_y(11),
                 0.5f, 0.5f, 1, 1});

            draw_manager.draw(
                IMG_EQ1_TR2,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 60 - 30,
                 bulletin_text_y(15),
                 0.5f, 0.5f, 1, 1});

            draw_manager.draw(
                IMG_EQ1_TR3,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 60 + 30,
                 bulletin_text_y(15),
                 0.5f, 0.5f, 1, 1});
        }
    }

    if (p == BMP_SpaceCombat_2) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Crew");
            bulletin_set_next_text("The more crew you have, the faster");
            bulletin_set_next_text("you can repair damage to your ship.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Bionic Probes");
            bulletin_set_next_text("These allow you to probe a planet");
            bulletin_set_next_text("before you commit to colonisation.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Escape Capsule");
            bulletin_set_next_text("Might just save you and your officers");
            bulletin_set_next_text("from a close encounter...");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Repair Hangar");
            bulletin_set_next_text("May allow you to salvage some");
            bulletin_set_next_text("damaged ships in the aftermath of");
            bulletin_set_next_text("battle.");

            draw_manager.draw(
                IMG_EQ1_TR4,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 40,
                 bulletin_text_y(2),
                 0.5f, 0.5f, 1, 1});

            draw_manager.draw(
                IMG_EQ1_TR5,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 40,
                 bulletin_text_y(6),
                 0.5f, 0.5f, 1, 1});

            draw_manager.draw(
                IMG_EQ1_TR6,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 40,
                 bulletin_text_y(10),
                 0.5f, 0.5f, 1, 1});

            draw_manager.draw(
                IMG_EQ1_TR7,
                {BULLETIN_X + BULLETIN_W - BULLETIN_BORDER - 40,
                 bulletin_text_y(14),
                 0.5f, 0.5f, 1, 1});

        }
    }

    if (p == BMP_SpaceCombat_3) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Space Combat");
            bulletin_set_next_text("This occurs in real time. If you win, you");
            bulletin_set_next_text("capture the enemy's transporters and scouts.");
            bulletin_set_next_text("Losing your starship        will end your game!");
            bulletin_set_next_text("");
            bulletin_set_next_text("Your ships are yellow; enemy ships are red.");
            bulletin_set_next_text("      - Scout ships");
            bulletin_set_next_text("      - Warships");
            bulletin_set_next_text("      - Transporters");
            bulletin_set_next_text("      - Bombers");
            bulletin_set_next_text("");
            bulletin_set_next_text("To command a ship to attack, select it with the");
            bulletin_set_next_text("left mouse button and assign a target with the");
            bulletin_set_next_text("right. Alternatively, switch to AUTO to have");
            bulletin_set_next_text("your Admiral take over. The distance from a");
            bulletin_set_next_text("ship to its target determines its accuracy.");

            draw_manager.draw(
                IMG_RD1_TYP1,
                {BULLETIN_TEXT_X + 193,
                 bulletin_text_y(3) + 6,
                 0, 0, 1, 1});
            draw_manager.draw(
                IMG_RD1_TYP4,
                {BULLETIN_TEXT_X,
                 bulletin_text_y(6) + 6,
                 0, 0, 1, 1});
            draw_manager.draw(
                IMG_RD1_TYP2,
                {BULLETIN_TEXT_X,
                 bulletin_text_y(7) + 6,
                 0, 0, 1, 1});
            draw_manager.draw(
                IMG_RD1_TYP3,
                {BULLETIN_TEXT_X,
                 bulletin_text_y(8) + 6,
                 0, 0, 1, 1});
            draw_manager.draw(
                IMG_RD1_TYP5,
                {BULLETIN_TEXT_X,
                 bulletin_text_y(9) + 6,
                 0, 0, 1, 1});
        }
    }

    if (p == BMP_EndingTheGame) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Ending the Game");
            bulletin_set_next_text("");
            bulletin_set_next_text("Once your mission is complete, you must");
            bulletin_set_next_text("return to the Space Guild HQ and claim the");
            bulletin_set_next_text("title of Guildmaster.");
            bulletin_set_next_text("");
            bulletin_set_next_text("You must first become a member of the Guild,");
            bulletin_set_next_text("if you have not done so already.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Go forth, and fulfil your destiny...");
        }
    }

    if (p == BMP_Tips) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Hints and Tips");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Sell Unused Transporters");
            bulletin_set_next_text("In the early game, you likely have more");
            bulletin_set_next_text("transporters than you need.");
            bulletin_set_next_text("");
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Reduce Science Funding");
            bulletin_set_next_text("When you start, MC income is likely more");
            bulletin_set_next_text("valuable than scientific endeavour. Use");
            bulletin_set_next_text("the CTRL menu to maximise taxes.");
        }
    }

    if (p == BMP_FAQ) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("FAQs");
            bulletin_set_next_text("");
            bulletin_set_next_text("How do I earn MC?");
            bulletin_set_next_text("Why am I receiving less MC than I should?");
            bulletin_set_next_text("Why is my planet losing food in reserve?");
            bulletin_set_next_text("Why won't a lord agree to an alliance?");
        } else {
            LINK(2, BMP_FAQ_HowEarnMC)
            LINK(3, BMP_FAQ_WhyLessMC)
            LINK(4, BMP_FAQ_WhyLessFood)
            LINK(5, BMP_FAQ_WhyNoAlliance)
        }
    }

    if (p == BMP_FAQ_HowEarnMC) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("How do I earn MC?");
            bulletin_set_next_text("");
            bulletin_set_next_text("Planets generate MC. Different types of");
            bulletin_set_next_text("planet have different worth. E.g. Forest");
            bulletin_set_next_text("planets are more valuable than Desert.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Larger planets are also worth a little more.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Cities increase MC production. Trade Centres");
            bulletin_set_next_text("facilitate sale of goods from the planet's");
            bulletin_set_next_text("reserves - if the law allows.");
            bulletin_set_next_text("");
            bulletin_set_next_text("You can sell goods - food, minerals, plutonium");
            bulletin_set_next_text("and combat units - to other powers for profit.");
            bulletin_set_next_text("");
            bulletin_set_next_text("You can even sell transporters!");
        }
    }

    if (p == BMP_FAQ_WhyLessMC) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Why am I receiving less MC than I should?");
            bulletin_set_next_text("");
            bulletin_set_next_text("Check how much of a planet's MC is allocated");
            bulletin_set_next_text("to army production.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Some of your income is invested in scientific");
            bulletin_set_next_text("research. You can adjust this in the CTRL menu");
            bulletin_set_next_text("under 'Change Officers & Taxes'.");
            bulletin_set_next_text("");
            bulletin_set_next_text("If you have upgraded any officers, these also");
            bulletin_set_next_text("require a monthly salary.");
        }
    }

    if (p == BMP_FAQ_WhyLessFood) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Why is my planet losing food in reserve?");
            bulletin_set_next_text("");
            bulletin_set_next_text("Food is perishable!");
            bulletin_set_next_text("");
            bulletin_set_next_text("This only affects food in a planetary reserve.");
            bulletin_set_next_text("Your transporters are equipped to preserve");
            bulletin_set_next_text("food indefinitely.");
        }
    }

    if (p == BMP_FAQ_WhyNoAlliance) {
        if (draw) {
            bulletin_set_text_col(COL_TEXT2);
            bulletin_set_next_text("Why won't a lord agree to an alliance?");
            bulletin_set_next_text("");
            bulletin_set_next_text("For a power to enter into an alliance, they");
            bulletin_set_next_text("need to be able to take you seriously. If");
            bulletin_set_next_text("you don't have an empire, or a good reputation,");
            bulletin_set_next_text("they are unlikely to be receptive to you.");
            bulletin_set_next_text("");
            bulletin_set_next_text("Similarly, perheps you've done something to");
            bulletin_set_next_text("sour your relationship with this power in the");
            bulletin_set_next_text("past.");
            bulletin_set_next_text("");
            bulletin_set_next_text("And sometimes, it's simply not in the lord's");
            bulletin_set_next_text("nature...");
        }
    }
}
