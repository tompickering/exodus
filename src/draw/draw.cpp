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
    sprite_click.id = ID_NONE;
}

void DrawManager::update(MousePos mouse_pos, MousePos new_click_pos) {
    DrawArea *area = nullptr;
    sprite_click.id = ID_NONE;
    click_pos = new_click_pos;
    if (draw_cursor && click_pos.x >= 0 && click_pos.y >= 0) {
        for (std::vector<DrawnSprite>::size_type i = 0; i < drawn_spr_info.size(); ++i) {
            if (drawn_spr_info[i].id == ID_CURSOR)
                continue;
            area = &(drawn_spr_info[i].area);
            if (area->w == 0 || area->h == 0)
                continue;
            float spr_x = (float)(click_pos.x - area->x) / (float)area->w;
            float spr_y = (float)(click_pos.y - area->y) / (float)area->h;
            if (   spr_x >= 0 && spr_x <= 1
                && spr_y >= 0 && spr_y <= 1) {
                sprite_click.id = drawn_spr_info[i].id;
                sprite_click.x = spr_x;
                sprite_click.y = spr_y;
                // Don't break in case multiple sprites overlap, in
                // which case we probably care about the later one.
            }
        }
    }

    if (sprite_click.id != ID_NONE) {
        L.debug("CLICK: %d: %f, %f", sprite_click.id, sprite_click.x, sprite_click.y);
    }
}

SpriteClick DrawManager::get_clicked_sprite() {
    return sprite_click;
}

SpriteClick DrawManager::query_click(SprID query) {
    DrawArea *area = nullptr;
    SpriteClick res;
    res.id = ID_NONE;

    if (!draw_cursor)
        return res;
    if (click_pos.x < 0 || click_pos.y < 0)
        return res;

    for (std::vector<DrawnSprite>::size_type i = 0; i < drawn_spr_info.size(); ++i) {
        if (drawn_spr_info[i].id != query)
            continue;
        area = &(drawn_spr_info[i].area);
        if (area->w == 0 || area->h == 0)
            continue;
        float spr_x = (float)(click_pos.x - area->x) / (float)area->w;
        float spr_y = (float)(click_pos.y - area->y) / (float)area->h;
        if (   spr_x >= 0 && spr_x <= 1
            && spr_y >= 0 && spr_y <= 1) {
            res.id = drawn_spr_info[i].id;
            res.x = spr_x;
            res.y = spr_y;
            break;
        }
    }

    return res;
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

void DrawManager::clear_sprite_ids() {
    drawn_spr_info.clear();
}

void DrawManager::clear(DrawTarget tgt) {
    if (tgt == DrawTarget::TGT_Primary) {
        clear_sprite_ids();
    }
}

void DrawManager::show_cursor(bool show) {
    draw_cursor = show;
}
