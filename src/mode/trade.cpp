#include "trade.h"

#include "util/value.h"

#include "assetpaths.h"

#define ACTIVE_H 30
#define ACTIVE_W 500

static const int PANEL_W = 328;
static const int PANEL_H = 200;
static const int PANEL_X = RES_X/2 - PANEL_W/2;
static const int PANEL_Y = 150;



static const int cost_data[3][8] = {
    {5, 2, 4, 4, 6, 8, 10, 20},
    {6, 3, 5, 5, 7, 9, 14, 25},
    {4, 1, 2, 3, 5, 7,  8, 16}
};

// FIXME: This is the orig data - but are the 'bad' values supposed to be better than 'good' and 'fair' in some case?!
// The only asset that can be profitable from trading alone is plutonium (buy for 2, sell for 3)
// Unless you buy food for 1MC whilst the planet is meeting consumption, then sell it back for 2MC when it isn't
static const int buy_data[3][8] = {
    {4, 1, 3, 2, 3, 4, 7,  8},
    {3, 1, 2, 1, 2, 3, 8, 10},
    {3, 1, 1, 3, 4, 5, 6,  9}
};

static const char* OFFICER_NOT_INFORMED = "\"I am not informed...\"";
static const char* OFFER_REGULAR = "\"A regular offer.\"";
static const char* OFFER_NO_GOOD = "\"Not a good offer.\""; // Orig: No good offer
static const char* OFFER_GOOD = "\"A very good offer.\"";
static const char* OFFER_FOOD_NEEDED = "\"The planet needs food.\"";
static const char* OFFER_NO_ARMS = "\"Arms are not offered.\"";

enum ID {
    ACTIVE_T,
    ACTIVE_L,
    ACTIVE_R,
    ACTIVE_B,
    TRANSPORTERS,
    MC,
    BUTTON_BAR,
    PANEL,
    PANEL_PATTERN,
    TRADE_PANEL,
    TRADE_STOCK,
    TRADE_PRICE,
    TRADE_FREIGHT,
    TRADE_MC,
    END,
};

Trade::Trade() : ModeBase("Trade") {
    active_row = 0;
    sell = false;
}

void Trade::enter() {
    ModeBase::enter(ID::END);

    stage = Overview;

    Planet *p = exostate().get_active_planet();

    if (!(p && p->is_owned())) {
        L.fatal("Entered trade with unowned planet");
    }

    Player *p0 = exostate().get_active_player();
    Player *p1 = exostate().get_player(p->get_owner());
    int p0idx = exostate().get_player_idx(p0);
    int p1idx = exostate().get_player_idx(p1);

    if (p0 == p1) {
        L.fatal("Entered trade with own planet");
    }

    int player_idx = exostate().get_player_idx(p0);
    quality = p->initiate_trade(player_idx);

    food_needed = (p->get_n_agri() < p->get_food_consumption());
    arms_offered = exostate().is_allied(p0idx, p1idx);

    draw_manager.draw(p->sprites()->map_bg);
    draw_manager.show_cursor(true);

    audio_manager.target_music(mpart2mus(6));

    const Fleet fleet = p0->get_fleet();
    const Freight &freight = fleet.freight;
    memcpy(&freight_initial, &freight, sizeof(Freight));

    active_row = 0;
    sell = false;

    rows[0].img = IMG_TD2_TR1;
    rows[0].name = "Minerals";
    rows[0].stock = RND(10) + 10;
    rows[0].icon = ENHANCED() ? IMG_TD1_TRADE1_NEW : IMG_TD1_TRADE1;

    rows[1].img = IMG_TD2_TR2;
    rows[1].name = "Food";
    rows[1].stock = RND(20) + 20;
    rows[1].icon = ENHANCED() ? IMG_TD1_TRADE2_NEW : IMG_TD1_TRADE2;

    rows[2].img = IMG_TD2_TR3;
    rows[2].name = "Plutonium";
    rows[2].stock = RND(20) + 15;
    rows[2].icon = ENHANCED() ? IMG_TD1_TRADE3_NEW : IMG_TD1_TRADE3;

    rows[3].img = IMG_TD2_TR4;
    rows[3].name = "Infantry";
    rows[3].stock = RND(15);
    rows[3].icon = ENHANCED() ? IMG_TD1_TRADE4_NEW : IMG_TD1_TRADE4;

    rows[4].img = IMG_TD2_TR5;
    rows[4].name = "Gliders";
    rows[4].stock = RND(10);
    rows[4].icon = ENHANCED() ? IMG_TD1_TRADE5_NEW : IMG_TD1_TRADE5;

    rows[5].img = IMG_TD2_TR6;
    rows[5].name = "Artillery";
    rows[5].stock = RND(5);
    rows[5].icon = ENHANCED() ? IMG_TD1_TRADE6_NEW : IMG_TD1_TRADE6;

    rows[6].img = IMG_TD2_TR7;
    rows[6].name = "Robots";
    rows[6].stock = RND(10);
    rows[6].icon = ENHANCED() ? IMG_TD1_TRADE7_NEW : IMG_TD1_TRADE7;

    rows[7].img = IMG_TD2_TR8;
    rows[7].name = "Transporters";
    rows[7].stock = RND(10);
    rows[7].icon = ENHANCED() ? IMG_TD1_TRADE8_NEW : IMG_TD1_TRADE8;

    for (int i = 0; i < 8; ++i) {
        rows[i].cost = get_cost(quality, i);
        rows[i].buy = get_buy(quality, i);
        rows[i].id_stock = draw_manager.new_sprite_id();
        rows[i].id_freight = draw_manager.new_sprite_id();
        rows[i].id_unavailable = draw_manager.new_sprite_id();
    }

    if (!arms_offered) {
        for (int i = 3; i < 8; ++i) {
            rows[i].stock = 0;
        }
    }

    // If the planet food production < consumption, higher cost and better sell value
    if (food_needed) {
        rows[1].cost += 2;
        rows[1].buy += 1;
    }

    draw_manager.draw_text(
        "Item",
        Justify::Left,
        100, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Cost",
        Justify::Left,
        280, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Buy",
        Justify::Left,
        350, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Stock",
        Justify::Left,
        420, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Freight",
        Justify::Left,
        490, 40,
        COL_TEXT2);

    char val[13];
    for (int i = 0; i < 8; ++i) {
        int y = 80 + 40*i;
        draw_manager.draw(
            rows[i].img,
            {20, y, 0, 0, 1, 1});
        draw_manager.draw_text(
            rows[i].name,
            Justify::Left,
            100, y+10,
            COL_TEXT);

        snprintf(val, sizeof(val), "%d", rows[i].cost);
        val[12] = '\0';
        draw_manager.draw_text(
            val,
            Justify::Left,
            280, y+10,
            COL_TEXT);

        snprintf(val, sizeof(val), "%d", rows[i].buy);
        val[12] = '\0';
        draw_manager.draw_text(
            val,
            Justify::Left,
            350, y+10,
            COL_TEXT);
    }

    draw_manager.draw_text(
        "Empty transporters:",
        Justify::Left,
        20, 420,
        COL_TEXT);
    draw_manager.draw_text(
        "MCredits:",
        Justify::Left,
        RES_X/2, 420,
        COL_TEXT);

    draw_manager.fill(
        {0, 460, RES_X, 26},
        COL_BORDERS);
    draw_manager.fill(
        {18, 460, 482, 26},
        {0, 0, 0});

    draw_manager.save_background();

    draw_manager.draw(
        id(ID::BUTTON_BAR),
        IMG_BR2_EXPORT,
        {20, 460, 0, 0, 1, 1});

    input_manager.enable_repeating_clicks(true);
}

void Trade::exit() {
    for (int i = 0; i < 8; ++i) {
        draw_manager.release_sprite_id(rows[i].id_stock);
        draw_manager.release_sprite_id(rows[i].id_freight);
    }

    ModeBase::exit();
}

ExodusMode Trade::update(float delta) {
    Player *p = exostate().get_active_player();

    switch (stage) {
        case Overview:
            {
                draw_stock_freight();

                for (int i = 0; i < 8; ++i) {
                    int y = 80 + 40*i - 4;
                    if (rows[i].stock <= 0) {
                        draw_manager.draw(
                            rows[i].id_unavailable,
                            IMG_TD2_TR0,
                            {20, y, 0, 0, 1, 1});
                    } else {
                        draw_manager.draw(rows[i].id_unavailable, nullptr);
                    }
                }

                draw_manager.fill(id(ID::ACTIVE_T), {92, 86+40*active_row, ACTIVE_W, 2}, {255, 0, 0});
                draw_manager.fill(id(ID::ACTIVE_B), {92, 86+40*active_row+ACTIVE_H-2, ACTIVE_W, 2}, {255, 0, 0});
                draw_manager.fill(id(ID::ACTIVE_L), {92, 86+40*active_row, 2, ACTIVE_H}, {255, 0, 0});
                draw_manager.fill(id(ID::ACTIVE_R), {92+ACTIVE_W-2, 86+40*active_row, 2, ACTIVE_H}, {255, 0, 0});

                char empty_transporters[8];
                snprintf(empty_transporters, sizeof(empty_transporters), "%d", p->get_freight_capacity());
                draw_manager.draw_text(
                    id(ID::TRANSPORTERS),
                    empty_transporters,
                    Justify::Left,
                    220, 420,
                    COL_TEXT2);

                char mc[32];
                snprintf(mc, 32, "%d", p->get_mc());
                mc[31] = '\0';
                draw_manager.draw_text(
                    id(ID::MC),
                    mc,
                    Justify::Left,
                    RES_X/2 + 100, 420,
                    COL_TEXT2);

                if (input_manager.consume(Input::K_B)) {
                    start_trade(false);
                    stage = DoTrade;
                    return ExodusMode::MODE_None;
                }

                if (input_manager.consume(Input::K_S)) {
                    start_trade(true);
                    stage = DoTrade;
                    return ExodusMode::MODE_None;
                }

                SpriteClick clk = draw_manager.query_click(id(ID::BUTTON_BAR));
                if (clk.id) {
                    if (clk.x < .25f) {
                        open_panel();
                        stage = Info;
                        TradeRow &r = rows[active_row];
                        draw_manager.draw(r.icon, {PANEL_X+235, PANEL_Y+80, .5f, .5f, 1, 1});
                        draw_manager.draw_text(
                            r.name,
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 4,
                            COL_TEXT2);
                        char n[8];
                        snprintf(n, sizeof(n), "%d", r.stock);
                        draw_manager.draw_text(
                            "Available:",
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 24,
                            COL_TEXT);
                        draw_manager.draw_text(
                            n,
                            Justify::Left,
                            PANEL_X + 120, PANEL_Y + 24,
                            COL_TEXT);
                        snprintf(n, sizeof(n), "%d", get_freight(active_row));
                        draw_manager.draw_text(
                            "Freight:",
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 44,
                            COL_TEXT);
                        draw_manager.draw_text(
                            n,
                            Justify::Left,
                            PANEL_X + 120, PANEL_Y + 44,
                            COL_TEXT);
                        snprintf(n, sizeof(n), "%d", r.cost);
                        draw_manager.draw_text(
                            "Cost:",
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 84,
                            COL_TEXT);
                        draw_manager.draw_text(
                            n,
                            Justify::Left,
                            PANEL_X + 120, PANEL_Y + 84,
                            COL_TEXT);
                        snprintf(n, sizeof(n), "%d", r.buy);
                        draw_manager.draw_text(
                            "Buy price:",
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 104,
                            COL_TEXT);
                        draw_manager.draw_text(
                            n,
                            Justify::Left,
                            PANEL_X + 120, PANEL_Y + 104,
                            COL_TEXT);
                        draw_manager.draw_text(
                            "Counsellor:",
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 144,
                            COL_TEXT2);

                        const char* officer_opinion = OFFICER_NOT_INFORMED;
                        if (p->get_officer(OFF_Counsellor) > OFFQ_Poor) {
                            officer_opinion = OFFER_REGULAR;
                            if (quality == TRADE_Bad) officer_opinion = OFFER_NO_GOOD;
                            if (quality == TRADE_Good) officer_opinion = OFFER_GOOD;
                        }
                        if (active_row == 1 && food_needed) {
                            officer_opinion = OFFER_FOOD_NEEDED;
                        }
                        if (active_row >= 3 && (!arms_offered)) {
                            officer_opinion = OFFER_NO_ARMS;
                        }
                        draw_manager.draw_text(
                            officer_opinion,
                            Justify::Left,
                            PANEL_X + 4, PANEL_Y + 164,
                            COL_TEXT2);
                    } else if (clk.x < .5f) {
                        start_trade(false);
                        stage = DoTrade;
                    } else if (clk.x < .75f) {
                        start_trade(true);
                        stage = DoTrade;
                    } else {
                        // PROClordbuy - allows planet owner to purchase goods
                        const Fleet fleet = p->get_fleet();
                        const Freight &freight = fleet.freight;
                        int sold_f = max(0, freight_initial.food - freight.food);
                        int sold_i = max(0, freight_initial.infantry - freight.infantry);
                        int sold_g = max(0, freight_initial.gliders - freight.gliders);
                        int sold_a = max(0, freight_initial.artillery - freight.artillery);
                        if (tradebuy_start(sold_f, sold_i, sold_g, sold_a)) {
                            return ExodusMode::MODE_Pop;
                        } else {
                            /*
                             * If tradebuy_start returns false, this is
                             * a human-owned planet and we should enter into a
                             * stage where we can call tradebuy_update() until
                             * it returns true.
                             */
                            stage = TradeBuy;
                        }
                    }
                } else if (draw_manager.clicked()) {
                    for (int i = 0; i < 8; ++i) {
                        int y = 80 + 40*i;
                        if (draw_manager.mouse_in_area({0, y, RES_X, 40})) {
                            active_row = i;
                            break;
                        }
                    }
                }
            }
            break;
        case Info:
            {
                if (draw_manager.clicked()) {
                    close_panel();
                    stage = Overview;
                }
            }
            break;
        case DoTrade:
            {
                int available_y = ENHANCED() ? 45 : 15;
                int price_y = ENHANCED() ? 15 : 45;

                TradeRow &r = rows[active_row];
                char val[16];
                snprintf(val, sizeof(val), "%d", r.stock);
                draw_manager.draw_text(
                    id(ID::TRADE_STOCK),
                    val,
                    Justify::Left,
                    PANEL_X + 120, PANEL_Y + available_y,
                    COL_TEXT2);
                snprintf(val, sizeof(val), "%d", sell ? r.buy : r.cost);
                draw_manager.draw_text(
                    id(ID::TRADE_PRICE),
                    val,
                    Justify::Left,
                    PANEL_X + 120, PANEL_Y + price_y,
                    COL_TEXT2);

                RGB col = COL_TEXT_BAD;

                if (ENHANCED()) {
                    col = COL_TEXT2;
                }

                snprintf(val, sizeof(val), "%d", get_freight(active_row));
                draw_manager.draw_text(
                    id(ID::TRADE_FREIGHT),
                    val,
                    Justify::Left,
                    PANEL_X + 120, PANEL_Y + 105,
                    col);
                snprintf(val, sizeof(val), "%d", p->get_mc());
                draw_manager.draw_text(
                    id(ID::TRADE_MC),
                    val,
                    Justify::Left,
                    PANEL_X + 120, PANEL_Y + 135,
                    col);

                SpriteClick clk = draw_manager.query_click(id(ID::TRADE_PANEL));
                if (clk.id) {
                    if (clk.x > .64f) {
                        if (clk.y > .86f) {
                            draw_manager.draw(id(ID::TRADE_PANEL), nullptr);
                            close_panel();
                            // trace% (end of PROCbuysell)
                            const Fleet &fleet = p->get_fleet();
                            const Freight &freight = fleet.freight;
                            p->add_trace(TRACE_MineralsSold, max(0, freight_initial.minerals - freight.minerals));
                            p->add_trace(TRACE_FoodSold, max(0, freight_initial.food - freight.food));
                            p->add_trace(TRACE_PlutoniumSold, max(0, freight_initial.plutonium - freight.plutonium));
                            stage = Overview;
                        } else if (clk.y > .7f) {
                            bool inc = clk.x > .82f;
                            adjust_trade(inc);
                        }
                    }
                }
            }
            break;
        case TradeBuy:
            {
                if (tradebuy_update()) {
                    return ExodusMode::MODE_Pop;
                }
            }
            break;
    }

    return ExodusMode::MODE_None;
}

int Trade::get_cost(TradeQuality q, int idx) {
    int qi = 0;
    if (q == TRADE_Bad) qi = 1;
    if (q == TRADE_Good) qi = 2;
    return cost_data[qi][idx];
}

int Trade::get_buy(TradeQuality q, int idx) {
    int qi = 0;
    if (q == TRADE_Bad) qi = 1;
    if (q == TRADE_Good) qi = 2;
    return buy_data[qi][idx];
}

int Trade::get_freight(int row) {
    Player *p = exostate().get_active_player();
    const Fleet fleet = p->get_fleet();
    const Freight &freight = fleet.freight;
    if (row == 0) return freight.minerals;
    if (row == 1) return freight.food;
    if (row == 2) return freight.plutonium;
    if (row == 3) return freight.infantry;
    if (row == 4) return freight.gliders;
    if (row == 5) return freight.artillery;
    if (row == 6) return freight.robots;
    if (row == 7) return fleet.transporters;
    return 0;
}

void Trade::draw_stock_freight() {
    char val[13];
    for (int i = 0; i < 8; ++i) {
        int y = 80 + 40*i;
        snprintf(val, sizeof(val), "%d", rows[i].stock);
        val[12] = '\0';
        draw_manager.draw_text(
            rows[i].id_stock,
            val,
            Justify::Left,
            420, y+10,
            COL_TEXT);

        snprintf(val, sizeof(val), "%d", get_freight(i));
        val[12] = '\0';
        draw_manager.draw_text(
            rows[i].id_freight,
            val,
            Justify::Left,
            490, y+10,
            COL_TEXT);
    }
}

void Trade::open_panel() {
    draw_manager.fill(
        id(ID::PANEL),
        {PANEL_X - BORDER, PANEL_Y - BORDER,
         PANEL_W + 2*BORDER, PANEL_H + 2*BORDER},
         COL_BORDERS);
    draw_manager.fill_pattern(
        id(ID::PANEL_PATTERN),
        {PANEL_X, PANEL_Y,
         PANEL_W, PANEL_H});
}

void Trade::close_panel() {
    draw_manager.draw(id(ID::PANEL_PATTERN), nullptr);
    draw_manager.draw(id(ID::PANEL), nullptr);
}

void Trade::start_trade(bool _sell) {
    sell = _sell;

    TradeRow &r = rows[active_row];

    open_panel();
    draw_manager.draw(
        id(ID::TRADE_PANEL),
        ENHANCED() ? IMG_TD1_TRADEBACK_NEW : IMG_TD1_TRADEBACK,
        {PANEL_X, PANEL_Y, 0, 0, 1, 1});
    draw_manager.draw(r.icon, {PANEL_X+264, PANEL_Y+84, .5f, .5f, 1, 1});

    draw_manager.draw_text(
        sell ? "Sell" : "Buy",
        Justify::Centre,
        PANEL_X+258, PANEL_Y+10,
        COL_TEXT);

    int available_y = ENHANCED() ? 45 : 15;
    int price_y = ENHANCED() ? 15 : 45;

    draw_manager.draw_text(
        "Available:",
        Justify::Left,
        PANEL_X + 4, PANEL_Y + available_y,
        COL_TEXT);
    draw_manager.draw_text(
        "Price:",
        Justify::Left,
        PANEL_X + 4, PANEL_Y + price_y,
        COL_TEXT);

    draw_manager.draw_text(
        "Freight:",
        Justify::Left,
        PANEL_X + 4, PANEL_Y + 105,
        COL_TEXT);
    draw_manager.draw_text(
        "MCredits:",
        Justify::Left,
        PANEL_X + 4, PANEL_Y + 135,
        COL_TEXT);

    draw_manager.refresh_sprite_id(id(ID::TRADE_STOCK));
    draw_manager.refresh_sprite_id(id(ID::TRADE_PRICE));
    draw_manager.refresh_sprite_id(id(ID::TRADE_FREIGHT));
    draw_manager.refresh_sprite_id(id(ID::TRADE_MC));

    bought = 0;
    sold = 0;
}

void Trade::adjust_trade(bool inc) {
    Player *p = exostate().get_active_player();
    TradeRow &r = rows[active_row];

    int price = sell ? r.buy : r.cost;
    bool transp = (active_row == 7);

    if (sell) {
        if (!inc && (sold == 0)) {
            // Can't reduce sold amount past 0
            return;
        }
        if (inc && !get_freight(active_row)) {
            // We have none to sell
            return;
        }
        if (inc && transp && (p->get_freight_capacity() <= 0)) {
            // We have no empty transporters to sell
            return;
        }

        if (inc) {
            p->give_mc(price, MC_Trade);
            sold++;
            r.stock++;
            adjust_freight(active_row, false);
        } else {
            if (p->attempt_spend(price, MC_Trade))
            {
                r.stock--;
                sold--;
                adjust_freight(active_row, true);
            } else {
                L.error("We should be able to afford to return goods we just sold");
            }
        }
    } else {
        if (!inc && (bought == 0)) {
            // Can't reduce bought amount past 0
            return;
        }
        if (inc && r.stock <= 0) {
            // There are none to buy
            return;
        }
        if (inc && !transp && (p->get_freight_capacity() <= 0)) {
            // We don't have room for these goods
            return;
        }

        if (inc) {
            if (p->attempt_spend(price, MC_Trade))
            {
                r.stock--;
                bought++;
                adjust_freight(active_row, true);
            }
        } else {
            p->give_mc(price, MC_Trade);
            r.stock++;
            bought--;
            adjust_freight(active_row, false);
        }
    }
}

void Trade::adjust_freight(int row, bool inc) {
    Player *p = exostate().get_active_player();
    Fleet &fleet = p->get_fleet_nonconst();
    int off = inc ? 1 : -1;
    bool ok = false;
    if (row == 0) ok = p->transfer_min(off) != 0;
    if (row == 1) ok = p->transfer_fd(off) != 0;
    if (row == 2) ok = p->transfer_plu(off) != 0;
    if (row == 3) ok = p->transfer_inf(off) != 0;
    if (row == 4) ok = p->transfer_gli(off) != 0;
    if (row == 5) ok = p->transfer_art(off) != 0;
    if (row == 6) ok = p->transfer_robots(off) != 0;
    if (row == 7) {
        if (inc || fleet.transporters > 0) {
            fleet.transporters += off;
            ok = true;
        }
    }

    if (!ok) {
        L.fatal("Could not adjust freight");
    }
}
