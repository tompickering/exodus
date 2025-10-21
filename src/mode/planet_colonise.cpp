#include "planet_colonise.h"

#include "anim.h"
#include "util/value.h"

#include "exodus_features.h"

#include "shared.h"

#define TEXT_START_DELAY 0.8f
#define CHARS_PER_SECOND 14.f
#define TEXT_X 10
#define TEXT_Y 400
#define DOOR_LEN 1.6f
#define SHUTTLE_DELAY 1
#define SHUTTLE_START_X 270
#define SHUTTLE_START_Y 260
#define SHUTTLE_END_X 610
#define SHUTTLE_END_Y 370
#define SHUTTLE_END_SZ 1
#define SHUTTLE_END 7

Anim anim_doors(
    7,
    nullptr,
    IMG_OP1_OPEN1,
    IMG_OP1_OPEN2,
    IMG_OP1_OPEN3,
    IMG_OP1_OPEN4,
    IMG_OP1_OPEN5,
    IMG_OP1_OPEN6
);


enum ID {
    TXT_THEPLANET,
    TXT_NEWHOME,
    TXT_LAUNCHING,
    DOOR,
    SHUTTLE,
    END,
};

PlanetColonise::PlanetColonise() : ModeBase("PlanetColonise") {
    stackable = false;
    planet = nullptr;
}

void PlanetColonise::enter() {
    ModeBase::enter(ID::END);

    planet = exostate().get_active_planet();
    player = exostate().get_active_player();

    if (!planet || !planet->exists()) {
        L.fatal("Entered PlanetColonise mode with invalid planet!");
    }

    audio_manager.target_music(MUS_ACHIEVEMENT);

    float planet_scale = 1.f;

#if FEATURE_MULTISIZE_PLANETS_COLONISE
    if (planet->get_size() == PLANET_Small) planet_scale = 0.6;
    if (planet->get_size() == PLANET_Medium) planet_scale = 0.8;
#endif

    draw_manager.draw(IMG_INTRO_SPACE);
    draw_manager.draw(
        planet->sprites()->bulletin_bg_trans,
        {RES_X/2, RES_Y/2 - 40,
         .5, .5, planet_scale, planet_scale});
    draw_manager.save_background();

    time = 0;
    stage = PlanetOverview0;
}

void PlanetColonise::exit() {
    if (exostate().get_n_planets(player) == 1) {
        if (exostate().is_recommended_planet(planet)) {
            achievement_manager.unlock(ACH_BestStartWorld);
        }
    }

    ModeBase::exit();
}

ExodusMode PlanetColonise::update(float dt) {
    size_t n_chars = 1 + max((time - TEXT_START_DELAY) * CHARS_PER_SECOND, 0);
    char text[40];

    if (draw_manager.clicked()) {
        return ExodusMode::MODE_PlanetMap;
    }

    switch (stage) {
        case PlanetOverview0:
            {
                snprintf(text, n_chars, "The planet %s.", planet->get_name());
                draw_manager.draw_text(
                    id(ID::TXT_THEPLANET),
                    text,
                    Justify::Left,
                    TEXT_X, TEXT_Y,
                    COL_TEXT);
                if (n_chars > strlen(text) + 14) {
                    time = 0;
                    stage = PlanetOverview1;
                }
            }
            break;
        case PlanetOverview1:
            {
                snprintf(text, n_chars, "Our people have found a new home.");
                draw_manager.draw_text(
                    id(ID::TXT_NEWHOME),
                    text,
                    Justify::Left,
                    TEXT_X, TEXT_Y + 20,
                    COL_TEXT);
                if (n_chars > strlen(text) + 26) {
                    time = 0;
                    draw_manager.draw(TGT_Secondary, IMG_BG_SHUTTLE);
                    draw_manager.pixelswap_start();
                    stage = Pixelswap;
                }
            }
            break;
        case Pixelswap:
            {
                if (draw_manager.pixelswap_active()) {
                    time = 0;
                    break;
                }

                draw_manager.save_background();
                time = 0;
                stage = LaunchingText;
            }
            break;
        case LaunchingText:
            {
                snprintf(text, n_chars, "Launching of colonist shuttle.");
                draw_manager.draw_text(
                    id(ID::TXT_LAUNCHING),
                    text,
                    Justify::Left,
                    TEXT_X, TEXT_Y,
                    COL_TEXT);

                if (n_chars > strlen(text) + 20) {
                    time = 0;
                    stage = Doors;
                }
            }
            break;
        case Doors:
            {
                float interp = fclamp(time / DOOR_LEN, 0, 1);
                draw_manager.draw(
                    id(ID::DOOR),
                    anim_doors.interp(interp),
                    {270, 228, 0, 0, 2, 2});

                if (time > DOOR_LEN + 1.8) {
                    draw_manager.draw(IMG_INTRO_SPACE);
                    draw_manager.draw(
                        IMG_SC1_PICTURE,
                        {280, 260, .5f, .5f, 2, 2});
                    draw_manager.draw(
                        planet->sprites()->map_bg,
                        {100, 320,
                         0, 0, 1, 1});
                    draw_manager.save_background();
                    time = 0;
                    stage = Shuttle;
                }
            }
            break;
        case Shuttle:
            {
                if (time < SHUTTLE_DELAY) {
                    break;
                }

                float i = fclamp((time - SHUTTLE_DELAY) / (SHUTTLE_END - SHUTTLE_DELAY), 0, 1);
                int x = SHUTTLE_START_X + i*(SHUTTLE_END_X - SHUTTLE_START_X);
                int y = SHUTTLE_START_Y + i*(SHUTTLE_END_Y - SHUTTLE_START_Y);
                float sz = i * SHUTTLE_END_SZ;

                draw_manager.draw(
                    id(ID::SHUTTLE),
                    IMG_SC1_SHUTTLE,
                    {x, y, .5f, .5f, sz, sz});

                if (time > SHUTTLE_END) {
                    return ExodusMode::MODE_PlanetMap;
                }
            }
            break;
    }

    time += dt;

    return ExodusMode::MODE_None;
}
