#include "draw.h"

SprID DrawManager::new_sprite_id() {
    if (++next_id == 0) {
        // Should we congratulate the player on their playtime?
        // Or me for us not having crashed by this point? ;)
        L.warn("Sprite ID wrapping");
    }
    return next_id;
}

void DrawManager::clear() {
    drawn_spr_info.clear();
}
