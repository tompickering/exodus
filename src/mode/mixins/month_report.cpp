#include "month_report.h"

#include "state/exodus_state.h"
#include "draw/draw.h"

#define NUM_MONTHREPORT_IDS 11

extern ExodusState exostate;
extern DRAWMANAGER draw_manager;

static SprID monthreport_ids[NUM_MONTHREPORT_IDS];

MonthReport::MonthReport() {
    _monthreport_open = false;
}

bool MonthReport::monthreport_is_open() {
    return _monthreport_open;
}

void MonthReport::monthreport_open() {
    if (monthreport_is_open()) {
        monthreport_close();
    }

    _monthreport_open = true;

    for (int i = 0; i < NUM_MONTHREPORT_IDS; ++i) {
        monthreport_ids[i] = draw_manager.new_sprite_id();
    }

    const int w = 440;
    const int h = 56;
    const int x = RES_X/2 - w/2;
    const int lw = 120;

    // FIXME: Should probably have a 'get_primary_player()' function or similar
    Player *p = exostate.get_player(0);
    int p_idx = exostate.get_player_idx(p);

    // TODO: Complete this
    for (int row = 0; row < 5; ++row) {
        const int b = BORDER;
        const int real_h = (h+2+2*b);
        const int base_y = (412-(real_h*5))/2;
        const int y = base_y + row*real_h - (row==0?4:0);
        if (row == 0) {
            SprID box_id = monthreport_ids[0];
            draw_manager.fill(box_id, {x-b, y-b, w+b*2, h+b*2}, COL_BORDERS);
            draw_manager.fill_pattern({x, y, w, h});

            char heading[32];
            snprintf(heading, sizeof(heading), "STATUS REPORT: MONTH %d", exostate.get_month());
            draw_manager.draw_text(heading, Justify::Centre, x+w/2, y+16, COL_TEXT);
        } else {
            SprID box_id_l = monthreport_ids[1 + 2*(row-1)];
            SprID box_id_r = monthreport_ids[1 + 2*(row-1) + 1];
            draw_manager.fill(box_id_l, {x-b, y-b, lw+b*2, h+b*2}, COL_BORDERS);
            draw_manager.fill_pattern({x, y, lw, h});

            const char* section = "Diplomacy";
            if (row == 2) section = "Army";
            if (row == 3) section = "Science";
            if (row == 4) section = "Population";
            draw_manager.draw_text(section, Justify::Left, x+4, y+16, COL_TEXT);

            const int rx = x+lw+b*2+2;
            const int rw = w-lw-2-2*b;
            draw_manager.fill(box_id_r, {rx-b, y-b, rw+2*b, h+b*2}, COL_BORDERS);
            draw_manager.fill_pattern({rx, y, rw, h});

            char text[64];

            switch (row) {
                case 1:
                    {
                        int allies = 0;
                        int enemies = 0;
                        for (int i = 0; i < N_PLAYERS; ++i) {
                            Player *other = exostate.get_player(i);
                            if (other == p) {
                                continue;
                            }
                            if (!other->is_participating()) {
                                continue;
                            }
                            if (exostate.is_allied(p_idx, i)) {
                                ++allies;
                            }
                            if (other->is_hostile_to(p_idx)) {
                                ++enemies;
                            }
                        }

                        char n_allies[4];
                        char n_enemies[4];
                        if (allies == 0) {
                            strncpy(n_allies, "no", sizeof(n_allies));
                        } else {
                            snprintf(n_allies, sizeof(n_allies), "%d", allies);
                        }
                        if (enemies == 0) {
                            strncpy(n_enemies, "no", sizeof(n_enemies));
                        } else {
                            snprintf(n_enemies, sizeof(n_enemies), "%d", enemies);
                        }

                        snprintf(text, sizeof(text),
                                 "We have %s all%s and %s",
                                 n_allies, allies==1?"y":"ies", n_enemies);

                        draw_manager.draw_text(text, Justify::Left, rx+4, y+4, COL_TEXT);

                        const char* summary = "A good level";
                        if (allies*2 < enemies*3) {
                            summary = "This is alarming";
                        }

                        snprintf(text, sizeof(text),
                                 "enem%s. %s.",
                                 enemies==1?"y":"ies", summary);

                        draw_manager.draw_text(text, Justify::Left, rx+4, y+24, COL_TEXT);
                    }
                    break;
                case 2:
                    {
                    }
                    break;
                case 3:
                    {
                    }
                    break;
                case 4:
                    {
                    }
                    break;
            }
        }
    }
}

void MonthReport::monthreport_close() {
    _monthreport_open = false;

    for (int i = 0; i < NUM_MONTHREPORT_IDS; ++i) {
        draw_manager.draw(monthreport_ids[i], nullptr);
        draw_manager.release_sprite_id(monthreport_ids[i]);
    }
}
