#include "frame_drawer.h"

#include "shared.h"

FrameDrawer::FrameDrawer() {
    _frame_open = false;
}

void FrameDrawer::frame_draw() {
    if (!_frame_open) {
        id_frame_t = draw_manager.new_sprite_id();
        id_frame_b = draw_manager.new_sprite_id();
        id_frame_l = draw_manager.new_sprite_id();
        id_frame_r = draw_manager.new_sprite_id();
    }
    _frame_open = true;
    draw_manager.fill(id_frame_t, {0, 0, RES_X, 7}, COL_BORDERS);
    draw_manager.fill(id_frame_b, {0, 403, RES_X, 7}, COL_BORDERS);
    draw_manager.fill(id_frame_l, {0, 0, 7, 403}, COL_BORDERS);
    draw_manager.fill(id_frame_r, {RES_X - 7, 0, 7, 403}, COL_BORDERS);

    draw_manager.fill(FILL_3D_Out_Hollow, {0, 0, RES_X, 403}, COL_BORDERS);
}

void FrameDrawer::frame_remove() {
    if (_frame_open) {
        draw_manager.draw(id_frame_t, nullptr);
        draw_manager.draw(id_frame_b, nullptr);
        draw_manager.draw(id_frame_l, nullptr);
        draw_manager.draw(id_frame_r, nullptr);
        draw_manager.release_sprite_id(id_frame_t);
        draw_manager.release_sprite_id(id_frame_b);
        draw_manager.release_sprite_id(id_frame_l);
        draw_manager.release_sprite_id(id_frame_r);
    }
    _frame_open = false;
}
