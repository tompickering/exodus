#include "planet_map.h"

#include "galaxy/planet.h"

#include "assetpaths.h"

#define STONE_SZ 28

const float ANIM_RATE = 0.8f;

enum ID {
    MENU_BG,
    MENU,
    SURF,
    END,
};

PlanetMap::PlanetMap() : ModeBase("PlanetMap") {
    stage = PM_Idle;
}

void PlanetMap::enter() {
    ModeBase::enter(ID::END);
    planet = exostate.get_active_planet();

    if (!planet) {
        L.fatal("Entered PlanetMap mode with no active planet");
    }

    DrawTarget tgt = TGT_Primary;

    draw_manager.draw(tgt, planet->sprites()->map_bg);

    DrawArea area = {0, 0, 126, 196};
    draw_manager.set_source_region(id(ID::MENU_BG), &area);
    draw_manager.draw(
        tgt,
        id(ID::MENU_BG),
        planet->sprites()->surf,
        {RES_X - 24, 36,
        1, 0, 1, 1});
    draw_manager.draw(
        tgt,
        id(ID::MENU),
        IMG_SU1_MENU,
        {RES_X - 24, 36,
        1, 0, 1, 1});

    int blocks = planet->get_size_blocks();
    int sz = blocks * STONE_SZ;
    area = {0, 0, sz, sz};
    draw_manager.set_source_region(id(ID::SURF), &area);
    surf_x = 24 + STONE_SZ * (16 - blocks) / 2;
    surf_y = 36 + STONE_SZ * (16 - blocks) / 2;
    draw_manager.fill({surf_x - 2, surf_y - 2, sz + 4, sz + 4}, {0, 0, 0});
    draw_manager.draw(
        tgt,
        id(ID::SURF),
        planet->sprites()->surf,
        {surf_x, surf_y,
         0, 0, 1, 1});

    draw_manager.save_background();

    for (int y = 0; y < blocks; ++y) {
        for (int x = 0; x < blocks; ++x) {
            Stone st = planet->get_stone(x, y);
            if (st != STONE_Clear) {
                draw_manager.draw(
                    tgt,
                    planet->sprites()->stones[st],
                    {surf_x + x*STONE_SZ, surf_y + y*STONE_SZ,
                     0, 0, 1, 1});
            }
        }
    }

    draw_manager.show_cursor(true);

    stage = PM_Idle;
}

ExodusMode PlanetMap::update(float delta) {
    SpriteClick click;

    PlayerInfo *player = exostate.get_active_player();

    switch(stage) {
        case PM_Idle:
            if (draw_manager.clicked()) {
                return ExodusMode::MODE_Pop;
            }
            break;
    }

    return ExodusMode::MODE_None;
}
