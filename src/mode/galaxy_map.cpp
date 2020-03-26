#include "galaxy_map.h"

#include "assetpaths.h"

enum ID {
    END,
};

GalaxyMap::GalaxyMap() : ModeBase("GalaxyMap"), GalaxyDrawer() {
    stage = GM_SwapIn;
}

void GalaxyMap::enter() {
    ModeBase::enter(ID::END);
    draw_galaxy(false);
    draw_manager.fill(TGT_Secondary, galaxy_panel_area, {0xA0, 0xA0, 0xA0});
    draw_manager.pixelswap_start();
    stage = GM_SwapIn;
}

const float FADE_SPEED = 10.f;

ExodusMode GalaxyMap::update(float delta) {
    FlyTarget *ft;

    if (draw_manager.pixelswap_active()) {
        return ExodusMode::MODE_None;
    }

    switch (stage) {
        case GM_SwapIn:
            draw_manager.save_background();
            draw_manager.show_cursor(true);
            stage = GM_Idle;
            break;
        case GM_Idle:
            ft = get_clicked_flytarget();
            if (ft) {
                L.debug("Clicked %s", ft->name);
            }
            break;
        default:
            break;
    }

    return ExodusMode::MODE_None;
}
