#include "draw.h"

DrawManager::DrawManager() {
    next_id = 0;
}

SprID DrawManager::new_sprite_id() {
    if (++next_id == 0) {
        // Should we congratulate the player on their playtime?
        // Or me for us not having crashed by this point? ;)
        L.warn("Sprite ID wrapping");
        ++next_id;
    }
    L.debug("Allocating SprID %d", next_id);
    return next_id;
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
