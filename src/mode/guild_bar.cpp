#include "guild_bar.h"

#include <cmath>

#include "anim.h"
#include "assetpaths.h"
#include "galaxy/flytarget.h"

#include "util/value.h"

#define RUMOUR_LINE_MAX 64

static const DrawArea AREA_BARKEEP  = {200, 230, 40, 50};
static const DrawArea AREA_SHERIFF  = {400, 230, 60, 40};
static const DrawArea AREA_PINBOARD = {590, 200, 50, 50};
static const DrawArea AREA_EXIT     = {490, 220, 50, 60};

static const int PINBOARD_X = 280;
static const int PINBOARD_Y = 40;
static const int PINBOARD_W = 332;
static const int PINBOARD_H = 318;

static const int SHERIFF_X = 96;
static const int SHERIFF_Y = 104;
static const int SHERIFF_W = 410;
static const int SHERIFF_H = 342;

typedef struct {
    char line0[RUMOUR_LINE_MAX];
    char line1[RUMOUR_LINE_MAX];
} Rumour;

Rumour rumours[5];

enum ID {
    TALK,
    TALKMIRR,
    LIGHT,
    TEXT,
    BARKEEP,
    PINBOARD,
    SCREEN,
    SHERIFF_FRAME,
    SHERIFF_BORDER,
    // TODO: Scale the source image!
    SHERIFF_BG,
    SHERIFF_EXIT,
    END,
};

GuildBar::GuildBar() : ModeBase("GuildBar") {
    last_update_month = -1;
    rumour_headings[0] = 0;
    rumour_headings[1] = 0;
    rumour_indices[0] = 0;
    rumour_indices[1] = 0;
}

void GuildBar::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_SG3_DUST);
    draw_manager.save_background();
    draw_manager.show_cursor(true);
    audio_manager.target_music(MUS_CELEBRATE);
    talk_loop = 0.f;
    light_loop = 0.f;
    screen_loop = 0.f;
    stage = GB_Idle;

    if (exostate.get_month() != last_update_month) {
        update_pin_and_rumours();
        last_update_month = exostate.get_month();
    }
}

Anim talk_anim(
    22,
    IMG_SG3_TALK1,
    IMG_SG3_TALK2,
    IMG_SG3_TALK3,
    IMG_SG3_TALK2,
    IMG_SG3_TALK1,
    IMG_SG3_TALK2,
    IMG_SG3_TALK1,
    IMG_SG3_TALK2,
    IMG_SG3_TALK3,
    IMG_SG3_TALK4,
    IMG_SG3_TALK3,
    IMG_SG3_TALK2,
    IMG_SG3_TALK1,
    IMG_SG3_TALK6,
    IMG_SG3_TALK3,
    IMG_SG3_TALK2,
    IMG_SG3_TALK1,
    IMG_SG3_TALK2,
    IMG_SG3_TALK3,
    IMG_SG3_TALK5,
    IMG_SG3_TALK3,
    IMG_SG3_TALK6
);

Anim talkmirr_anim(
    22,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR1,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR3,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR2,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR3
);

Anim screen_anim(
    4,
    IMG_SG3_SCREEN1,
    IMG_SG3_SCREEN2,
    IMG_SG3_SCREEN3,
    IMG_SG3_SCREEN4
);

Anim sheriff_ship_anim(
    12,
    IMG_GM1_T1,
    IMG_GM1_T2,
    IMG_GM1_T3,
    IMG_GM1_T4,
    IMG_GM1_T5,
    IMG_GM1_T6,
    IMG_GM1_T7,
    IMG_GM1_T8,
    IMG_GM1_T9,
    IMG_GM1_T10,
    IMG_GM1_T11,
    IMG_GM1_T12
);

Anim sheriff_exp_anim(
    6,
    IMG_GM1_XP1,
    IMG_GM1_XP2,
    IMG_GM1_XP3,
    IMG_GM1_XP4,
    IMG_GM1_XP5,
    IMG_GM1_XP6
);

Anim sheriff_bonus_anim(
    4,
    IMG_GM1_B1,
    IMG_GM1_B2,
    IMG_GM1_B3,
    IMG_GM1_B4
);

extern const char* RUMOUR_HEADINGS[];
extern const char* PINS[];

void GuildBar::update_pin_and_rumours() {
    pin = RND(20) - 1;
    rumour_headings[0] = RND(5) - 1;
    rumour_headings[1] = RND(5) - 1;

    rumour_indices[0] = RND(5) - 1;
    rumour_indices[1] = RND(4) - 1;
    // Using >= here to avoid bias
    if (rumour_indices[1] >= rumour_indices[0]) {
        ++rumour_indices[1];
    }

    Galaxy *gal = exostate.get_galaxy();
    int n_stars;
    Star *stars = gal->get_stars(n_stars);
    Player *player;
    Star *star;
    Planet *planet;
    PlanetInfo planet_info;

    // Rumour 0: Number of planets
    player = exostate.get_random_active_player();
    int n_planets = exostate.get_n_planets(player);
    if (onein(3)) {
        n_planets = RND(5);
    }
    snprintf(rumours[0].line0,
             RUMOUR_LINE_MAX,
             "%s owns %d planet%s.",
             player->get_name(),
             n_planets,
             n_planets == 1 ? "" : "s");
    rumours[0].line1[0] = '\0';

    // Rumour 1: Defence
    int min_army_size = 10000;
    if (onein(3)) {
        min_army_size = 100;
    }
    planet_info = exostate.get_random_owned_planet_info();
    for (int i = 0; i < n_stars; ++i) {
        star = &stars[i];
        for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
            planet = star->get_planet(planet_idx);
            if (planet && planet->exists() && planet->is_owned()) {
                if (planet->get_army_size() < min_army_size) {
                    min_army_size = planet->get_army_size();
                    planet_info.planet = planet;
                    planet_info.star = star;
                    planet_info.index = planet_idx;
                }
            }
        }
    }
    snprintf(rumours[1].line0,
             RUMOUR_LINE_MAX,
             "%s at %s",
             planet_info.planet->is_named() ? planet_info.planet->get_name() : "Utopia",
             planet_info.star->name);
    snprintf(rumours[1].line1,
             RUMOUR_LINE_MAX,
             "has the weakest defence.");

    // Rumour 2: Goods
    int max_reserves = 0;
    if (onein(3)) {
        max_reserves = RND(200);
    }
    planet_info = exostate.get_random_owned_planet_info();
    for (int i = 0; i < n_stars; ++i) {
        star = &stars[i];
        for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
            planet = star->get_planet(planet_idx);
            if (planet && planet->exists()) {
                if (planet->get_total_reserves() > max_reserves) {
                    max_reserves = planet->get_total_reserves();
                    planet_info.planet = planet;
                    planet_info.star = star;
                    planet_info.index = planet_idx;
                }
            }
        }
    }
    snprintf(rumours[2].line0,
             RUMOUR_LINE_MAX,
             "%s at %s",
             planet_info.planet->is_named() ? planet_info.planet->get_name() : "Utopia",
             planet_info.star->name);
    snprintf(rumours[2].line1,
             RUMOUR_LINE_MAX,
             "holds the most goods.");

    // Rumour 3: Population
    int max_pop = 0;
    if (onein(3)) {
        max_pop = RND(20);
    }
    planet_info = exostate.get_random_owned_planet_info();
    for (int i = 0; i < n_stars; ++i) {
        star = &stars[i];
        for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
            planet = star->get_planet(planet_idx);
            if (planet && planet->exists()) {
                if (planet->get_n_cities() > max_pop) {
                    max_pop = planet->get_n_cities();
                    planet_info.planet = planet;
                    planet_info.star = star;
                    planet_info.index = planet_idx;
                }
            }
        }
    }
    snprintf(rumours[3].line0,
             RUMOUR_LINE_MAX,
             "%s at %s",
             planet_info.planet->is_named() ? planet_info.planet->get_name() : "Utopia",
             planet_info.star->name);
    snprintf(rumours[3].line1,
             RUMOUR_LINE_MAX,
             "has the largest population.");

    // Rumour 4: Reputation
    player = exostate.get_random_active_player();
    int rep = player->get_reputation();
    if (onein(3)) {
        rep = RND(5);
    }
    snprintf(rumours[4].line0,
             RUMOUR_LINE_MAX,
             "%s has %s good",
             player->get_name(),
             rep < 3 ? "no" : "a");
    snprintf(rumours[4].line1,
             RUMOUR_LINE_MAX,
             "reputation.");

    for (int i = 0; i < 5; ++i) {
        rumours[i].line0[RUMOUR_LINE_MAX - 1] = '\0';
        rumours[i].line1[RUMOUR_LINE_MAX - 1] = '\0';
    }
}

ExodusMode GuildBar::update(float delta) {
    bool click = draw_manager.clicked();

    talk_loop = fmod(talk_loop + delta * 0.2, 1);
    light_loop = fmod(light_loop + delta, 6);
    screen_loop = fmod(screen_loop + delta * 0.12, 1);

    if (stage < GB_StarSheriffSetup) {
        draw_manager.draw(id(ID::TALK), talk_anim.interp(talk_loop), {224, 246, 0.5, 0.5, 2, 2});
        draw_manager.draw(id(ID::TALKMIRR), talkmirr_anim.interp(talk_loop), {222, 318, 0, 1, 2, 2});
    }

    switch (stage) {
        case GB_Idle:
            if (light_loop > 5.7) {
                draw_manager.draw(id(ID::LIGHT), IMG_SG3_LAMP0, {546, 166, 0.5, 0.5, 2, 2});
            } else {
                draw_manager.draw(id(ID::LIGHT), nullptr);
            }

            draw_manager.draw(id(ID::SCREEN), screen_anim.interp(screen_loop), {427, 252, 0.5, 0.5, 2, 2});

            if (draw_manager.mouse_in_area(AREA_BARKEEP)) {
                draw_manager.draw_text(
                    id(ID::TEXT),
                    "The Barkeeper",
                    Justify::Left,
                    10, RES_Y - 30,
                    COL_TEXT2);
                if (click) {
                    draw_manager.fill(
                        id(ID::BARKEEP),
                        {PINBOARD_X - BORDER, PINBOARD_Y - BORDER,
                         PINBOARD_W + 2*BORDER, PINBOARD_H + 2*BORDER},
                         COL_BORDERS);
                    draw_manager.fill_pattern(
                        {PINBOARD_X, PINBOARD_Y,
                         PINBOARD_W, PINBOARD_H});
                    draw_manager.draw_text(
                        "Today's rumours",
                        Justify::Left,
                        PINBOARD_X + 4, PINBOARD_Y + 4,
                        COL_TEXT2);

                    draw_manager.draw_text(
                        RUMOUR_HEADINGS[rumour_headings[0]],
                        Justify::Left,
                        PINBOARD_X + 4, PINBOARD_Y + 60,
                        COL_TEXT);
                    draw_manager.draw_text(
                        rumours[rumour_indices[0]].line0,
                        Justify::Left,
                        PINBOARD_X + 4, PINBOARD_Y + 80,
                        COL_TEXT);
                    draw_manager.draw_text(
                        rumours[rumour_indices[0]].line1,
                        Justify::Left,
                        PINBOARD_X + 4, PINBOARD_Y + 100,
                        COL_TEXT);

                    draw_manager.draw_text(
                        RUMOUR_HEADINGS[rumour_headings[1]],
                        Justify::Left,
                        PINBOARD_X + 4, PINBOARD_Y + 160,
                        COL_TEXT);
                    draw_manager.draw_text(
                        rumours[rumour_indices[1]].line0,
                        Justify::Left,
                        PINBOARD_X + 4, PINBOARD_Y + 180,
                        COL_TEXT);
                    draw_manager.draw_text(
                        rumours[rumour_indices[1]].line1,
                        Justify::Left,
                        PINBOARD_X + 4, PINBOARD_Y + 200,
                        COL_TEXT);

                    stage = GB_Barkeeper;
                }
            } else if (draw_manager.mouse_in_area(AREA_SHERIFF)) {
                draw_manager.draw_text(
                    id(ID::TEXT),
                    "Video Game",
                    Justify::Left,
                    10, RES_Y - 30,
                    COL_TEXT2);
                if (click) {
                    stage = GB_StarSheriffSetup;
                }
            } else if (draw_manager.mouse_in_area(AREA_PINBOARD)) {
                draw_manager.draw_text(
                    id(ID::TEXT),
                    "Pinboard",
                    Justify::Left,
                    10, RES_Y - 30,
                    COL_TEXT2);
                if (click) {
                    draw_manager.fill(
                        id(ID::PINBOARD),
                        {PINBOARD_X - BORDER, PINBOARD_Y - BORDER,
                         PINBOARD_W + 2*BORDER, PINBOARD_H + 2*BORDER},
                         COL_BORDERS);
                    draw_manager.fill_pattern(
                        {PINBOARD_X, PINBOARD_Y,
                         PINBOARD_W, PINBOARD_H});
                    draw_manager.draw_text(
                        "Pinboard",
                        Justify::Left,
                        PINBOARD_X + 4, PINBOARD_Y + 4,
                        COL_TEXT2);
                    draw_manager.draw_text(
                        "EXPERT'S REPORT:",
                        Justify::Left,
                        PINBOARD_X + 4, PINBOARD_Y + 60,
                        COL_TEXT);
                    {
                        const char* report_text[3];
                        char planet_line[16 + FT_MAX_NAME + PLANET_MAX_NAME];
                        if (exostate.get_n_human_players() == 1) {
                            PlanetInfo info = exostate.recommend_planet();
                            if (info.planet->is_named()) {
                                snprintf(
                                    planet_line,
                                    16 + FT_MAX_NAME + PLANET_MAX_NAME,
                                    "%s at %s.",
                                    info.planet->get_name(),
                                    info.star->name);
                            } else {
                                snprintf(
                                    planet_line,
                                    16 + FT_MAX_NAME + PLANET_MAX_NAME,
                                    "planet %d at %s.",
                                    info.index + 1,
                                    info.star->name);
                            }

                            // FIXME: There is no case for when no planets are free
                            report_text[0] = "The best free planet that waits";
                            report_text[1] = "for colonization is probably";
                            report_text[2] = planet_line;

                        } else {
                            report_text[0] = "Leaving an airlock without";
                            report_text[1] = "a proper space suit seriously";
                            report_text[2] = "damages your health.";
                        }

                        for (int i = 0; i < 3; ++i) {
                            draw_manager.draw_text(
                                report_text[i],
                                Justify::Left,
                                PINBOARD_X + 4, PINBOARD_Y + 100 + 20*i,
                                COL_TEXT);
                        }
                    }
                    for (int i = 0; i < 3; ++i) {
                        draw_manager.draw_text(
                            PINS[3*pin + i],
                            Justify::Left,
                            PINBOARD_X + 4, PINBOARD_Y + 200 + 20*i,
                            COL_TEXT);
                    }
                    stage = GB_Pinboard;
                }
            } else if (draw_manager.mouse_in_area(AREA_EXIT)) {
                draw_manager.draw_text(
                    id(ID::TEXT),
                    "Exit door",
                    Justify::Left,
                    10, RES_Y - 30,
                    COL_TEXT2);
                if (click) {
                    return ExodusMode::MODE_Pop;
                }
            } else {
                draw_manager.draw(id(ID::TEXT), nullptr);
            }
            break;
        case GB_Barkeeper:
            if (click) {
                draw_manager.draw(id(ID::BARKEEP), nullptr);
                stage = GB_Idle;
            }
            break;
        case GB_Pinboard:
            if (click) {
                draw_manager.draw(id(ID::PINBOARD), nullptr);
                stage = GB_Idle;
            }
            break;
        case GB_StarSheriffSetup:
            audio_manager.target_music(mpart2mus(11));

            for (int i = 0; i < SHERIFF_N_SHIPS; ++i) {
                sheriff_ships[i].init(SheriffShip::InitType::Demo);
                sheriff_ships[i].id = draw_manager.new_sprite_id();
                sheriff_ships[i].blasters_id = draw_manager.new_sprite_id();
            }

            draw_manager.fill(
                id(ID::SHERIFF_FRAME),
                {SHERIFF_X -   BORDER, SHERIFF_Y -   BORDER,
                 SHERIFF_W + 2*BORDER, SHERIFF_H + 2*BORDER},
                 COL_BORDERS);
            draw_manager.fill(
                id(ID::SHERIFF_BORDER),
                {SHERIFF_X + 4, SHERIFF_Y + SHERIFF_H - 36,
                 SHERIFF_W - 8, 36},
                {0, 0, 0});
            draw_manager.draw(
                id(ID::SHERIFF_BG),
                IMG_GM1_PICTURE,
                {SHERIFF_X + 4, SHERIFF_Y + 4,
                 0, 0, 1, 2});
            draw_manager.draw(
                id(ID::SHERIFF_EXIT),
                IMG_GM1_EXIT,
                {SHERIFF_X + 6, SHERIFF_Y + SHERIFF_H - 2,
                 0, 1, 1, 1});
            draw_manager.fill(
                {SHERIFF_X + 126, SHERIFF_Y + SHERIFF_H - 28,
                 SHERIFF_W - 132, 26},
                 COL_BORDERS);
            stage = GB_StarSheriffSetupGame;
            break;
        case GB_StarSheriffSetupGame:
            sheriff_demo = true;
            sheriff_level = 1;
            sheriff_shot_interp = -1;
            sheriff_hittime = 0;
            sheriff_shield = 4;
            sheriff_announce = SA_Go;
            sheriff_announce_time = -1;
            sheriff_game_time = 0;
            sheriff_score = 0;
            sheriff_tokill = 10 * sheriff_level;
            sheriff_gameover = false;
            stage = GB_StarSheriff;
            break;
        case GB_StarSheriff:
            {
                bool done = update_star_sheriff(delta);

                if (done) {
                    // TODO: High scores
                    stage = GB_StarSheriffSetupGame;
                    break;
                }

                if (draw_manager.query_click(id(ID::SHERIFF_EXIT)).id) {
                    stage = GB_StarSheriffExit;
                }
                break;
            }
        case GB_StarSheriffExit:
            for (int i = 0; i < SHERIFF_N_SHIPS; ++i) {
                draw_manager.draw(sheriff_ships[i].blasters_id, nullptr);
                draw_manager.release_sprite_id(sheriff_ships[i].blasters_id);
                draw_manager.draw(sheriff_ships[i].id, nullptr);
                draw_manager.release_sprite_id(sheriff_ships[i].id);
            }

            draw_manager.draw(ID::SHERIFF_EXIT, nullptr);
            draw_manager.draw(ID::SHERIFF_BORDER, nullptr);
            draw_manager.draw(ID::SHERIFF_BG, nullptr);
            draw_manager.draw(ID::SHERIFF_FRAME, nullptr);
            draw_manager.draw(IMG_SG3_DUST);
            audio_manager.target_music(MUS_CELEBRATE);
            stage = GB_Idle;
            break;
    }

    return ExodusMode::MODE_None;
}

void SheriffShip::init(SheriffShip::InitType t) {
    //x = RND(375);
    //y = RND(225);
    x = 30 + RND(345);
    y = 30 + RND(195);
    z = 200;
    switch (t) {
        case SheriffShip::InitType::Demo:
            delay = 5.f * (float)(rand() % 1000)/1000.f;
            break;
        case SheriffShip::InitType::Game:
            delay = 10.f * (float)(rand() % 1000)/1000.f;
            break;
        case SheriffShip::InitType::Respawn:
            x = 30 + RND(270);
            y = 30 + RND(120);
            delay = 2.f * (float)(rand() % 1000)/1000.f;
            break;
        default:
            L.error("Unknown init type: %d", t);
    }
    anim_interp = (float)(rand() % 1000)/1000.f;
    live = true;
    explosion_interp = 0.f;
    blasters_interp = 0.f;
}

bool GuildBar::update_star_sheriff(float delta) {

    if (sheriff_gameover && sheriff_announce_time <= 0) {
        return true;
    }

    sheriff_game_time += delta;

    if (sheriff_hittime > 0) {
        draw_manager.fill(
            id(ID::SHERIFF_BG),
            {SHERIFF_X + 4, SHERIFF_Y + 4,
             SHERIFF_W - 8, SHERIFF_H - 32},
             {0xFF, 0, 0});

        sheriff_hittime -= delta;
    } else {
        draw_manager.draw(
            id(ID::SHERIFF_BG),
            IMG_GM1_PICTURE,
            {SHERIFF_X + 4, SHERIFF_Y + 4,
             0, 0, 1, 2});
    }

    if (sheriff_shot_interp >= 1) {
        sheriff_shot_interp = -1;
    }

    // Player laser fire
    if (!sheriff_gameover && sheriff_shot_interp < 0) {
        SpriteClick clk = draw_manager.query_click(id(ID::SHERIFF_BG));

        if (clk.id) {
            if (sheriff_demo) {
                sheriff_demo = false;
                sheriff_shield = 4;
                sheriff_announce = SA_Go;
                sheriff_announce_time = 2;
                for (int i = 0; i < SHERIFF_N_SHIPS; ++i) {
                    sheriff_ships[i].init(SheriffShip::InitType::Game);
                }
                return false;
            }

            sheriff_laser_x = SHERIFF_X + (clk.x * ((float)SHERIFF_W - 8.f));
            sheriff_laser_y = SHERIFF_Y + (clk.y * ((float)SHERIFF_H - 32.f));
            sheriff_shot_interp = 0.f;
        }

        for (int i = 0; i < SHERIFF_N_SHIPS; ++i) {
            if (!sheriff_ships[i].live) {
                continue;
            }
            if (draw_manager.query_click(sheriff_ships[i].id).id) {
                sheriff_ships[i].live = false;
                sheriff_score += 99;
                sheriff_tokill--;

                if (sheriff_tokill <= 0) {
                    sheriff_level++;
                    sheriff_announce = SA_Stage;
                    sheriff_announce_time = 2;
                    sheriff_score += 2000;
                    sheriff_tokill = sheriff_level * 10;
                }

                // Can't kill two ships with one laser
                break;
            }
        }
    }

    for (int i = 0; i < SHERIFF_N_SHIPS; ++i) {
        SheriffShip &ship = sheriff_ships[i];

        if (ship.delay >= 0) {
            ship.delay -= delta;
            continue;
        }

        float sc = (float)ship.z / 1000.f;

        if (ship.live) {
            draw_manager.draw(
                ship.id,
                sheriff_ship_anim.interp(ship.anim_interp),
                {SHERIFF_X + ship.x, SHERIFF_Y + ship.y,
                 0.5, 0.5, sc, sc});

            if (sheriff_level < 5 && ship.delay <= 0 && ship.z < 250) {
                draw_manager.draw(
                    IMG_GM1_MARKNEW,
                    {SHERIFF_X + ship.x, SHERIFF_Y + ship.y,
                     0.5, 0.5, 1, 1});
            }
        } else {
            if (ship.explosion_interp < 1) {
                draw_manager.draw(
                    ship.id,
                    sheriff_exp_anim.interp(ship.explosion_interp),
                    {SHERIFF_X + ship.x, SHERIFF_Y + ship.y,
                     0.5, 0.5, sc, sc});

                ship.explosion_interp += delta * 2;
            } else {
                ship.init(SheriffShip::InitType::Respawn);
            }
        }

        ship.anim_interp = fmod(ship.anim_interp + delta * 0.9f, 1.f);

        if (ship.live) {
            ship.z += (15 * delta) * (float)(10 + (sheriff_level*5));

            if (ship.z > 1000) {
                if (ship.blasters_interp <= 0.5) {
                    draw_manager.draw(
                        ship.blasters_id,
                        IMG_GM1_TFIRE,
                        {SHERIFF_X + ship.x, SHERIFF_Y + ship.y,
                         0.5, 0.5, sc, sc});
                } else {
                    draw_manager.draw(ship.blasters_id, nullptr);
                }
                ship.blasters_interp = fmod(ship.blasters_interp + (delta * 5.f), 1.f);
            }

            if (ship.z > 1200) {
                draw_manager.draw(ship.blasters_id, nullptr);
                draw_manager.draw(ship.id, nullptr);

                // Been hit

                if (!sheriff_demo) {
                    sheriff_shield -= 1;
                }

                if (!sheriff_gameover && sheriff_shield < 0) {
                    sheriff_announce = SA_GameOver;
                    sheriff_announce_time = 3;
                    sheriff_gameover = true;
                }

                sheriff_hittime = 0.1;
                ship.init(SheriffShip::InitType::Respawn);
            }
        }
    }

    // Update laser
    if (sheriff_shot_interp >= 0) {
        sheriff_shot_interp = fmin(1, sheriff_shot_interp + delta * 9.f);

        int origin_xl = SHERIFF_X + 4;
        int origin_xr = SHERIFF_X + SHERIFF_W - 4;
        int origin_y  = SHERIFF_Y + 300;

        float start_interp = fmax(0, sheriff_shot_interp - 0.3);

        int xl0 = ilerp(origin_xl, sheriff_laser_x, start_interp);
        int xr0 = ilerp(origin_xr, sheriff_laser_x, start_interp);
        int xl1 = ilerp(origin_xl, sheriff_laser_x, sheriff_shot_interp);
        int xr1 = ilerp(origin_xr, sheriff_laser_x, sheriff_shot_interp);

        int y0 = ilerp(origin_y, sheriff_laser_y, start_interp);
        int y1 = ilerp(origin_y, sheriff_laser_y, sheriff_shot_interp);

        draw_manager.draw_line(xl0, y0, xl1, y1, {0xFF, 0, 0});
        draw_manager.draw_line(xr0, y0, xr1, y1, {0xFF, 0, 0});
    }

    if (sheriff_announce_time > 0) {
        sheriff_announce_time -= delta;
        if (fmod(sheriff_announce_time, 0.2) < 0.1) {
            const char* spr = IMG_GM1_SHERIFF;

            int x = SHERIFF_X + SHERIFF_W/2;

            switch (sheriff_announce) {
                case SA_Go:
                    spr = IMG_GM1_GO;
                    break;
                case SA_Stage:
                    spr = IMG_GM1_STAGE;
                    x -= 40;
                    break;
                case SA_GameOver:
                    spr = IMG_GM1_GAMEOVER;
                    break;
                case SA_BonusKill:
                    spr = IMG_GM1_KBON;
                    break;
                case SA_BonusShield:
                    spr = IMG_GM1_SBON;
                    break;
            }

            draw_manager.draw(
                spr,
                {x, SHERIFF_Y + 140,
                 0.5, 0.5, 1, 1});

            if (sheriff_announce == SA_Stage) {
                spr = IMG_GM1_S0;
                if (sheriff_level == 1) spr = IMG_GM1_S1;
                if (sheriff_level == 2) spr = IMG_GM1_S2;
                if (sheriff_level == 3) spr = IMG_GM1_S3;
                if (sheriff_level == 4) spr = IMG_GM1_S4;
                if (sheriff_level == 5) spr = IMG_GM1_S5;
                if (sheriff_level == 6) spr = IMG_GM1_S6;
                if (sheriff_level == 7) spr = IMG_GM1_S7;
                if (sheriff_level == 8) spr = IMG_GM1_S8;
                if (sheriff_level == 9) spr = IMG_GM1_S9;

                // FIXME: Do we care about what happens after level 9? Force gameover?

                draw_manager.draw(
                    spr,
                    {x + 120, SHERIFF_Y + 140,
                     0.5, 0.5, 1, 1});
            }
        }
    }

    // Draw HUD
    if (sheriff_shield > 0 || fmod(sheriff_game_time, 0.4) < 0.2) {
        const char* spr = IMG_GM1_SHIELD0;
        if (sheriff_shield == 4) spr = IMG_GM1_SHIELD4;
        if (sheriff_shield == 3) spr = IMG_GM1_SHIELD3;
        if (sheriff_shield == 2) spr = IMG_GM1_SHIELD2;
        if (sheriff_shield == 1) spr = IMG_GM1_SHIELD1;

        draw_manager.draw(
            spr,
            {SHERIFF_X + 10, SHERIFF_Y + 304,
             0, 1, 1, 1});
    }

    draw_manager.draw(
        IMG_GM1_TOKILL,
        {SHERIFF_X + 180, SHERIFF_Y + 288,
         0, 0, 1, 1});

    char str[12];

    snprintf(str, sizeof(str), "%d", sheriff_level);
    draw_manager.draw_text(
        str,
        Justify::Left,
        SHERIFF_X + 90, SHERIFF_Y + 286,
        COL_TEXT_SPEECH);

    snprintf(str, sizeof(str), "%d", sheriff_tokill);
    draw_manager.draw_text(
        str,
        Justify::Left,
        SHERIFF_X + 200, SHERIFF_Y + 286,
        COL_TEXT_SPEECH);

    snprintf(str, sizeof(str), "%d", sheriff_score);
    draw_manager.draw_text(
        str,
        Justify::Right,
        SHERIFF_X + SHERIFF_W - 12, SHERIFF_Y + 286,
        COL_TEXT_SPEECH);

    if (sheriff_demo) {
        draw_manager.draw(
            IMG_GM1_SHERIFF,
            {SHERIFF_X + SHERIFF_W/2, SHERIFF_Y + 140,
             0.5, 0.5, 1, 1});
    }

    return false;
}

const char* RUMOUR_HEADINGS[] = {
    "It is said that",
    "There are rumors that",
    "There is a rumor that",
    "There's a strong rumor that",
    "It has been heard that"
};

const char* PINS[] = {
    "WANTED!",
    "Kabirk Gooma",
    "Bounty 1000 Credits",
    "WANTED!",
    "Sobik Gamas",
    "Bounty 500 Credits",
    "WANTED!",
    "Burak Burak",
    "Bounty 2000 Credits",
    "WANTED!",
    "Esolk Naj",
    "Bounty 15000 Credits",
    "WANTED!",
    "Kriz'Ippes",
    "Bounty 12000 Credits",
    "WANTED!",
    "The Reuab",
    "Bounty 8000 Credits",
    "WANTED!",
    "Nieh Lein'ad",
    "Bounty 9000 Credits",
    "WANTED!",
    "Whiskey Man",
    "Bounty 6000 Credits",
    "SELLING Hypershuttle",
    "100% o.k.",
    "Andy Jones",
    "Has anybody seen my",
    "small furry pet?",
    "Call: A58-HT6",
    "VISIT the Space Yoga Center",
    "for zero-g Yoga!",
    "Call: 6G8-87E",
    "VISIT our Urkash Language",
    "Course! Best teachers, best",
    "prices! Call: F92-UY7",
    "SELLING lovely small furry",
    "pets!",
    "Call: 584-635",
    "SELLING old starship",
    "100% o.k.",
    "Call: 1701-D",
    "We're looking for a new",
    "WAITRESS. Ask barkeeper",
    "for details.",
    "Call the SPACEPARTY for",
    "fun around the clock!",
    "Call: 331-331",
    "SELLING second-hand lasers",
    "for dumping prices!",
    "Call: FAG-7YE",
    "Test our new ADNIRIM drink!",
    "For more infos,",
    "please call: 507-1930",
    "SELLING second-hand",
    "guard robots!",
    "Call: 765-T2Y",
    "This month's best Star",
    "Sheriff Player will",
    "receive a reward!"
};
