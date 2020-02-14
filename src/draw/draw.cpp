#include "draw.h"

const float UPSCALE_X = (float)SCREEN_WIDTH  / (float)RES_X;
const float UPSCALE_Y = (float)SCREEN_HEIGHT / (float)RES_Y;

const SprID ID_NONE = 0;
const SprID ID_CURSOR = 1;

DrawManager::DrawManager() {
    next_id = ID_CURSOR + 1;
    fade_timer.start();
    fade_seconds = 0;
    fade_stage = 0;
    draw_cursor = false;
    clicked_sprite = ID_NONE;
}

void DrawManager::update(MousePos mouse_pos, MousePos click_pos) {
    DrawArea *area = nullptr;
    clicked_sprite = ID_NONE;
    if (click_pos.x >= 0 && click_pos.y >= 0) {
        for (std::vector<DrawnSprite>::size_type i = 0; i < drawn_spr_info.size(); ++i) {
            area = &(drawn_spr_info[i].area);
            if (   click_pos.x >= area->x
                && click_pos.y >= area->y
                && click_pos.x <= area->x + area->w
                && click_pos.y <= area->y + area->h) {
                clicked_sprite = drawn_spr_info[i].id;
            }
        }
    }

    if (clicked_sprite != ID_NONE) {
        L.debug("CLICKED: %d", clicked_sprite);
    }
}

SprID DrawManager::get_clicked_sprite() {
    return clicked_sprite;
}

SprID DrawManager::new_sprite_id() {
    if (next_id == ID_NONE) {
        // Should we congratulate the player on their playtime?
        // Or me for us not having crashed by this point? ;)
        L.warn("Sprite ID wrapping");
        next_id = ID_CURSOR + 1;
    }
    L.debug("Allocating SprID %d", next_id);
    return next_id++;
}

void DrawManager::release_sprite_id(SprID id) {
    for (std::vector<DrawnSprite>::size_type i = 0; i < drawn_spr_info.size(); ++i) {
        if (drawn_spr_info[i].id == id) {
            drawn_spr_info.erase(drawn_spr_info.begin() + i);
            break;
        }
    }
}

DrawArea* DrawManager::get_drawn_area(SprID id) {
    for (std::vector<DrawnSprite>::size_type i = 0; i < drawn_spr_info.size(); ++i) {
        if (drawn_spr_info[i].id == id) {
            return &(drawn_spr_info[i].area);
        }
    }
    return nullptr;
}

void DrawManager::clear() {
    drawn_spr_info.clear();
}

void DrawManager::show_cursor(bool show) {
    draw_cursor = show;
}
