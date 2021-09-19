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

    // TODO: Complete this
    for (int i = 0; i < 5; ++i) {
        const int b = BORDER;
        const int real_h = (h+2+2*b);
        const int base_y = (412-(real_h*5))/2;
        const int y = base_y + i*real_h - (i==0?4:0);
        if (i == 0) {
            SprID box_id = monthreport_ids[0];
            draw_manager.fill(box_id, {x-b, y-b, w+b*2, h+b*2}, COL_BORDERS);
            draw_manager.fill_pattern({x, y, w, h});

            char heading[32];
            snprintf(heading, sizeof(heading), "STATUS REPORT: MONTH %d", exostate.get_month());
            draw_manager.draw_text(heading, Justify::Centre, x+w/2, y+16, COL_TEXT);
        } else {
            SprID box_id_l = monthreport_ids[1 + 2*(i-1)];
            SprID box_id_r = monthreport_ids[1 + 2*(i-1) + 1];
            draw_manager.fill(box_id_l, {x-b, y-b, lw+b*2, h+b*2}, COL_BORDERS);
            draw_manager.fill_pattern({x, y, lw, h});

            const char* section = "Diplomacy";
            if (i == 2) section = "Army";
            if (i == 3) section = "Science";
            if (i == 4) section = "Population";
            draw_manager.draw_text(section, Justify::Left, x+4, y+16, COL_TEXT);

            const int rx = x+lw+b*2+2;
            const int rw = w-lw-2-2*b;
            draw_manager.fill(box_id_r, {rx-b, y-b, rw+2*b, h+b*2}, COL_BORDERS);
            draw_manager.fill_pattern({rx, y, rw, h});
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
