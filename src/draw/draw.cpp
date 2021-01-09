#include "draw.h"

#include <cmath>

#include "util/value.h"

#define COL_PULSE_RATE 1.2

const SprID ID_NONE = 0;

const RGB col_pulse_0 {0x90, 0x90, 0xFF}; // TODO: Check this
const RGB col_pulse_1 {0xFF, 0xFF, 0xFF};

DrawManager::DrawManager() {
    next_id = ID_NONE + 1;
    pixelswap_time = 0;
    fade_time = 0;
    fade_seconds = 0;
    fade_stage = 0;
    draw_cursor = false;
    sprite_click.id = ID_NONE;
    clicked_this_frame = false;
    prev_mouseover_selectable_text_id = ID_NONE;
    mouseover_selectable_text_id = ID_NONE;
}

void DrawManager::update(float delta, MousePos new_mouse_pos, MousePos new_click_pos) {
    if (pixelswap_active()) {
        pixelswap_time += delta;
    }

    if (fade_active()) {
        fade_time += delta;
    }

    // N.B. mouse_pos and click_pos are both in screen
    // co-ordinates, not resolution co-ordinates.

    DrawArea *area = nullptr;
    sprite_click.id = ID_NONE;
    mouse_pos = new_mouse_pos;
    click_pos = new_click_pos;
    clicked_this_frame = (click_pos.x >= 0 && click_pos.y >= 0);
    if (clicked_this_frame) {
        float up_x, up_y;
        get_upscale(up_x, up_y);
        L.debug("Click: %d, %d", (int)((float)click_pos.x / up_x), (int)((float)click_pos.y / up_y));
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

    bool found_mouseover_selectable_text = false;
    for (std::vector<SprID>::size_type i = 0; i < selectable_text_ids.size(); ++i) {
        if (query_mouseover(selectable_text_ids[i]).id) {
            found_mouseover_selectable_text = true;
            mouseover_selectable_text_id = selectable_text_ids[i];
        }
    }

    if (found_mouseover_selectable_text) {
        if (mouseover_selectable_text_id != prev_mouseover_selectable_text_id) {
            mouseover_selectable_text_time = 0;
        } else {
            mouseover_selectable_text_time += delta;
        }
    } else {
        mouseover_selectable_text_id = ID_NONE;
    }

    prev_mouseover_selectable_text_id = mouseover_selectable_text_id;

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

SpriteClick DrawManager::query_mouseover(SprID query) {
    DrawArea *area = nullptr;
    SpriteClick res;
    res.id = ID_NONE;

    if (!draw_cursor)
        return res;
    if (mouse_pos.x < 0 || mouse_pos.y < 0)
        return res;

    for (std::vector<DrawnSprite>::size_type i = 0; i < drawn_spr_info.size(); ++i) {
        if (drawn_spr_info[i].id != query)
            continue;
        area = &(drawn_spr_info[i].area);
        if (area->w == 0 || area->h == 0)
            continue;
        float spr_x = (float)(mouse_pos.x - area->x) / (float)area->w;
        float spr_y = (float)(mouse_pos.y - area->y) / (float)area->h;
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

void DrawManager::consume_click() {
    click_pos = {-1, -1};
    clicked_this_frame = false;
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
        float up_x, up_y;
        get_upscale(up_x, up_y);
        return in_area(
                (int)((float)mouse_pos.x), (int)((float)mouse_pos.y),
                area->x, area->y,
                area->w, area->h);
    }

    return false;
}

bool DrawManager::mouse_in_area(DrawArea area) {
    if (!draw_cursor)
        return false;

    float up_x, up_y;
    get_upscale(up_x, up_y);

    return in_area(
            mouse_pos.x / up_x, mouse_pos.y / up_y,
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

DrawnSprite* DrawManager::get_drawn_info(SprID id) {
    for (std::vector<DrawnSprite>::size_type i = 0; i < drawn_spr_info.size(); ++i) {
        if (drawn_spr_info[i].id == id) {
            return &drawn_spr_info[i];
        }
    }
    return nullptr;
}

DrawArea* DrawManager::get_drawn_area(SprID id) {
    DrawnSprite *drawn_info = get_drawn_info(id);
    return drawn_info ? &(drawn_info->area) : nullptr;
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
    selectable_text_ids.clear();
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

    float up_x, up_y;
    get_upscale(up_x, up_y);
    return (area->x + area->w) / up_x;
}

void DrawManager::get_upscale(float& up_x, float& up_y) {
    up_x = 1; up_y = 1;
}

RGB DrawManager::text_pulse_col(float t) {
    t *= COL_PULSE_RATE;
    t = fmod(t, 1);
    t = sin(t * 3.141592);
    unsigned char r0 = col_pulse_0.r;
    unsigned char g0 = col_pulse_0.g;
    unsigned char b0 = col_pulse_0.b;
    unsigned char r1 = col_pulse_1.r;
    unsigned char g1 = col_pulse_1.g;
    unsigned char b1 = col_pulse_1.b;
    unsigned char r = (unsigned char)((int16_t)r0 + (int16_t)(t * ((float)r1 - (float)r0)));
    unsigned char g = (unsigned char)((int16_t)g0 + (int16_t)(t * ((float)g1 - (float)g0)));
    unsigned char b = (unsigned char)((int16_t)b0 + (int16_t)(t * ((float)b1 - (float)b0)));
    return {r, g, b};
}

void DrawManager::set_selectable(SprID id) {
    for (std::vector<SprID>::size_type i = 0; i < selectable_text_ids.size(); ++i) {
        if (id == selectable_text_ids[i]) {
            return;
        }
    }
    selectable_text_ids.push_back(id);
}

void DrawManager::unset_selectable(SprID id) {
    for (std::vector<SprID>::iterator i = selectable_text_ids.begin();
         i < selectable_text_ids.end();
         ++i) {
        if (id == *i) {
            selectable_text_ids.erase(i);
            return;
        }
    }
}

void DrawManager::adjust_selectable_text_col(SprID id, RGB& rgb) {
    if (id && id == mouseover_selectable_text_id) {
        rgb = text_pulse_col(mouseover_selectable_text_time);
    }
}
