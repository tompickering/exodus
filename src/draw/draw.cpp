#include "draw.h"

#include "util/value.h"

const float UPSCALE_X = (float)SCREEN_WIDTH  / (float)RES_X;
const float UPSCALE_Y = (float)SCREEN_HEIGHT / (float)RES_Y;

const SprID ID_NONE = 0;

DrawManager::DrawManager() {
    next_id = ID_NONE + 1;
    pixelswap_time = 0;
    fade_time = 0;
    fade_seconds = 0;
    fade_stage = 0;
    draw_cursor = false;
    sprite_click.id = ID_NONE;
    clicked_this_frame = false;
}

void DrawManager::update(float delta, MousePos new_mouse_pos, MousePos new_click_pos) {
    if (pixelswap_active()) {
        pixelswap_time += delta;
    }

    if (fade_active()) {
        fade_time += delta;
    }

    DrawArea *area = nullptr;
    sprite_click.id = ID_NONE;
    mouse_pos = new_mouse_pos;
    click_pos = new_click_pos;
    clicked_this_frame = (click_pos.x >= 0 && click_pos.y >= 0);
    if (clicked_this_frame) {
        L.debug("Click: %d, %d", (int)((float)click_pos.x / UPSCALE_X), (int)((float)click_pos.y / UPSCALE_Y));
    }
    if (draw_cursor && click_pos.x >= 0 && click_pos.y >= 0) {
        for (std::vector<DrawnSprite>::size_type i = 0; i < drawn_spr_info.size(); ++i) {
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

bool DrawManager::mouse_over(SprID query) {
    DrawArea *area = nullptr;

    if (!draw_cursor)
        return false;

    if (mouse_pos.x < 0 || mouse_pos.y < 0)
        return false;

    for (std::vector<DrawnSprite>::size_type i = 0; i < drawn_spr_info.size(); ++i) {
        if (drawn_spr_info[i].id != query)
            continue;
        area = &(drawn_spr_info[i].area);
        if (area->w == 0 || area->h == 0)
            return false;
        return in_area(
                (int)((float)mouse_pos.x * UPSCALE_X), (int)((float)mouse_pos.y * UPSCALE_Y),
                area->x, area->y,
                area->w, area->h);
    }

    return false;
}

bool DrawManager::mouse_in_area(DrawArea area) {
    if (!draw_cursor)
        return false;

    return in_area(
            mouse_pos.x, mouse_pos.y,
            area.x, area.y, area.w, area.h);
}

SprID DrawManager::new_sprite_id() {
    if (next_id == ID_NONE) {
        // Should we congratulate the player on their playtime?
        // Or me for us not having crashed by this point? ;)
        L.warn("Sprite ID wrapping");
        next_id = ID_NONE + 1;
    }
    //L.debug("Allocating SprID %d", next_id);
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

void DrawManager::set_source_region(SprID id, DrawArea* area) {
    if (!area) {
        if (id_source_regions.count(id)) {
            id_source_regions.erase(id);
        }
        return;
    }

    id_source_regions[id] = *area;
}

DrawArea* DrawManager::get_source_region(SprID id) {
    if (id_source_regions.count(id)) {
        return &id_source_regions[id];
    }
    return nullptr;
}

void DrawManager::clear_sprite_ids() {
    drawn_spr_info.clear();
    id_source_regions.clear();
}

void DrawManager::clear(DrawTarget tgt) {
    if (tgt == DrawTarget::TGT_Primary) {
        clear_sprite_ids();
    }
}

void DrawManager::show_cursor(bool show) {
    draw_cursor = show;
}

bool DrawManager::clicked() {
    return clicked_this_frame;
}

int DrawManager::right(SprID id) {
    DrawArea *area = get_drawn_area(id);
    if (!area) {
        L.warn("Attemmpt to get position to the right of an unknown sprite ID");
        return 0;
    }

    return (area->x + area->w) / UPSCALE_X;
}
