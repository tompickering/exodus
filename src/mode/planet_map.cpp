#include "planet_map.h"

#include <cmath>

#include "galaxy/planet.h"

#include "assetpaths.h"

#define STONE_SZ 28

const float ANIM_RATE = 0.5f;

enum ID {
    MENU_BG,
    MENU,
    SURF,
    END,
};

PlanetMap::PlanetMap() : ModeBase("PlanetMap") {
    stage = PM_Idle;
    blocks = 0;
    anim_cycle = 0;
}

void PlanetMap::enter() {
    ModeBase::enter(ID::END);
    planet = exostate.get_active_planet();

    if (!planet) {
        L.fatal("Entered PlanetMap mode with no active planet");
    }

    anim_cycle = 0;

    new (&natural_anim) Anim(2, planet->sprites()->stones[STONE_NaturalAnim],
                                planet->sprites()->natural_anim_alt);

    draw_manager.draw(planet->sprites()->map_bg);

    DrawArea area = {0, 0, 126, 196};
    draw_manager.set_source_region(id(ID::MENU_BG), &area);
    draw_manager.draw(
        id(ID::MENU_BG),
        planet->sprites()->surf,
        {RES_X - 24, 36,
        1, 0, 1, 1});
    draw_manager.draw(
        id(ID::MENU),
        IMG_SU1_MENU,
        {RES_X - 24, 36,
        1, 0, 1, 1});

    blocks = planet->get_size_blocks();
    int sz = blocks * STONE_SZ;
    area = {0, 0, sz, sz};
    draw_manager.set_source_region(id(ID::SURF), &area);
    surf_x = 24 + STONE_SZ * (16 - blocks) / 2;
    surf_y = 36 + STONE_SZ * (16 - blocks) / 2;
    draw_manager.fill({surf_x - 2, surf_y - 2, sz + 4, sz + 4}, {0, 0, 0});
    draw_manager.draw(
        id(ID::SURF),
        planet->sprites()->surf,
        {surf_x, surf_y,
         0, 0, 1, 1});

    draw_manager.save_background();

    draw_stones(true);

    draw_manager.show_cursor(true);

    stage = PM_Idle;
}

ExodusMode PlanetMap::update(float delta) {
    SpriteClick click;

    PlayerInfo *player = exostate.get_active_player();

    anim_cycle = fmod(anim_cycle + delta * ANIM_RATE, 1);

    switch(stage) {
        case PM_Idle:
            draw_stones(false);
            if (draw_manager.clicked()) {
                return ExodusMode::MODE_Pop;
            }
            break;
    }

    return ExodusMode::MODE_None;
}

/*
 * N.B. we use the term 'stone' from the original to mean 'any object
 * which can exist on the planet surface'.
 */
void PlanetMap::draw_stones(bool all) {
    for (int y = 0; y < blocks; ++y) {
        for (int x = 0; x < blocks; ++x) {
            Stone st = planet->get_stone(x, y);
            // Update the stones that animate
            if (st == STONE_NaturalAnim) {
                draw_manager.draw(
                    natural_anim.interp(anim_cycle),
                    {surf_x + x*STONE_SZ, surf_y + y*STONE_SZ,
                     0, 0, 1, 1});
            } else if (all) {
                if (st == STONE_Clear)
                    continue;
                draw_manager.draw(
                    planet->sprites()->stones[st],
                    {surf_x + x*STONE_SZ, surf_y + y*STONE_SZ,
                     0, 0, 1, 1});
            }
        }
    }
}
