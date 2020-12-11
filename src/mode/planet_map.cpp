#include "planet_map.h"

#include <map>
#include <cmath>

#include "galaxy/planet.h"

#include "assetpaths.h"

using std::map;

#define STONE_SZ 28
#define CONSTRUCT_RATE 6
#define DEFAULT_TOOL TOOL_HQ
#define DESTRUCT_DELAY 0.4
#define EXPLOSION_RATE 1.0
#define TARGETING_RATE 0.6

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
    DESC0,
    DESC1,
    COST,
    MC,
    ARMY,
    TRIBUTTONS,
    LAWBUILD,
    EXIT,
    TARGET,
    EXPLOSION,
    END,
};

map<PlanetClass, map<Stone, Anim>> stone_anims_specific;
map<Stone, Anim> stone_anims_generic;
map<PlanetClass, map<Stone, Anim>> construct_anims_specific;
map<Stone, Anim> construct_anims_generic;
bool stone_anims_initialised = false;

Anim anim_target {
    6,
    IMG_SU1_MARK,
    nullptr,
    IMG_SU1_MARK,
    nullptr,
    IMG_SU1_MARK,
    nullptr
};

static Anim anim_explode(
    11,
    IMG_SU1_DEAD1,
    IMG_SU1_DEAD2,
    IMG_SU1_DEAD3,
    IMG_SU1_DEAD4,
    IMG_SU1_DEAD5,
    IMG_SU1_DEAD6,
    IMG_SU1_DEAD7,
    IMG_SU1_DEAD8,
    IMG_SU1_DEAD9,
    IMG_SU1_DEAD10,
    nullptr
);

Stone get_destroyed_stone(Stone st) {
    if (   st == STONE_Agri
        || st == STONE_AgriDead
        || st == STONE_Clear
        || st == STONE_NaturalSmall
        || st == STONE_NaturalLarge
        || st == STONE_NaturalAnim) {
        return STONE_AgriDead;
    }
    if (   st == STONE_Plu
        || st == STONE_Port2
        || st == STONE_Radiation) {
        return STONE_Radiation;
    }
    return STONE_Rubble;
}

PlanetMap::PlanetMap() : ModeBase("PlanetMap") {
    stage = PM_Idle;
    draw_menu = true;
    do_animations = true;
    blocks = 0;
    anim_cycle = 0;
    active_tool = DEFAULT_TOOL;
    menu_x = 0;
    menu_y = 0;
    construct_progress = 0;
    construct_anim = nullptr;
    construct_x = 0;
    construct_y = 0;
    construct_stone = STONE_Clear;
    destruction_time = 0;
    targeting_interp = 0;
    explosion_interp = 0;
    destruction_done = false;
    exploding_stone = STONE_Clear;
    chained_explosion_idx = 0;
    chained_explosion_head = 0;
}

void PlanetMap::enter() {
    ModeBase::enter(ID::END);
    planet = exostate.get_active_planet();
    player = exostate.get_active_player();

    if (!planet) {
        L.fatal("Entered PlanetMap mode with no active planet");
    }

    anim_cycle = 0;

    menu_x = RES_X - 150;
    menu_y = 36;

    blocks = planet->get_size_blocks();

    surf_x = 24 + STONE_SZ * (16 - blocks) / 2;
    surf_y = 36 + STONE_SZ * (16 - blocks) / 2;

    construct_progress = 0;
    construct_anim = nullptr;
    construct_x = 0;
    construct_y = 0;
    construct_stone = STONE_Clear;

    destruction_time = 0;
    targeting_interp = 0;
    explosion_interp = 0;
    destruction_done = false;
    exploding_stone = STONE_Clear;

    if (ephstate.get_ephemeral_state() == EPH_Destruction) {
        draw_menu = false;
        do_animations = false;
        if (ephstate.destruction.draw) {
            draw();
            draw_stones();
            draw_manager.save_background();
        }
        stage = PM_Destruction;
    } else {
        draw_menu = true;
        do_animations = true;
        draw();
        stage = PM_Idle;
    }
}

void PlanetMap::draw() {
    draw_manager.draw(planet->sprites()->map_bg);

    char title[15 + PLANET_MAX_NAME];
    snprintf(title, 14 + PLANET_MAX_NAME, "The planet %s", planet->get_name());
    draw_manager.draw_text(
        title,
        Justify::Left,
        26, 8,
        COL_TEXT);

    DrawArea area;

    if (draw_menu) {
        draw_manager.fill(
            {menu_x, menu_y, 126, 447},
            COL_BORDERS);

        area = {0, 0, 126, 196};
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

        draw_manager.fill_pattern({menu_x + 4, menu_y + 198, 118, 24});
        draw_manager.draw(
            id(ID::ICON_FOOD),
            IMG_SU1_CTRL1,
            {menu_x + 6, menu_y + 200,
            0, 0, 1, 1});

        draw_manager.draw(
            id(ID::ICON_PLU),
            IMG_SU1_CTRL2,
            {menu_x + 48, menu_y + 200,
            0, 0, 1, 1});

        draw_manager.draw(
            id(ID::ICON_UNREST),
            IMG_SU1_CTRL3,
            {menu_x + 90, menu_y + 200,
            0, 0, 1, 1});

        update_gauges();

        draw_manager.fill_pattern({menu_x + 4, menu_y + 226, 118, 66});
        draw_manager.fill_pattern({menu_x + 4, menu_y + 296, 118, 26});
        draw_manager.fill_pattern({menu_x + 4, menu_y + 326, 118, 26});

        draw_manager.draw(
            id(ID::TRIBUTTONS),
            IMG_SU1_TRI,
            {menu_x + 4, menu_y + 383,
            0, 1, 1, 1});

        set_build_button(false);

        draw_manager.draw(
            id(ID::EXIT),
            IMG_SU1_EXPORT,
            {menu_x + 4, menu_y + 443,
            0, 1, 1, 1});

        draw_manager.draw(
            get_stone_anim(STONE_Agri)->frame(0),
            {541, 47,
            0, 0, 1, 1});

        if (planet->has_lunar_base())
            hide_lunar_base_tool();

        for (Tool t = (Tool)0; t < TOOL_END; t = (Tool)((int)t + 1)) {
            draw_tool_rect(t, COL_TOOL);
        }

    }

    int sz = blocks * STONE_SZ;
    area = {0, 0, sz, sz};
    draw_manager.set_source_region(id(ID::SURF), &area);

    draw_manager.fill({surf_x - 2, surf_y - 2, sz + 4, sz + 4}, {0, 0, 0});
    draw_manager.draw(
        id(ID::SURF),
        planet->sprites()->surf,
        {surf_x, surf_y,
         0, 0, 1, 1});

    draw_manager.save_background();

    draw_stones();

    draw_manager.show_cursor(true);

    if (draw_menu) {
        set_tool(DEFAULT_TOOL);
    }
}


ExodusMode PlanetMap::update(float delta) {
    SpriteClick click;

    if (do_animations) {
        anim_cycle = fmod(anim_cycle + delta * ANIM_RATE, 1);
    }

    switch(stage) {
        case PM_Idle:
            draw_stones();
            update_gauges();
            draw_mc();

            // FIXME: Ideally not called every update as it involves a surface iteration
            draw_army_funding();

            click = draw_manager.query_click(id(ID::MENU));
            if (click.id) {
                Tool t = get_tool_for_click(click.x, click.y);
                set_tool(t);
            }

            if (active_tool != TOOL_LunarBase) {
                click = draw_manager.query_click(id(ID::SURF));
                if (click.id) {
                    int block_x = (int)(0.9999f * click.x * blocks);
                    int block_y = (int)(0.9999f * click.y * blocks);
                    Stone existing = planet->get_stone(block_x, block_y);
                    if (active_tool == TOOL_Clear || (can_build_on(existing))) {
                        if (player->attempt_spend(tool2cost(active_tool))) {
                            clear_surf(block_x, block_y);
                            construct_stone = tool2stone(active_tool);
                            construct_anim = get_construct_anim(construct_stone);
                            if (construct_anim) {
                                construct_progress = 0;
                                construct_x = block_x;
                                construct_y = block_y;
                                stage = PM_Construct;
                                return ExodusMode::MODE_None;
                            } else {
                                planet->set_stone(block_x, block_y, construct_stone);
                            }
                        }
                    }
                }
            }

            click = draw_manager.query_click(id(ID::TRIBUTTONS));
            if (click.id) {
                if (click.x <= 0.33) {
                    planet->adjust_army_funding(-1);
                } else if (click.x <= 0.67) {
                    planet->adjust_army_funding(1);
                } else {
                    // TODO: Query
                    L.debug("Query clicked");
                }
            }

            if (draw_manager.query_click(id(ID::LAWBUILD)).id) {
                if (active_tool == TOOL_LunarBase) {
                    if (!planet->has_lunar_base()) {
                        if (player->attempt_spend(tool2cost(TOOL_LunarBase))) {
                            planet->build_lunar_base();
                            hide_lunar_base_tool();
                            set_build_button(false);
                        }
                    }
                } else {
                    // TODO: Law
                    L.debug("Law clicked");
                }
            }

            if (draw_manager.query_click(id(ID::EXIT)).id) {
                return ExodusMode::MODE_Pop;
            }
            break;
        case PM_Construct:
            draw_stones();

            if (!construct_anim) {
                // Should not happen
                L.fatal("Logical error during stone construction");
                stage = PM_Idle;
                return ExodusMode::MODE_None;
            }

            clear_surf(construct_x, construct_y);
            draw_manager.draw(
                construct_anim->interp(construct_progress),
                {surf_x + construct_x*STONE_SZ, surf_y + construct_y*STONE_SZ,
                 0, 0, 1, 1});

            if (construct_progress >= 1) {
                planet->set_stone(construct_x, construct_y, construct_stone);
                construct_anim = nullptr;
                construct_progress = 0;
                stage = PM_Idle;
                return ExodusMode::MODE_None;
            }

            construct_progress += delta * CONSTRUCT_RATE / (float)construct_anim->get_n_frames();
            if (construct_progress >= 1) {
                construct_progress = 1;
            }

            break;
        case PM_Destruction:
            return update_destruction(delta);
    }

    return ExodusMode::MODE_None;
}

/*
 * N.B. we use the term 'stone' from the original to mean 'any object
 * which can exist on the planet surface'.
 */
void PlanetMap::draw_stones() {
    for (int y = 0; y < blocks; ++y) {
        for (int x = 0; x < blocks; ++x) {
            Stone st = planet->get_stone(x, y);

            if (st == STONE_Clear)
                continue;

            Anim *anim = get_stone_anim(st);

            const char* sprite = IMG_SU1_STONE7;
            if (anim) sprite = anim->interp(anim_cycle);

            /*
             * Special case - there are 2 alternate sprites for Terran rocks.
             * In the original, these are randomised every time you view the planet!
             * Here, we keep them consistent by using the co-ordinates.
             */
            if (planet->get_class() == Terra && st == STONE_NaturalLarge) {
                unsigned char r = (unsigned char)(0xF0 + pow(3, x) + pow(7, y)) % 3;
                if (r == 1) {
                    sprite = IMG_SF6_STONE24;
                } else if (r == 2) {
                    sprite = IMG_SF6_STONE25;
                }
            }

            draw_manager.draw(
                sprite,
                {surf_x + x*STONE_SZ, surf_y + y*STONE_SZ,
                 0, 0, 1, 1});
        }
    }

    if (exploding == EXP_Drawing) {
        if (targeting_interp < 1) {
            draw_manager.draw(
                id(ID::TARGET),
                anim_target.interp(targeting_interp),
                {surf_x + explode_x*STONE_SZ, surf_y + explode_y*STONE_SZ,
                 0, 0, 1, 1});
        } else {
            draw_manager.draw(
                id(ID::EXPLOSION),
                anim_explode.interp(explosion_interp),
                {surf_x + explode_x*STONE_SZ + STONE_SZ/2,
                 surf_y + explode_y*STONE_SZ + STONE_SZ/2,
                 0.5, 0.5, 1, 1});
        }
    }
}

void PlanetMap::update_gauges() {
    draw_manager.draw(
        id(ID::GAUGE_FOOD),
        planet->food_prod_sufficient() ? IMG_SU1_CTB : IMG_SU1_CTA,
        {menu_x + 28, menu_y + 200,
        0, 0, 1, 1});
    draw_manager.draw(
        id(ID::GAUGE_PLU),
        planet->plu_prod_sufficient() ? IMG_SU1_CTB : IMG_SU1_CTA,
        {menu_x + 70, menu_y + 200,
        0, 0, 1, 1});

    const char* unrest_gauge = IMG_SU1_CTB;
    if (planet->get_unrest() >= 4) unrest_gauge = IMG_SU1_CTC;
    if (planet->get_unrest() >= 8) unrest_gauge = IMG_SU1_CTA;

    draw_manager.draw(
        id(ID::GAUGE_UNREST),
        unrest_gauge,
        {menu_x + 112, menu_y + 200,
        0, 0, 1, 1});
}

void PlanetMap::clear_surf(int x, int y) {
    int sz = planet->get_size_blocks();
    x = (x + sz) % sz;
    y = (y + sz) % sz;
    int x_off = x * STONE_SZ;
    int y_off = y * STONE_SZ;
    SprID id_surfclear = draw_manager.new_sprite_id();
    DrawArea area = {x_off, y_off, STONE_SZ, STONE_SZ};
    draw_manager.set_source_region(id_surfclear, &area);
    draw_manager.draw(
        id_surfclear,
        planet->sprites()->surf,
        {surf_x + x_off, surf_y + y_off,
         0, 0, 1, 1});
    draw_manager.set_source_region(id_surfclear, nullptr);
    draw_manager.release_sprite_id(id_surfclear);
}

void init_stone_anims() {
    stone_anims_specific[Forest][STONE_Agri]             = Anim(1,  IMG_SF1_STONE2);
    stone_anims_specific[Forest][STONE_AgriDead]         = Anim(1,  IMG_SF1_STONE10);
    stone_anims_specific[Forest][STONE_NaturalSmall]     = Anim(1,  IMG_SF1_STONE9);
    stone_anims_specific[Forest][STONE_NaturalLarge]     = Anim(1,  IMG_SF1_STONE8);
    stone_anims_specific[Forest][STONE_NaturalAnim]      = Anim(2,  IMG_SF1_STONE29,
                                                                    IMG_SF1_STONE29B);

    stone_anims_specific[Desert][STONE_Agri]             = Anim(1,  IMG_SF2_STONE2);
    stone_anims_specific[Desert][STONE_AgriDead]         = Anim(1,  IMG_SF2_STONE10);
    stone_anims_specific[Desert][STONE_NaturalSmall]     = Anim(1,  IMG_SF2_STONE9);
    stone_anims_specific[Desert][STONE_NaturalLarge]     = Anim(1,  IMG_SF2_STONE8);
    stone_anims_specific[Desert][STONE_NaturalAnim]      = Anim(2,  IMG_SF2_STONE29,
                                                                    IMG_SF2_STONE29B);

    stone_anims_specific[Volcano][STONE_Agri]            = Anim(1,  IMG_SF3_STONE2);
    stone_anims_specific[Volcano][STONE_AgriDead]        = Anim(1,  IMG_SF3_STONE10);
    stone_anims_specific[Volcano][STONE_NaturalSmall]    = Anim(1,  IMG_SF3_STONE9);
    stone_anims_specific[Volcano][STONE_NaturalLarge]    = Anim(1,  IMG_SF3_STONE8);
    stone_anims_specific[Volcano][STONE_NaturalAnim]     = Anim(2,  IMG_SF3_STONE29,
                                                                    IMG_SF3_STONE29B);

    stone_anims_specific[Rock][STONE_Agri]               = Anim(1,  IMG_SF4_STONE2);
    stone_anims_specific[Rock][STONE_AgriDead]           = Anim(1,  IMG_SF4_STONE10);
    stone_anims_specific[Rock][STONE_NaturalSmall]       = Anim(1,  IMG_SF4_STONE9);
    stone_anims_specific[Rock][STONE_NaturalLarge]       = Anim(1,  IMG_SF4_STONE8);
    stone_anims_specific[Rock][STONE_NaturalAnim]        = Anim(2,  IMG_SF4_STONE29,
                                                                    IMG_SF4_STONE29B);

    stone_anims_specific[Ice][STONE_Agri]                = Anim(1,  IMG_SF5_STONE2);
    stone_anims_specific[Ice][STONE_AgriDead]            = Anim(1,  IMG_SF5_STONE10);
    stone_anims_specific[Ice][STONE_NaturalSmall]        = Anim(1,  IMG_SF5_STONE9);
    stone_anims_specific[Ice][STONE_NaturalLarge]        = Anim(1,  IMG_SF5_STONE8);
    stone_anims_specific[Ice][STONE_NaturalAnim]         = Anim(2,  IMG_SF5_STONE29,
                                                                    IMG_SF5_STONE29B);

    stone_anims_specific[Terra][STONE_Agri]              = Anim(1,  IMG_SF6_STONE2);
    stone_anims_specific[Terra][STONE_AgriDead]          = Anim(1,  IMG_SF6_STONE10);
    stone_anims_specific[Terra][STONE_NaturalSmall]      = Anim(1,  IMG_SF6_STONE9);
    stone_anims_specific[Terra][STONE_NaturalLarge]      = Anim(1,  IMG_SF6_STONE8);
    stone_anims_specific[Terra][STONE_NaturalAnim]       = Anim(2,  IMG_SF6_STONE29,
                                                                    IMG_SF6_STONE29B);

    stone_anims_specific[Artificial][STONE_Agri]         = Anim(1,  IMG_SF8_STONE2);
    stone_anims_specific[Artificial][STONE_AgriDead]     = Anim(1,  IMG_SF8_STONE10);
    stone_anims_specific[Artificial][STONE_NaturalSmall] = Anim(1,  IMG_SF8_STONE9);
    stone_anims_specific[Artificial][STONE_NaturalLarge] = Anim(1,  IMG_SF8_STONE8);
    stone_anims_specific[Artificial][STONE_NaturalAnim]  = Anim(2,  IMG_SF8_STONE29,
                                                                    IMG_SF8_STONE29B);

    stone_anims_generic[STONE_Village]                   = Anim(1,  IMG_SU1_STONE22);
    stone_anims_generic[STONE_Base]                      = Anim(1,  IMG_SU1_STONE1);
    stone_anims_generic[STONE_Mine]                      = Anim(1,  IMG_SU1_STONE3);
    stone_anims_generic[STONE_Plu]                       = Anim(2,  IMG_SU1_STONE4,
                                                                    IMG_SU1_STONE4B);
    stone_anims_generic[STONE_City]                      = Anim(18, IMG_SU1_STONE5A1,
                                                                    IMG_SU1_STONE5A2,
                                                                    IMG_SU1_STONE5A3,
                                                                    IMG_SU1_STONE5A4,
                                                                    IMG_SU1_STONE5A5,
                                                                    IMG_SU1_STONE5A6,
                                                                    IMG_SU1_STONE5A7,
                                                                    IMG_SU1_STONE5A8,
                                                                    IMG_SU1_STONE5A9,
                                                                    IMG_SU1_STONE5A10,
                                                                    IMG_SU1_STONE5A11,
                                                                    IMG_SU1_STONE5A12,
                                                                    IMG_SU1_STONE5A13,
                                                                    IMG_SU1_STONE5A14,
                                                                    IMG_SU1_STONE5A15,
                                                                    IMG_SU1_STONE5A16,
                                                                    IMG_SU1_STONE5A17,
                                                                    IMG_SU1_STONE5A18);
    stone_anims_generic[STONE_Inf]                       = Anim(1,  IMG_SU1_STONE11);
    stone_anims_generic[STONE_Gli]                       = Anim(1,  IMG_SU1_STONE12);
    stone_anims_generic[STONE_Art]                       = Anim(1,  IMG_SU1_STONE13);
    stone_anims_generic[STONE_Port0]                     = Anim(1,  IMG_SU1_STONE14);
    stone_anims_generic[STONE_Port1]                     = Anim(1,  IMG_SU1_STONE15);
    stone_anims_generic[STONE_Port2]                     = Anim(2,  IMG_SU1_STONE16,
                                                                    IMG_SU1_STONE16B);
    stone_anims_generic[STONE_Trade]                     = Anim(1,  IMG_SU1_STONE17);
    stone_anims_generic[STONE_Park]                      = Anim(1,  IMG_SU1_STONE23);
    stone_anims_generic[STONE_Radiation]                 = Anim(1,  IMG_SU1_STONE6);
    stone_anims_generic[STONE_Rubble]                    = Anim(1,  IMG_SU1_STONE20);

    construct_anims_specific[Forest][STONE_Agri]         = Anim(1,  IMG_SF1_STONE2_0);
    construct_anims_specific[Desert][STONE_Agri]         = Anim(1,  IMG_SF2_STONE2_0);
    construct_anims_specific[Volcano][STONE_Agri]        = Anim(1,  IMG_SF3_STONE2_0);
    construct_anims_specific[Rock][STONE_Agri]           = Anim(1,  IMG_SF4_STONE2_0);
    construct_anims_specific[Ice][STONE_Agri]            = Anim(1,  IMG_SF5_STONE2_0);
    construct_anims_specific[Terra][STONE_Agri]          = Anim(1,  IMG_SF6_STONE2_0);
    construct_anims_specific[Artificial][STONE_Agri]     = Anim(1,  IMG_SF8_STONE2_0);

    construct_anims_generic[STONE_Base]                  = Anim(4,  IMG_SU1_STONE1_0,
                                                                    IMG_SU1_STONE1_1,
                                                                    IMG_SU1_STONE1_2,
                                                                    IMG_SU1_STONE1_3);
    construct_anims_generic[STONE_Mine]                  = Anim(3,  IMG_SU1_STONE3_0,
                                                                    IMG_SU1_STONE3_1,
                                                                    IMG_SU1_STONE3_2);
    construct_anims_generic[STONE_Plu]                   = Anim(4,  IMG_SU1_STONE4_0,
                                                                    IMG_SU1_STONE4_1,
                                                                    IMG_SU1_STONE4_2,
                                                                    IMG_SU1_STONE4_3);
    construct_anims_generic[STONE_City]                  = Anim(7,  IMG_SU1_STONE5_0,
                                                                    IMG_SU1_STONE5_1,
                                                                    IMG_SU1_STONE5_2,
                                                                    IMG_SU1_STONE5_3,
                                                                    IMG_SU1_STONE5_4,
                                                                    IMG_SU1_STONE5_5,
                                                                    IMG_SU1_STONE5);
    construct_anims_generic[STONE_Inf]                   = Anim(4,  IMG_SU1_STONE11_0,
                                                                    IMG_SU1_STONE11_1,
                                                                    IMG_SU1_STONE11_2,
                                                                    IMG_SU1_STONE11_3);
    construct_anims_generic[STONE_Gli]                   = Anim(4,  IMG_SU1_STONE12_0,
                                                                    IMG_SU1_STONE12_1,
                                                                    IMG_SU1_STONE12_2,
                                                                    IMG_SU1_STONE12_3);
    construct_anims_generic[STONE_Art]                   = Anim(4,  IMG_SU1_STONE13_0,
                                                                    IMG_SU1_STONE13_1,
                                                                    IMG_SU1_STONE13_2,
                                                                    IMG_SU1_STONE13_3);
    construct_anims_generic[STONE_Port0]                 = Anim(3,  IMG_SU1_STONE14_0,
                                                                    IMG_SU1_STONE14_1,
                                                                    IMG_SU1_STONE14_2);
    construct_anims_generic[STONE_Port1]                 = Anim(3,  IMG_SU1_STONE15_0,
                                                                    IMG_SU1_STONE15_1,
                                                                    IMG_SU1_STONE15_2);
    construct_anims_generic[STONE_Port2]                 = Anim(2,  IMG_SU1_STONE16_0,
                                                                    IMG_SU1_STONE16_1);
    construct_anims_generic[STONE_Trade]                 = Anim(2,  IMG_SU1_STONE17_0,
                                                                    IMG_SU1_STONE17_1);
    construct_anims_generic[STONE_Park]                  = Anim(4,  IMG_SU1_STONE23_0,
                                                                    IMG_SU1_STONE23_1,
                                                                    IMG_SU1_STONE23_2,
                                                                    IMG_SU1_STONE23_3);

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

Anim* PlanetMap::get_construct_anim(Stone stone) {
    if (!stone_anims_initialised)
        init_stone_anims();

    PlanetClass cls = planet->get_class();
    if (construct_anims_specific[cls].count(stone)) {
        return &construct_anims_specific[cls][stone];
    }

    if (construct_anims_generic.count(stone)) {
        return &construct_anims_generic[stone];
    }

    // Fine for stones not to have a construct anim
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
    return DEFAULT_TOOL;
}

void PlanetMap::set_tool(Tool t) {
    L.debug("Setting tool: %d", t);
    draw_tool_rect(active_tool, COL_TOOL);
    active_tool = t;
    draw_tool_rect(active_tool, COL_TOOL_HL);

    set_build_button(t == TOOL_LunarBase && !planet->has_lunar_base());

    const char *tool_desc0 = "";
    const char *tool_desc1 = "";
    char cost_str[12];

    switch(t) {
        case TOOL_HQ:
            tool_desc0 = "Command";
            tool_desc1 = "base";
            break;
        case TOOL_Cultivate:
            tool_desc0 = "Cultivate";
            break;
        case TOOL_Mine:
            tool_desc0 = "Mining";
            break;
        case TOOL_Plu:
            tool_desc0 = "Plutonium";
            tool_desc1 = "Production";
            break;
        case TOOL_City:
            tool_desc0 = "City";
            break;
        case TOOL_Clear:
            tool_desc0 = "Clear";
            tool_desc1 = "ground";
            break;
        case TOOL_Inf:
            tool_desc0 = "Infantry";
            tool_desc1 = "production";
            break;
        case TOOL_Gli:
            tool_desc0 = "Glider";
            tool_desc1 = "production";
            break;
        case TOOL_Art:
            tool_desc0 = "Artillery";
            tool_desc1 = "production";
            break;
        case TOOL_Port0:
            tool_desc0 = "Spaceport,";
            tool_desc1 = "Control";
            break;
        case TOOL_Port1:
            tool_desc0 = "Spaceport,";
            tool_desc1 = "Landing";
            break;
        case TOOL_Port2:
            tool_desc0 = "Power";
            tool_desc1 = "plant";
            break;
        case TOOL_Trade:
            tool_desc0 = "Trading";
            tool_desc1 = "centre";
            break;
        case TOOL_LunarBase:
            tool_desc0 = "Lunar";
            tool_desc1 = "battle base";
            break;
        case TOOL_Park:
            tool_desc0 = "Park";
            break;
        case TOOL_END:
            break;
    }

    snprintf(cost_str, 11, "Cost: %d", tool2cost(t));

    draw_manager.draw_text(
        id(ID::DESC0),
        tool_desc0,
        Justify::Left,
        menu_x + 8,
        menu_y + 228,
        COL_TEXT2
    );

    draw_manager.draw_text(
        id(ID::DESC1),
        tool_desc1,
        Justify::Left,
        menu_x + 8,
        menu_y + 248,
        COL_TEXT2
    );

    draw_manager.draw_text(
        id(ID::COST),
        cost_str,
        Justify::Left,
        menu_x + 8,
        menu_y + 268,
        COL_TEXT2
    );
}

void PlanetMap::draw_mc() {
    char mc_str[16];
    snprintf(mc_str, 16, "MC: %d", player->get_mc());
    draw_manager.draw_text(
        id(ID::MC),
        mc_str,
        Justify::Left,
        menu_x + 8,
        menu_y + 298,
        COL_TEXT
    );
}

void PlanetMap::draw_army_funding() {
    draw_manager.fill_pattern({menu_x + 4, menu_y + 326, 118, 26});
    char ar_str[21];
    snprintf(ar_str, 20, "Ar: %d/%d", planet->get_army_funding(), planet->get_income());
    RGB col = COL_TEXT;
    if (!planet->army_funding_sufficient()) {
        col = COL_TEXT_BAD;
    }
    draw_manager.draw_text(
        id(ID::ARMY),
        ar_str,
        Justify::Left,
        menu_x + 8,
        menu_y + 328,
        col
    );
}

void PlanetMap::draw_tool_rect(Tool t, RGB col) {
    int x = 0;
    int y = 0;

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

Stone PlanetMap::tool2stone(Tool t) {
    switch(t) {
        case TOOL_HQ:
            return STONE_Base;
        case TOOL_Cultivate:
            return STONE_Agri;
        case TOOL_Mine:
            return STONE_Mine;
        case TOOL_Plu:
            return STONE_Plu;
        case TOOL_City:
            return STONE_City;
        case TOOL_Clear:
            return STONE_Clear;
        case TOOL_Inf:
            return STONE_Inf;
        case TOOL_Gli:
            return STONE_Gli;
        case TOOL_Art:
            return STONE_Art;
        case TOOL_Port0:
            return STONE_Port0;
        case TOOL_Port1:
            return STONE_Port1;
        case TOOL_Port2:
            return STONE_Port2;
        case TOOL_Trade:
            return STONE_Trade;
        case TOOL_LunarBase:
            break;
        case TOOL_Park:
            return STONE_Park;
        case TOOL_END:
            break;
    }

    L.fatal("Invalid stone request for tool %d", t);
    return STONE_Clear;
}

int PlanetMap::tool2cost(Tool t) {
    switch(t) {
        case TOOL_HQ:
            return Planet::stone_cost(STONE_Base);
        case TOOL_Cultivate:
            return Planet::stone_cost(STONE_Agri);
        case TOOL_Mine:
            return Planet::stone_cost(STONE_Mine);
        case TOOL_Plu:
            return Planet::stone_cost(STONE_Plu);
        case TOOL_City:
            return Planet::stone_cost(STONE_City);
        case TOOL_Clear:
            return Planet::stone_cost(STONE_Clear);
        case TOOL_Inf:
            return Planet::stone_cost(STONE_Inf);
        case TOOL_Gli:
            return Planet::stone_cost(STONE_Gli);
        case TOOL_Art:
            return Planet::stone_cost(STONE_Art);
        case TOOL_Port0:
            return Planet::stone_cost(STONE_Port0);
        case TOOL_Port1:
            return Planet::stone_cost(STONE_Port1);
        case TOOL_Port2:
            return Planet::stone_cost(STONE_Port2);
        case TOOL_Trade:
            return Planet::stone_cost(STONE_Trade);
        case TOOL_LunarBase:
            return COST_LUNAR_BASE;
        case TOOL_Park:
            return Planet::stone_cost(STONE_Park);
        case TOOL_END:
            break;
    }

    L.fatal("Invalid cost request for tool %d", t);
    return 0;
}

bool PlanetMap::can_build_on(Stone st) {
    return (st == STONE_Clear || st == STONE_NaturalSmall);
}

void PlanetMap::set_build_button(bool on) {
    draw_manager.draw(
        id(ID::LAWBUILD),
        on ? IMG_SU1_BUILD : IMG_SU1_PEOPLE,
        {menu_x + 4, menu_y + 413,
        0, 1, 1, 1});
}

void PlanetMap::hide_lunar_base_tool() {
    draw_manager.draw(
        IMG_SU1_STONEXX,
        {541, 199,
        0, 0, 1, 1});
}

ExodusMode PlanetMap::update_destruction(float delta) {
    PlanetDestruction &d = ephstate.destruction;

    destruction_time += delta;

    if (d.draw) {
        draw_stones();
        if (explosion_interp >= 1) {
            exploding = EXP_Done;
            explosion_interp = 0;
        }
        if (exploding == EXP_Drawing) {
            if (targeting_interp < 1) {
                targeting_interp += delta * TARGETING_RATE;
                if (targeting_interp >= 1) {
                    targeting_interp = 1;
                }
            } else {
                explosion_interp += delta * EXPLOSION_RATE;
                if (explosion_interp >= 1) {
                    explosion_interp = 1;
                }
            }
        }
        if (destruction_time < DESTRUCT_DELAY || exploding == EXP_Drawing) {
            return ExodusMode::MODE_None;
        }
    }

    while (d.n_strikes >= 0) {
        // If we have chained explosions to apply, do these first!
        if (chained_explosion_idx < chained_explosion_head) {
            if (exploding == EXP_None) {
                ChainedExplosion &c = chained_explosions[chained_explosion_idx++];
                exploding_stone = c.radiation ? STONE_Plu : STONE_Port2;
                explode_x = c.x;
                explode_y = c.y;
                if (d.draw) {
                    targeting_interp = 1;
                    exploding = EXP_Drawing;
                    break;
                } else {
                    exploding = EXP_Done;
                }
            }
        } else if (d.n_strikes > 0 ) {
            if (exploding == EXP_None) {
                switch (d.type) {
                    case DESTROY_NRandom:
                        exploding_stone = planet->get_random_point(explode_x, explode_y);
                        break;
                    case DESTROY_NStones:
                        exploding_stone = d.tgt_stone;
                        if (!planet->find_random_stone(d.tgt_stone, explode_x, explode_y)) {
                            L.info("Run out of bombing targets for stone %d", d.tgt_stone);
                            destruction_done = true;
                        }
                        break;
                }

                if (destruction_done)
                    break;

                --d.n_strikes;

                if (d.draw) {
                    targeting_interp = d.show_target ? 0 : 1;
                    exploding = EXP_Drawing;
                    break;
                } else {
                    exploding = EXP_Done;
                }
            }
        }

        if (exploding == EXP_Done) {
            if (d.draw) {
                clear_surf(explode_x, explode_y);
            }
            planet->set_stone_wrap(
                explode_x, explode_y,
                d.irradiated ? STONE_Radiation : get_destroyed_stone(exploding_stone));

            if (d.enable_explosions) {
                if (exploding_stone == STONE_Plu || exploding_stone == STONE_Port2) {
                    for (int j = -1; j <= 1; ++j) {
                        for (int i = -1; i <= 1; ++i) {
                            if (i == 0 && j == 0) {
                                continue;
                            }
                            int cx = explode_x + i;
                            int cy = explode_y + j;
                            Stone s = planet->get_stone_wrap(cx, cy);
                            if (s == STONE_Plu || s == STONE_Port2) {
                                // TODO: Ensure we don't overflow
                                // (We won't at 256 - but that's not the point and
                                // should possibly have a lower cap!)
                                ChainedExplosion &c =
                                    chained_explosions[chained_explosion_head++];
                                c.x = cx;
                                c.y = cy;
                                c.radiation = s == STONE_Plu;
                            }

                            Stone to_stone = STONE_Radiation;
                            if (exploding_stone == STONE_Port2)
                                to_stone = get_destroyed_stone(s);
                            if (d.draw) {
                                clear_surf(cx, cy);
                            }
                            planet->set_stone_wrap(cx, cy, to_stone);
                        }
                    }
                }
            }

            exploding = EXP_None;
        } else {
            break;
        }
    }

    if (d.n_strikes <= 0) {
        destruction_done = true;
    }

    if (destruction_done && (!d.draw || draw_manager.clicked())) {
        L.debug("Destruction complete");
        ephstate.clear_ephemeral_state();
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
