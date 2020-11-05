#include "guild_bar.h"

#include <cmath>

#include "anim.h"
#include "assetpaths.h"
#include "galaxy/flytarget.h"

#define RUMOUR_LINE_MAX 64

static const DrawArea AREA_BARKEEP  = {200, 230, 40, 50};
static const DrawArea AREA_SHERIFF  = {400, 230, 60, 40};
static const DrawArea AREA_PINBOARD = {590, 200, 50, 50};
static const DrawArea AREA_EXIT     = {490, 220, 50, 60};

static const int PINBOARD_X = 280;
static const int PINBOARD_Y = 40;
static const int PINBOARD_W = 332;
static const int PINBOARD_H = 318;

typedef struct {
    char line0[RUMOUR_LINE_MAX];
    char line1[RUMOUR_LINE_MAX];
} Rumour;

Rumour rumours[5];

enum ID {
    TALK,
    LIGHT,
    TEXT,
    BARKEEP,
    PINBOARD,
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
    talk_loop = 0.f;
    light_loop = 0.f;
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
    unsigned int n_stars;
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
    for (unsigned int i = 0; i < n_stars; ++i) {
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
    for (unsigned int i = 0; i < n_stars; ++i) {
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
    for (unsigned int i = 0; i < n_stars; ++i) {
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

    draw_manager.draw(id(ID::TALK), talk_anim.interp(talk_loop), {225, 246, 0.5, 0.5, 2, 2});

    switch (stage) {
        case GB_Idle:
            if (light_loop > 5.7) {
                draw_manager.draw(id(ID::LIGHT), IMG_SG3_LAMP0, {546, 166, 0.5, 0.5, 2, 2});
            } else {
                draw_manager.draw(id(ID::LIGHT), nullptr);
            }

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
    }

    return ExodusMode::MODE_None;
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
