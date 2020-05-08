#include "star_map.h"

#include "assetpaths.h"

enum ID {
    PLANET1,
    PLANET2,
    PLANET3,
    PLANET4,
    PLANET5,
    END,
};

StarMap::StarMap() : ModeBase("StarMap"), PanelDrawer(PNL_Star) {
}

void StarMap::enter() {
    ModeBase::enter(ID::END);

    DrawTarget tgt = TGT_Primary;
    if (exodus.get_prev_mode() == ExodusMode::MODE_GalaxyMap) {
        tgt = TGT_Secondary;
    }

    draw_manager.draw(tgt, IMG_MAP_SOLAR);
    draw_panel_bg(tgt);
    draw_manager.save_background();
    draw_manager.show_cursor(true);

    if (tgt != TGT_Primary) {
        draw_manager.fade_start(1.f, 12);
    }
}

ExodusMode StarMap::update(float delta) {
    return ExodusMode::MODE_None;
}
