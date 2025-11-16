#include "trade_buy.h"

#include "draw/draw.h"
#include "input/input.h"
#include "state/exodus_state.h"

#include "util/value.h"

#include "shared.h"

const int TRADEBUY_BORDER = 6;
const int TRADEBUY_W = 436 + (TRADEBUY_BORDER) * 2;
const int TRADEBUY_H = 306 + (TRADEBUY_BORDER) * 2;
const int TRADEBUY_X = (RES_X / 2) - (TRADEBUY_W / 2);
const int TRADEBUY_Y = (RES_Y / 2) - (TRADEBUY_H / 2) - 12;
const int TRADEBUY_TEXT_X = TRADEBUY_X + TRADEBUY_BORDER * 2 + 4;
const int TRADEBUY_FLAG_BG_X = (RES_X / 2) - 48 - TRADEBUY_BORDER;
const int TRADEBUY_FLAG_BG_Y = TRADEBUY_Y - 2 - 56 - TRADEBUY_BORDER*2;
const int TRADEBUY_FLAG_BG_W = 96 + TRADEBUY_BORDER*2;
const int TRADEBUY_FLAG_BG_H = 56 + TRADEBUY_BORDER*2;
const int TRADEBUY_BG_X = TRADEBUY_X + TRADEBUY_BORDER;
const int TRADEBUY_BG_Y = TRADEBUY_Y + TRADEBUY_BORDER;

const int TRADEBUY_TEXT_Y_OFF = -8;

extern DRAWMANAGER draw_manager;
extern INPUTMANAGER input_manager;


const char* CORP_NAMES[] = {
    " Corporation", " Industries", " Systems", " Technics", " Productions",
    " Hardware", " Cybertech", " Cybertron", " Chemics", " Boitech",
    " Chemtech", "chem", "tron", "tech", " Electronics",
    " Laboratories", " Mechanix", " Technix", " Mechanic", " Bionics",
    " Alpha", " Beta", " Gamma", " Delta", " Epsilon"
};

/*
 * This represents an opportunity for the owner of a planet
 * to buy goods, following a rival player conducting trade
 * with the planet.
 */

TradeBuy::TradeBuy() {
}

/*
 * If CPU, resolve trade immediately and return true. tradebuy_update() calls not needed.
 * If player, set up UI and return false. tradebuy_update() calls needed.
 */
bool TradeBuy::tradebuy_start(int fd, int inf, int gli, int art) {
    Planet *p = exostate().get_active_planet();
    Player *owner = exostate().get_player(p->get_owner());

    L.debug("[%s]: Available: %dfd %dinf %dgli %dart",
            owner->get_full_name(), fd, inf, gli, art);

    // FIXME: In multiplayer, should the CPU control this for a non-active human?
    if (!owner->is_human()) {
        // CPU player

        /*
         * FIXME: Nothing here checks that the planet has capacity for these bought goods.
         * The original allows unchecked increment of the value, however these will likely
         * be sson discarded anyway. Here, the adjust_reserves and adjust_army functions
         * impose a cap. However, we still allow overbuy.
         */

        if (fd > 0 && !p->agri_sufficient()) {
            int cost = RND(3) + 2;
            if (owner->can_afford(cost)) {
                int to_buy = min(owner->get_mc() / cost, fd);
                to_buy = min(to_buy, 30);

                L.debug("[%s]: BUY %dfd", owner->get_full_name(), to_buy);

                if (owner->attempt_spend(to_buy * cost, MC_TradeBuy)) {
                    p->adjust_reserves(0, to_buy, 0);
                } else {
                    L.fatal("TradeBuy: Could not spend MC we checked we had (fd)");
                }
            }
        }

        if (inf > 0 && onein(3)) {
            int cost = RND(2) + 4;
            if (owner->can_afford(cost)) {
                int to_buy = min(owner->get_mc() / cost, inf);
                to_buy = min(to_buy, 10);

                L.debug("[%s]: BUY %dinf", owner->get_full_name(), to_buy);

                if (owner->attempt_spend(to_buy * cost, MC_TradeBuy)) {
                    p->adjust_army(to_buy, 0, 0);
                } else {
                    L.fatal("TradeBuy: Could not spend MC we checked we had (inf)");
                }
            }
        }

        if (gli > 0 && onein(5)) {
            int cost = RND(2) + 6;
            if (owner->can_afford(cost)) {
                int to_buy = min(owner->get_mc() / cost, gli);
                to_buy = min(to_buy, 10);

                L.debug("[%s]: BUY %dgli", owner->get_full_name(), to_buy);

                if (owner->attempt_spend(to_buy * cost, MC_TradeBuy)) {
                    p->adjust_army(0, to_buy, 0);
                } else {
                    L.fatal("TradeBuy: Could not spend MC we checked we had (gli)");
                }
            }
        }

        if (art > 0 && onein(7)) {
            int cost = RND(2) + 8;
            if (owner->can_afford(cost)) {
                int to_buy = min(owner->get_mc() / cost, art);
                to_buy = min(to_buy, 10);

                L.debug("[%s]: BUY %dart", owner->get_full_name(), to_buy);

                if (owner->attempt_spend(to_buy * cost, MC_TradeBuy)) {
                    p->adjust_army(0, 0, to_buy);
                } else {
                    L.fatal("TradeBuy: Could not spend MC we checked we had (art)");
                }
            }
        }

        return true;
    }

    // Human player
    // N.B. Costs are different for human

    tradebuy_available[0].type = TBGT_Food;
    tradebuy_available[0].avail = fd;
    tradebuy_available[0].cost = RND(2) + 1;

    tradebuy_available[1].type = TBGT_Inf;
    tradebuy_available[1].avail = inf;
    tradebuy_available[1].cost = RND(4) + 2;

    tradebuy_available[2].type = TBGT_Gli;
    tradebuy_available[2].avail = gli;
    tradebuy_available[2].cost = RND(6) + 2;

    tradebuy_available[3].type = TBGT_Art;
    tradebuy_available[3].avail = art;
    tradebuy_available[3].cost = RND(6) + 2;

    for (int i = 0; i < TRADEBUY_OPTIONS; ++i) {
        tradebuy_available[i].buy = 0;
        tradebuy_available[i].id_img = draw_manager.new_sprite_id();
        tradebuy_available[i].id_unavailable = draw_manager.new_sprite_id();
        tradebuy_available[i].id_offer = draw_manager.new_sprite_id();
        tradebuy_available[i].id_buy = draw_manager.new_sprite_id();
        tradebuy_available[i].id_adj = draw_manager.new_sprite_id();
    }

    tradebuy_open();

    return false;
}

void TradeBuy::tradebuy_open() {
    Planet *p = exostate().get_active_planet();
    int owner_idx = exostate().get_active_player_idx();

    input_manager.enable_repeating_clicks(true);

    id_tradebuy_header_flag = draw_manager.new_sprite_id();
    id_tradebuy_header_l    = draw_manager.new_sprite_id();
    id_tradebuy_header_r    = draw_manager.new_sprite_id();
    id_tradebuy_panel       = draw_manager.new_sprite_id();
    id_tradebuy_black       = draw_manager.new_sprite_id();
    id_tradebuy_exit        = draw_manager.new_sprite_id();

    draw_manager.fill(
        id_tradebuy_panel,
        {TRADEBUY_X, TRADEBUY_Y,
         TRADEBUY_W, TRADEBUY_H},
        COL_BORDERS);
    draw_manager.fill(
        FILL_3D_Out_Hollow,
        {TRADEBUY_X, TRADEBUY_Y,
         TRADEBUY_W, TRADEBUY_H},
        COL_BORDERS);

    draw_manager.fill(
        id_tradebuy_black,
        {TRADEBUY_X + TRADEBUY_BORDER,
         TRADEBUY_Y + TRADEBUY_BORDER,
         436, 306},
        {0, 0, 0});

    draw_manager.fill(
        id_tradebuy_header_flag,
        {TRADEBUY_FLAG_BG_X, TRADEBUY_FLAG_BG_Y,
         TRADEBUY_FLAG_BG_W, TRADEBUY_FLAG_BG_H},
        COL_BORDERS);
    draw_manager.fill(
        FILL_3D_Out_Hollow,
        {TRADEBUY_FLAG_BG_X, TRADEBUY_FLAG_BG_Y,
         TRADEBUY_FLAG_BG_W, TRADEBUY_FLAG_BG_H},
        COL_BORDERS);

    draw_manager.draw(
        flags[owner_idx],
        {TRADEBUY_FLAG_BG_X + TRADEBUY_BORDER,
         TRADEBUY_FLAG_BG_Y + TRADEBUY_BORDER,
        0, 0, 1, 1});

    draw_manager.draw(
        id_tradebuy_header_l,
        ENHANCED() ? IMG_HEADER_L : IMG_TS1_HEADER,
        {TRADEBUY_FLAG_BG_X - 2,
         TRADEBUY_Y - 2,
         1, 1, 1, 1});

    draw_manager.draw(
        id_tradebuy_header_r,
        ENHANCED() ? IMG_HEADER_R : IMG_TS1_HEADER,
        {TRADEBUY_FLAG_BG_X + TRADEBUY_FLAG_BG_W + 2,
         TRADEBUY_Y - 2,
         0, 1, 1, 1});

    draw_manager.fill(
        FILL_3D_Out,
        {TRADEBUY_X + TRADEBUY_BORDER + 2, TRADEBUY_Y + TRADEBUY_H - 2*TRADEBUY_BORDER - 22,
         30, 26},
        COL_BORDERS);

    draw_manager.fill(
        FILL_3D_Out,
        {TRADEBUY_X + TRADEBUY_BORDER + 154, TRADEBUY_Y + TRADEBUY_H - 2*TRADEBUY_BORDER - 22,
         280, 26},
        COL_BORDERS);

    draw_manager.draw(
        id_tradebuy_exit,
        IMG_BR5_EXPORT4,
        {TRADEBUY_X + 40,
         TRADEBUY_Y + TRADEBUY_H - 8,
         0, 1, 1, 1});

    char text[128];

    const char* suffix = "";

    if (onein(3)) {
        /*
         * In the original, the 5th option is unreachable.
         * I'll kick the can down the road and do the same!
         */
        switch (rand() % 4) {
            case 0:
                suffix = " Interstellar";
                break;
            case 1:
                suffix = " Inc.";
                break;
            case 2:
                suffix = " Worldwide";
                break;
            case 3:
                suffix = " Starwide";
                break;
            case 4:
                suffix = " Ltd.";
                break;
        }
    }

    snprintf(text, sizeof(text), "Offer from %s%s%s",
            p->get_name(), CORP_NAMES[rand() % 25], suffix);

    draw_manager.draw_text(text,
        Justify::Left, TRADEBUY_TEXT_X, tradebuy_text_y(0), {0, 0xFF, 0});

    draw_manager.draw_text("We have just got hold of some wares",
        Justify::Left, TRADEBUY_TEXT_X, tradebuy_text_y(1), COL_TEXT);

    draw_manager.draw_text("which might be of interest.",
        Justify::Left, TRADEBUY_TEXT_X, tradebuy_text_y(2), COL_TEXT);

    draw_manager.draw_text("Offer",
        Justify::Centre, tradebuy_text_x(1), tradebuy_text_y(4), COL_TEXT);

    draw_manager.draw_text("Price",
        Justify::Centre, tradebuy_text_x(2), tradebuy_text_y(4), COL_TEXT);

    draw_manager.draw_text("Planet",
        Justify::Centre, tradebuy_text_x(3), tradebuy_text_y(4), COL_TEXT);

    draw_manager.draw_text("Buy",
        Justify::Centre, tradebuy_text_x(4), tradebuy_text_y(4), COL_TEXT);

    for (int i = 0; i < TRADEBUY_OPTIONS; ++i) {
        const char* icon = IMG_TD2_TR2;
        switch (tradebuy_available[i].type) {
            case TBGT_Food:
                icon = IMG_TD2_TR2;
                break;
            case TBGT_Inf:
                icon = IMG_TD2_TR4;
                break;
            case TBGT_Gli:
                icon = IMG_TD2_TR5;
                break;
            case TBGT_Art:
                icon = IMG_TD2_TR6;
                break;
        }

        draw_manager.draw(
            tradebuy_available[i].id_img,
            icon,
            {tradebuy_text_x(0), tradebuy_row_y(i),
             0.5, 0.5, 1, 1}
        );

        snprintf(text, sizeof(text), "%d", tradebuy_available[i].cost);
        draw_manager.draw_text(
            text,
            Justify::Centre,
            tradebuy_text_x(2), tradebuy_row_y(i) + TRADEBUY_TEXT_Y_OFF,
            COL_TEXT);

        int reserves = 0;
        int _i, _g, _a;
        p->get_army(_i, _g, _a);
        switch (tradebuy_available[i].type) {
            case TBGT_Food:
                reserves = p->get_reserves_food();
                break;
            case TBGT_Inf:
                reserves = _i;
                break;
            case TBGT_Gli:
                reserves = _g;
                break;
            case TBGT_Art:
                reserves = _a;
                break;
        }

        snprintf(text, sizeof(text), "%d", reserves);
        draw_manager.draw_text(
            text,
            Justify::Centre,
            tradebuy_text_x(3), tradebuy_row_y(i) + TRADEBUY_TEXT_Y_OFF,
            COL_TEXT);

        draw_manager.draw(
            tradebuy_available[i].id_adj,
            IMG_BR4_EXPORT3,
            {tradebuy_text_x(5), tradebuy_row_y(i),
             0.5, 0.5, 1, 1}
        );
    }
}

void TradeBuy::tradebuy_close() {
    input_manager.enable_repeating_clicks(false);

    for (int i = 0; i < TRADEBUY_OPTIONS; ++i) {
        draw_manager.draw(tradebuy_available[i].id_buy, nullptr);
        draw_manager.draw(tradebuy_available[i].id_unavailable, nullptr);
        draw_manager.draw(tradebuy_available[i].id_offer, nullptr);
        draw_manager.draw(tradebuy_available[i].id_buy, nullptr);
        draw_manager.draw(tradebuy_available[i].id_adj, nullptr);
        draw_manager.release_sprite_id(tradebuy_available[i].id_buy);
        draw_manager.release_sprite_id(tradebuy_available[i].id_unavailable);
        draw_manager.release_sprite_id(tradebuy_available[i].id_offer);
        draw_manager.release_sprite_id(tradebuy_available[i].id_buy);
        draw_manager.release_sprite_id(tradebuy_available[i].id_adj);
    }

    draw_manager.draw(id_tradebuy_exit,        nullptr);
    draw_manager.draw(id_tradebuy_black,       nullptr);
    draw_manager.draw(id_tradebuy_panel,       nullptr);
    draw_manager.draw(id_tradebuy_header_r,    nullptr);
    draw_manager.draw(id_tradebuy_header_l,    nullptr);
    draw_manager.draw(id_tradebuy_header_flag, nullptr);

    draw_manager.release_sprite_id(id_tradebuy_header_flag);
    draw_manager.release_sprite_id(id_tradebuy_header_l);
    draw_manager.release_sprite_id(id_tradebuy_header_r);
    draw_manager.release_sprite_id(id_tradebuy_panel);
    draw_manager.release_sprite_id(id_tradebuy_black);
    draw_manager.release_sprite_id(id_tradebuy_exit);
}

int TradeBuy::tradebuy_text_x(int idx) {
    int pad = 6;
    int sep = (TRADEBUY_W - (2*pad)) / 6;
    return TRADEBUY_X + pad + sep/2 + idx*sep;
}

int TradeBuy::tradebuy_text_y(int idx) {
    return TRADEBUY_Y + TRADEBUY_BORDER + 4 + idx * 18;
}

int TradeBuy::tradebuy_row_y(int idx) {
    return tradebuy_text_y(7 + 2*idx) + (idx-2)*6;
}

// Return true when we're done
bool TradeBuy::tradebuy_update() {
    Planet *p = exostate().get_active_planet();
    Player *owner = exostate().get_player(p->get_owner());

    char text[8];

    for (int i = 0; i < TRADEBUY_OPTIONS; ++i) {
        TradeBuyGoods &r = tradebuy_available[i];
        SpriteClick clk = draw_manager.query_click(r.id_adj);
        if (clk.id) {
            if (clk.x < 0.5f) {
                if (r.buy > 0) {
                    owner->give_mc(r.cost, MC_TradeBuy);
                    r.buy--;
                }
            } else {
                if (r.buy < r.avail) {
                    int reserves = 0;
                    int _i, _g, _a;
                    p->get_army(_i, _g, _a);
                    switch (r.type) {
                        case TBGT_Food:
                            reserves = p->get_reserves_food();
                            break;
                        case TBGT_Inf:
                            reserves = _i;
                            break;
                        case TBGT_Gli:
                            reserves = _g;
                            break;
                        case TBGT_Art:
                            reserves = _a;
                            break;
                    }

                    if ((reserves + r.buy) < p->get_resource_cap()) {
                        if (owner->attempt_spend(r.cost, MC_TradeBuy)) {
                            r.buy++;
                        }
                    }
                }
            }
        }

        if (r.avail - r.buy <= 0) {
            draw_manager.draw(
                r.id_unavailable,
                IMG_TD2_TR0,
                {tradebuy_text_x(0), tradebuy_row_y(i),
                 0.5, 0.5, 1, 1}
            );
        } else {
            draw_manager.draw(r.id_unavailable, nullptr);
        }

        snprintf(text, sizeof(text), "%d", r.avail - r.buy);
        draw_manager.draw_text(
            r.id_offer,
            text,
            Justify::Centre,
            tradebuy_text_x(1), tradebuy_row_y(i) + TRADEBUY_TEXT_Y_OFF,
            COL_TEXT);

        snprintf(text, sizeof(text), "%d", tradebuy_available[i].buy);
        draw_manager.draw_text(
            r.id_buy,
            text,
            Justify::Centre,
            tradebuy_text_x(4), tradebuy_row_y(i) + TRADEBUY_TEXT_Y_OFF,
            COL_TEXT);
    }

    if (draw_manager.query_click(id_tradebuy_exit).id) {
        for (int i = 0; i < TRADEBUY_OPTIONS; ++i) {
            TradeBuyGoods &r = tradebuy_available[i];
            switch (r.type) {
                case TBGT_Food:
                    p->adjust_reserves(0, r.buy, 0);
                    break;
                case TBGT_Inf:
                    p->adjust_army(r.buy, 0, 0);
                    break;
                case TBGT_Gli:
                    p->adjust_army(0, r.buy, 0);
                    break;
                case TBGT_Art:
                    p->adjust_army(0, 0, r.buy);
                    break;
            }
        }
        tradebuy_close();
        return true;
    }

    return false;
}
