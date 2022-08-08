#include "planet_map.h"

#include <map>
#include <cmath>

#include "galaxy/planet.h"
#include "util/value.h"

#include "assetpaths.h"
#include "exodus_features.h"

using std::map;

#define STONE_SZ 28
#define CONSTRUCT_RATE 6
#define DEFAULT_TOOL TOOL_HQ
#define DESTRUCT_DELAY 0.4
#define EXPLOSION_RATE 1.0
#define TARGETING_RATE 0.6

const int LAW_W = 300;
const int LAW_H = 240;
const int LAW_X = RES_X/2 - LAW_W/2;
const int LAW_Y = RES_Y/2 - LAW_H/2;

const float ANIM_RATE = 0.5f;

static const int TOOL_DESC_LINES = 10;

static const char* tool_desc_hq[] = {
    "The command station",
    "This is the central institution of a",
    "planet. From here, the planet is",
    "ruled and guarded. Without such a",
    "command base, the planet will lose",
    "its owner.",
    "",
    "To survive, the command base units need",
    "one cultivated area.",
    "",
};

static const char* tool_desc_agri[] = {
    "Cultivated areas",
    "These areas are huge farms that produce",
    "food for the command stations and the",
    "cities.",
    "",
    "1 unit is needed for the base",
    "3 units are needed for each city",
    "",
    "The food that is non needed is going to",
    "be stored in the planet's depots.",
};

static const char* tool_desc_mine[] = {
    "Mining",
    "The mining units haul minerals that",
    "are then stored in the planet's depots.",
    "To work, each mining unit needs to",
    "consume one plutonium unit per month.",
    "",
    "If the player's raw materials have been",
    "exploited, the mining units will not be able",
    "to haul any more minerals.",
    "",
};

static const char* tool_desc_plu[] = {
    "Plutonium units",
    "These large reactors produce energy",
    "for army production and mining units.",
    "One plutonium unit is needed for",
    "each army production factory and for",
    "each mining unit.",
    "",
    "If a plutonium unit explodes for",
    "any reason, it will destroy its",
    "environment. Beware of chain reactions.",
};

static const char* tool_desc_city[] = {
    "Cities",
    "Some million citizens can live in one",
    "of these gigantic cities.",
    "A city needs to consume 3 food units each",
    "month to survive. Each month, a city pays",
    "2 Mega Credits to the government.",
    "Without cities, no science is possible.",
    "If the population of a city gets too big,",
    "the city will expand automatically if",
    "the environment allows that.",
};

static const char* tool_desc_clear[] = {
    "Clear ground",
    "This clears a land unit of the planet.",
    "Everything previously located there",
    "will be destroyed.",
    "Citizens do not like to be resettled in",
    "other cities, so clearing their cities",
    "will make them angry.",
    "",
    "If an alien village is cleared, its",
    "inhabitants will not survive.",
};

static const char* tool_desc_inf[] = {
    "Infantry production",
    "This army production unit produces",
    "one infantry unit per month.",
    "",
    "It needs one plutonium unit",
    "and one Army Credit (Ar) to work.",
    "",
    "",
    "",
    "",
};

static const char* tool_desc_gli[] = {
    "Glider production",
    "This army production unit produces",
    "one glider unit per month.",
    "",
    "It needs one plutonium unit",
    "and two Army Credits (Ar) to work.",
    "",
    "",
    "",
    "",
};

static const char* tool_desc_art[] = {
    "Artillery production",
    "This army production unit produces",
    "one artillery unit per month.",
    "",
    "It needs one plutonium unit",
    "and three Army Credits (Ar) to work.",
    "",
    "",
    "",
    "",
};

static const char* tool_desc_port0[] = {
    "Spaceport: Control",
    "A spaceport consists of a control",
    "unit, a landing platform unit and",
    "a power plant.",
    "",
    "The spaceport makes it possible to",
    "transfer goods between the space",
    "fleet and the planet.",
    "",
    "",
};

static const char* tool_desc_port1[] = {
    "Spaceport: Landing",
    "A spaceport consists of a control",
    "unit, a landing platform unit and",
    "a power plant.",
    "",
    "The spaceport makes it possible to",
    "transfer goods between the space",
    "fleet and the planet.",
    "",
    "",
};

static const char* tool_desc_port2[] = {
    "Spaceport: Power plant",
    "A spaceport consists of a control",
    "unit, a landing platform unit and",
    "a power plant.",
    "",
    "The spaceport makes it possible to",
    "transfer goods between the space",
    "fleet and the planet.",
    "",
    "",
};

static const char* tool_desc_trade[] = {
    "Trading Centre",
    "Private space traders dock at the",
    "trading centre to buy the food and the",
    "minerals that are stored in the",
    "planet's depots.",
    "",
    "Each month, a trading centre sells a",
    "certain amount of the planet's stock.",
    "",
    "",
};

static const char* tool_desc_lunar_base[] = {
    "Lunar battle base",
    "The battle base is built on the moon",
    "and acts as an effective artillery",
    "unit with four towers. Its fire range",
    "is not limited.",
    "",
    // TODO: Does the original behave like this? If so needs implementing - otherwise needs correcting!
    "If the control unit of the base is not",
    "destroyed during battle, the base is",
    "able to recreate its destroyed units",
    "after battle.",
};

static const char* tool_desc_park[] = {
    "Parks",
    "Parks should be built near cities. If",
    "people have access to such a huge park,",
    "they feel happier.",
    "",
    "A park offers sufficient space for",
    "inhabitants of at least 3 cities.",
    "",
    "",
    "",
};

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
    FRAME,
    FRAME_BG,
    LAW_PANEL,
    LAW_PATTERN,
    LAW_JUSTICE,
    LAW_TRADE,
    LAW_TAXES,
    LAW_FESTIVAL,
    LAW_FREESPEECH,
    LAW_PRIVATEINDUSTRY,
    LAW_DRUGS,
    LAW_SYSENEMIES,
    LAW_RELIGION,
    LAW_CIVARMS,
    LAW_SELLMIN,
    LAW_SELLFD,
    LAW_SELLPLU,
    LAW_TAXYESNO,
    LAW_FESTSM,
    LAW_FESTLG,
    LAW_EXIT,
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

    for (int i = 0; i < MAX_STONES; ++i) {
        id_stones[i] = draw_manager.new_sprite_id();
    }

    bool play_music = true;
    const char* mus = mpart2mus(13);
    int s = planet->get_unrest();
    if (s >= 4) mus = mpart2mus(20);
    if (s >= 8) mus = mpart2mus(21);

    // MODES:
    // 1 = normal
    // 2 = scout / planning bomb attack
    // 3 = bombing

    if (ephstate.get_ephemeral_state() == EPH_ProbePlanet
     || ephstate.get_ephemeral_state() == EPH_ScoutPlanet) {
        mus = mpart2mus(4);
        draw_menu = false;
        do_animations = false;
        draw();
        draw_stones();
        draw_class_and_population();
        stage = PM_Scout;
    } else if (ephstate.get_ephemeral_state() == EPH_Destruction) {
        PlanetDestruction &d = ephstate.destruction;

        mus = mpart2mus(8);
        draw_menu = false;
        do_animations = false;
        if (ephstate.destruction.draw) {
            draw();
            draw_stones();
            // TODO: It'd be fun to keep redrawing population to watch it tick down with cities destroyed... >:D
            draw_class_and_population();

            if (d.destroyer_idx >= 0 && !d.nuke) {
                int y = 240;
                draw_manager.draw_text(
                    "Aerospace",
                    Justify::Left,
                    500, y,
                    COL_TEXT);
                draw_manager.draw_text(
                    "Bomber",
                    Justify::Left,
                    500, y+20,
                    COL_TEXT);
                draw_manager.draw_text(
                    "attack",
                    Justify::Left,
                    500, y+40,
                    COL_TEXT);
            }
        } else {
            play_music = false;
        }

        if (planet->is_owned() && d.destroyer_idx >= 0) {
            int owner_idx = planet->get_owner();
            exostate.unset_alliances(planet->get_owner(), d.destroyer_idx);
            Player *owner = exostate.get_player(owner_idx);
            if (!owner->is_human() && (owner->get_flag(0) != AI_Lo)) {
                owner->set_hostile_to(d.destroyer_idx);
            }
        }

        if ((d.type == DESTROY_NStones) && d.tgt_stones.has(STONE_City)) {
            planet->adjust_unrest(2);
        }

        stage = PM_Destruction;
    } else {
        draw_menu = true;
        do_animations = true;
        draw();
        stage = PM_Idle;
    }

    // TODO: Music for scouts / bomb planning (mode 2) - mpart 4
    if (play_music) {
        audio_manager.target_music(mus);
    }

    for (ID i = LAW_JUSTICE; i <= LAW_EXIT; i = (ID)((int)i+1)) {
        draw_manager.set_selectable(id(i));
    }
}

void PlanetMap::exit() {
    for (int i = 0; i < MAX_STONES; ++i) {
        draw_manager.release_sprite_id(id_stones[i]);
    }
    for (ID i = LAW_JUSTICE; i <= LAW_EXIT; i = (ID)((int)i+1)) {
        draw_manager.unset_selectable(id(i));
    }

    ModeBase::exit();
}

void PlanetMap::draw() {
    draw_manager.draw(planet->sprites()->map_bg);

    char title[15 + PLANET_MAX_NAME];

    if (planet->is_named()) {
        snprintf(title, sizeof(title), "The planet %s", planet->get_name());
    } else {
        snprintf(title, sizeof(title), "Unexplored planet");
    }

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

    draw_manager.save_background();

    draw_manager.draw(
        id(ID::SURF),
        planet->sprites()->surf,
        {surf_x, surf_y,
         0, 0, 1, 1});

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
                    bool ok = can_build_on(existing);
                    if (active_tool == TOOL_Clear) {
                        // Can clear anything except radiation
                        ok = existing != STONE_Radiation;
                    }
                    if (ok) {
                        if (player->attempt_spend(tool2cost(active_tool))) {
                            if (active_tool == TOOL_Clear) {
                                // Clearing cities increases unrest
                                if (existing == STONE_City) {
                                    planet->adjust_unrest(3);
                                    // TODO: trace%
                                }
                                // Clearing villages increases unrest
                                if (existing == STONE_Village) {
                                    planet->adjust_unrest(2);
                                    // TODO: trace%
                                }
                            }

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
                    draw_frame_help(active_tool);
                    stage = PM_Frame;
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
                    open_law_panel();
                    return ExodusMode::MODE_None;
                }
            }

            if (draw_manager.query_click(id(ID::EXIT)).id) {
                // 'lookagain' in orig

                if (planet->laws_cause_unrest() && onein(3)) {
                    ephstate.set_ephemeral_state(EPH_PostPlanet);
                    ephstate.set_postplanet(PPA_BadLaws);
                }

                /*
                 * It's weird that having a city precludes this warning, but this
                 * is how the original works. Maybe if the player has a city then
                 * they should be competent enough to realise this for themselves!
                 */
                if (!planet->has_stone(STONE_Base)
                    || (!planet->has_stone(STONE_Agri) && !planet->has_stone(STONE_City))) {
                    ephstate.set_ephemeral_state(EPH_PostPlanet);
                    ephstate.set_postplanet(PPA_NoEssentials);
                }

                if (ephstate.get_ephemeral_state() == EPH_PostPlanet) {
                    return ephstate.get_appropriate_mode();
                }
                return ExodusMode::MODE_Pop;
            }

            if (draw_manager.query_click(id(ID::ICON_FOOD)).id) {
                draw_frame_fd();
                stage = PM_Frame;
            } else if (draw_manager.query_click(id(ID::ICON_PLU)).id) {
                draw_frame_pl();
                stage = PM_Frame;
            } else if (draw_manager.query_click(id(ID::ICON_UNREST)).id) {
                draw_frame_unrest();
                stage = PM_Frame;
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
        case PM_Scout:
            {
                if (draw_manager.clicked()) {
                    // We need to remember that we're in the middle of a probe
                    if (ephstate.get_ephemeral_state() != EPH_ProbePlanet) {
                        ephstate.clear_ephemeral_state();
                    }
                    return ExodusMode::MODE_Pop;
                }
            }
            break;
        case PM_Frame:
            if (draw_manager.clicked()) {
                close_frame();
                set_tool(active_tool);
                stage = PM_Idle;
            }
            break;
        case PM_Law:
            {
                bool can_tax = planet->can_collect_taxes();
                RGB tax_col = COL_TEXT;
                if (can_tax) {
                    draw_manager.set_selectable(id(ID::LAW_TAXES));
                } else {
                    draw_manager.unset_selectable(id(ID::LAW_TAXES));
                    tax_col = COL_TEXT_GREYED;
                }

                draw_manager.draw_text(
                    id(ID::LAW_JUSTICE),
                    "Justice",
                    Justify::Left,
                    LAW_X + 20, LAW_Y+44,
                    COL_TEXT);
                draw_manager.draw_text(
                    id(ID::LAW_TRADE),
                    "Trading Centre Rules",
                    Justify::Left,
                    LAW_X + 20, LAW_Y+84,
                    COL_TEXT);
                draw_manager.draw_text(
                    id(ID::LAW_TAXES),
                    "Additional Taxes",
                    Justify::Left,
                    LAW_X + 20, LAW_Y+124,
                    tax_col);

                RGB fest_col = COL_TEXT;

                bool festival_ok = true;

#if FEATURE_FESTIVAL_LIMIT
                if (planet->festival_happened_this_month()) {
                    festival_ok = false;
                }
#endif

#if FEATURE_FESTIVAL_REQUIRES_CITY
                if (!planet->has_stone(STONE_City)) {
                    festival_ok = false;
                }
#endif

                if (festival_ok) {
                    draw_manager.set_selectable(id(ID::LAW_FESTIVAL));
                } else {
                    fest_col = COL_TEXT_GREYED;
                    draw_manager.unset_selectable(id(ID::LAW_FESTIVAL));
                }

                draw_manager.draw_text(
                    id(ID::LAW_FESTIVAL),
                    "Public Festival",
                    Justify::Left,
                    LAW_X + 20, LAW_Y+164,
                    fest_col);

                draw_manager.draw_text(
                    id(ID::LAW_EXIT),
                    "Exit",
                    Justify::Left,
                    LAW_X + 20, LAW_Y+204,
                    COL_TEXT);

                if (draw_manager.query_click(id(ID::LAW_JUSTICE)).id) {
                    clear_law_ids();
                    draw_law_panel();
                    draw_manager.draw_text(
                        Font::Large,
                        "The Law",
                        Justify::Centre,
                        RES_X/2, LAW_Y+4,
                        COL_TEXT2);
                    stage = PM_LawJustice;
                    return ExodusMode::MODE_Redo;
                }

                if (draw_manager.query_click(id(ID::LAW_TRADE)).id) {
                    clear_law_ids();
                    draw_law_panel();
                    draw_manager.draw_text(
                        "TRADING CENTRE RULES",
                        Justify::Centre,
                        RES_X/2, LAW_Y+4,
                        COL_TEXT2);
                    draw_manager.draw_text(
                        "Which goods do you allow",
                        Justify::Left,
                        LAW_X+4, LAW_Y+44,
                        COL_TEXT);
                    draw_manager.draw_text(
                        "to be sold by trading",
                        Justify::Left,
                        LAW_X+4, LAW_Y+64,
                        COL_TEXT);
                    draw_manager.draw_text(
                        "centres?",
                        Justify::Left,
                        LAW_X+4, LAW_Y+84,
                        COL_TEXT);
                    stage = PM_LawTrade;
                    return ExodusMode::MODE_Redo;
                }

                if (can_tax && draw_manager.query_click(id(ID::LAW_TAXES)).id) {
                    clear_law_ids();
                    draw_law_panel();
                    draw_manager.draw_text(
                        "ADDITIONAL TAXES",
                        Justify::Centre,
                        RES_X/2, LAW_Y+4,
                        COL_TEXT2);
                    char text[32];
                    snprintf(text, sizeof(text), "Amount: %dMC", planet->get_tax_amount());
                    draw_manager.draw_text(
                        text,
                        Justify::Left,
                        LAW_X+20, LAW_Y+44,
                        COL_TEXT);
                    draw_manager.draw_text(
                        "Shall we really collect",
                        Justify::Left,
                        LAW_X+20, LAW_Y+84,
                        COL_TEXT);
                    draw_manager.draw_text(
                        "additional taxes?",
                        Justify::Left,
                        LAW_X+20, LAW_Y+104,
                        COL_TEXT);
                    draw_manager.fill({LAW_X+2, LAW_Y+LAW_H-28, 28, 26}, COL_BORDERS);
                    draw_manager.fill({LAW_X+LAW_W-30, LAW_Y+LAW_H-28, 28, 26}, COL_BORDERS);
                    draw_manager.draw(
                        id(ID::LAW_TAXYESNO),
                        IMG_BR14_EXPORT,
                        {RES_X/2, LAW_Y+LAW_H-2,
                         .5f, 1, 1, 1});
                    stage = PM_LawTaxes;
                    return ExodusMode::MODE_Redo;
                }

                if (festival_ok && draw_manager.query_click(id(ID::LAW_FESTIVAL)).id) {
                    clear_law_ids();
                    draw_law_panel();
                    draw_manager.draw_text(
                        "PUBLIC FESTIVAL",
                        Justify::Centre,
                        RES_X/2, LAW_Y+4,
                        COL_TEXT2);
                    draw_manager.draw_text(
                        "How shall the festival",
                        Justify::Left,
                        LAW_X+4, LAW_Y+44,
                        COL_TEXT);
                    draw_manager.draw_text(
                        "be planned?",
                        Justify::Left,
                        LAW_X+4, LAW_Y+64,
                        COL_TEXT);
                    stage = PM_LawFestival;
                    return ExodusMode::MODE_Redo;
                }

                if (draw_manager.query_click(id(ID::LAW_EXIT)).id) {
                    close_law_panel();
                }
            }
            break;
        case PM_LawJustice:
            {
                int y = LAW_Y+44;
                int ysp = 26;
                // TODO: Use weird symbol instead of *
                draw_manager.draw_text(
                    id(ID::LAW_FREESPEECH),
                    "*1 Freedom of Speech",
                    Justify::Left,
                    LAW_X+20, y,
                    COL_TEXT);
                draw_manager.draw(
                    planet->has_law(LAW_FreeSpeech)?IMG_SU1_ALLOW1:IMG_SU1_ALLOW0,
                    {LAW_X+LAW_W-20, y+1,
                     1, 0, 1, 1});
                y += ysp;
                draw_manager.draw_text(
                    id(ID::LAW_PRIVATEINDUSTRY),
                    "*2 Private Industry",
                    Justify::Left,
                    LAW_X+20, y,
                    COL_TEXT);
                draw_manager.draw(
                    planet->has_law(LAW_PrivateIndustry)?IMG_SU1_ALLOW1:IMG_SU1_ALLOW0,
                    {LAW_X+LAW_W-20, y+1,
                     1, 0, 1, 1});
                y += ysp;
                draw_manager.draw_text(
                    id(ID::LAW_DRUGS),
                    "*3 Drug Legalisation",
                    Justify::Left,
                    LAW_X+20, y,
                    COL_TEXT);
                draw_manager.draw(
                    planet->has_law(LAW_DrugLegalisation)?IMG_SU1_ALLOW1:IMG_SU1_ALLOW0,
                    {LAW_X+LAW_W-20, y+1,
                     1, 0, 1, 1});
                y += ysp;
                draw_manager.draw_text(
                    id(ID::LAW_SYSENEMIES),
                    "*4 Allow System Enemies",
                    Justify::Left,
                    LAW_X+20, y,
                    COL_TEXT);
                draw_manager.draw(
                    planet->has_law(LAW_AllowSystemEnemies)?IMG_SU1_ALLOW1:IMG_SU1_ALLOW0,
                    {LAW_X+LAW_W-20, y+1,
                     1, 0, 1, 1});
                y += ysp;
                draw_manager.draw_text(
                    id(ID::LAW_RELIGION),
                    "*5 Different Religions",
                    Justify::Left,
                    LAW_X+20, y,
                    COL_TEXT);
                draw_manager.draw(
                    planet->has_law(LAW_DifferentReligions)?IMG_SU1_ALLOW1:IMG_SU1_ALLOW0,
                    {LAW_X+LAW_W-20, y+1,
                     1, 0, 1, 1});
                y += ysp;
                draw_manager.draw_text(
                    id(ID::LAW_CIVARMS),
                    "*6 Weapons for Civilians",
                    Justify::Left,
                    LAW_X+20, y,
                    COL_TEXT);
                draw_manager.draw(
                    planet->has_law(LAW_CivilianWeapons)?IMG_SU1_ALLOW1:IMG_SU1_ALLOW0,
                    {LAW_X+LAW_W-20, y+1,
                     1, 0, 1, 1});

                if (draw_manager.query_click(id(ID::LAW_FREESPEECH)).id) {
                    planet->toggle_law(LAW_FreeSpeech);
                }
                if (draw_manager.query_click(id(ID::LAW_PRIVATEINDUSTRY)).id) {
                    planet->toggle_law(LAW_PrivateIndustry);
                }
                if (draw_manager.query_click(id(ID::LAW_DRUGS)).id) {
                    planet->toggle_law(LAW_DrugLegalisation);
                }
                if (draw_manager.query_click(id(ID::LAW_SYSENEMIES)).id) {
                    planet->toggle_law(LAW_AllowSystemEnemies);
                }
                if (draw_manager.query_click(id(ID::LAW_RELIGION)).id) {
                    planet->toggle_law(LAW_DifferentReligions);
                }
                if (draw_manager.query_click(id(ID::LAW_CIVARMS)).id) {
                    planet->toggle_law(LAW_CivilianWeapons);
                }

                draw_manager.draw_text(
                    id(ID::LAW_EXIT),
                    "Exit",
                    Justify::Left,
                    LAW_X+20, LAW_Y+204,
                    COL_TEXT);

                if (draw_manager.query_click(id(ID::LAW_EXIT)).id) {
                    clear_law_ids();
                    open_law_panel();
                    stage = PM_Law;
                    return ExodusMode::MODE_Redo;
                }
            }
            break;
        case PM_LawTrade:
            {
                int y = LAW_Y + 124;
                draw_manager.draw_text(
                    id(ID::LAW_SELLMIN),
                    "Sell Minerals",
                    Justify::Left,
                    LAW_X+20, y,
                    COL_TEXT);
                draw_manager.draw(
                    planet->has_law(LAW_TradeMinerals)?IMG_SU1_ALLOW1:IMG_SU1_ALLOW0,
                    {LAW_X+LAW_W-20, y+1,
                     1, 0, 1, 1});
                y += 20;
                draw_manager.draw_text(
                    id(ID::LAW_SELLFD),
                    "Sell Food",
                    Justify::Left,
                    LAW_X+20, y,
                    COL_TEXT);
                draw_manager.draw(
                    planet->has_law(LAW_TradeFood)?IMG_SU1_ALLOW1:IMG_SU1_ALLOW0,
                    {LAW_X+LAW_W-20, y+1,
                     1, 0, 1, 1});
                y += 20;
                draw_manager.draw_text(
                    id(ID::LAW_SELLPLU),
                    "Sell Plutonium (illegal)",
                    Justify::Left,
                    LAW_X+20, y,
                    COL_TEXT);
                draw_manager.draw(
                    planet->has_law(LAW_TradePlutonium)?IMG_SU1_ALLOW1:IMG_SU1_ALLOW0,
                    {LAW_X+LAW_W-20, y+1,
                     1, 0, 1, 1});

                if (draw_manager.query_click(id(ID::LAW_SELLMIN)).id) {
                    planet->toggle_law(LAW_TradeMinerals);
                }
                if (draw_manager.query_click(id(ID::LAW_SELLFD)).id) {
                    planet->toggle_law(LAW_TradeFood);
                }
                if (draw_manager.query_click(id(ID::LAW_SELLPLU)).id) {
                    planet->toggle_law(LAW_TradePlutonium);
                }

                draw_manager.draw_text(
                    id(ID::LAW_EXIT),
                    "Exit",
                    Justify::Left,
                    LAW_X+20, LAW_Y+204,
                    COL_TEXT);

                if (draw_manager.query_click(id(ID::LAW_EXIT)).id) {
                    clear_law_ids();
                    open_law_panel();
                    stage = PM_Law;
                    return ExodusMode::MODE_Redo;
                }
            }
            break;
        case PM_LawTaxes:
            {
                SpriteClick clk = draw_manager.query_click(id(ID::LAW_TAXYESNO));
                if (clk.id) {
                    if (clk.x < .5f) {
                        planet->collect_taxes();
                    }
                    close_law_panel();
                    stage = PM_Idle;
                }
            }
            break;
        case PM_LawFestival:
            {
                RGB col_sm = COL_TEXT;
                RGB col_lg = COL_TEXT;

                draw_manager.set_selectable(id(ID::LAW_FESTSM));
                draw_manager.set_selectable(id(ID::LAW_FESTLG));

                if (!player->can_afford(20)) {
                    col_sm = COL_TEXT_GREYED;
                    col_lg = COL_TEXT_GREYED;
                    draw_manager.unset_selectable(id(ID::LAW_FESTSM));
                    draw_manager.unset_selectable(id(ID::LAW_FESTLG));
                } else if (!player->can_afford(100)) {
                    col_lg = COL_TEXT_GREYED;
                    draw_manager.unset_selectable(id(ID::LAW_FESTLG));
                }

                draw_manager.draw_text(
                    id(ID::LAW_FESTSM),
                    "Small festival (20 MC)",
                    Justify::Left,
                    LAW_X+20, LAW_Y+100,
                    col_sm);
                draw_manager.draw_text(
                    id(ID::LAW_FESTLG),
                    "Big festival (100 MC)",
                    Justify::Left,
                    LAW_X+20, LAW_Y+140,
                    col_lg);
                draw_manager.draw_text(
                    id(ID::LAW_EXIT),
                    "Exit",
                    Justify::Left,
                    LAW_X+20, LAW_Y+204,
                    COL_TEXT);

                bool festival = false;

                if (draw_manager.query_click(id(ID::LAW_FESTSM)).id) {
                    if (player->attempt_spend(20)) {
                        festival = true;
                        planet->adjust_unrest(-1);
                    }
                }

                if (draw_manager.query_click(id(ID::LAW_FESTLG)).id) {
                    if (player->attempt_spend(100)) {
                        festival = true;
                        planet->adjust_unrest(-2);
                    }
                }

                if (festival) {
                    planet->register_festival();
                    ephstate.set_ephemeral_state(EPH_PostPlanet);
                    ephstate.set_postplanet(PPA_Festival);
                    clear_law_ids();
                    draw_law_panel();
                    draw_manager.draw_text(
                        "The celebrations will",
                        Justify::Centre,
                        RES_X/2, LAW_Y+80,
                        COL_TEXT);
                    draw_manager.draw_text(
                        "soon begin.",
                        Justify::Centre,
                        RES_X/2, LAW_Y+100,
                        COL_TEXT);
                    stage = PM_LawFestivalConfirm;
                    return ExodusMode::MODE_Redo;
                }

                if (draw_manager.query_click(id(ID::LAW_EXIT)).id) {
                    clear_law_ids();
                    open_law_panel();
                    stage = PM_Law;
                    return ExodusMode::MODE_Redo;
                }
            }
            break;
        case PM_LawFestivalConfirm:
            {
                if (draw_manager.clicked()) {
                    close_law_panel();
                    stage = PM_Idle;
                    return ExodusMode::MODE_None;
                }
            }
            break;
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
                id_stones[y*blocks+x],
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
    draw_manager.draw(id_stones[y*blocks+x], nullptr);
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

void PlanetMap::draw_class_and_population() {
    int x = 500;
    int y = 50;

    draw_manager.draw_text(
        "Class:",
        Justify::Left,
        x, y,
        COL_TEXT);
    draw_manager.draw_text(
        planet->get_class_str(),
        Justify::Left,
        x, y+20,
        COL_TEXT);

    y += 80;

    char population[16];
    snprintf(population, sizeof(population), "%d m.", planet->get_population());

    draw_manager.draw_text(
        "Population:",
        Justify::Left,
        x, y,
        COL_TEXT);
    draw_manager.draw_text(
        population,
        Justify::Left,
        x, y+20,
        COL_TEXT);
}

ExodusMode PlanetMap::update_destruction(float delta) {
    PlanetDestruction &d = ephstate.destruction;

    if (destruction_done) {
        if (d.nuke && planet->get_class() != Desert) {
            planet->change_class(Desert);
            if (d.draw) draw();
        }

        if (!d.draw || draw_manager.clicked()) {
            L.debug("Destruction complete");
            ephstate.clear_ephemeral_state();
            return ExodusMode::MODE_Pop;
        }

        if (!d.draw) {
            return ExodusMode::MODE_None;
        }
    }

    int nukes_per_update = 1;

    if (d.draw) {
        destruction_time += delta;

        draw_stones();
        if (!d.enable_explosion_anims || explosion_interp >= 1) {
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

        float delay = DESTRUCT_DELAY;
        if (d.nuke) {
            delay /= 10;
            if (d.n_strikes < 350) {
                nukes_per_update = 4000*delta;
            }
        }

        if (destruction_time < delay || exploding == EXP_Drawing) {
            return ExodusMode::MODE_None;
        }

        destruction_time = 0;
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
                    exploding = d.enable_explosion_anims ? EXP_Drawing : EXP_Done;
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
                        if (!planet->find_random_stone(d.tgt_stones, exploding_stone, explode_x, explode_y)) {
                            L.info("Run out of bombing targets for stone %d", exploding_stone);
                            destruction_done = true;
                        }
                        break;
                }

                if (destruction_done)
                    break;

                --d.n_strikes;

                if (d.draw) {
                    targeting_interp = d.show_target ? 0 : 1;
                    exploding = d.enable_explosion_anims ? EXP_Drawing : EXP_Done;
                    // Continue bomb updates if nuking multiple per update
                    if (!(d.nuke && (--nukes_per_update > 0 ))) {
                        break;
                    }
                } else {
                    exploding = EXP_Done;
                }
            }
        }

        if (exploding == EXP_Done) {
            if (d.draw && d.enable_explosion_anims) {
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
                            if (d.draw && d.enable_explosion_anims) {
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

    return ExodusMode::MODE_None;
}

void PlanetMap::draw_frame(int w, int h, bool planet_bg) {
    int border = 6;
    draw_manager.fill(
        id(ID::FRAME),
        {(RES_X - w) / 2 - border,
         (RES_Y - h) / 2 - border - 18,
         w + 2*border,
         h + 2*border},
         COL_BORDERS);
    if (planet_bg) {
        DrawArea area = {108, 90, w, h};
        draw_manager.set_source_region(id(ID::FRAME_BG), &area);
        draw_manager.draw(
            id(ID::FRAME_BG),
            planet->sprites()->landscape,
            {(RES_X - w) / 2,
             (RES_Y - h) / 2 - 18,
             0, 0, 1, 1});
        draw_manager.set_source_region(id(ID::FRAME_BG), nullptr);
    } else {
        draw_manager.fill_pattern(
            {(RES_X - w) / 2,
             (RES_Y - h) / 2 - 18,
             w, h});
    }
}

void PlanetMap::close_frame() {
    draw_manager.draw(id(ID::FRAME_BG), nullptr);
    draw_manager.draw(id(ID::FRAME), nullptr);
}

void PlanetMap::draw_frame_fd() {
    int prod = planet->get_food_production();
    int cons = planet->get_food_consumption();
    int stock = planet->get_reserves_food();

    char str_prod[8];
    char str_cons[8];
    char str_stock[8];
    char str_advisor[128];

    snprintf(str_prod, 7, "%d", prod);
    str_prod[7] = '\0';
    snprintf(str_cons, 7, "%d", cons);
    str_cons[7] = '\0';
    snprintf(str_stock, 7, "%d", stock);
    str_stock[7] = '\0';

    if (prod > cons) {
        int surp = prod - cons;
        snprintf(str_advisor, 127,
            "\"There is a monthly surplus of %d unit%s.\"",
            surp, surp == 1 ? "" : "s");
    } else if (cons > prod) {
        int lack = cons - prod;
        snprintf(str_advisor, 127,
            "\"%d more unit%s needed monthly.\"",
            lack, lack == 1 ? " is" : "s are");
    } else {
        snprintf(str_advisor, 127, "\"Perfect calculation.\"");
    }

    draw_frame(436, 306, false);
    draw_manager.draw_text(
        "Information about cultivated area",
        Justify::Left,
        120, 105,
        COL_TEXT2);

    draw_manager.draw_text(
        "Monthly Production",
        Justify::Left,
        135, 165,
        COL_TEXT);
    draw_manager.draw_text(
        str_prod,
        Justify::Left,
        360, 165,
        COL_TEXT);

    draw_manager.draw_text(
        "Monthly Consumption",
        Justify::Left,
        135, 205,
        COL_TEXT);
    draw_manager.draw_text(
        str_cons,
        Justify::Left,
        360, 205,
        COL_TEXT);

    draw_manager.draw_text(
        "Stock",
        Justify::Left,
        135, 245,
        COL_TEXT);
    draw_manager.draw_text(
        str_stock,
        Justify::Left,
        360, 245,
        COL_TEXT);

    draw_manager.draw_text(
        "Advisor:",
        Justify::Left,
        120, 325,
        COL_TEXT2);
    draw_manager.draw_text(
        str_advisor,
        Justify::Left,
        120, 345,
        COL_TEXT2);
}

void PlanetMap::draw_frame_pl() {
    draw_frame(436, 306, false);

    int prod = planet->get_plu_production();
    int cons = planet->get_plu_consumption();
    int stock = planet->get_reserves_plu();

    char str_prod[8];
    char str_cons[8];
    char str_stock[8];
    char str_advisor[128];

    snprintf(str_prod, 7, "%d", prod);
    str_prod[7] = '\0';
    snprintf(str_cons, 7, "%d", cons);
    str_cons[7] = '\0';
    snprintf(str_stock, 7, "%d", stock);
    str_stock[7] = '\0';

    if (prod > cons) {
        int surp = prod - cons;
        snprintf(str_advisor, 127,
            "\"There is a monthly surplus of %d unit%s.\"",
            surp, surp == 1 ? "" : "s");
    } else if (cons > prod) {
        int lack = cons - prod;
        snprintf(str_advisor, 127,
            "\"%d more unit%s needed monthly.\"",
            lack, lack == 1 ? " is" : "s are");
    } else {
        snprintf(str_advisor, 127, "\"Perfect calculation.\"");
    }

    draw_frame(436, 306, false);
    draw_manager.draw_text(
        "Information about energy systems",
        Justify::Left,
        120, 105,
        COL_TEXT2);

    draw_manager.draw_text(
        "Monthly Plutonium",
        Justify::Left,
        135, 165,
        COL_TEXT);
    draw_manager.draw_text(
        str_prod,
        Justify::Left,
        360, 165,
        COL_TEXT);

    draw_manager.draw_text(
        "Monthly Demand",
        Justify::Left,
        135, 205,
        COL_TEXT);
    draw_manager.draw_text(
        str_cons,
        Justify::Left,
        360, 205,
        COL_TEXT);

    draw_manager.draw_text(
        "Stock",
        Justify::Left,
        135, 245,
        COL_TEXT);
    draw_manager.draw_text(
        str_stock,
        Justify::Left,
        360, 245,
        COL_TEXT);

    draw_manager.draw_text(
        "Advisor:",
        Justify::Left,
        120, 325,
        COL_TEXT2);
    draw_manager.draw_text(
        str_advisor,
        Justify::Left,
        120, 345,
        COL_TEXT2);
}

void PlanetMap::draw_frame_unrest() {
    draw_frame(436, 306, false);

    const int graph_w = 300;
    const int graph_h = 200;
    const int graph_x = RES_X/2 - graph_w/2;
    const int graph_y = RES_Y/2 - graph_h/2 - 30;

    draw_manager.draw_text(
        "Information about the people",
        Justify::Left,
        108, 90,
        COL_TEXT2
    );

    draw_manager.draw_text(
        Font::Tiny,
        "HAPPY",
        Justify::Left,
        108, 124,
        {0x90, 0x90, 0x90});
    draw_manager.draw_text(
        Font::Tiny,
        "UNHAPPY",
        Justify::Left,
        108, 222,
        {0x90, 0x90, 0x90});
    draw_manager.draw_text(
        Font::Tiny,
        "ANGRY",
        Justify::Left,
        108, 320,
        {0x90, 0x90, 0x90});

    // TODO: Graph draw needs cleaning up

    for (int y = 0; y <= 4; y++) {
        draw_manager.draw_line(graph_x, graph_y+(y*graph_h)/4, graph_x + graph_w, graph_y+(y*graph_h)/4, 4, {0x90, 0x90, 0x90});
    }

    for (int x = 0; x < N_UNREST; x++) {
        draw_manager.draw_line(graph_x+(x*graph_w)/(N_UNREST-1), graph_y, graph_x+(x*graph_w)/(N_UNREST-1), graph_y + graph_h, 4, {0x90, 0x90, 0x90});
    }

    int prog = 0;
    for (int i = 0; i < 5; ++i) {
        prog += planet->get_unrest(i);
    }
    prog /= 5;

    const char *prog_str = nullptr;
    if (prog > planet->get_unrest()) {
        prog_str = "Prognosis: positive";
    } else if (prog < planet->get_unrest()) {
        prog_str = "Prognosis: negative";
    } else {
        prog_str = "Prognosis: no change";
    }

    draw_manager.draw_text(
        prog_str,
        Justify::Left,
        108, 360,
        COL_TEXT2);

    int scale_y = graph_h/10;;
    int x0 = graph_x;
    for (int m = N_UNREST-1; m > 0; --m) {
        int x1 = (int)((float)x0 + (float)graph_w/(float)(N_UNREST-1) + .5f);
        int data0 = min(10, planet->get_unrest(m));
        int data1 = min(10, planet->get_unrest(m-1));
        int y0 = graph_y + data0*scale_y;
        int y1 = graph_y + data1*scale_y;
        draw_manager.draw_line(x0, y0, x1, y1, 5, COL_TEXT2);
        x0 = x1;
    }
}

void PlanetMap::draw_frame_help(Tool tool) {
    draw_frame(436, 306, true);

    const char** desc = tool_desc_hq;

    switch (tool) {
        case TOOL_HQ:
            desc = tool_desc_hq;
            break;
        case TOOL_Cultivate:
            desc = tool_desc_agri;
            break;
        case TOOL_Mine:
            desc = tool_desc_mine;
            break;
        case TOOL_Plu:
            desc = tool_desc_plu;
            break;
        case TOOL_City:
            desc = tool_desc_city;
            break;
        case TOOL_Clear:
            desc = tool_desc_clear;
            break;
        case TOOL_Inf:
            desc = tool_desc_inf;
            break;
        case TOOL_Gli:
            desc = tool_desc_gli;
            break;
        case TOOL_Art:
            desc = tool_desc_art;
            break;
        case TOOL_Port0:
            desc = tool_desc_port0;
            break;
        case TOOL_Port1:
            desc = tool_desc_port1;
            break;
        case TOOL_Port2:
            desc = tool_desc_port2;
            break;
        case TOOL_Trade:
            desc = tool_desc_trade;
            break;
        case TOOL_LunarBase:
            desc = tool_desc_lunar_base;
            break;
        case TOOL_Park:
            desc = tool_desc_park;
            break;
        default:
            L.error("No help for tool %d", (int)tool);
            return;
    }

    int y = 110;
    draw_manager.draw_text(desc[0], Justify::Left, 108, y, COL_TEXT2);
    y += 40;
    for (int i = 1; i < TOOL_DESC_LINES; ++i) {
        draw_manager.draw_text(desc[i], Justify::Left, 118, y, COL_TEXT);
        y += 20;
    }
}

void PlanetMap::open_law_panel() {
    draw_law_panel();
    draw_manager.draw_text(
        "LAW AND ORGANIZATION",
        Justify::Centre,
        RES_X/2, LAW_Y+4,
        COL_TEXT2);
    stage = PM_Law;
}

void PlanetMap::close_law_panel() {
    clear_law_ids();
    draw_manager.draw(id(ID::LAW_PATTERN), nullptr);
    draw_manager.draw(id(ID::LAW_PANEL), nullptr);
    stage = PM_Idle;
}

void PlanetMap::draw_law_panel() {
    draw_manager.fill(
        id(ID::LAW_PANEL),
        {LAW_X - BORDER, LAW_Y - BORDER,
         LAW_W + 2*BORDER, LAW_H + 2*BORDER},
         COL_BORDERS);
    // TODO: Use alternate pattern here
    draw_manager.fill_pattern(
        id(ID::LAW_PATTERN),
        {LAW_X, LAW_Y, LAW_W, LAW_H});
}

void PlanetMap::clear_law_ids() {
    for (ID i = LAW_JUSTICE; i <= LAW_EXIT; i = (ID)((int)i+1)) {
        draw_manager.draw(id(i), nullptr);
    }
}
