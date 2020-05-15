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
    ICON_FOOD,
    ICON_PLU,
    ICON_UNREST,
    GAUGE_FOOD,
    GAUGE_PLU,
    GAUGE_UNREST,
    TRIBUTTONS,
    LAWBUILD,
    EXIT,
    END,
};

map<PlanetClass, map<Stone, Anim>> stone_anims_specific;
map<Stone, Anim> stone_anims_generic;
bool stone_anims_initialised = false;

PlanetMap::PlanetMap() : ModeBase("PlanetMap") {
    stage = PM_Idle;
    blocks = 0;
    anim_cycle = 0;
    active_tool = TOOL_None;
    menu_x = 0;
    menu_y = 0;
}

void PlanetMap::enter() {
    ModeBase::enter(ID::END);
    planet = exostate.get_active_planet();

    if (!planet) {
        L.fatal("Entered PlanetMap mode with no active planet");
    }

    anim_cycle = 0;
    active_tool = TOOL_None;

    draw_manager.draw(planet->sprites()->map_bg);

    menu_x = RES_X - 150;
    menu_y = 36;

    draw_manager.fill(
        {menu_x, menu_y, 126, 447},
        COL_BORDERS);

    DrawArea area = {0, 0, 126, 196};
    draw_manager.set_source_region(id(ID::MENU_BG), &area);
    draw_manager.draw(
        id(ID::MENU_BG),
        planet->sprites()->surf,
        {menu_x, menu_y,
        0, 0, 1, 1});
    draw_manager.draw(
        id(ID::MENU),
        IMG_SU1_MENU,
        {menu_x, menu_y,
        0, 0, 1, 1});

    draw_manager.pattern_fill({menu_x + 4, menu_y + 198, 118, 24});
    draw_manager.draw(
        id(ID::ICON_FOOD),
        IMG_SU1_CTRL1,
        {menu_x + 6, menu_y + 200,
        0, 0, 1, 1});
    draw_manager.draw(
        id(ID::GAUGE_FOOD),
        IMG_SU1_CTA,
        {menu_x + 28, menu_y + 200,
        0, 0, 1, 1});

    draw_manager.draw(
        id(ID::ICON_PLU),
        IMG_SU1_CTRL2,
        {menu_x + 48, menu_y + 200,
        0, 0, 1, 1});
    draw_manager.draw(
        id(ID::GAUGE_PLU),
        IMG_SU1_CTB,
        {menu_x + 70, menu_y + 200,
        0, 0, 1, 1});

    draw_manager.draw(
        id(ID::ICON_UNREST),
        IMG_SU1_CTRL3,
        {menu_x + 90, menu_y + 200,
        0, 0, 1, 1});
    draw_manager.draw(
        id(ID::GAUGE_UNREST),
        IMG_SU1_CTC,
        {menu_x + 112, menu_y + 200,
        0, 0, 1, 1});

    draw_manager.pattern_fill({menu_x + 4, menu_y + 226, 118, 66});
    draw_manager.pattern_fill({menu_x + 4, menu_y + 296, 118, 26});
    draw_manager.pattern_fill({menu_x + 4, menu_y + 326, 118, 26});

    draw_manager.draw(
        id(ID::TRIBUTTONS),
        IMG_SU1_TRI,
        {menu_x + 4, menu_y + 383,
        0, 1, 1, 1});

    draw_manager.draw(
        id(ID::LAWBUILD),
        IMG_SU1_PEOPLE,
        {menu_x + 4, menu_y + 413,
        0, 1, 1, 1});

    draw_manager.draw(
        id(ID::EXIT),
        IMG_SU1_EXPORT,
        {menu_x + 4, menu_y + 443,
        0, 1, 1, 1});

    for (Tool t = (Tool)0; t < TOOL_END; t = (Tool)((int)t + 1)) {
        draw_tool_rect(t, COL_TOOL);
    }

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
            click = draw_manager.query_click(id(ID::MENU));
            if (click.id) {
                Tool t = get_tool_for_click(click.x, click.y);
                if (t != TOOL_None) {
                    set_tool(t);
                }
            } else if (draw_manager.clicked()) {
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

Tool PlanetMap::get_tool_for_click(float x, float y) {
    int row = (int)(y * 4.999f);
    int col = (int)(x * 2.999f);
    if (col == 0) {
        if (row == 0) return TOOL_HQ;
        if (row == 1) return TOOL_Plu;
        if (row == 2) return TOOL_Inf;
        if (row == 3) return TOOL_Port0;
        if (row == 4) return TOOL_Trade;
    } else if (col == 1) {
        if (row == 0) return TOOL_Cultivate;
        if (row == 1) return TOOL_City;
        if (row == 2) return TOOL_Gli;
        if (row == 3) return TOOL_Port1;
        if (row == 4) return TOOL_LunarBase;
    } else {
        if (row == 0) return TOOL_Mine;
        if (row == 1) return TOOL_Clear;
        if (row == 2) return TOOL_Art;
        if (row == 3) return TOOL_Port2;
        if (row == 4) return TOOL_Park;
    }
    return TOOL_None;
}

void PlanetMap::set_tool(Tool t) {
    L.debug("Setting tool: %d", t);
    draw_tool_rect(active_tool, COL_TOOL);
    active_tool = t;
    draw_tool_rect(active_tool, COL_TOOL_HL);
}

void PlanetMap::draw_tool_rect(Tool t, RGB col) {
    int x = 0;
    int y = 0;

    if (t == TOOL_None)
        return;

    switch(t) {
        case TOOL_HQ:
        case TOOL_Plu:
        case TOOL_Inf:
        case TOOL_Port0:
        case TOOL_Trade:
            x = 499;
            break;
        case TOOL_Cultivate:
        case TOOL_City:
        case TOOL_Gli:
        case TOOL_Port1:
        case TOOL_LunarBase:
            x = 539;
            break;
        case TOOL_Mine:
        case TOOL_Clear:
        case TOOL_Art:
        case TOOL_Port2:
        case TOOL_Park:
            x = 579;
            break;
        default:
            break;
    }

    switch(t) {
        case TOOL_HQ:
        case TOOL_Cultivate:
        case TOOL_Mine:
            y = 45;
            break;
        case TOOL_Plu:
        case TOOL_City:
        case TOOL_Clear:
            y = 83;
            break;
        case TOOL_Inf:
        case TOOL_Gli:
        case TOOL_Art:
            y = 121;
            break;
        case TOOL_Port0:
        case TOOL_Port1:
        case TOOL_Port2:
            y = 159;
            break;
        case TOOL_Trade:
        case TOOL_LunarBase:
        case TOOL_Park:
            y = 197;
            break;
        default:
            break;
    }

    draw_manager.draw_rect({x, y, 30, 30}, 2, col);
}
