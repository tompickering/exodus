#include "galaxy_intro.h"

#include "mixins/panel_drawer.h"

#include "assetpaths.h"

#define GALAXY_INTRO_TIME 10

static const int n_lines = 14;
static const char* text[] = {
    "Our fleet has arrived at the new galaxy.",
    "We have found an institution here",
    "that calls itself the 'Space Guild'.",
    "",
    "Our ships have entered the orbit",
    "of the Space Guild Station,",
    "Our people need somebody who is able",
    "to lead and to protect them in this",
    "time of uncertainty and anxiety.",
    "We have chosen you because you have",
    "proven your skills before.",
    "",
    "We will call you '%s' as it is usual",
    "in this galaxy.",
};

enum ID {
    TEXT0,
    TEXT1,
    TEXT2,
    TEXT3,
    TEXT4,
    TEXT5,
    TEXT6,
    TEXT7,
    TEXT8,
    TEXT9,
    TEXT10,
    TEXT11,
    TEXT12,
    TEXT13,
    END,
};

GalaxyIntro::GalaxyIntro() : ModeBase("GalaxyIntro") {
    time = 0;
}

void GalaxyIntro::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(
        TGT_Secondary,
        IMG_CT2_EXPORT,
        {7, 7, 0, 0, 1, 1});
    draw_manager.fill(TGT_Secondary, {0, 0, RES_X, 7}, COL_BORDERS);
    draw_manager.fill(TGT_Secondary, {0, 403, RES_X, 7}, COL_BORDERS);
    draw_manager.fill(TGT_Secondary, {0, 0, 7, 403}, COL_BORDERS);
    draw_manager.fill(TGT_Secondary, {RES_X-7, 0, 7, 403}, COL_BORDERS);
    DrawArea area = {0, 0, RES_X, PNL_TOP};
    draw_manager.pixelswap_start(&area);

    audio_manager.target_music(MUS_GAMESTART);

    Player *player = exostate().get_active_player();
    snprintf(title_text, sizeof(title_text), text[12], player->get_title());

    time = 0;
    fade_started = false;
}

const int INDENT_X = RES_X / 2;
const int INDENT_Y = 60;
const int SEP_Y = 20;

ExodusMode GalaxyIntro::update(float delta) {
    if (draw_manager.pixelswap_active()) {
        return ExodusMode::MODE_None;
    }

    if (time == 0) {
        draw_manager.save_background();
    }

    Player *player = exostate().get_active_player();
    player->set_intro_seen();

    if (input_manager.consume(K_Space)) {
        return ExodusMode::MODE_Pop;
    }

    time += delta;

    float progress = time / GALAXY_INTRO_TIME;
    if (progress > 1) progress = 1;

    int this_text_idx = (int) (progress * n_lines);
    if (this_text_idx > n_lines - 1) this_text_idx = n_lines - 1;

    unsigned char grey;
    if (this_text_idx <= n_lines) {
        const char* this_text = text[this_text_idx];
        if (this_text_idx == 12) {
            this_text = (const char*)(&title_text);
        }

        if (this_text_idx > 0) {
            int last_text_idx = this_text_idx - 1;
            const char* last_text = text[last_text_idx];
            if (last_text_idx == 12) {
                last_text = (const char*)(&title_text);
            }
            draw_manager.draw_text(
                id(ID::TEXT0 + last_text_idx),
                last_text,
                Justify::Centre,
                INDENT_X,
                INDENT_Y + (this_text_idx - 1) * SEP_Y,
                {0xD0, 0xD0, 0xD0});
        }
        float this_text_interp = n_lines * fmod(progress, 1.f/n_lines);
        grey = (char)(this_text_interp * 0xD0);
        draw_manager.draw_text(
            id(ID::TEXT0 + this_text_idx),
            this_text,
            Justify::Centre,
            INDENT_X,
            INDENT_Y + this_text_idx * SEP_Y,
            {grey, grey, grey});
    }

    if (time > 20 && !fade_started) {
        fade_started = true;
        audio_manager.fade_out(1000);
    }

    if (time > 21) {
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
