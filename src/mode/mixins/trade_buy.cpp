#include "trade_buy.h"

#include "draw/draw.h"
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

extern DRAWMANAGER draw_manager;
extern ExodusState exostate;

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
    Planet *p = exostate.get_active_planet();
    Player *owner = exostate.get_player(p->get_owner());

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

                if (owner->attempt_spend(to_buy * cost)) {
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

                if (owner->attempt_spend(to_buy * cost)) {
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

                if (owner->attempt_spend(to_buy * cost)) {
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

                if (owner->attempt_spend(to_buy * cost)) {
                    p->adjust_army(0, 0, to_buy);
                } else {
                    L.fatal("TradeBuy: Could not spend MC we checked we had (art)");
                }
            }
        }

        return true;
    }

    // Human player

    tradebuy_available[0].type = TBGT_Food;
    tradebuy_available[0].avail = fd;

    tradebuy_available[1].type = TBGT_Inf;
    tradebuy_available[1].avail = inf;

    tradebuy_available[2].type = TBGT_Gli;
    tradebuy_available[2].avail = gli;

    tradebuy_available[3].type = TBGT_Art;
    tradebuy_available[3].avail = art;

    tradebuy_open();

    return false;
}

void TradeBuy::tradebuy_open() {
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

    draw_manager.draw(
        id_tradebuy_header_l,
        IMG_TS1_HEADER,
        {TRADEBUY_FLAG_BG_X - 2,
         TRADEBUY_Y - 2,
         1, 1, 1, 1});

    draw_manager.draw(
        id_tradebuy_header_r,
        IMG_TS1_HEADER,
        {TRADEBUY_FLAG_BG_X + TRADEBUY_FLAG_BG_W + 2,
         TRADEBUY_Y - 2,
         0, 1, 1, 1});

    draw_manager.draw(
        id_tradebuy_exit,
        IMG_BR5_EXPORT4,
        {TRADEBUY_X + 10,
         TRADEBUY_Y + TRADEBUY_H - 10,
         0, 1, 1, 1});
}

void TradeBuy::tradebuy_close() {
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

// Return true when we're done
bool TradeBuy::tradebuy_update() {
    Planet *p = exostate.get_active_planet();
    Player *owner = exostate.get_player(p->get_owner());

    // TODO: Present UI and perform updates until human player is done
    if (draw_manager.query_click(id_tradebuy_exit).id) {
        return true;
    }

    return false;
}
