#include "comm_panel_drawer.h"

#include "assetpaths.h"
#include "shared.h"

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

void CommPanelDrawer::comm_open() {
    id_comm_panel = draw_manager.new_sprite_id();
    draw_manager.fill(id_comm_panel, {100, 100, 100, 100}, COL_BORDERS);
    _comm_is_open = true;
}

void CommPanelDrawer::comm_close() {
    draw_manager.draw(id_comm_panel, nullptr);
    draw_manager.release_sprite_id(id_comm_panel);
    _comm_is_open = false;
}

bool CommPanelDrawer::comm_is_open() {
    return _comm_is_open;
}
