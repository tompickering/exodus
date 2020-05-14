#include "planet_map.h"

#include <map>
#include <cmath>

#include "galaxy/planet.h"

#include "assetpaths.h"

using std::map;

#define STONE_SZ 28

const float ANIM_RATE = 0.5f;

enum ID {
    MENU_BG,
    MENU,
    SURF,
    END,
};

map<PlanetClass, map<Stone, Anim>> stone_anims_specific;
map<Stone, Anim> stone_anims_generic;
bool stone_anims_initialised = false;

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

            if (st == STONE_Clear)
                continue;

            Anim *anim = get_stone_anim(st);

            // If 'all' is set, don't draw stones that don't animate
            if (anim && all && anim->is_static()) {
                continue;
            }

            const char* sprite = IMG_SU1_STONE7;
            if (anim) sprite = anim->interp(anim_cycle);

            draw_manager.draw(
                sprite,
                {surf_x + x*STONE_SZ, surf_y + y*STONE_SZ,
                 0, 0, 1, 1});
        }
    }
}

void init_stone_anims() {
    stone_anims_specific[Forest][STONE_Agri]             = Anim(1, IMG_SF1_STONE2);
    stone_anims_specific[Forest][STONE_NaturalSmall]     = Anim(1, IMG_SF1_STONE9);
    stone_anims_specific[Forest][STONE_NaturalLarge]     = Anim(1, IMG_SF1_STONE8);
    stone_anims_specific[Forest][STONE_NaturalAnim]      = Anim(2, IMG_SF1_STONE29,
                                                                   IMG_SF1_STONE29B);

    stone_anims_specific[Desert][STONE_Agri]             = Anim(1, IMG_SF2_STONE2);
    stone_anims_specific[Desert][STONE_NaturalSmall]     = Anim(1, IMG_SF2_STONE9);
    stone_anims_specific[Desert][STONE_NaturalLarge]     = Anim(1, IMG_SF2_STONE8);
    stone_anims_specific[Desert][STONE_NaturalAnim]      = Anim(2, IMG_SF2_STONE29,
                                                                   IMG_SF2_STONE29B);

    stone_anims_specific[Volcano][STONE_Agri]            = Anim(1, IMG_SF3_STONE2);
    stone_anims_specific[Volcano][STONE_NaturalSmall]    = Anim(1, IMG_SF3_STONE9);
    stone_anims_specific[Volcano][STONE_NaturalLarge]    = Anim(1, IMG_SF3_STONE8);
    stone_anims_specific[Volcano][STONE_NaturalAnim]     = Anim(2, IMG_SF3_STONE29,
                                                                   IMG_SF3_STONE29B);

    stone_anims_specific[Rock][STONE_Agri]               = Anim(1, IMG_SF4_STONE2);
    stone_anims_specific[Rock][STONE_NaturalSmall]       = Anim(1, IMG_SF4_STONE9);
    stone_anims_specific[Rock][STONE_NaturalLarge]       = Anim(1, IMG_SF4_STONE8);
    stone_anims_specific[Rock][STONE_NaturalAnim]        = Anim(2, IMG_SF4_STONE29,
                                                                   IMG_SF4_STONE29B);

    stone_anims_specific[Ice][STONE_Agri]                = Anim(1, IMG_SF5_STONE2);
    stone_anims_specific[Ice][STONE_NaturalSmall]        = Anim(1, IMG_SF5_STONE9);
    stone_anims_specific[Ice][STONE_NaturalLarge]        = Anim(1, IMG_SF5_STONE8);
    stone_anims_specific[Ice][STONE_NaturalAnim]         = Anim(2, IMG_SF5_STONE29,
                                                                   IMG_SF5_STONE29B);

    stone_anims_specific[Terra][STONE_Agri]              = Anim(1, IMG_SF6_STONE2);
    stone_anims_specific[Terra][STONE_NaturalSmall]      = Anim(1, IMG_SF6_STONE9);
    stone_anims_specific[Terra][STONE_NaturalLarge]      = Anim(1, IMG_SF6_STONE8);
    stone_anims_specific[Terra][STONE_NaturalAnim]       = Anim(2, IMG_SF6_STONE29,
                                                                   IMG_SF6_STONE29B);

    stone_anims_specific[Artificial][STONE_Agri]         = Anim(1, IMG_SF8_STONE2);
    stone_anims_specific[Artificial][STONE_NaturalSmall] = Anim(1, IMG_SF8_STONE9);
    stone_anims_specific[Artificial][STONE_NaturalLarge] = Anim(1, IMG_SF8_STONE8);
    stone_anims_specific[Artificial][STONE_NaturalAnim]  = Anim(2, IMG_SF8_STONE29,
                                                                   IMG_SF8_STONE29B);

    stone_anims_generic[STONE_Village]                   = Anim(1, IMG_SU1_STONE22);

    stone_anims_initialised = true;
}

Anim* PlanetMap::get_stone_anim(Stone stone) {
    if (!stone_anims_initialised)
        init_stone_anims();

    PlanetClass cls = planet->get_class();
    if (stone_anims_specific[cls].count(stone)) {
        return &stone_anims_specific[cls][stone];
    }

    if (stone_anims_generic.count(stone)) {
        return &stone_anims_generic[stone];
    }

    L.warn("Missing anim for %s stone %d", planet->get_class_str(), stone);
    return nullptr;
}
