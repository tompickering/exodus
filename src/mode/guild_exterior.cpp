#include "guild_exterior.h"

#include <cmath>

#include "assetpaths.h"

const int HQ_X = 310;
const int HQ_Y = 30;
const int HQ_W = 80;
const int HQ_H = 80;

const int BAR_X = 300;
const int BAR_Y = 300;
const int BAR_W = 80;
const int BAR_H = 80;

const int HANGAR_X = 160;
const int HANGAR_Y = 300;
const int HANGAR_W = 80;
const int HANGAR_H = 80;

enum ID {
    LIGHTCAP_TL,
    LIGHTCAP_TR,
    LIGHTCAP_BL,
    LIGHTCAP_BR,
    UFO0,
    UFO1,
    TRANSPORTER,
    LOC_TEXT,
    END,
};

GuildExterior::GuildExterior() : ModeBase("GuildExterior") {
}

void GuildExterior::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_SG1_GUILD);
    draw_manager.save_background();
    draw_manager.show_cursor(true);
    blink_cycle = 0;
    ufo0_x = 180.f;
    ufo0_y = 180.f;
    ufo1_x = 90.f;
    ufo1_y = 190.f;
    transport_time = 0.f;
}

ExodusMode GuildExterior::update(float delta) {
    if (draw_manager.fade_active()) {
        return ExodusMode::MODE_None;
    }

    blink_cycle = fmod(blink_cycle + delta, 2.f);
    transport_time += delta;

    // Lights
    if (blink_cycle > 0.15) {
        draw_manager.draw(id(ID::LIGHTCAP_TL), IMG_SG1_LGHT1, { 18,  48, 0.5, 0.5, 1, 1});
        draw_manager.draw(id(ID::LIGHTCAP_TR), IMG_SG1_LGHT2, {572,  21, 0.5, 0.5, 1, 1});
    } else {
        draw_manager.draw(id(ID::LIGHTCAP_TL), nullptr);
        draw_manager.draw(id(ID::LIGHTCAP_TR), nullptr);
    }

    if (blink_cycle < 0.65 || blink_cycle > 0.8) {
        draw_manager.draw(id(ID::LIGHTCAP_BL), IMG_SG1_LGHT3, {200, 263, 0.5, 0.5, 1, 1});
        draw_manager.draw(id(ID::LIGHTCAP_BR), IMG_SG1_LGHT4, {474, 360, 0.5, 0.5, 1, 1});
    } else {
        draw_manager.draw(id(ID::LIGHTCAP_BL), nullptr);
        draw_manager.draw(id(ID::LIGHTCAP_BR), nullptr);
    }

    // UFOs
    ufo0_x += delta * 9;
    ufo0_y += delta * 4;

    ufo1_x += delta * 12;
    ufo1_y += delta * 5;

    if (ufo0_x > RES_X + 30) {
        ufo0_x = -30.f;
        ufo0_y = 90.f;
    }

    if (ufo1_x > RES_X + 30) {
        ufo1_x = -70.f;
        ufo1_y = 110.f;
    }

    draw_manager.draw(id(ID::UFO0), IMG_SG1_UFO1, {(int)ufo0_x, (int)ufo0_y, 0.5, 0.5, 2, 2});
    draw_manager.draw(id(ID::UFO1), IMG_SG1_UFO2, {(int)ufo1_x, (int)ufo1_y, 0.5, 0.5, 2, 2});

    // Transporter
    const char *trans_spr = IMG_SG1_TRANS1;

    if (transport_time > 1.8) {
        if (fmod(transport_time, 0.9) > 0.6) {
            trans_spr = IMG_SG1_TRANS3;
        } else if (fmod(transport_time, 0.9) > 0.3) {
            trans_spr = IMG_SG1_TRANS2;
        }
    }

    float transp_y = 404;
    if (transport_time > 3) {
        float interp = (transport_time - 3);
        interp = interp > 1 ? 1 : interp;
        transp_y += interp * 4;
    }

    float scale = 1.f;
    if (transport_time > 5) {
        scale = 1.f - (transport_time - 5) * 0.2;
    }

    if (scale > 0) {
        draw_manager.draw(id(ID::TRANSPORTER), trans_spr, {333, transp_y, 0.5, 0.5, scale, scale});
    } else {
        draw_manager.draw(id(ID::TRANSPORTER), nullptr);
    }

    bool click = draw_manager.clicked();
    if (mouse_over_hq()) {
        draw_manager.draw_text(
            id(ID::LOC_TEXT),
            "Guildmaster Headquarters",
            Justify::Left,
            10, RES_Y - 30,
            {0xFF, 0xFF, 0xFF});
        if (click)
            return ExodusMode::MODE_GuildHQ;
    } else if (mouse_over_bar()) {
        draw_manager.draw_text(
            id(ID::LOC_TEXT),
            "The Stardust Bar",
            Justify::Left,
            10, RES_Y - 30,
            {0xFF, 0xFF, 0xFF});
        if (click)
            return ExodusMode::MODE_GuildBar;
    } else if (mouse_over_hangar()) {
        draw_manager.draw_text(
            id(ID::LOC_TEXT),
            "Shuttle Hangar",
            Justify::Left,
            10, RES_Y - 30,
            {0xFF, 0xFF, 0xFF});
        if (click)
            return ExodusMode::MODE_GalaxyMap;
    } else {
        draw_manager.draw(
            id(ID::LOC_TEXT),
            nullptr);
    }

    return ExodusMode::MODE_None;
}

bool GuildExterior::mouse_over_hq() {
    MousePos m = input_manager.get_mouse_pos();
    return m.x >= HQ_X
        && m.x <= HQ_X + HQ_W
        && m.y >= HQ_Y
        && m.y <= HQ_Y + HQ_H;
}

bool GuildExterior::mouse_over_bar() {
    MousePos m = input_manager.get_mouse_pos();
    return m.x >= BAR_X
        && m.x <= BAR_X + BAR_W
        && m.y >= BAR_Y
        && m.y <= BAR_Y + BAR_H;
}

bool GuildExterior::mouse_over_hangar() {
    MousePos m = input_manager.get_mouse_pos();
    return m.x >= HANGAR_X
        && m.x <= HANGAR_X + HANGAR_W
        && m.y >= HANGAR_Y
        && m.y <= HANGAR_Y + HANGAR_H;
}
