#include "comm_panel_drawer.h"

#include "input/input.h"

#include "assetpaths.h"
#include "shared.h"

#include "util/str.h"
#include "util/value.h"

extern INPUTMANAGER input_manager;

const int COMM_BORDER = 8;
const int COMM_W = 580;
const int COMM_H = 206 + (COMM_BORDER + 1) * 2;
const int COMM_X = (RES_X / 2) - (COMM_W / 2);
const int COMM_Y = (RES_Y / 2) - (COMM_H / 2);
const int COMM_RCOL_X = COMM_X + 196 + COMM_BORDER * 2;
const float COMM_SPEECH_SPEED = 10.f;

Anim comm_anim_human(            7, IMG_LD0_LD0  , IMG_LD0_LD0_1,
                                    IMG_LD0_LD0_2, IMG_LD0_LD0_3,
                                    IMG_LD0_LD0_2, IMG_LD0_LD0_1,
                                    IMG_LD0_LD0);
Anim comm_anim_human_thoughtful( 1, IMG_CS2_C  );
Anim comm_anim_human_confident(  1, IMG_CS3_C  );
Anim comm_anim_human_planet(     1, IMG_CS4_C  );
Anim comm_anim_yokon(            1, IMG_LD1_LD1  , IMG_LD1_LD1_1,
                                    IMG_LD1_LD1_2, IMG_LD1_LD1_3,
                                    IMG_LD1_LD1_2, IMG_LD1_LD1_1,
                                    IMG_LD1_LD1);
Anim comm_anim_teri(             7, IMG_LD2_LD2  , IMG_LD2_LD2_1,
                                    IMG_LD2_LD2_2, IMG_LD2_LD2_3,
                                    IMG_LD2_LD2_2, IMG_LD2_LD2_1,
                                    IMG_LD2_LD2);
Anim comm_anim_urkash(           7, IMG_LD3_LD3  , IMG_LD3_LD3_1,
                                    IMG_LD3_LD3_2, IMG_LD3_LD3_3,
                                    IMG_LD3_LD3_2, IMG_LD3_LD3_1,
                                    IMG_LD3_LD3);
Anim comm_anim_gordoon(          7, IMG_LD4_LD4  , IMG_LD4_LD4_1,
                                    IMG_LD4_LD4_2, IMG_LD4_LD4_3,
                                    IMG_LD4_LD4_2, IMG_LD4_LD4_1,
                                    IMG_LD4_LD4);
Anim comm_anim_rebels(           1, IMG_LD5_LD5);

Anim anim_throbber(9,
    IMG_GF4_CLOGO1, IMG_GF4_CLOGO2, IMG_GF4_CLOGO3,
    IMG_GF4_CLOGO4, IMG_GF4_CLOGO5, IMG_GF4_CLOGO6,
    IMG_GF4_CLOGO7, IMG_GF4_CLOGO8, IMG_GF4_CLOGO9);

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
    comm_text_disabled_mask = 0;
    comm_exit_anim_time = 0;
    comm_exit_anim_active = false;
    _comm_is_open = false;

    comm_other = nullptr;

    comm_is_counsellor = false;
}

CommAction CommPanelDrawer::comm_update(float dt) {
    comm_time += dt;
    comm_time_since_open += dt;

    if (comm_exit_anim_active) {
        comm_exit_anim_update(dt);
    } else {
        if (comm_enable_distort) {
            float interp = fmax(0, fmod(comm_time_since_open*1.4, 6) - 5);
            draw_manager.draw(
                id_comm_img,
                comm_anim.interp(interp),
                {COMM_X + COMM_BORDER + 1,
                 COMM_Y + COMM_BORDER + 1,
                 0, 0, 1, 1});

            // If the draw manager repairs sprites on top when ones underneath
            // change, there may be no need for this
            comm_draw_over_img_elements();
        }

        if (comm_enable_throbber) {
            float interp = fmod(comm_time_since_open/2, 1.f);
            draw_manager.draw(
                id_comm_throbber,
                anim_throbber.interp(interp),
                {COMM_X + 176, COMM_Y + 194,
                .5, .5, 1, 1});
        }

        comm_time_since_text_mouseover += dt;
        comm_process_responses();
        comm_draw_text();
    }

    return comm_action_check();
}

void CommPanelDrawer::comm_exit_anim(CommAction act) {
    comm_exit_anim_active = true;
    comm_exit_anim_time = 0;
    comm_exit_anim_action = act;
    draw_manager.draw(id_comm_throbber, nullptr);
}

void CommPanelDrawer::comm_exit_anim_update(float dt) {
    comm_exit_anim_active = true;

    float interp = 0;

    const float shrink_time = 0.3f;
    const float line_time = 0.6f;

    if (comm_exit_anim_time < shrink_time) {
        interp = fmin(1, comm_exit_anim_time / shrink_time);
        float sc_v = 1.f - interp;
        float sc_h = 1.f - (interp/3.f);
        draw_manager.draw(
            id_comm_img,
            comm_anim.frame(0),
            {COMM_X + COMM_BORDER + 98,
             COMM_Y + COMM_BORDER + 104,
             .5f, .5f, sc_h, sc_v});
    } else if (comm_exit_anim_time < shrink_time + line_time) {
        interp = fmin(1, (comm_exit_anim_time-shrink_time) / line_time);
        unsigned char r, g, b = 0;
        r = g = max(0, 0xFF - max(0, (interp*1.4)*0xFF));
        b = 0xFF;
        float fade_interp = 1-interp;
        r *= fade_interp;
        g *= fade_interp;
        b *= fade_interp;
        draw_manager.fill(
            id_comm_img,
            {COMM_X + COMM_BORDER + 1,
             COMM_Y + COMM_BORDER + 103,
             194, 2},
             {r, g, b});
    } else if (comm_exit_anim_time > 2) {
        comm_exit_anim_active = false;
        comm_exit_anim_time = 0;
        comm_report_action = comm_exit_anim_action;
        return;
    }

    comm_exit_anim_time += dt;
}

void CommPanelDrawer::comm_complete_speech() {
    if (comm_speech) {
        comm_speech = false;
        draw_manager.draw_text(
            id_comm_title,
            comm_title,
            Justify::Left,
            COMM_RCOL_X + 8,
            COMM_Y + 11,
            COL_TEXT_SPEECH);
    }
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
        } else {
            draw_manager.unset_selectable(id_text[i]);
        }

        RGB col = COL_TEXT;
        if (comm_text_disabled_mask & (1 << i)) {
            draw_manager.unset_selectable(id_text[i]);
            col = COL_TEXT_GREYED;
        }

        draw_manager.draw_text(
            id_text[i],
            comm_text[i],
            Justify::Left,
            COMM_RCOL_X + 8,
            comm_text_y(i),
            col);
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
            comm_anim = comm_anim_human;
            break;
        case CI_HumanThoughtful:
            comm_anim = comm_anim_human_thoughtful;
            break;
        case CI_HumanConfident:
            comm_anim = comm_anim_human_confident;
            break;
        case CI_Yokon:
            comm_anim = comm_anim_yokon;
            break;
        case CI_Teri:
            comm_anim = comm_anim_teri;
            break;
        case CI_Urkash:
            comm_anim = comm_anim_urkash;
            break;
        case CI_Gordoon:
            comm_anim = comm_anim_gordoon;
            break;
        case CI_Rebels:
            comm_anim = comm_anim_rebels;
            break;
        case CI_HumanPlanet:
            comm_anim = comm_anim_human_planet;
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

void CommPanelDrawer::comm_open(CommSend input) {
    comm_time_since_open = 0;
    comm_enable_distort = false;
    comm_enable_throbber = false;
    comm_is_counsellor = false;

    comm_init(input);

    comm_state = DIA_R_None;
    comm_report_action = CA_None;

    /*
     * There are two ways to use the exit anim, depending on whether
     * the final message is sent or received by the human player.
     *
     * If the last thing said in conversation comes from the HUMAN,
     * then we can call comm_exit_anim() instead of comm_send when
     * that final speech option is selected. This function causes the
     * anim to fire as soon as it is called. This will be inside a
     * DIA_R, reflecting the last thing that was being said to the
     * player. For an example, see DIA_R_AwaitingOrders.
     *
     * If the last thing said in conversation comes from the CPU,
     * then we don't want the anim to fire until after the player
     * has had chance to see the last message from the CPU player.
     * In this case, we will choose the CPU's final message inside
     * a DIA_S. We don't want to start the anim immediately, so
     * instead we set comm_exit_anim_action to a value other than
     * CA_None, and then close comms with comm_recv(DIA_R_Close).
     * For an example, see DIA_S_B_NeverMind.
     */

    comm_exit_anim_active = false;
    comm_exit_anim_action = CA_None;

    id_comm_panel = draw_manager.new_sprite_id();
    id_comm_bg_t  = draw_manager.new_sprite_id();
    id_comm_bg_b  = draw_manager.new_sprite_id();
    id_comm_title = draw_manager.new_sprite_id();
    id_comm_img = draw_manager.new_sprite_id();
    id_comm_img_bg = draw_manager.new_sprite_id();
    id_comm_throbber = draw_manager.new_sprite_id();
    id_comm_ally_trade = draw_manager.new_sprite_id();
    id_comm_ally_nonattack = draw_manager.new_sprite_id();
    id_comm_ally_war = draw_manager.new_sprite_id();
    id_comm_buttons = draw_manager.new_sprite_id();
    id_comm_buttons_bg = draw_manager.new_sprite_id();
    id_comm_adj = draw_manager.new_sprite_id();
    id_comm_adj_ok = draw_manager.new_sprite_id();
    id_comm_input_frame = draw_manager.new_sprite_id();
    id_comm_input_box = draw_manager.new_sprite_id();

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

    draw_manager.fill(
        id_comm_img_bg,
        {COMM_X + COMM_BORDER + 1,
         COMM_Y + COMM_BORDER + 1,
         194, 206},
         {0, 0, 0});
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

    comm_draw_over_img_elements();

    _comm_is_open = true;

    comm_send(input);
    comm_draw_text();
}

void CommPanelDrawer::comm_draw_over_img_elements() {
    if (comm_other && !comm_is_counsellor) {
        int x = 4;
        int p_idx = exostate.get_player_idx(comm_player);
        int o_idx = exostate.get_player_idx(comm_other);
        if (exostate.has_alliance(p_idx, o_idx, ALLY_Trade)) {
            draw_manager.draw(
                id_comm_ally_trade,
                IMG_TS1_ALL1,
                {COMM_X + COMM_BORDER + 1 + x,
                 COMM_Y + COMM_BORDER + 1 + 4,
                 0, 0, 1, 1});
            x += 32;
        }
        if (exostate.has_alliance(p_idx, o_idx, ALLY_NonAttack)) {
            draw_manager.draw(
                id_comm_ally_nonattack,
                IMG_TS1_ALL2,
                {COMM_X + COMM_BORDER + 1 + x,
                 COMM_Y + COMM_BORDER + 1 + 4,
                 0, 0, 1, 1});
            x += 32;
        }
        if (exostate.has_alliance(p_idx, o_idx, ALLY_War)) {
            draw_manager.draw(
                id_comm_ally_war,
                IMG_TS1_ALL3,
                {COMM_X + COMM_BORDER + 1 + x,
                 COMM_Y + COMM_BORDER + 1 + 4,
                 0, 0, 1, 1});
            x += 32;
        }
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
}

void CommPanelDrawer::comm_prepare(int text_slots) {
    comm_time = 0;

    comm_text_slots = text_slots;

    comm_mouseover_text = -1;
    comm_time_since_text_mouseover = 0;

    if (comm_text_slots <= 0) {
        comm_text_slots = 1;
    }

    comm_text_interactive_mask = 0;
    comm_text_disabled_mask = 0;

    comm_show_buttons(false);
    comm_show_adj(false);

    // Clear old per-dialogue-phase info
    for (int i = 0; i < 6; ++i) {
        draw_manager.draw(id_text[i], nullptr);
        comm_text[i][0] = '\0';
    }

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
    draw_manager.release_sprite_id(id_comm_img_bg);
    draw_manager.release_sprite_id(id_comm_throbber);
    draw_manager.release_sprite_id(id_comm_adj);
    draw_manager.release_sprite_id(id_comm_adj_ok);
    draw_manager.release_sprite_id(id_comm_ally_trade);
    draw_manager.release_sprite_id(id_comm_ally_nonattack);
    draw_manager.release_sprite_id(id_comm_ally_war);
    draw_manager.release_sprite_id(id_comm_buttons);
    draw_manager.release_sprite_id(id_comm_buttons_bg);
    draw_manager.release_sprite_id(id_comm_input_frame);
    draw_manager.release_sprite_id(id_comm_input_box);

    draw_manager.draw(id_comm_bg_t, nullptr);
    draw_manager.draw(id_comm_bg_b, nullptr);

    draw_manager.release_sprite_id(id_comm_bg_t);
    draw_manager.release_sprite_id(id_comm_bg_b);

    draw_manager.draw(id_comm_panel, nullptr);
    draw_manager.release_sprite_id(id_comm_panel);
    _comm_is_open = false;

    comm_mouseover_text = -1;
    comm_time_since_text_mouseover = 0;
    comm_text_interactive_mask = 0;
    comm_text_disabled_mask = 0;

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

CommAction CommPanelDrawer::comm_action_check() {
    return comm_report_action;
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

void CommPanelDrawer::comm_show_adj(bool show) {
    if (show) {
        draw_manager.draw(
            id_comm_adj,
            IMG_BR4_EXPORT3,
            {COMM_RCOL_X + 2,
             COMM_Y + COMM_H - COMM_BORDER - 2,
             0, 1, 1, 1});

        draw_manager.draw(
            id_comm_adj_ok,
            IMG_BR6_OK,
            {COMM_RCOL_X + 2 + 236,
             COMM_Y + COMM_H - COMM_BORDER - 2,
             0, 1, 1, 1});
    } else {
        draw_manager.draw(id_comm_adj, nullptr);
        draw_manager.draw(id_comm_adj_ok, nullptr);
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
        case DIA_S_ThrustBroken:
            comm_is_counsellor = false;
            comm_set_img(CI_HumanThoughtful);
            comm_set_title("Message from counsellor");
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_PlanFly:
            comm_set_img(CI_Human);
            comm_set_title("Message from counsellor");
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_PlanSettle:
        case DIA_S_PlanProbe:
            comm_set_img(CI_Human);
            comm_set_title("Message from counsellor");
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_LookAgainBadLaws:
            comm_set_img(CI_HumanThoughtful);
            comm_set_title("Message from counsellor");
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_LookAgainNoEssentials:
            comm_set_img(CI_HumanThoughtful);
            comm_set_title("Message from counsellor");
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_CommsBroken:
            comm_set_img(CI_HumanThoughtful);
            comm_set_title("Message from counsellor");
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_PlanetComm:
            comm_set_img(CI_HumanPlanet);
            comm_set_img_caption("COUNSELLOR");
            comm_enable_throbber = true;
            break;
        case DIA_S_HailPlanet:
            comm_set_img_caption_upper(comm_other->get_full_name());
            comm_set_img_caption_lower("RACE: %s", comm_other->get_race_str());
            comm_set_race(comm_other->get_race());
            comm_enable_throbber = true;
            comm_enable_distort = true;
            break;
        case DIA_S_PlanAttack:
            {
                comm_set_title("Message from counsellor");
                comm_set_img_caption("COUNSELLOR");
                comm_is_counsellor = true;
                comm_prepare(6);
                int army = comm_player->get_fleet().freight.army_size_weighted();
                if (army <= 0) {
                    comm_set_img(CI_HumanThoughtful);
                    comm_set_text(0, "We do not have any troops.");
                    comm_ctx.may_proceed = false;
                } else {
                    int army_enemy = comm_planet->get_army_size_weighted();
                    bool base = comm_planet->has_lunar_base();
                    if (base) {
                        army_enemy += 20;
                    }
                    if (army > army_enemy) {
                        comm_set_img(CI_HumanConfident);
                    } else {
                        comm_set_img(CI_Human);
                    }
                    comm_set_text(0, "Attack %s", comm_planet->get_name());

                    comm_ctx.att_odds = ODDS_Good;
                    if (army_enemy >= army) comm_ctx.att_odds = ODDS_Bad;
                    if (army > army_enemy*2) comm_ctx.att_odds = ODDS_VGood;
                    if (army_enemy > army*2) comm_ctx.att_odds = ODDS_VBad;

                    const char *odds = "";

                    switch (comm_ctx.att_odds) {
                        case ODDS_VBad:
                            odds = "extremely bad";
                            break;
                        case ODDS_Bad:
                            odds = "not good";
                            break;
                        case ODDS_Good:
                            odds = "good";
                            break;
                        case ODDS_VGood:
                            odds = "very good";
                            break;
                        default:
                            L.error("Unknown odds %d", (int)(comm_ctx.att_odds));
                            break;
                    }

                    comm_set_text(2, "The winning chances are %s.", odds);
                    if (base) {
                        comm_set_text(4, "%s owns a lunar base.", comm_planet->get_name());
                    } else {
                        comm_set_text(4, "No lunar base is present.");
                    }
                    comm_set_text(5, "Do you wish to attack?");
                    comm_ctx.may_proceed = true;
                }
            }
            break;
        case DIA_S_NoFirstSave:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_SaveSuccess:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_Human);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_SaveFailure:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_FirstPlanetAdvice:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_FirstPlanetAdviceUrgent:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_FleetNotInSystem:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_AlreadyBombed:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_NoScouts:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_NoCoverBombers:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_NoBombers:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_ArtificialPlanetStarInvalid:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_ArtificialPlanetStarMoveDest:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_FlyAlreadyFlying:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_FlyAlreadyThere:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_Human);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_ZoomButNotVisited:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_Quit:
            comm_set_title("Message from counsellor");
            comm_set_img(CI_HumanThoughtful);
            comm_set_img_caption("COUNSELLOR");
            comm_is_counsellor = true;
            break;
        case DIA_S_CPU_Attack:
            comm_set_img_caption_upper(comm_player->get_full_name());
            comm_set_img_caption_lower("RACE: %s", comm_player->get_race_str());
            comm_set_race(comm_player->get_race());
            break;
        case DIA_S_CPU_Trade:
            comm_set_img_caption_upper(comm_player->get_full_name());
            comm_set_img_caption_lower("RACE: %s", comm_player->get_race_str());
            comm_set_race(comm_player->get_race());
            break;
        case DIA_S_CPU_Offer:
            comm_set_img_caption_upper(comm_player->get_full_name());
            comm_set_img_caption_lower("RACE: %s", comm_player->get_race_str());
            comm_set_race(comm_player->get_race());
            break;
        case DIA_S_B_OpenCommsAttacker:
            comm_set_img_caption_upper(comm_other->get_full_name());
            comm_set_img_caption_lower("RACE: %s", comm_other->get_race_str());
            comm_set_race(comm_other->get_race());
            comm_enable_throbber = true;
            break;
        case DIA_S_B_OpenCommsDefender:
            comm_set_img_caption_upper(comm_player->get_full_name());
            comm_set_img_caption_lower("RACE: %s", comm_player->get_race_str());
            comm_set_race(comm_player->get_race());
            comm_enable_throbber = true;
            break;
        case DIA_S_B_CPU_OpenCommsAttacker:
            comm_set_img_caption_upper(comm_player->get_full_name());
            comm_set_img_caption_lower("RACE: %s", comm_player->get_race_str());
            comm_set_race(comm_player->get_race());
            break;
        case DIA_S_B_CPU_OpenCommsDefender:
            comm_set_img_caption_upper(comm_other->get_full_name());
            comm_set_img_caption_lower("RACE: %s", comm_other->get_race_str());
            comm_set_race(comm_other->get_race());
            break;
        case DIA_S_B_CPU_OpenCommsRebels:
            comm_set_img_caption_upper("REBELS");
            comm_set_img_caption_lower("REBELS");
            comm_set_img(CI_Rebels);
            break;
        default:
            L.warn("Unhandled comm input on init: %d", (int)input);
    }

    comm_planet_name_confirm_time = -1;
}

void CommPanelDrawer::comm_send(CommSend input) {
    int comm_player_idx = -1;
    int comm_other_idx  = -1;

    if (comm_player) {
        comm_player_idx = exostate.get_player_idx(comm_player);
    }

    if (comm_other) {
        comm_other_idx = exostate.get_player_idx(comm_other);
    }

    switch (input) {
        case DIA_S_ThrustBroken:
            comm_prepare(6);
            // SUGGEST: Would be cool to check crew numbers and vary response accordingly
            comm_set_text(0, "The engines of our starship are");
            comm_set_text(1, "heavily damaged. We cannot fly");
            comm_set_text(2, "until they are repaired.");
            comm_set_text(3, "You should check if we have");
            comm_set_text(4, "enough crew members for that.");
            comm_recv(DIA_R_Close);
            break;
        case DIA_S_PlanFly:
            {
                comm_prepare(6);
                Galaxy *gal = exostate.get_galaxy();
                FlyTarget *loc = exostate.loc2tgt(comm_ctx.location);
                const char *pl = (comm_ctx.months==1)?"":"s";
                if (loc == gal->get_guild()) {
                    comm_set_text(0, "For our flight to the space");
                    comm_set_text(1, "guild, we need %d month%s.", comm_ctx.months, pl);
                } else {
                    Star *st = (Star*)loc;
                    comm_set_text(0, "For our flight to the star");
                    comm_set_text(1, "%s, we need %d month%s.", st->name, comm_ctx.months, pl);
                }

                FlyTarget *curr = exostate.loc2tgt(comm_player->get_location().get_target());
                const char* prob_str = "unlikely";
                if (curr->pirates == 1) prob_str = "possible";
                if (curr->pirates == 2) prob_str = "likely";
                if (curr->pirates >= 3) prob_str = "very likely";

                comm_set_text(2, "A pirate attack is %s.", prob_str);
                comm_set_text(4, "Do you wish to start?");
                comm_show_buttons(true);
                comm_recv(DIA_R_ProceedOrAbort);
            }
            break;
        case DIA_S_PlanSettle:
            if (!comm_player->can_afford(comm_planet->get_settlement_cost())) {
                comm_prepare(6);
                comm_set_text(0, "We do not have the money for a");
                comm_set_text(1, "colonization.");
                comm_set_text(3, "(Cost: %d MC)", comm_planet->get_settlement_cost());
                comm_show_buttons(true);
                comm_recv(DIA_R_SettleCannotAfford);
                break;
            }

            if (comm_player->get_starship().bionic_probes > 0) {
                comm_prepare(6);
                comm_set_text(0, "Shall we launch a bionic probe?");
                comm_show_buttons(true);
                comm_recv(DIA_R_SettleQueryProbe);
                break;
            }

            // Still needed, otherwise we hit a div-by-zero when it tries to draw...
            comm_prepare(6);

            comm_recv_proxyto(DIA_S_PlanSettle2);
            break;
        case DIA_S_PlanProbe:
            {
                comm_prepare(6);

                StoneSet buildset;
                buildset.add(STONE_Base);
                buildset.add(STONE_Mine);
                buildset.add(STONE_Plu);
                buildset.add(STONE_City);
                buildset.add(STONE_Inf);
                buildset.add(STONE_Gli);
                buildset.add(STONE_Art);
                buildset.add(STONE_Port0);
                buildset.add(STONE_Port1);
                buildset.add(STONE_Port2);
                buildset.add(STONE_Trade);
                buildset.add(STONE_Village);
                buildset.add(STONE_Park);

                StoneSet freeset;
                freeset.add(STONE_Clear);
                freeset.add(STONE_NaturalSmall);

                comm_set_text(0, "Bionic Probe Results:");
                comm_set_text(2, "Minerals: %d", comm_planet->get_minerals());
                comm_set_text(3, "Buildings: %d", comm_planet->count_stones(buildset));
                comm_set_text(4, "Free space: %d", comm_planet->count_stones(freeset));

                comm_recv(DIA_R_SettleProbeResults);
            }
            break;
        case DIA_S_PlanSettle2:
            if (comm_planet->has_stone(STONE_Village)) {
                comm_prepare(6);
                comm_set_text(0, "Our experts have discovered");
                comm_set_text(1, "intelligent life on this planet. It");
                comm_set_text(2, "seems to be a primitive and");
                comm_set_text(3, "peaceful race, living in small");
                comm_set_text(4, "villages.");
                comm_recv(DIA_R_SettleIntelligentLife);
                break;
            }

            // Still needed, otherwise we hit a div-by-zero when it tries to draw...
            comm_prepare(6);

            comm_recv_proxyto(DIA_S_PlanClaim);
            break;
        case DIA_S_PlanClaim:
            {
                comm_prepare(6);
                const char *size_str = "small";
                if (comm_planet->get_size() == PLANET_Medium) size_str = "medium";
                if (comm_planet->get_size() == PLANET_Large) size_str = "large";
                if (!strnlen(comm_planet->get_name(), 1)) {
                    comm_set_text(0, "Claim this %s planet?",
                            comm_planet->get_class_str_lower());
                } else {
                    comm_set_text(0, "Claim planet %s?",
                            comm_planet->get_name());
                }
                comm_set_text(1, "  Size: %s -> Cost: %d MC",
                        size_str, comm_planet->get_settlement_cost());
                comm_set_text(2, "Advantages / Disadvantages:");
                int textidx = 3;
                switch (comm_planet->get_class()) {
                    case Forest:
                        comm_set_text(textidx++, "  Good basis for agriculture");
                        break;
                    case Desert:
                        comm_set_text(textidx++, "  Possibly rich in minerals");
                        comm_set_text(textidx++, "  Bad basis for agriculture");
                        break;
                    case Volcano:
                        comm_set_text(textidx++, "  Plutonium prod. is very effective");
                        comm_set_text(textidx++, "  Bad basis for agriculture");
                        break;
                    case Rock:
                        comm_set_text(textidx++, "  Mining is very effective");
                        break;
                    case Ice:
                        comm_set_text(textidx++, "  Possibly rich in minerals");
                        comm_set_text(textidx++, "  Bad basis for agriculture");
                        break;
                    case Terra:
                        comm_set_text(textidx++, "  Good basis for agriculture");
                        break;
                    case Artificial:
                        comm_set_text(textidx++, "  The planet may be moved");
                        comm_set_text(textidx++, "  No mining possible");
                        break;
                    default:
                        break;
                }

                if (comm_planet->is_named()) {
                    comm_set_text(textidx++, "  Possibly already built-up");
                }

                comm_show_buttons(true);
                comm_recv(DIA_R_SettlePlanetInfo);
            }
            break;
        case DIA_S_NamePlanet:
            comm_prepare(6);
            comm_set_title("Claim a planet");
            comm_set_text(0, "Please name the new planet.");
            input_manager.start_text_input();

            if (comm_planet->is_named()) {
                input_manager.set_input_text(comm_planet->get_name());
            } else {
                input_manager.set_input_text(comm_planet->get_name_suggestion());
            }

            draw_manager.fill(
                id_comm_input_frame,
                {COMM_RCOL_X + 4, comm_text_y(2) - 2 - BORDER,
                 220 + 2*BORDER, 26 + 2*BORDER},
                COL_BORDERS);
            draw_manager.fill(
                id_comm_input_box,
                {COMM_RCOL_X + 4 + BORDER, comm_text_y(2) - 2,
                 220, 26},
                {0, 0, 0});

            comm_recv(DIA_R_SettleNamePlanet);
            break;
        case DIA_S_NamePlanetConfirm:
            {
                if (comm_planet_name_confirm_time == comm_time) {
                    const char *name = input_manager.get_input_text(PLANET_MAX_NAME);
                    if (comm_player->attempt_spend(comm_planet->get_settlement_cost())) {
                        comm_planet->set_name(name);
                        comm_planet->set_owner(exostate.get_player_idx(comm_player));
                        comm_player->add_trace(TRACE_PlanetsClaimed);
                        comm_set_text(4, "%s is now yours.", name);
                        comm_recv(DIA_R_SettleNamePlanetProceed);
                    } else {
                        comm_report_action = CA_Abort;
                        L.error("Cannot afford planet - but should have checked sooner!");
                    }
                }
            }
            break;
        case DIA_S_ChangeClimate:
            comm_prepare(6);
            comm_set_speech("The change has begun.");
            comm_complete_speech();
            comm_recv(DIA_R_ChangeClimateConfirm);
            break;
        case DIA_S_LookAgainBadLaws:
            comm_prepare(6);
            comm_set_text(0, "I am not sure if your people");
            comm_set_text(1, "accept the new rules you have");
            comm_set_text(2, "created.");
            comm_set_text(3, "Maybe you should take a look at");
            comm_set_text(4, "them again.");
            comm_show_buttons(true);
            comm_recv(DIA_R_ProceedOrAbort);
            break;
        case DIA_S_LookAgainNoEssentials:
            comm_prepare(6);
            comm_set_text(0, "You have to build at least one");
            comm_set_text(1, "command station and one food");
            comm_set_text(2, "unit for its supply.");
            comm_set_text(3, "We have no control over the planet");
            comm_set_text(4, "without a command base.");
            comm_show_buttons(true);
            comm_recv(DIA_R_ProceedOrAbort);
            break;
        case DIA_S_CommsBroken:
            comm_prepare(6);
            // SUGGEST: Would be cool to check crew numbers and vary response accordingly
            comm_set_text(0, "The communication systems are");
            comm_set_text(1, "heavily damaged. We cannot");
            comm_set_text(2, "communicate until they are repaired.");
            comm_set_text(3, "You should check if we have");
            comm_set_text(4, "enough crew members for that.");
            comm_recv(DIA_R_Close);
            break;
        case DIA_S_PlanetComm:
            comm_prepare(4);
            comm_set_speech("What are your orders?");
            comm_complete_speech();
            comm_set_text(0, "Activate a goods transfer.");
            comm_set_text(1, "Start Fleet/AirDef production.");
            if (comm_player->get_location().in_flight()) {
                comm_text_disabled_mask |= 0x3;
            }
            if (comm_player->get_location().get_target() != exostate.get_active_star_idx()) {
                comm_text_disabled_mask |= 0x3;
            }
            if (!comm_planet->has_spaceport()) {
                comm_text_disabled_mask |= 0x3;
            }
            if (comm_planet->get_class() == Artificial) {
                comm_set_text(2, "Move the artificial planet");
                if (!comm_player->has_invention(INV_OrbitalMassThrust)) {
                    comm_text_disabled_mask |= 0x4;
                }
                if (comm_planet->get_star_target() >= 0) {
                    comm_text_disabled_mask |= 0x4;
                }
            } else {
                comm_set_text(2, "Change global climate (500MC)");
                if (!comm_player->has_invention(INV_WeatherInfluence)) {
                    comm_text_disabled_mask |= 0x4;
                }
                if (!comm_player->can_afford(500)) {
                    comm_text_disabled_mask |= 0x4;
                }
            }
            comm_set_text(3, "Never mind...");
            comm_text_interactive_mask = 0xF;
            comm_recv(DIA_R_AwaitingOrders);
            break;
        case DIA_S_HailPlanet:
            comm_prepare(4);
            if (exostate.is_allied(comm_player_idx, comm_other_idx)){
                comm_set_speech("Welcome to %s, %s!",
                                 comm_planet->get_name(),
                                 comm_player->get_name());
            } else {
                comm_set_speech("What do you want, %s...?",
                                comm_player->get_name());
            }
            comm_set_text(0, "Let us not talk. I want %s.", comm_planet->get_name());
            comm_set_text(1, "I wish to trade.");
            if (!comm_planet->trade_possible(comm_player_idx)) {
                comm_text_disabled_mask |= 2;
            }
            comm_set_text(2, "I have an interesting offer.");
            if (exostate.has_all_alliances(comm_player_idx, comm_other_idx)) {
                comm_text_disabled_mask |= 4;
            }
            comm_set_text(3, "I have something to say.");
            comm_text_interactive_mask = 0xF;

            if (exostate.attack_prevented(comm_planet)) {
                comm_text_disabled_mask |= 1;
            }

            comm_recv(DIA_R_Greeting);
            break;
        case DIA_S_Attack:
            if (exostate.has_alliance(comm_player_idx, comm_other_idx, ALLY_NonAttack)) {
                comm_prepare(4);
                if (onein(2)) {
                    comm_set_speech("But we have an ALLIANCE!");
                } else {
                    comm_set_speech("Aren't you my ALLY?");
                }
                comm_ctx.mc = min(comm_player->get_mc()/3, 200);
                // FIXME: This is a bit weird - 50MC->16MC but 49MC->49MC?
                if (comm_ctx.mc < 50) {
                    comm_ctx.mc = comm_player->get_mc();
                }
                comm_set_text(0, "Forgive me! I will pay %dMC!", comm_ctx.mc);
                if (comm_ctx.mc <= 0) {
                    comm_text_disabled_mask |= 0x1;
                }
                comm_set_text(1, "Uh... sorry, I forgot!");
                comm_set_text(2, "What? I cannot remember!");
                comm_set_text(3, "I herewith quit the alliance.");
                comm_text_interactive_mask = 0xF;
                comm_recv(DIA_R_AttackAlly);
            } else {
                exostate.unset_alliances(comm_player_idx, comm_other_idx);
                if (comm_other->get_flag(0) == AI_Hi) {
                    comm_other->set_hostile_to(comm_player_idx);
                }
                int n_planets = exostate.get_n_planets(comm_other);
                int army_size = comm_player->get_fleet().freight.army_size();
                if (army_size>30 && onein(5) && comm_other->can_afford(50) && n_planets<2) {
                    comm_prepare(4);
                    comm_ctx.mc = min(comm_other->get_mc(), 100);
                    comm_ctx.mc = max(comm_ctx.mc - 5, 5);
                    comm_set_speech("I offer you %dMC if you leave.", comm_ctx.mc);
                    comm_set_text(0, "I wish for more.");
                    comm_set_text(1, "I accept this.");
                    comm_set_text(2, "I do not want your money...");
                    comm_text_interactive_mask = 0x7;
                    comm_recv(DIA_R_AttackPayOff);
                } else {
                    comm_prepare(1);
                    int r = rand() % 3;
                    if (r == 2) {
                        if (comm_other->get_flag(0) != AI_Hi) {
                            comm_set_speech("Let us both live in peace.");
                        } else {
                            r = 0;
                        }
                    }
                    switch (r) {
                        case 0:
                            comm_set_speech("The %s are not afraid!",
                                            comm_other->get_race_str());
                            break;
                        case 1:
                            comm_set_speech("Do not risk a war.");
                            break;
                    }

                    if (onein(comm_other->get_flag(0) == AI_Hi ? 2 : 4)) {
                        comm_other->set_hostile_to(comm_player_idx);
                    }

                    comm_recv(DIA_R_NoAttackResponse);
                }
            }
            break;
        case DIA_S_AttackPayOffWantMore:
            if (onein(3)) {
                comm_prepare(4);
                comm_ctx.mc = min(comm_other->get_mc(), 100+(RND(10)*20));
                comm_set_speech("Well... I offer %dMC...", comm_ctx.mc);
                comm_set_text(1, "This is acceptable.");
                comm_set_text(2, "I do not want your money...");
                comm_text_interactive_mask = 0x3;
                comm_recv(DIA_R_AttackPayOffMore);
            } else {
                comm_prepare(1);
                comm_set_speech("You will have to attack.");
                comm_recv(DIA_R_NoAttackResponse);
            }
            break;
        case DIA_S_AttackAllyCompensate:
            if (!comm_player->attempt_spend(comm_ctx.mc)) {
                L.fatal("Not able to afford alliance breach compensation");
            }
            comm_prepare(1);
            if ((comm_other->get_flag(0) == AI_Hi) || onein(5)) {
                exostate.unset_alliances(comm_player_idx, comm_other_idx);
                comm_set_speech("There is no more alliance!");
            } else {
                comm_set_speech("I am deeply disappointed.");
            }
            comm_exit_anim_action = CA_Abort;
            comm_recv(DIA_R_Close);
            break;
        case DIA_S_AttackAllyApology:
            comm_prepare(1);
            if ((comm_other->get_flag(0) == AI_Hi) || onein(2)) {
                exostate.unset_alliances(comm_player_idx, comm_other_idx);
                comm_set_speech("There is no more alliance!");
            } else {
                comm_set_speech("I am deeply disappointed.");
            }
            comm_exit_anim_action = CA_Abort;
            comm_recv(DIA_R_Close);
            break;
        case DIA_S_AttackAllyProceed:
            {
                comm_prepare(1);
                exostate.unset_alliances(comm_player_idx, comm_other_idx);
                comm_player->adjust_reputation(-1);
                comm_player->add_trace(TRACE_AlliancesBroken);
                int r = RND(3);
                if ((r == 2) && (comm_other->get_flag(5) == AI_Hi)) {
                    r = 1;
                }
                switch (r) {
                    case 1:
                        comm_other->set_hostile_to(comm_player_idx);
                        comm_set_speech("You will pay for this.");
                        break;
                    case 2:
                        comm_set_speech("The %s are not afraid!", comm_other->get_race_str());
                        break;
                    case 3:
                        comm_set_speech("%s bastard.", comm_player->get_race_str());
                        comm_other->set_hostile_to(comm_player_idx);
                        break;
                }
                comm_recv(DIA_R_NoAttackResponse);
            }
            break;
        case DIA_S_Trade:
            {
                if (exostate.has_alliance(comm_player_idx, comm_other_idx, ALLY_Trade)) {
                    comm_prepare(1);
                    comm_set_speech("You may trade.");
                    comm_recv(DIA_R_TradeOK);
                } else {
                    int player_idx = exostate.get_player_idx(comm_player);
                    if (comm_other->is_hostile_to(player_idx)) {
                        if (comm_other->get_flag(0) == AI_Hi) {
                            comm_prepare(4);
                            comm_set_speech("The %s do not trade with you!",
                                            comm_other->get_race_str());
                            comm_set_text(0, "Do not enrage me.");
                            comm_set_text(1, "Curb your tongue!");
                            comm_set_text(2, "So I'm going to leave.");
                            comm_text_interactive_mask = 0x7;
                            comm_recv(DIA_R_TradeRefuse);
                        } else {
                            comm_prepare(1);
                            comm_set_speech("I do not want you to trade.");
                            comm_exit_anim_action = CA_Abort;
                            comm_recv(DIA_R_Close);
                        }
                    } else {
                        comm_prepare(4);
                        comm_ctx.mc = comm_other->get_trade_charge();
                        comm_set_speech("You have to pay a fee of %dMC.", comm_ctx.mc);
                        comm_set_text(0, "I accept this.");
                        if (!comm_player->can_afford(comm_ctx.mc)) {
                            comm_text_disabled_mask |= 1;
                        }
                        comm_set_text(1, "I will not trade then.");
                        comm_text_interactive_mask = 0x3;
                        comm_recv(DIA_R_TradeFee);
                    }
                }
            }
            break;
        case DIA_S_TradeRefuseRebuke:
            comm_prepare(1);
            if (comm_other->get_flag(5) == AI_Hi) {
                comm_set_speech("I am not afraid of you.");
                if (onein(3)) {
                    comm_other->set_hostile_to(comm_player_idx);
                }
            } else {
                // SUGGEST: This seems like a weird thing to say...
                comm_set_speech("Why should I do so?");
            }
            comm_exit_anim_action = CA_Abort;
            comm_recv(DIA_R_Close);
            break;
        case DIA_S_Offer:
            comm_prepare(4);
            if (onein(2)) {
                comm_set_speech("Tell me more.");
            } else {
                comm_set_speech("Do you?");
            }
            comm_set_text(0, "I propose a trading alliance.");
            comm_set_text(1, "I propose a non-attack alliance.");
            comm_set_text(2, "I propose a war alliance.");
            comm_set_text(3, "Become my ally or I attack.");
            comm_set_text(4, "Never mind...");
            if (exostate.has_alliance(comm_player_idx, comm_other_idx, ALLY_Trade)) {
                comm_text_disabled_mask |= 1;
            }
            if (exostate.has_alliance(comm_player_idx, comm_other_idx, ALLY_NonAttack)) {
                comm_text_disabled_mask |= 2;
            }
            if (exostate.has_alliance(comm_player_idx, comm_other_idx, ALLY_War)) {
                comm_text_disabled_mask |= 4;
            }

            if (!exostate.can_request_alliance(comm_other_idx, ALLY_Trade)) {
                comm_text_disabled_mask |= 1;
            }
            if (!exostate.can_request_alliance(comm_other_idx, ALLY_NonAttack)) {
                comm_text_disabled_mask |= 2;
            }
            if (!exostate.can_request_alliance(comm_other_idx, ALLY_War)) {
                comm_text_disabled_mask |= 4;
            }

            comm_text_interactive_mask = 0x1F;

            if (exostate.attack_prevented(comm_planet)) {
                comm_text_disabled_mask |= 8;
            }

            comm_recv(DIA_R_OfferListen);
            break;
        case DIA_S_ProposeAlliance:
            {
                int idx = exostate.get_player_idx(comm_player);
                if (comm_other->is_hostile_to(idx) || comm_player->get_reputation() < 1) {
                    comm_prepare(1);
                    comm_set_speech("I am not interested.");
                    comm_exit_anim_action = CA_Abort;
                    comm_recv(DIA_R_Close);
                    break;
                } else if (comm_other->get_flag(2) == AI_Lo) {
                    comm_prepare(1);
                    comm_set_speech("Sorry, but I have to refuse.");
                    comm_exit_anim_action = CA_Abort;
                    comm_recv(DIA_R_Close);
                    break;
                }
                comm_prepare(4);
                comm_set_speech("Why should I do so?");
                comm_set_text(0, "Because I offer money.");
                if (comm_player->get_mc() <= 0) {
                    comm_text_disabled_mask |= 1;
                }
                comm_set_text(1, "There are strong enemies.");
                comm_set_text(2, "My alliance is valuable");
                comm_text_interactive_mask = 0x7;
                comm_recv(DIA_R_OfferQuery);
            }
            break;
        case DIA_S_ProposeAllianceAggressively:
            {
                comm_prepare(1);
                const Freight &f = comm_player->get_fleet().freight;
                int army = f.infantry + f.gliders + f.artillery*2;
                if ((army > comm_planet->get_army_size()) && (comm_other->get_flag(3) != AI_Lo)) {
                    // FIXME: Should this say something else...?
                    comm_set_speech("I... have to accept this.");
                    exostate.set_all_alliances(comm_player_idx, comm_other_idx);
                    comm_player->add_trace(TRACE_AlliancesCreated);

                    if (comm_player->is_human()) {
                        achievement_manager.unlock(ACH_AllianceForced);
                    }

                    comm_recv(DIA_R_Close);
                } else {
                    if ((comm_other->get_flag(0) == AI_Hi) || onein(4)) {
                        comm_other->set_hostile_to(comm_player_idx);
                    }
                    comm_set_speech("Do not risk a war.");
                    comm_recv(DIA_R_NoAttackResponse);
                }
            }
            break;
        case DIA_S_ProposeAllianceCancel:
            {
                comm_prepare(1);
                comm_set_speech("...?");
                comm_exit_anim_action = CA_Abort;
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_OfferAllianceMoney:
            comm_prepare(1);
            comm_set_speech("I will listen.");
            comm_ctx.mc = comm_player->get_mc() / 2;
            comm_set_text(0, "I offer %dMC.", comm_ctx.mc);
            comm_show_adj(true);
            input_manager.enable_repeating_clicks(true);
            comm_recv(DIA_R_OfferAllianceMoneyResponse);
            break;
        case DIA_S_OfferAllianceResponse:
            comm_prepare(1);
            // Alliance very unlikely if we don't have a planet
            if (exostate.get_n_planets(comm_player) <= 0) {
                comm_ctx.alliance_prob = 99;
            }
            // TODO_MP: Multiplayer - 'verhalten'
            if (onein(max(comm_ctx.alliance_prob, 1))) {
                if (!comm_player->attempt_spend(comm_ctx.mc)) {
                    // Shouldn't be able to set value higher than we can afford
                    L.fatal("We offered more than we could afford");
                }
                // TODO_MP: It looks like the player is charged these credits -
                //          but they don't go to the other lord! More important
                //          in multiplayer.
                exostate.set_alliance(comm_player_idx, comm_other_idx, comm_ctx.alliance_type);

                if (comm_ctx.mc == 0 && comm_player->is_human()) {
                    achievement_manager.unlock(ACH_AllianceNoMC);
                }

                comm_player->add_trace(TRACE_AlliancesCreated);
                comm_set_speech("I accept this.");
                comm_exit_anim_action = CA_Abort;
                comm_recv(DIA_R_Close);
            } else {
                comm_set_speech("I am not interested.");
                comm_exit_anim_action = CA_Abort;
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_Comment:
            comm_prepare(4);
            if (onein(2)) {
                comm_set_speech("Go on then...");
            } else {
                comm_set_speech("I will listen.");
            }
            comm_set_text(0, "Accept my apologies.");
            comm_set_text(1, "I appreciate your empire.");
            comm_set_text(2, "The %s are futile beings!", comm_other->get_race_str());
            comm_set_text(3, "Your 'empire' is doomed to die.");
            comm_text_interactive_mask = 0xF;
            comm_recv(DIA_R_CommentListen);
            break;
        case DIA_S_CommentApology:
            {
                bool hostile = comm_other->is_hostile_to(comm_player_idx);
                if ((!hostile) && (comm_other->get_flag(4) != AI_Hi)) {
                    comm_prepare(1);
                    comm_set_speech("This is not necessary.");
                    comm_exit_anim_action = CA_Abort;
                    comm_recv(DIA_R_Close);
                } else if (comm_other->get_flag(2) == AI_Lo) {
                    comm_prepare(1);
                    comm_set_speech("I do not wish you as my friend.");
                    comm_exit_anim_action = CA_Abort;
                    comm_recv(DIA_R_Close);
                } else {
                    comm_prepare(4);
                    comm_ctx.mc = RND(5)*100;
                    if (onein(3)) {
                        comm_ctx.mc *= 2;
                    }
                    comm_set_speech("You could pay me %dMC...", comm_ctx.mc);
                    comm_ctx.mc2 = min((int)((float)comm_ctx.mc/1.5f), comm_player->get_mc());
                    comm_set_text(0, "That is too much! I offer %dMC.", comm_ctx.mc2);
                    comm_set_text(1, "I will not pay this sum!");
                    comm_set_text(2, "I will pay.");
                    comm_text_interactive_mask = 0x7;
                    if (!comm_player->can_afford(comm_ctx.mc)) {
                        comm_text_disabled_mask = 0x4;
                    }
                    comm_recv(DIA_R_CommentRequestCompensation);
                }
            }
            break;
        case DIA_S_CommentCompensationLower:
            {
                comm_prepare(1);
                if (comm_other->get_flag(4) == AI_Hi || comm_ctx.mc < 50) {
                    comm_set_speech("Sorry, but I have to refuse.");
                    comm_exit_anim_action = CA_Abort;
                    comm_recv(DIA_R_Close);
                } else {
                    comm_set_speech("I accept this.");
                    if (!comm_player->attempt_spend(comm_ctx.mc)) {
                        L.fatal("Compensate (lower) with more than we can afford (%d/%d)",
                                comm_ctx.mc, comm_player->get_mc());
                    }
                    comm_other->give_mc(comm_ctx.mc);
                    comm_other->clear_hostility();
                    comm_exit_anim_action = CA_Abort;
                    comm_recv(DIA_R_Close);
                }
            }
            break;
        case DIA_S_CommentCompensationRefuse:
            {
                comm_prepare(1);
                comm_set_speech("I do not wish you as my friend.");
                comm_exit_anim_action = CA_Abort;
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_CommentCompensationAccept:
            {
                comm_prepare(1);
                comm_set_speech("I accept this.");
                if (!comm_player->attempt_spend(comm_ctx.mc)) {
                    L.fatal("Compensate with more than we can afford (%d/%d)",
                            comm_ctx.mc, comm_player->get_mc());
                }
                comm_other->give_mc(comm_ctx.mc);
                comm_other->clear_hostility();
                comm_exit_anim_action = CA_Abort;
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_CommentCompliment:
            {
                comm_prepare(1);
                if (comm_other->get_flag(0) == AI_Lo) {
                    // FIXME: Should we clear hostilities that aren't against us?!
                    comm_other->clear_hostility();
                }
                bool allied = exostate.is_allied(comm_player_idx, comm_other_idx);
                if (comm_other->is_hostile_to(comm_player_idx) && !allied) {
                    comm_set_speech("I do not wish you as my friend.");
                } else {
                    comm_set_speech("Thank you very much.");
                }
                comm_exit_anim_action = CA_Abort;
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_CommentInsult:
            {
                comm_prepare(1);
                exostate.unset_alliances(comm_player_idx, comm_other_idx);
                if (onein(2)) {
                    comm_set_speech("You will pay for this.");
                } else {
                    comm_set_speech("The humans will die.");
                }
                comm_other->set_hostile_to(comm_player_idx);
                comm_exit_anim_action = CA_Abort;
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_CommentThreaten:
            {
                comm_prepare(1);
                exostate.unset_alliances(comm_player_idx, comm_other_idx);
                if (comm_other->get_flag(0) != AI_Lo || onein(3)) {
                    if (onein(2)) {
                        comm_set_speech("You will pay for this.");
                    } else {
                        comm_set_speech("The humans will die.");
                    }
                    comm_other->set_hostile_to(comm_player_idx);
                } else {
                    comm_set_speech("I am not afraid of you.");
                }
                comm_exit_anim_action = CA_Abort;
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_PlanAttack:
            if (comm_ctx.may_proceed) {
                comm_show_buttons(true);
                comm_recv(DIA_R_ProceedOrAbort);
            } else {
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_NoFirstSave:
            {
                comm_prepare(6);
                comm_set_text(0, "For the first game saving, please");
                comm_set_text(1, "use the CTRL-menu.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_SaveSuccess:
            {
                comm_prepare(6);
                comm_set_text(0, "The save operation was a success,");
                comm_set_text(1, "%s.", comm_player->get_ref());
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_SaveFailure:
            {
                comm_prepare(6);
                comm_set_text(0, "The save operation resulted in,");
                comm_set_text(1, "failure, %s.", comm_player->get_ref());
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_FirstPlanetAdvice:
            {
                comm_prepare(6);
                comm_set_text(0, "The first thing we should do now");
                comm_set_text(1, "is to find a new home for our");
                comm_set_text(2, "settlers.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_FirstPlanetAdviceUrgent:
            {
                comm_prepare(6);
                comm_set_text(0, "Our settlers need a new planet.");
                comm_set_text(1, "The longer we are waiting, the");
                comm_set_text(2, "stronger our opponents are");
                comm_set_text(3, "getting.");
                comm_set_text(4, "We should act now.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_FleetNotInSystem:
            {
                comm_prepare(6);
                comm_set_text(0, "Our fleet is not in this");
                comm_set_text(1, "system, so orbital flights");
                comm_set_text(2, "are impossible.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_AlreadyBombed:
            {
                comm_prepare(6);
                comm_set_text(0, "It's too risky to launch");
                comm_set_text(1, "a second mission against");
                comm_set_text(2, "this planet in one month.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_NoScouts:
            {
                comm_prepare(6);
                comm_set_text(0, "We do not have any scout");
                comm_set_text(1, "ships, so a scout flight is");
                comm_set_text(2, "impossible.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_NoCoverBombers:
            {
                comm_prepare(6);
                comm_set_text(0, "We do not have any bombers");
                comm_set_text(1, "to cover our scout ships.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_NoBombers:
            {
                comm_prepare(6);
                comm_set_text(0, "We do not have any aerospace");
                comm_set_text(1, "bombers, so a bomb attack is");
                comm_set_text(2, "impossible.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_ArtificialPlanetStarInvalid:
            {
                comm_prepare(6);
                /*
                 * Adjusted the text here, as it might be wrong (inner
                 * planets may have been destroyed by an expanding star,
                 * or it may have fewer than 5 planets but includes
                 * another artificial planet in the outermost slot.
                 */
                //comm_set_text(0, "This star has already five planets.");
                comm_set_text(0, "This star has no suitable orbit.");
                comm_set_text(2, "We cannot place the artificial");
                comm_set_text(3, "planet here.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_ArtificialPlanetStarMoveDest:
            {
                comm_prepare(6);
                comm_set_text(0, "Another War Planet has this");
                comm_set_text(1, "destination.");
                comm_set_text(3, "Choose a new system");
                comm_set_text(4, "or abort.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_FlyAlreadyFlying:
            {
                comm_prepare(6);
                comm_set_text(0, "Our fleet is already in flight.");
                comm_set_text(2, "We are unable to plot a new route");
                comm_set_text(3, "until we arrive.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_FlyAlreadyThere:
            {
                comm_prepare(6);
                comm_set_text(0, "Our fleet is already in the selected");
                comm_set_text(1, "system.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_ZoomButNotVisited:
            {
                comm_prepare(6);
                comm_set_text(0, "We do not yet have detailed data");
                comm_set_text(1, "on this system.");
                comm_set_text(3, "We need to fly there first.");
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_Quit:
            {
                comm_prepare(6);
                comm_set_text(0, "Do you really wish to give up");
                comm_set_text(1, "this game...?");
                comm_set_text(4, "Click <Proceed> to quit.");
                comm_show_buttons(true);
                comm_recv(DIA_R_ProceedOrAbort);
            }
            break;
        case DIA_S_CPU_Attack:
            {
                comm_prepare(4);
                comm_set_speech("Let us not talk. I want %s.", comm_planet->get_name());

                bool ally = false;
                if (exostate.has_alliance(comm_player_idx, comm_other_idx, ALLY_NonAttack)) {
                    ally = true;
                }
                if (exostate.has_alliance(comm_player_idx, comm_other_idx, ALLY_War)) {
                    ally = true;
                }
                if (ally) {
                    comm_set_text(0, "But we have an ALLIANCE!");
                    comm_set_text(1, "Aren't you my ALLY?");
                    comm_text_interactive_mask = 0x3;
                    comm_recv(DIA_R_CPU_AttackResponseAllied);
                } else {
                    comm_set_text(0, "We are not afraid!");
                    comm_set_text(1, "I offer money if you leave.");
                    if (!comm_other->can_afford(1)) {
                        comm_text_disabled_mask = 0x2;
                    }
                    comm_set_text(2, "Do not risk a war.");
                    comm_set_text(3, "Let us both live in peace.");
                    comm_text_interactive_mask = 0xF;
                    comm_recv(DIA_R_CPU_AttackResponse);
                }
            }
            break;
        case DIA_S_CPU_AttackAlly:
            comm_prepare(4);
            if (onein(2)) {
                comm_set_speech("What? I cannot remember!");
            } else {
                comm_set_speech("I herewith quit the alliance.");
            }

            exostate.unset_alliances(comm_player_idx, comm_other_idx);
            comm_player->adjust_reputation(-1);

            comm_set_text(0, "You will pay for this.");
            comm_set_text(1, "The humans are not afraid!");
            comm_set_text(2, "%s bastard.", comm_player->get_race_str());
            comm_text_interactive_mask = 0x7;
            comm_recv(DIA_R_CPU_AttackAllyProceed);
            break;
        case DIA_S_CPU_AttackPayOff:
            comm_prepare(4);
            comm_set_speech("I will listen.");
            comm_ctx.mc = comm_other->get_mc() / 2;
            comm_set_text(0, "I offer %dMC.", comm_ctx.mc);
            comm_show_adj(true);
            input_manager.enable_repeating_clicks(true);
            comm_recv(DIA_R_CPU_AttackPayOffListen);
            break;
        case DIA_S_CPU_AttackPayOffOffer:
            {
                comm_prepare(1);
                bool accept = false;
                if (comm_ctx.mc > (int)((float)exostate.get_orig_month()*2.5f)) {
                    accept = true;
                }
                if (comm_ctx.mc < 15) {
                    accept = false;
                }
                if (comm_player->get_flag(0) == AI_Hi) {
                    accept = false;
                }
                if (accept && comm_other->attempt_spend(comm_ctx.mc)) {
                    comm_set_speech("I accept this.");
                    // FIXME: Orig doesn't increment lord's credits here
                    comm_exit_anim_action = CA_Abort;
                    comm_recv(DIA_R_Close);
                } else {
                    comm_set_speech("I do not want your money...");
                    comm_recv(DIA_R_CPU_AttackPayOffReject);
                }
            }
            break;
        case DIA_S_CPU_Trade:
            comm_prepare(4);
            comm_set_speech("I wish to trade.");
            comm_set_text(0, "You will have to pay some MC...");
            comm_set_text(1, "I do not want you to trade.");
            comm_set_text(2, "You may trade.");
            comm_set_text(3, "We do not trade with %s.", comm_player->get_race_str());
            comm_text_interactive_mask = 0xF;
            comm_recv(DIA_R_CPU_TradeResponse);
            break;
        case DIA_S_CPU_TradeRequestFee:
            {
                if (exostate.has_alliance(comm_player_idx, comm_other_idx, ALLY_Trade)) {
                    comm_prepare(1);
                    comm_set_speech("I will not trade then.");
                    comm_exit_anim_action = CA_Abort;
                    comm_recv(DIA_R_Close);
                } else {
                    comm_prepare(4);
                    comm_set_speech("I will listen.");
                    comm_ctx.mc = 10;
                    comm_set_text(0, "I want %dMC.", comm_ctx.mc);
                    comm_show_adj(true);
                    input_manager.enable_repeating_clicks(true);
                    comm_recv(DIA_R_CPU_TradeFeeListen);
                }
            }
            break;
        case DIA_S_CPU_TradeFeeOffer:
            {
                bool accept = false;
                if (comm_ctx.mc <= 10 && comm_player->can_afford(10)) {
                    accept = true;
                }
                if (comm_ctx.mc >= 20 && comm_player->can_afford(comm_ctx.mc) && onein(3)) {
                    accept = true;
                }
                if (comm_ctx.mc >= 30) {
                    accept = false;
                }
                if (comm_ctx.mc >= 15 && onein(4)) {
                    accept = false;
                }

                comm_prepare(1);
                if (accept && comm_player->attempt_spend(comm_ctx.mc)) {
                    comm_other->give_mc(comm_ctx.mc);
                    comm_set_speech("I accept this.");
                    comm_exit_anim_action = CA_Trade;
                } else {
                    comm_set_speech("I will not trade then.");
                    comm_exit_anim_action = CA_Abort;
                }

                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_CPU_TradeInsult:
            {
                comm_prepare(4);
                exostate.unset_alliances(comm_player_idx, comm_other_idx);
                int r = rand() % 3;
                if (r == 2 && comm_player->get_flag(0) == AI_Hi) {
                    r = 0;
                }

                if (r < 2) {
                    if (r == 0) {
                        comm_set_speech("Do not enrage me.");
                        comm_set_text(0, "I am not afraid of you.");
                        comm_set_text(1, "Why should I do so?");
                        comm_text_interactive_mask = 0x3;
                        comm_recv(DIA_R_CPU_TradeInsultResponse);
                    } else {
                        comm_set_speech("Curb your tongue!");
                        comm_set_text(0, "Why should I do so?");
                        comm_set_text(1, "I do not want you to trade.");
                        comm_text_interactive_mask = 0x3;
                        comm_recv(DIA_R_CPU_TradeInsultResponse);
                    }
                } else {
                    comm_set_speech("So I'm going to leave.");
                    comm_recv(DIA_R_Close);
                }
            }
            break;
        case DIA_S_CPU_Offer:
            comm_prepare(4);
            comm_set_speech("I have an interesting offer.");
            comm_set_text(0, "Tell me more.");
            comm_set_text(1, "Do you?");
            comm_text_interactive_mask = 0x3;
            comm_recv(DIA_R_CPU_OfferElaborate);
            break;
        case DIA_S_CPU_ProposeAlliance:
            comm_prepare(4);
            switch (comm_ctx.alliance_type) {
                case ALLY_Trade:
                    comm_set_speech("I propose a trading alliance.");
                    break;
                case ALLY_NonAttack:
                    comm_set_speech("I propose a non-attack alliance.");
                    break;
                case ALLY_War:
                    comm_set_speech("I propose a war alliance.");
                    break;
            }
            comm_set_text(0, "I am not interested.");
            comm_set_text(1, "Why should I do so?");
            comm_set_text(2, "Sorry, but I have to refuse.");
            comm_set_text(3, "I accept this.");
            comm_text_interactive_mask = 0xF;
            comm_recv(DIA_R_CPU_ProposeAlliance);
            break;
        case DIA_S_CPU_ProposeAllianceAggressively:
            comm_prepare(4);
            comm_set_speech("Become my ally or I attack.");
            comm_set_text(0, "Do not risk a war.");
            comm_set_text(1, "I... have to accept this.");
            comm_text_interactive_mask = 0x3;
            comm_recv(DIA_R_CPU_ProposeAllianceAggressively);
            break;
        case DIA_S_CPU_AllianceQuery:
            comm_prepare(4);
            comm_ctx.mc = 0;
            if (comm_player->get_flag(3) == AI_Lo) {
                comm_ctx.mc = exostate.get_orig_month()*3;
                if (comm_ctx.mc > 100) {
                    comm_ctx.mc = 100 - RND(3)*10;
                }
                comm_ctx.mc = min(comm_ctx.mc, comm_player->get_mc());
                comm_set_speech("Well... I offer %dMC.", comm_ctx.mc);
            } else {
                if (onein(2)) {
                    comm_set_speech("There are strong enemies.");
                } else {
                    comm_set_speech("My alliance is valuable.");
                }
            }
            comm_set_text(0, "I accept this.");
            comm_set_text(1, "I am not interested.");
            comm_text_interactive_mask = 0x3;
            comm_recv(DIA_R_CPU_ProposeAllianceMoney);
            break;
        case DIA_S_B_OpenCommsAttacker:
            comm_prepare(4);
            if (comm_ctx.battle_first_comms) {
                comm_set_speech("What do you want, %s?", comm_player->get_name());
            } else {
                comm_set_speech("You again, %s?", comm_player->get_name());
            }
            comm_ctx.mc = 100;
            comm_set_text(0, "Pay %dMC and I will leave.", comm_ctx.mc);
            comm_set_text(1, "Do you call THIS a defense?");
            comm_set_text(2, "The victory is mine.");
            comm_set_text(3, "Never mind...");
            comm_text_interactive_mask = 0xF;
            comm_enable_throbber = true;
            comm_recv(DIA_R_B_OpenCommsAttacker);
            break;
        case DIA_S_B_OpenCommsDefender:
            comm_prepare(4);
            if (comm_ctx.battle_first_comms) {
                /*
                 * N.B. The player is opening the speech as defender
                 * This means the player is being attacked on a non-player turn.
                 * Therefore, the player is comm_other.
                 */
                comm_set_speech("What do you want, %s?", comm_other->get_name());
            } else {
                comm_set_speech("You again, %s?", comm_other->get_name());
            }
            comm_set_text(0, "I offer money if you retreat.");

            if (!comm_other->can_afford(1)) {
                comm_text_disabled_mask &= 1;
            }

            comm_ctx.mc = 20;
            comm_set_text(1, "Pay %dMC and you may retreat.", comm_ctx.mc);
            comm_set_text(2, "You will lose this battle.");
            comm_set_text(3, "Never mind...");
            comm_text_interactive_mask = 0xF;
            comm_recv(DIA_R_B_OpenCommsDefender);
            break;
        case DIA_S_B_OfferMoneyAttacker:
            {
                comm_prepare(4);

                int &a = comm_ctx.battle_strength_att;
                int &d = comm_ctx.battle_strength_def;

                int p = exostate.get_n_planets(comm_other);

                if (p <= 1 && a*2 > d*3 && comm_other->attempt_spend(comm_ctx.mc)) {
                    comm_set_speech("I think I will accept this.");

                    // Prevent further attacks this month (orig doesn't do this)
                    exostate.prevent_attack(comm_planet);

                    comm_player->give_mc(comm_ctx.mc);
                    comm_exit_anim_action = CA_CallOffAttack;
                } else {
                    comm_set_speech("Forget that.");
                    comm_exit_anim_action = CA_Abort;
                }

                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_B_MockAttacker:
            {
                comm_prepare(4);

                int &a = comm_ctx.battle_strength_att;
                int &d = comm_ctx.battle_strength_def;

                if (d*3 >= a*2) {
                    comm_set_speech("Indeed, I do so.");
                } else {
                    switch (comm_other->get_flag(0)) {
                        case AI_Lo:
                            comm_set_speech("I wanted peace.");
                            break;
                        case AI_Md:
                            comm_set_speech("You will pay for this.");
                            comm_other->set_hostile_to(comm_player_idx);
                            break;
                        case AI_Hi:
                            comm_set_speech("Go to hell.");
                            comm_other->set_hostile_to(comm_player_idx);
                            break;
                    }
                }

                comm_exit_anim_action = CA_Abort;
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_B_IntimidateAttacker:
            {
                comm_prepare(4);

                int &a = comm_ctx.battle_strength_att;
                int &d = comm_ctx.battle_strength_def;

                if (d*3 >= a*2) {
                    comm_set_speech("It's not the time for joking.");
                } else {
                    if (comm_other->get_flag(0) == AI_Hi) {
                        comm_other->set_hostile_to(comm_player_idx);
                        comm_set_speech("Fight, bloody bastard.");
                    } else {
                        comm_set_speech("We will see.");
                    }
                }

                comm_exit_anim_action = CA_Abort;
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_B_OfferMoneyDefender:
            {
                if (comm_player->is_hostile_to(comm_other_idx)) {
                    comm_set_speech("Forget that.");
                    comm_exit_anim_action = CA_Abort;
                    comm_recv(DIA_R_Close);
                } else {
                    comm_set_speech("I will listen.");
                    comm_ctx.mc = comm_other->get_mc() / 2;
                    comm_prepare(1);
                    comm_set_text(0, "I offer %dMC.", comm_ctx.mc);
                    comm_show_adj(true);
                    input_manager.enable_repeating_clicks(true);
                    comm_recv(DIA_R_B_OfferMoneyDefenderListen);
                }
            }
            break;
        case DIA_S_B_OfferMoneyDefenderAccept:
            {
                comm_prepare(1);
                comm_set_speech("I think I will accept this.");
                comm_exit_anim_action = CA_CallOffAttack;
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_B_OfferMoneyDefenderReject:
            {
                comm_prepare(1);

                if (comm_player->get_flag(0) == AI_Hi) {
                    comm_set_speech("Forget that.");
                } else {
                    comm_set_speech("That is not enough.");
                }

                comm_exit_anim_action = CA_Abort;
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_B_RequestMoneyDefender:
            {
                comm_prepare(4);

                comm_exit_anim_action = CA_Abort;

                int &a = comm_ctx.battle_strength_att;
                int &d = comm_ctx.battle_strength_def;

                if (d > a*3) {
                    if (comm_player->attempt_spend(comm_ctx.mc)) {
                        comm_other->give_mc(comm_ctx.mc);
                        comm_set_speech("This is a fair offer.");
                        comm_exit_anim_action = CA_CallOffAttack;
                    } else {
                        comm_set_speech("If I had the money...");
                    }
                } else {
                    if (comm_player->get_flag(0) == AI_Hi) {
                        comm_set_speech("Are you afraid or what?");
                    } else {
                        comm_set_speech("You are kidding.");
                    }
                }

                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_B_IntimidateDefender:
            {
                comm_prepare(4);

                int &a = comm_ctx.battle_strength_att;
                int &d = comm_ctx.battle_strength_def;

                if (onein(3)) {
                    comm_set_speech("We will see.");
                } else if (a >= d) {
                    comm_set_speech("It's not the time for joking.");
                } else {
                    comm_set_speech("Do not make me angry.");
                }

                comm_exit_anim_action = CA_Abort;
                comm_recv(DIA_R_Close);
            }
            break;
        case DIA_S_B_NeverMind:
            comm_prepare(4);
            comm_set_speech("Hum?!");
            comm_exit_anim_action = CA_Abort;
            comm_recv(DIA_R_Close);
            break;
        case DIA_S_B_CPU_OpenCommsAttacker:
            comm_prepare(4);
            if (onein(2)) {
                comm_set_speech("Do you call THIS a defense?");
                comm_set_text(0, "Indeed, I do so.");
                comm_set_text(1, "I wanted peace.");
                comm_set_text(2, "Go to hell.");
                comm_set_text(3, "You will pay for this.");
            } else {
                comm_set_speech("The victory is mine.");
                comm_set_text(0, "You will pay for this.");
                comm_set_text(1, "We will see.");
                comm_set_text(2, "Fight, bloody bastard.");
                comm_set_text(3, "It's not the time for joking.");
            }
            comm_text_interactive_mask = 0xF;
            comm_recv(DIA_R_B_CPU_CommsAttacker);
            break;
        case DIA_S_B_CPU_CommsAttackerResponse:
            comm_prepare(1);
            comm_set_speech("So let us fight...");
            comm_exit_anim_action = CA_Abort;
            comm_recv(DIA_R_Close);
            break;
        case DIA_S_B_CPU_OpenCommsDefender:
            comm_prepare(4);
            comm_ctx.mc = 100;
            comm_set_speech("I offer you %dMC if you leave.", comm_ctx.mc);
            comm_set_text(0, "Forget that.");
            comm_set_text(1, "That is not enough.");
            comm_set_text(2, "I think I will accept this.");
            comm_text_interactive_mask = 0x7;
            comm_recv(DIA_R_B_CPU_CommsDefender);
            break;
        case DIA_S_B_CPU_CommsDefenderReject:
            comm_prepare(1);
            comm_set_speech("Do not make me angry.");
            comm_recv(DIA_R_Close);
            break;
        case DIA_S_B_CPU_CommsDefenderRequestMore:
            comm_prepare(1);
            comm_set_speech("So let us fight...");
            comm_exit_anim_action = CA_Abort;
            comm_recv(DIA_R_Close);
            break;
        case DIA_S_B_CPU_CommsDefenderAccept:
            if (!comm_other->attempt_spend(comm_ctx.mc)) {
                L.error("Can't afford payoff of %dMC - setting to 0", comm_ctx.mc);
                if (comm_player->get_mc() > 0) {
                    comm_player->attempt_spend(comm_player->get_mc());
                }
            }
            comm_player->give_mc(comm_ctx.mc);

            // Prevent further attacks this month (orig doesn't do this)
            exostate.prevent_attack(comm_planet);

            comm_prepare(1);
            comm_set_speech("You are a worthy enemy.");
            comm_recv(DIA_R_B_CPU_CommsDefenderAcceptResponse);
            break;
        case DIA_S_B_CPU_OpenCommsRebels:
            comm_prepare(4);
            switch (RND(4)) {
                case 1:
                    comm_set_speech("We want freedom!");
                    break;
                case 2:
                    comm_set_speech("We want a better life!");
                    break;
                case 3:
                    comm_set_speech("Revolution! Revolution!");
                    break;
                case 4:
                    comm_set_speech("Against the tyranny!");
                    break;
            }
            comm_set_text(0, "We will find a compromise.");
            comm_set_text(1, "You will get a better life.");
            comm_set_text(2, "The revolution is useless!");
            comm_set_text(3, "You dare? Die, unworthy!");
            comm_text_interactive_mask = 0xF;
            comm_recv(DIA_R_B_CPU_RebelComms);
            break;
        case DIA_S_B_CPU_RebelsOfferCompromise:
            comm_prepare(1);
            comm_set_speech("We do not want compromises!");
            comm_exit_anim_action = CA_Abort;
            comm_recv(DIA_R_Close);
            break;
        case DIA_S_B_CPU_RebelsOfferBetterLife:
            comm_prepare(4);
            comm_set_speech("What do you offer?");
            comm_set_text(0, "I will build new cities.");
            comm_set_text(1, "I will build parks.");
            comm_set_text(2, "I will improve the laws.");
            comm_set_text(3, "I will pay for improvements.");
            comm_text_interactive_mask = 0xF;
            if (!comm_other->can_afford(1)) {
                comm_text_disabled_mask |= 0x8;
            }
            comm_recv(DIA_R_B_CPU_RebelsListen);
            break;
        case DIA_S_B_CPU_RebelsOfferNothing:
            comm_prepare(1);
            switch (RND(4)) {
                case 1:
                    comm_set_speech("We want freedom!");
                    break;
                case 2:
                    comm_set_speech("We want a better life!");
                    break;
                case 3:
                    comm_set_speech("Revolution! Revolution!");
                    break;
                case 4:
                    comm_set_speech("Against the tyranny!");
                    break;
            }
            comm_exit_anim_action = CA_Abort;
            comm_recv(DIA_R_Close);
            break;
        case DIA_S_B_CPU_RebelsProposeNoMC:
            comm_prepare(1);
            if (onein(2)) {
                comm_set_speech("These are just words!");
            } else {
                comm_set_speech("We do not believe that!");
            }
            comm_exit_anim_action = CA_Abort;
            comm_recv(DIA_R_Close);
            break;
        case DIA_S_B_CPU_RebelsProposeMC:
            comm_prepare(1);
            comm_ctx.mc = comm_planet->get_n_cities()*5;
            comm_set_speech("We want %dMC!", comm_ctx.mc);
            comm_ctx.mc2 = max(comm_other->get_mc()/2, 1);
            comm_set_text(0, "I offer %dMC.", comm_ctx.mc2);
            comm_show_adj(true);
            input_manager.enable_repeating_clicks(true);
            comm_recv(DIA_R_B_CPU_RebelsDemandMC);
            break;
        case DIA_S_B_CPU_RebelsMCOffer:
            {
                comm_prepare(1);

                bool ok = true;
                if (comm_ctx.mc2 < (comm_ctx.mc-(comm_ctx.mc/4))) {
                    ok = false;
                }
                if (!comm_other->attempt_spend(comm_ctx.mc2)) {
                    L.error("Offered more MC to rebels than we could afford");
                    ok = false;
                }
                if (ok) {
                    comm_set_speech("We accept this.");
                    comm_recv(DIA_R_B_CPU_RebelsAcceptOffer);
                } else {
                    comm_set_speech("That is not enough!");
                    comm_exit_anim_action = CA_Abort;
                    comm_recv(DIA_R_Close);
                }
            }
            break;
        default:
            L.warn("Unhandled comm input on send: %d", (int)input);
            break;
    }
}

void CommPanelDrawer::comm_recv(CommRecv output) {
    comm_state = output;
}

void CommPanelDrawer::comm_recv_proxyto(CommSend proxyto) {
    comm_proxyto = proxyto;
    comm_recv(DIA_R_Proxy);
}

void CommPanelDrawer::comm_process_responses() {
    /*
     * Respond to user interaction.
     * This could involve triggering a new phase of the dialogue with
     * comm_send(), or setting comm_report_action in order to flag
     * the ultimate outcome of the conversation to the caller.
     */
    int comm_player_idx = -1;
    int comm_other_idx  = -1;

    if (comm_player) {
        comm_player_idx = exostate.get_player_idx(comm_player);
    }

    if (comm_other) {
        comm_other_idx = exostate.get_player_idx(comm_other);
    }

    bool proceed = false;
    bool abort = false;

    bool clicked = draw_manager.clicked();

    if (comm_speech) {
        if (clicked) {
            comm_complete_speech();
        }
        return;
    }

    SpriteClick click;
    click = draw_manager.query_click(id_comm_buttons);
    if (click.id) {
        proceed = click.x < 0.55;
        abort = !proceed;
    }

    int opt = -1;
    for (int i = 0; i < 6; ++i) {
        if (comm_text_disabled_mask & (1 << i)) {
            continue;
        }
        if (draw_manager.query_click(id_text[i]).id) {
            opt = i;
            break;
        }
    }

    switch (comm_state) {
        case DIA_R_Close:
            if (clicked) {
                if (comm_exit_anim_action != CA_None) {
                    comm_exit_anim(comm_exit_anim_action);
                } else {
                    comm_report_action = CA_Abort;
                }
            }
            break;
        case DIA_R_ProceedOrAbort:
            if (proceed) {
                comm_report_action = CA_Proceed;
            } else if (abort) {
                comm_report_action = CA_Abort;
            }
            break;
        case DIA_R_Proxy:
            comm_send(comm_proxyto);
            break;
        case DIA_R_SettleCannotAfford:
            if (clicked) {
                comm_report_action = CA_Abort;
            }
            break;
        case DIA_R_SettleQueryProbe:
            if (proceed) {
                if (comm_player->get_starship().bionic_probes > 0) {
                    comm_player->get_starship().bionic_probes--;
                    comm_report_action = CA_BionicProbe;
                } else {
                    L.error("Should not have been prompted to use probe we don't have");
                }
            } else if (abort) {
                comm_send(DIA_S_PlanSettle2);
            }
            break;
        case DIA_R_SettleProbeResults:
            if (clicked) {
                comm_send(DIA_S_PlanSettle2);
            }
            break;
        case DIA_R_SettleIntelligentLife:
            if (clicked) {
                comm_send(DIA_S_PlanClaim);
            }
            break;
        case DIA_R_SettlePlanetInfo:
            if (proceed) {
                comm_send(DIA_S_NamePlanet);
            } else if (abort) {
                comm_report_action = CA_Abort;
            }
            break;
        case DIA_R_SettleNamePlanet:
            {
                if (input_manager.consume(K_Backspace)) {
                    input_manager.backspace();
                }

                const char *name = input_manager.get_input_text(PLANET_MAX_NAME);
                char display_name[PLANET_MAX_NAME + 2];
                snprintf(display_name, sizeof(display_name), "  %s", name);

                comm_set_text(2, display_name);
                if (input_manager.consume(K_Enter) && strnlen(name, 1)) {
                    comm_planet_name_confirm_time = comm_time;
                    comm_send(DIA_S_NamePlanetConfirm);
                }
            }
            break;
        case DIA_R_SettleNamePlanetProceed:
            {
                if ((comm_time - comm_planet_name_confirm_time) > 1.6f) {
                    comm_report_action = CA_Proceed;
                }
            }
        case DIA_R_AwaitingOrders:
            switch (opt) {
                case 0:
                    comm_exit_anim(CA_GoodsTransfer);
                    break;
                case 1:
                    comm_exit_anim(CA_StartProduction);
                    break;
                case 2:
                    if (comm_planet->get_class() == Artificial) {
                        // Move artificial planet
                        comm_report_action = CA_MovePlanet;
                    } else {
                        // Change global climate
                        if (comm_player->attempt_spend(500)) {
                            comm_send(DIA_S_ChangeClimate);
                        } else {
                            L.error("Unaffordable change climate should not be enabled");
                        }
                    }
                    break;
                case 3:
                    comm_exit_anim(CA_Abort);
                    break;
            }
            break;
        case DIA_R_ChangeClimateConfirm:
            {
                if (clicked) {
                    comm_report_action = CA_ChangeClimate;
                }
            }
            break;
        case DIA_R_Greeting:
            switch (opt) {
                case 0:
                    comm_send(DIA_S_Attack);
                    break;
                case 1:
                    comm_send(DIA_S_Trade);
                    break;
                case 2:
                    comm_send(DIA_S_Offer);
                    break;
                case 3:
                    comm_send(DIA_S_Comment);
                    break;
            }
            break;
        case DIA_R_AttackAlly:
            switch (opt) {
                case 0:
                    comm_send(DIA_S_AttackAllyCompensate);
                    break;
                case 1:
                    comm_send(DIA_S_AttackAllyApology);
                    break;
                case 2:
                    comm_send(DIA_S_AttackAllyProceed);
                    break;
                case 3:
                    comm_send(DIA_S_AttackAllyProceed);
                    break;
            }
            break;
        case DIA_R_AttackPayOff:
            switch (opt) {
                case 0:
                    comm_send(DIA_S_AttackPayOffWantMore);
                    break;
                case 1:
                    if (!comm_other->attempt_spend(comm_ctx.mc)) {
                        L.fatal("CPU offered more than they could afford");
                    }
                    comm_player->give_mc(comm_ctx.mc);

                    // Prevent further attacks this month (orig doesn't do this)
                    exostate.prevent_attack(comm_planet);

                    comm_report_action = CA_Abort;
                    break;
                case 2:
                    comm_report_action = CA_PlanAttack;
                    break;
            }
            break;
        case DIA_R_AttackPayOffMore:
            switch (opt) {
                case 0:
                    if (!comm_other->attempt_spend(comm_ctx.mc)) {
                        L.fatal("CPU offered more than they could afford after increase");
                    }
                    comm_player->give_mc(comm_ctx.mc);

                    // Prevent further attacks this month (orig doesn't do this)
                    exostate.prevent_attack(comm_planet);

                    comm_report_action = CA_Abort;
                    break;
                case 1:
                    comm_exit_anim(CA_PlanAttack);
                    break;
            }
            break;
        case DIA_R_NoAttackResponse:
            if (clicked) {
                comm_exit_anim(CA_PlanAttack);
            }
            break;
        case DIA_R_TradeOK:
            if (clicked) {
                comm_exit_anim(CA_Trade);
            }
            break;
        case DIA_R_TradeFee:
            switch (opt) {
                case 0:
                    if (comm_player->attempt_spend(comm_ctx.mc)) {
                        comm_exit_anim(CA_Trade);
                    }
                    break;
                case 1:
                    comm_exit_anim(CA_Abort);
                    break;
            }
            break;
        case DIA_R_TradeRefuse:
            switch (opt) {
                case 0:
                    comm_send(DIA_S_TradeRefuseRebuke);
                    break;
                case 1:
                    // FIXME: Should "Curb your tongue!" do more than close the comms?
                    comm_exit_anim(CA_Abort);
                    break;
                case 2:
                    comm_exit_anim(CA_Abort);
                    break;
            }
            break;
        case DIA_R_OfferListen:
            switch (opt) {
                case 0:
                    // Trading alliance
                    comm_ctx.alliance_type = ALLY_Trade;
                    exostate.register_request_alliance(comm_other_idx, comm_ctx.alliance_type);
                    comm_send(DIA_S_ProposeAlliance);
                    break;
                case 1:
                    // Non-attack alliance
                    comm_ctx.alliance_type = ALLY_NonAttack;
                    exostate.register_request_alliance(comm_other_idx, comm_ctx.alliance_type);
                    comm_send(DIA_S_ProposeAlliance);
                    break;
                case 2:
                    // War alliance
                    comm_ctx.alliance_type = ALLY_War;
                    exostate.register_request_alliance(comm_other_idx, comm_ctx.alliance_type);
                    comm_send(DIA_S_ProposeAlliance);
                    break;
                case 3:
                    // Threaten
                    comm_send(DIA_S_ProposeAllianceAggressively);
                    break;
                case 4:
                    // Abort (not in orig)
                    comm_send(DIA_S_ProposeAllianceCancel);
                    break;
            }
            break;
        case DIA_R_OfferQuery:
            comm_ctx.mc = 0;
            switch (opt) {
                case 0:
                    comm_ctx.alliance_prob = 2;
                    comm_send(DIA_S_OfferAllianceMoney);
                    break;
                case 1:
                    comm_ctx.alliance_prob = 3;
                    comm_send(DIA_S_OfferAllianceResponse);
                    break;
                case 2:
                    comm_ctx.alliance_prob = 5;
                    {
                        int player_ttl = (int)comm_player->get_guild_title();
                        int other_ttl = (int)comm_other->get_guild_title();
                        if (player_ttl > other_ttl) {
                            comm_ctx.alliance_prob = 1;
                        }
                        if (comm_other->get_flag(1) == AI_Hi) {
                            comm_ctx.alliance_prob = 2;
                        }
                    }
                    comm_send(DIA_S_OfferAllianceResponse);
                    break;
            }
            break;
        case DIA_R_OfferAllianceMoneyResponse:
            {
                SpriteClick adjclick = draw_manager.query_click(id_comm_adj);
                if (adjclick.id) {
                    if (adjclick.x < 0.5f) {
                        comm_ctx.mc = max(comm_ctx.mc - 1, 0);
                    } else {
                        comm_ctx.mc = min(comm_ctx.mc + 1, comm_player->get_mc());
                    }
                    comm_set_text(0, "I offer %dMC.", comm_ctx.mc);
                }

                if (draw_manager.query_click(id_comm_adj_ok).id) {
                    input_manager.enable_repeating_clicks(false);
                    if (comm_ctx.mc >= 5 && comm_ctx.mc >= exostate.get_orig_month()/2) {
                        comm_ctx.alliance_prob -= 2;
                    }
                    if (comm_ctx.mc < 5) {
                        comm_ctx.alliance_prob = 20;
                    }
                    comm_send(DIA_S_OfferAllianceResponse);
                }
            }
            break;
        case DIA_R_CommentListen:
            switch (opt) {
                case 0:
                    comm_send(DIA_S_CommentApology);
                    break;
                case 1:
                    comm_send(DIA_S_CommentCompliment);
                    break;
                case 2:
                    comm_send(DIA_S_CommentInsult);
                    break;
                case 3:
                    comm_send(DIA_S_CommentThreaten);
                    break;
            }
            break;
        case DIA_R_CommentRequestCompensation:
            switch (opt) {
                case 0:
                    comm_ctx.mc = comm_ctx.mc2;
                    comm_send(DIA_S_CommentCompensationLower);
                    break;
                case 1:
                    comm_send(DIA_S_CommentCompensationRefuse);
                    break;
                case 2:
                    comm_send(DIA_S_CommentCompensationAccept);
                    break;
            }
            break;
        case DIA_R_CPU_AttackResponse:
            switch (opt) {
                case 0:
                    exostate.unset_alliances(comm_player_idx, comm_other_idx);
                    comm_report_action = CA_Attack;
                    break;
                case 1:
                    comm_send(DIA_S_CPU_AttackPayOff);
                    break;
                case 2:
                    exostate.unset_alliances(comm_player_idx, comm_other_idx);
                    comm_report_action = CA_Attack;
                    break;
                case 3:
                    exostate.unset_alliances(comm_player_idx, comm_other_idx);
                    comm_report_action = CA_Attack;
                    break;
            }
            break;
        case DIA_R_CPU_AttackResponseAllied:
            switch (opt) {
                case 0:
                    comm_send(DIA_S_CPU_AttackAlly);
                    break;
                case 1:
                    comm_send(DIA_S_CPU_AttackAlly);
                    break;
            }
            break;
        case DIA_R_CPU_AttackAllyProceed:
            switch (opt) {
                case 0:
                    comm_report_action = CA_Attack;
                    break;
                case 1:
                    comm_report_action = CA_Attack;
                    break;
                case 2:
                    comm_report_action = CA_Attack;
                    break;
            }
            break;
        case DIA_R_CPU_AttackPayOffListen:
            {
                SpriteClick adjclick = draw_manager.query_click(id_comm_adj);
                if (adjclick.id) {
                    if (adjclick.x < 0.5f) {
                        comm_ctx.mc = max(comm_ctx.mc - 1, 0);
                    } else {
                        comm_ctx.mc = min(comm_ctx.mc + 1, comm_other->get_mc());
                    }
                    comm_set_text(0, "I offer %dMC.", comm_ctx.mc);
                }

                if (draw_manager.query_click(id_comm_adj_ok).id) {
                    input_manager.enable_repeating_clicks(false);
                    comm_send(DIA_S_CPU_AttackPayOffOffer);
                }
            }
            break;
        case DIA_R_CPU_AttackPayOffReject:
            if (clicked) {
                exostate.unset_alliances(comm_player_idx, comm_other_idx);
                comm_exit_anim(CA_Attack);
            }
            break;
        case DIA_R_CPU_TradeResponse:
            switch (opt) {
                case 0:
                    comm_send(DIA_S_CPU_TradeRequestFee);
                    break;
                case 1:
                    comm_report_action = CA_Abort;
                    break;
                case 2:
                    comm_report_action = CA_Trade;
                    break;
                case 3:
                    comm_send(DIA_S_CPU_TradeInsult);
                    break;
            }
            break;
        case DIA_R_CPU_TradeFeeListen:
            {
                SpriteClick adjclick = draw_manager.query_click(id_comm_adj);
                if (adjclick.id) {
                    if (adjclick.x < 0.5f) {
                        comm_ctx.mc = max(comm_ctx.mc - 1, 0);
                    } else {
                        comm_ctx.mc = max(comm_ctx.mc + 1, comm_ctx.mc);
                    }
                    comm_set_text(0, "I want %dMC.", comm_ctx.mc);
                }

                if (draw_manager.query_click(id_comm_adj_ok).id) {
                    input_manager.enable_repeating_clicks(false);
                    comm_send(DIA_S_CPU_TradeFeeOffer);
                }
            }
            break;
        case DIA_R_CPU_TradeInsultResponse:
            switch (opt) {
                case 0:
                    comm_exit_anim(CA_Abort);
                    break;
                case 1:
                    comm_exit_anim(CA_Abort);
                    break;
            }
            break;
        case DIA_R_CPU_OfferElaborate:
            if (opt >= 0) {
                int army = comm_player->get_fleet().freight.army_size();
                bool agg = ((comm_player->get_flag(0) == AI_Hi) && onein(3)) || onein(10);
                if (agg && army > 0) {
                    comm_send(DIA_S_CPU_ProposeAllianceAggressively);
                } else {
                    // FIXME: Test each of the 3 in a random order
                    AllianceType &t = comm_ctx.alliance_type;
                    while (1) {
                        switch (rand() % 3) {
                            case 0:
                                t = ALLY_Trade;
                                break;
                            case 1:
                                t = ALLY_NonAttack;
                                break;
                            case 2:
                                t = ALLY_War;
                                break;
                        }
                        if (!exostate.has_alliance(comm_player_idx, comm_other_idx, t)) {
                            break;
                        }
                    }
                    comm_send(DIA_S_CPU_ProposeAlliance);
                }
            }
            break;
        case DIA_R_CPU_ProposeAlliance:
            switch (opt) {
                case 0:
                    // I am not interested
                    comm_exit_anim(CA_Abort);
                    break;
                case 1:
                    // Why should I do so?
                    comm_send(DIA_S_CPU_AllianceQuery);
                    break;
                case 2:
                    // Sorry, but I have to refuse
                    comm_exit_anim(CA_Abort);
                    break;
                case 3:
                    // I accept this
                    exostate.set_alliance(comm_player_idx, comm_other_idx, comm_ctx.alliance_type);
                    comm_report_action = CA_Abort;
                    break;
            }
            break;
        case DIA_R_CPU_ProposeAllianceMoney:
            switch (opt) {
                case 0:
                    // I accept this
                    // FIXME: Orig doesn't subtract MC from CPU lord, so neither do we
                    comm_other->give_mc(comm_ctx.mc);
                    exostate.set_alliance(comm_player_idx, comm_other_idx, comm_ctx.alliance_type);
                    comm_exit_anim(CA_Abort);
                    break;
                case 1:
                    // I am not interested
                    comm_exit_anim(CA_Abort);
                    break;
            }
            break;
        case DIA_R_CPU_ProposeAllianceAggressively:
            switch (opt) {
                case 0:
                    comm_exit_anim(CA_Attack);
                    break;
                case 1:
                    exostate.set_all_alliances(comm_player_idx, comm_other_idx);
                    comm_exit_anim(CA_Abort);
                    break;
            }
            break;
        case DIA_R_B_OpenCommsAttacker:
            switch (opt) {
                case 0:
                    // Pay X MC and I will leave
                    comm_send(DIA_S_B_OfferMoneyAttacker);
                    break;
                case 1:
                    // Do you call THIS a defense?
                    comm_send(DIA_S_B_MockAttacker);
                    break;
                case 2:
                    // The victory is mine
                    comm_send(DIA_S_B_IntimidateAttacker);
                    break;
                case 3:
                    // Never mind...
                    comm_send(DIA_S_B_NeverMind);
                    break;
            }
            break;
        case DIA_R_B_OpenCommsDefender:
            switch (opt) {
                case 0:
                    // I offer money if you retreat
                    comm_send(DIA_S_B_OfferMoneyDefender);
                    break;
                case 1:
                    // Pay X MC and you may retreat
                    comm_send(DIA_S_B_RequestMoneyDefender);
                    break;
                case 2:
                    // You will lose this battle
                    comm_send(DIA_S_B_IntimidateDefender);
                    break;
                case 3:
                    // Never mind...
                    comm_send(DIA_S_B_NeverMind);
                    break;
            }
            break;
        case DIA_R_B_OfferMoneyDefenderListen:
            {
                comm_text_interactive_mask = 0;

                SpriteClick adjclick = draw_manager.query_click(id_comm_adj);
                if (adjclick.id) {
                    if (adjclick.x < 0.5f) {
                        comm_ctx.mc = max(comm_ctx.mc - 1, 0);
                    } else {
                        comm_ctx.mc = min(comm_ctx.mc + 1, comm_other->get_mc());
                    }
                    comm_set_text(0, "I offer %dMC.", comm_ctx.mc);
                }

                if (draw_manager.query_click(id_comm_adj_ok).id) {
                    input_manager.enable_repeating_clicks(false);
                    if (comm_ctx.mc > exostate.get_orig_month()/2) {
                        if (comm_other->attempt_spend(comm_ctx.mc)) {
                            // FIXME: Orig doesn't give MC to CPU player either
                            comm_send(DIA_S_B_OfferMoneyDefenderAccept);
                        } else {
                            L.warn("Shouldn't have been able to set this much MC");
                        }
                    } else {
                        comm_send(DIA_S_B_OfferMoneyDefenderReject);
                    }
                }
            }
            break;
        case DIA_R_B_CPU_CommsAttacker:
            if (opt >= 0) {
                comm_send(DIA_S_B_CPU_CommsAttackerResponse);
            }
            break;
        case DIA_R_B_CPU_CommsDefender:
            switch (opt) {
                case 0:
                    comm_send(DIA_S_B_CPU_CommsDefenderReject);
                    break;
                case 1:
                    comm_send(DIA_S_B_CPU_CommsDefenderRequestMore);
                    break;
                case 2:
                    comm_send(DIA_S_B_CPU_CommsDefenderAccept);
                    break;
            }
            break;
        case DIA_R_B_CPU_CommsDefenderAcceptResponse:
            if (clicked) {
                comm_exit_anim(CA_CallOffAttack);
            }
            break;
        case DIA_R_B_CPU_RebelComms:
            switch (opt) {
                case 0:
                    comm_send(DIA_S_B_CPU_RebelsOfferCompromise);
                    break;
                case 1:
                    comm_send(DIA_S_B_CPU_RebelsOfferBetterLife);
                    break;
                case 2:
                    comm_send(DIA_S_B_CPU_RebelsOfferNothing);
                    break;
                case 3:
                    comm_send(DIA_S_B_CPU_RebelsOfferNothing);
                    break;
            }
            break;
        case DIA_R_B_CPU_RebelsListen:
            switch (opt) {
                case 0:
                    comm_send(DIA_S_B_CPU_RebelsProposeNoMC);
                    break;
                case 1:
                    comm_send(DIA_S_B_CPU_RebelsProposeNoMC);
                    break;
                case 2:
                    comm_send(DIA_S_B_CPU_RebelsProposeNoMC);
                    break;
                case 3:
                    comm_send(DIA_S_B_CPU_RebelsProposeMC);
                    break;
            }
            break;
        case DIA_R_B_CPU_RebelsDemandMC:
            {
                SpriteClick adjclick = draw_manager.query_click(id_comm_adj);
                if (adjclick.id) {
                    if (adjclick.x < 0.5f) {
                        comm_ctx.mc2 = max(comm_ctx.mc2 - 1, 0);
                    } else {
                        comm_ctx.mc2 = min(comm_ctx.mc2 + 1, comm_other->get_mc());
                    }
                    comm_set_text(0, "I offer %dMC.", comm_ctx.mc);
                }

                if (draw_manager.query_click(id_comm_adj_ok).id) {
                    input_manager.enable_repeating_clicks(false);
                    comm_send(DIA_S_B_CPU_RebelsMCOffer);
                }
            }
            break;
        case DIA_R_B_CPU_RebelsAcceptOffer:
            comm_exit_anim(CA_CallOffAttack);
            break;
        default:
            break;
    }
}
