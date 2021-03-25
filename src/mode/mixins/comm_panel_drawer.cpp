#include "comm_panel_drawer.h"

#include "assetpaths.h"
#include "shared.h"

#include "util/str.h"

const int COMM_BORDER = 8;
const int COMM_W = 580;
const int COMM_H = 206 + (COMM_BORDER + 1) * 2;
const int COMM_X = (RES_X / 2) - (COMM_W / 2);
const int COMM_Y = (RES_Y / 2) - (COMM_H / 2);
const int COMM_RCOL_X = COMM_X + 196 + COMM_BORDER * 2;
const float COMM_SPEECH_SPEED = 10.f;

const Anim comm_anim_human_static(  1, IMG_LD0_LD0);
const Anim comm_anim_yokon_static(  1, IMG_LD1_LD1);
const Anim comm_anim_teri_static(   1, IMG_LD2_LD2);
const Anim comm_anim_urkash_static( 1, IMG_LD3_LD3);
const Anim comm_anim_gordoon_static(1, IMG_LD4_LD4);
const Anim comm_anim_rebels_static( 1, IMG_LD5_LD5);

CommPanelDrawer::CommPanelDrawer() {
    comm_text[0] = comm_text0; comm_text[1] = comm_text1; comm_text[2] = comm_text2;
    comm_text[3] = comm_text3; comm_text[4] = comm_text4; comm_text[5] = comm_text5;

    for (int i = 0; i < 6; ++i) {
        strncpy(comm_text[i], "", 1);
    }

    strncpy(comm_title, "", 1);
    strncpy(comm_img_caption_upper, "", 1);
    strncpy(comm_img_caption_lower, "", 1);

    new (&comm_anim) Anim(1, IMG_LD0_LD0);

    comm_mouseover_text = -1;
    comm_time_since_text_mouseover = 0;
    comm_text_interactive_mask = 0;
    _comm_is_open = false;

    comm_other = nullptr;
}

void CommPanelDrawer::comm_update(float dt) {
    comm_time += dt;
    comm_time_since_text_mouseover += dt;
    comm_draw_text();
}

void CommPanelDrawer::comm_draw_text() {
    if (comm_speech) {
        int max_chars_to_draw = COMM_SPEECH_SPEED * comm_time;
        char comm_title_tmp[COMM_MAX_TEXT];
        strncpy(comm_title_tmp, comm_title, max_chars_to_draw);
        comm_title_tmp[max_chars_to_draw] = '\0';
        draw_manager.draw_text(
            id_comm_title,
            comm_title_tmp,
            Justify::Left,
            COMM_RCOL_X + 8,
            COMM_Y + 11,
            COL_TEXT_SPEECH);

        if (max_chars_to_draw >= comm_title_len) {
            comm_speech = false;
        }

        return;
    }

    bool mouseover_any = false;
    // FIXME: We can optimise redraws by only drawing text that has just stopped
    // or having or currently has the mouse over it - but we'll also need to know whether
    // this is a redraw vs an initial draw...
    for (int i = 0; i < 6; ++i) {
        if (comm_text_interactive_mask & (1 << i)) {
            draw_manager.set_selectable(id_text[i]);
        }

        draw_manager.draw_text(
            id_text[i],
            comm_text[i],
            Justify::Left,
            COMM_RCOL_X + 8,
            comm_text_y(i),
            COL_TEXT);
    }

    if (!mouseover_any) {
        comm_mouseover_text = -1;
    }
}

void CommPanelDrawer::comm_set_title(const char* text, ...) {
    va_list args;
    va_start(args, text);
    vsnprintf(comm_title, COMM_MAX_TEXT - 1, text, args);
    va_end(args);

    comm_title_len = strnlen(comm_title, COMM_MAX_TEXT);
    comm_speech = false;
}

void CommPanelDrawer::comm_set_speech(const char* text, ...) {
    va_list args;
    va_start(args, text);
    vsnprintf(comm_title, COMM_MAX_TEXT - 1, text, args);
    va_end(args);

    comm_title_len = strnlen(comm_title, COMM_MAX_TEXT);
    comm_speech = true;
}

void CommPanelDrawer::comm_set_race(Race race) {
    switch (race) {
        case RACE_Human:
            comm_set_img(CI_Human);
            break;
        case RACE_Yokon:
            comm_set_img(CI_Yokon);
            break;
        case RACE_Teri:
            comm_set_img(CI_Teri);
            break;
        case RACE_Urkash:
            comm_set_img(CI_Urkash);
            break;
        case RACE_Gordoon:
            comm_set_img(CI_Gordoon);
            break;
    }
}

void CommPanelDrawer::comm_set_img(CommImg img) {
    switch (img) {
        case CI_Human:
            comm_anim = comm_anim_human_static;
            break;
        case CI_Yokon:
            comm_anim = comm_anim_yokon_static;
            break;
        case CI_Teri:
            comm_anim = comm_anim_teri_static;
            break;
        case CI_Urkash:
            comm_anim = comm_anim_urkash_static;
            break;
        case CI_Gordoon:
            comm_anim = comm_anim_gordoon_static;
            break;
        case CI_Rebels:
            comm_anim = comm_anim_rebels_static;
            break;
    }
}

void CommPanelDrawer::comm_set_img_caption(const char* text) {
    comm_set_img_caption_lower(text);
}

void CommPanelDrawer::comm_set_img_caption_upper(const char* text, ...) {
    va_list args;
    va_start(args, text);
    vsnprintf(comm_img_caption_upper, COMM_MAX_TEXT - 1, text, args);
    va_end(args);

    capitalise(comm_img_caption_upper, COMM_MAX_TEXT);
}

void CommPanelDrawer::comm_set_img_caption_lower(const char* text, ...) {
    va_list args;
    va_start(args, text);
    vsnprintf(comm_img_caption_lower, COMM_MAX_TEXT - 1, text, args);
    va_end(args);

    capitalise(comm_img_caption_lower, COMM_MAX_TEXT);
}

void CommPanelDrawer::comm_set_text(int idx, const char* in_text, ...) {
    if (idx >= 6) {
        L.fatal("Tried to set invalid text index %d to %s", idx, in_text);
    }

    va_list args;
    va_start(args, in_text);
    vsnprintf(comm_text[idx], COMM_MAX_TEXT - 1, in_text, args);
    va_end(args);
}

void CommPanelDrawer::comm_set_text_interactive_mask(unsigned char mask) {
    comm_text_interactive_mask = mask;
}

void CommPanelDrawer::comm_open(CommSend input) {
    comm_init(input);

    id_comm_panel = draw_manager.new_sprite_id();
    id_comm_bg_t  = draw_manager.new_sprite_id();
    id_comm_bg_b  = draw_manager.new_sprite_id();
    id_comm_title = draw_manager.new_sprite_id();
    id_comm_img = draw_manager.new_sprite_id();
    id_comm_buttons = draw_manager.new_sprite_id();
    id_comm_buttons_bg = draw_manager.new_sprite_id();

    for (int i = 0; i < 6; ++i) {
        id_text[i] = draw_manager.new_sprite_id();
    }

    draw_manager.fill(
        id_comm_panel,
        {COMM_X, COMM_Y,
         COMM_W, COMM_H},
         COL_BORDERS);
    draw_manager.fill(
        {COMM_X + COMM_BORDER,
         COMM_Y + COMM_BORDER,
         196, 208},
         {0, 0, 0});
    draw_manager.fill_pattern(
        id_comm_bg_t,
        {COMM_RCOL_X,
         COMM_Y + COMM_BORDER,
         COMM_X + COMM_W - COMM_RCOL_X - COMM_BORDER,
         28});
    draw_manager.fill_pattern(
        id_comm_bg_b,
        {COMM_RCOL_X,
         COMM_Y + 28 + COMM_BORDER*2,
         COMM_X + COMM_W - COMM_RCOL_X - COMM_BORDER,
         172});

    draw_manager.draw(
        id_comm_img,
        comm_anim.frame(0),
        {COMM_X + COMM_BORDER + 1,
         COMM_Y + COMM_BORDER + 1,
         0, 0, 1, 1});

    if (!comm_speech) {
        draw_manager.draw_text(
            id_comm_title,
            comm_title,
            Justify::Left,
            COMM_RCOL_X + 8,
            COMM_Y + 11,
            COL_TEXT);
    }

    draw_manager.draw_text(
        Font::Tiny,
        comm_img_caption_upper,
        Justify::Left,
        COMM_X + COMM_BORDER + 6,
        COMM_Y + COMM_H - COMM_BORDER - 28,
        COL_TEXT2);

    draw_manager.draw_text(
        Font::Tiny,
        comm_img_caption_lower,
        Justify::Left,
        COMM_X + COMM_BORDER + 6,
        COMM_Y + COMM_H - COMM_BORDER - 16,
        COL_TEXT2);

    _comm_is_open = true;

    comm_send(input);
    comm_draw_text();
}

void CommPanelDrawer::comm_prepare(int text_slots) {
    comm_time = 0;

    comm_text_slots = text_slots;

    comm_mouseover_text = -1;
    comm_time_since_text_mouseover = 0;

    if (comm_text_slots <= 0) {
        comm_text_slots = 1;
    }

    comm_show_buttons(false);
}

void CommPanelDrawer::comm_close() {
    if (!_comm_is_open) {
        L.fatal("Attempt to close comm panel whilst not open");
    }

    for (int i = 0; i < 6; ++i) {
        draw_manager.release_sprite_id(id_text[i]);
    }

    draw_manager.release_sprite_id(id_comm_title);
    draw_manager.release_sprite_id(id_comm_img);
    draw_manager.release_sprite_id(id_comm_buttons);
    draw_manager.release_sprite_id(id_comm_buttons_bg);

    draw_manager.draw(id_comm_bg_t, nullptr);
    draw_manager.draw(id_comm_bg_b, nullptr);

    draw_manager.release_sprite_id(id_comm_bg_t);
    draw_manager.release_sprite_id(id_comm_bg_b);

    draw_manager.draw(id_comm_panel, nullptr);
    draw_manager.release_sprite_id(id_comm_panel);
    _comm_is_open = false;

    // Wipe all info
    for (int i = 0; i < 6; ++i) {
        strncpy(comm_text[i], "", 1);
    }

    comm_mouseover_text = -1;
    comm_time_since_text_mouseover = 0;
    comm_text_interactive_mask = 0;

    strncpy(comm_title, "", 1);
    strncpy(comm_img_caption_upper, "", 1);
    strncpy(comm_img_caption_lower, "", 1);

    comm_set_img(CI_Human);
}

void CommPanelDrawer::comm_ensure_closed() {
    if (_comm_is_open)
        comm_close();
}

bool CommPanelDrawer::comm_is_open() {
    return _comm_is_open;
}

CommAction CommPanelDrawer::comm_check_action() {
    if (comm_speech) {
        return CA_None;
    }

    SpriteClick click;
    click = draw_manager.query_click(id_comm_buttons);
    if (click.id) {
        return click.x < 0.55 ? CA_Proceed : CA_Abort;
    }
    for (int i = 0; i < 6; ++i) {
        if (draw_manager.query_click(id_text[i]).id) {
            return (CommAction)((int)CA_Text0 + i);
        }
    }
    return CA_None;
}

int CommPanelDrawer::comm_text_y(int idx) {
    return COMM_Y + 36 + COMM_BORDER*2 + idx*120/comm_text_slots;
}

void CommPanelDrawer::comm_show_buttons(bool show) {
    if (show) {
        draw_manager.draw(
            id_comm_buttons,
            IMG_BR1_EXPORT,
            {COMM_RCOL_X + 2,
             COMM_Y + COMM_H - COMM_BORDER - 2,
             0, 1, 1, 1});

        draw_manager.fill(
            id_comm_buttons_bg,
            {COMM_RCOL_X + 4 + 236,
             COMM_Y + COMM_H - COMM_BORDER - 28,
             118, 26},
             COL_BORDERS);
    } else {
        draw_manager.draw(id_comm_buttons_bg, nullptr);
        draw_manager.draw(id_comm_buttons, nullptr);
    }
}


void CommPanelDrawer::comm_init(CommSend input) {
    comm_player = exostate.get_active_player();
    comm_planet = exostate.get_active_planet();
    comm_other = nullptr;

    if (comm_planet && comm_planet->is_owned()) {
        comm_other = exostate.get_player(comm_planet->get_owner());
    }

    switch (input) {
        case DIA_S_PlanFly:
            comm_set_img(CI_Human);
            comm_set_title("Message from counsellor");
            comm_set_img_caption("COUNSELLOR");
            break;
        case DIA_S_HailPlanet:
            comm_set_img_caption_upper(comm_other->get_full_name());
            comm_set_img_caption_lower("RACE: %s", comm_other->get_race_str());
            comm_set_race(comm_other->get_race());
        default:
            L.warn("Unhandled comm input on init: %d", (int)input);
    }
}

void CommPanelDrawer::comm_send(CommSend input) {
    switch (input) {
        case DIA_S_PlanFly:
            comm_prepare(6);
            // TODO: Need way to pass star / space guild
            comm_set_text(0, "For our flight to the space");
            comm_set_text(1, "guild, we need X months.");
            //comm_set_text(0, "For our flight to the star");
            //comm_set_text(1, "XXX, we need X months.");
            comm_set_text(2, "A pirate attack is unlikely.");
            comm_set_text(4, "Do you wish to start?");
            comm_show_buttons(true);
            break;
        case DIA_S_HailPlanet:
            comm_prepare(4);
            // TODO: "Welcome to X" / "You again?"
            comm_set_speech("What do you want, %s...?", comm_player->get_name());
            comm_set_text(0, "Let us not talk. I want %s.", comm_planet->get_name());
            comm_set_text(1, "I wish to trade.");
            comm_set_text(2, "I have an interesting offer.");
            comm_set_text(3, "I have something to say.");
            comm_set_text_interactive_mask(0xF);
            comm_recv(DIA_R_Greeting);
            break;
        default:
            L.warn("Unhandled comm input on send: %d", (int)input);
            break;
    }
}

void CommPanelDrawer::comm_recv(CommRecv output) {
    comm_state = output;
}
