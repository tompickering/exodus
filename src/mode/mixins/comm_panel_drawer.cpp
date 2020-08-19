#include "comm_panel_drawer.h"

#include "assetpaths.h"
#include "shared.h"

#include "util/str.h"

const int COMM_BORDER = 8;
const int COMM_W = 580;
const int COMM_H = 206 + (COMM_BORDER + 1) * 2;
const int COMM_X = (RES_X / 2) - (COMM_W / 2);
const int COMM_Y = (RES_Y / 2) - (COMM_H / 2);
const int COMM_RCOL_X = COMM_X + 196 + COMM_BORDER * 2;

CommPanelDrawer::CommPanelDrawer() {
    comm_text[0] = comm_text0; comm_text[1] = comm_text1; comm_text[2] = comm_text2;
    comm_text[3] = comm_text3; comm_text[4] = comm_text4; comm_text[5] = comm_text5;

    for (int i = 0; i < 6; ++i) {
        strncpy(comm_text[i], "", 1);
    }

    strncpy(comm_title, "", 1);
    strncpy(comm_img_caption_upper, "", 1);
    strncpy(comm_img_caption_lower, "", 1);

    new (&comm_anim) Anim(1, IMG_LD0_LD0);

    show_buttons = true;
    comm_mouseover_text = -1;
    comm_time_since_text_mouseover = 0;
    comm_text_interactive_mask = 0;
    _comm_is_open = false;
}

void CommPanelDrawer::comm_update(float dt) {
    comm_time_since_text_mouseover += dt;
    comm_draw_text();
}

void CommPanelDrawer::comm_draw_text() {
    bool mouseover_any = false;
    // FIXME: We can optimise redraws by only drawing text that has just stopped
    // or having or currently has the mouse over it - but we'll also need to know whether
    // this is a redraw vs an initial draw...
    for (int i = 0; i < 6; ++i) {
        RGB col = COL_TEXT;

        if (comm_text_interactive_mask & (1 << i)) {
            if (draw_manager.mouse_over(id_text[i])) {
                mouseover_any = true;
                if (i != comm_mouseover_text) {
                    comm_mouseover_text = i;
                    comm_time_since_text_mouseover = 0;
                }
                col = draw_manager.text_pulse_col(comm_time_since_text_mouseover);
                L.debug("%d", col.r);
            }
        }

        draw_manager.draw_text(
            id_text[i],
            comm_text[i],
            Justify::Left,
            COMM_RCOL_X + 8,
            comm_text_y(i),
            col);
    }

    if (!mouseover_any) {
        comm_mouseover_text = -1;
    }
}

void CommPanelDrawer::comm_set_title(const char* text) {
    strncpy(comm_title, text, COMM_MAX_TEXT - 1);
}

void CommPanelDrawer::comm_set_anim(const Anim& a) {
    comm_anim = a;
}

void CommPanelDrawer::comm_set_img_caption(const char* text) {
    comm_set_img_caption(nullptr, text);
}

void CommPanelDrawer::comm_set_img_caption(const char* text_upper, const char* text_lower) {
    if (text_upper) {
        strncpy(comm_img_caption_upper, text_upper, COMM_MAX_TEXT - 1);
        capitalise(comm_img_caption_upper, COMM_MAX_TEXT);
    }

    if (text_lower) {
        strncpy(comm_img_caption_lower, text_lower, COMM_MAX_TEXT - 1);
        capitalise(comm_img_caption_lower, COMM_MAX_TEXT);
    }
}

void CommPanelDrawer::comm_set_buttons(bool buttons_on) {
    show_buttons = buttons_on;
}

void CommPanelDrawer::comm_set_text(int idx, const char* in_text, ...) {
    if (idx >= 6) {
        L.fatal("Tried to set invalid text index %d to %s", idx, in_text);
    }

    va_list args;
    va_start(args, in_text);
    vsnprintf(comm_text[idx], COMM_MAX_TEXT - 1, in_text, args);
    va_end(args);
}

void CommPanelDrawer::comm_set_text_interactive_mask(unsigned char mask) {
    comm_text_interactive_mask = mask;
}

void CommPanelDrawer::comm_open(int text_slots) {
    comm_text_slots = text_slots;

    comm_mouseover_text = -1;
    comm_time_since_text_mouseover = 0;

    if (comm_text_slots <= 0) {
        comm_text_slots = 1;
    }

    id_comm_panel = draw_manager.new_sprite_id();
    id_comm_img = draw_manager.new_sprite_id();
    id_comm_buttons = draw_manager.new_sprite_id();

    for (int i = 0; i < 6; ++i) {
        id_text[i] = draw_manager.new_sprite_id();
    }

    draw_manager.fill(
        id_comm_panel,
        {COMM_X, COMM_Y,
         COMM_W, COMM_H},
         COL_BORDERS);
    draw_manager.fill(
        {COMM_X + COMM_BORDER,
         COMM_Y + COMM_BORDER,
         196, 208},
         {0, 0, 0});
    draw_manager.pattern_fill(
        {COMM_RCOL_X,
         COMM_Y + COMM_BORDER,
         COMM_X + COMM_W - COMM_RCOL_X - COMM_BORDER,
         28});
    draw_manager.pattern_fill(
        {COMM_RCOL_X,
         COMM_Y + 28 + COMM_BORDER*2,
         COMM_X + COMM_W - COMM_RCOL_X - COMM_BORDER,
         172});

    if (show_buttons) {
        draw_manager.draw(
            id_comm_buttons,
            IMG_BR1_EXPORT,
            {COMM_RCOL_X + 2,
             COMM_Y + COMM_H - COMM_BORDER - 2,
             0, 1, 1, 1});

        draw_manager.fill(
            {COMM_RCOL_X + 4 + 236,
             COMM_Y + COMM_H - COMM_BORDER - 28,
             118, 26},
             COL_BORDERS);
    }

    draw_manager.save_background({COMM_X, COMM_Y,
                                  COMM_W, COMM_H});

    draw_manager.draw(
        id_comm_img,
        comm_anim.frame(0),
        {COMM_X + COMM_BORDER + 1,
         COMM_Y + COMM_BORDER + 1,
         0, 0, 1, 1});

    draw_manager.draw_text(
        comm_title,
        Justify::Left,
        COMM_RCOL_X + 8,
        COMM_Y + 11,
        COL_TEXT);

    draw_manager.draw_text(
        Font::Tiny,
        comm_img_caption_upper,
        Justify::Left,
        COMM_X + COMM_BORDER + 6,
        COMM_Y + COMM_H - COMM_BORDER - 28,
        COL_TEXT2);

    draw_manager.draw_text(
        Font::Tiny,
        comm_img_caption_lower,
        Justify::Left,
        COMM_X + COMM_BORDER + 6,
        COMM_Y + COMM_H - COMM_BORDER - 16,
        COL_TEXT2);

    comm_draw_text();

    _comm_is_open = true;
}

void CommPanelDrawer::comm_close() {
    if (!_comm_is_open) {
        L.fatal("Attempt to close comm panel whilst not open");
    }

    for (int i = 0; i < 6; ++i) {
        draw_manager.release_sprite_id(id_text[i]);
    }

    draw_manager.release_sprite_id(id_comm_img);
    draw_manager.release_sprite_id(id_comm_buttons);

    draw_manager.draw(id_comm_panel, nullptr);
    draw_manager.release_sprite_id(id_comm_panel);
    show_buttons = true;
    _comm_is_open = false;

    // Wipe all info
    for (int i = 0; i < 6; ++i) {
        strncpy(comm_text[i], "", 1);
    }

    comm_mouseover_text = -1;
    comm_time_since_text_mouseover = 0;
    comm_text_interactive_mask = 0;

    strncpy(comm_title, "", 1);
    strncpy(comm_img_caption_upper, "", 1);
    strncpy(comm_img_caption_lower, "", 1);

    new (&comm_anim) Anim(1, IMG_LD0_LD0);
}

void CommPanelDrawer::comm_ensure_closed() {
    if (_comm_is_open)
        comm_close();
}

bool CommPanelDrawer::comm_is_open() {
    return _comm_is_open;
}

CommAction CommPanelDrawer::comm_check_action() {
    SpriteClick click;
    click = draw_manager.query_click(id_comm_buttons);
    if (click.id) {
        return click.x < 0.55 ? CA_Proceed : CA_Abort;
    }
    for (int i = 0; i < 6; ++i) {
        if (draw_manager.query_click(id_comm_buttons).id) {
            return (CommAction)((int)CA_Text0 + i);
        }
    }
    return CA_None;
}

int CommPanelDrawer::comm_text_y(int idx) {
    return COMM_Y + 36 + COMM_BORDER*2 + idx*120/comm_text_slots;
}
