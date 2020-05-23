#include "comm_panel_drawer.h"

#include "assetpaths.h"
#include "shared.h"

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
    strncpy(comm_img_caption, "", 1);

    new (&comm_anim) Anim(1, IMG_LD0_LD0);

    _comm_is_open = false;
}

void CommPanelDrawer::comm_set_title(const char* text) {
    strncpy(comm_title, text, MAX_TEXT - 1);
}

void CommPanelDrawer::comm_set_anim(const Anim& a) {
    comm_anim = a;
}

void CommPanelDrawer::comm_set_img_caption(const char* text) {
    strncpy(comm_img_caption, text, MAX_TEXT - 1);
}

void CommPanelDrawer::comm_set_text(int idx, const char* in_text) {
    if (idx >= 6) {
        L.fatal("Tried to set invalid text index %d to %s", idx, in_text);
    }

    strncpy(comm_text[idx], in_text, MAX_TEXT - 1);
}

void CommPanelDrawer::comm_open(int text_slots) {
    comm_text_slots = text_slots;

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
        comm_img_caption,
        Justify::Left,
        COMM_X + COMM_BORDER + 6,
        COMM_Y + COMM_H - COMM_BORDER - 16,
        COL_TEXT2);

    for (int i = 0; i < 6; ++i) {
        draw_manager.draw_text(
            id_text[i],
            comm_text[i],
            Justify::Left,
            COMM_RCOL_X + 8,
            comm_text_y(i),
            COL_TEXT);
    }

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
    _comm_is_open = false;

    // Wipe all info
    for (int i = 0; i < 6; ++i) {
        strncpy(comm_text[i], "", 1);
    }

    strncpy(comm_title, "", 1);
    strncpy(comm_img_caption, "", 1);

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
    return CA_None;
}

int CommPanelDrawer::comm_text_y(int idx) {
    return COMM_Y + 36 + COMM_BORDER*2 + idx*120/comm_text_slots;
}
