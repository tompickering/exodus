#include "trade.h"

#include "assetpaths.h"

#define ACTIVE_H 30
#define ACTIVE_W 500

extern ExodusState exostate;

static const int cost_data[3][8] = {
    {5, 2, 4, 4, 6, 8, 10, 20},
    {6, 3, 5, 5, 7, 9, 14, 25},
    {4, 1, 2, 3, 5, 7,  8, 16}
};

// FIXME: This is the orig data - but are the 'bad' values supposed to be better than 'good' and 'fair' in some case?!
static const int buy_data[3][8] = {
    {4, 1, 3, 2, 3, 4, 7,  8},
    {3, 1, 2, 1, 2, 3, 8, 10},
    {3, 1, 1, 3, 4, 5, 6,  9}
};

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
    END,
};

Trade::Trade() : ModeBase("Trade") {
    active_row = 0;
    sell = false;
}

void Trade::enter() {
    Planet *p = exostate.get_active_planet();

    if (!(p && p->is_owned())) {
        L.fatal("Entered trade with unowned planet");
    }

    Player *p0 = exostate.get_active_player();
    Player *p1 = exostate.get_player(p->get_owner());
    int p0idx = exostate.get_player_idx(p0);
    int p1idx = exostate.get_player_idx(p1);

    if (p0 == p1) {
        L.fatal("Entered trade with own planet");
    }

    int player_idx = exostate.get_player_idx(p0);
    TradeQuality quality = p->initiate_trade(player_idx);

    //const Fleet &fleet = p->get_fleet();
    //const Freight &freight = p->get_freight();

    ModeBase::enter(ID::END);
    draw_manager.draw(p->sprites()->map_bg);
    draw_manager.save_background();
    draw_manager.show_cursor(true);

    audio_manager.target_music(mpart2mus(6));

    active_row = 0;
    sell = false;

    rows[0].img = IMG_TD2_TR1;
    rows[0].name = "Minerals";
    rows[0].stock = RND(10) + 10;

    rows[1].img = IMG_TD2_TR2;
    rows[1].name = "Food";
    rows[1].stock = RND(20) + 20;

    rows[2].img = IMG_TD2_TR3;
    rows[2].name = "Plutonium";
    rows[2].stock = RND(20) + 15;

    rows[3].img = IMG_TD2_TR4;
    rows[3].name = "Infantry";
    rows[3].stock = RND(15);

    rows[4].img = IMG_TD2_TR5;
    rows[4].name = "Gliders";
    rows[4].stock = RND(10);

    rows[5].img = IMG_TD2_TR6;
    rows[5].name = "Artillery";
    rows[5].stock = RND(5);

    rows[6].img = IMG_TD2_TR7;
    rows[6].name = "Robots";
    rows[6].stock = RND(10);

    rows[7].img = IMG_TD2_TR8;
    rows[7].name = "Transporters";
    rows[7].stock = RND(10);

    for (int i = 0; i < 8; ++i) {
        rows[i].cost = get_cost(quality, i);
        rows[i].buy = get_buy(quality, i);
        rows[i].id_stock = draw_manager.new_sprite_id();
        rows[i].id_freight = draw_manager.new_sprite_id();
        // TODO: Implement this
        rows[i].id_unavailable = draw_manager.new_sprite_id();
    }

    if (!exostate.is_allied(p0idx, p1idx)) {
        for (int i = 3; i < 8; ++i) {
            rows[i].stock = 0;
        }
    }

    // If the planet food production < consumption, higher cost and better sell value
    if (p->get_n_agri() < p->get_food_consumption()) {
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

        snprintf(val, 12, "%d", rows[i].cost);
        val[12] = '\0';
        draw_manager.draw_text(
            val,
            Justify::Left,
            280, y+10,
            COL_TEXT);

        snprintf(val, 12, "%d", rows[i].buy);
        val[12] = '\0';
        draw_manager.draw_text(
            val,
            Justify::Left,
            350, y+10,
            COL_TEXT);

        snprintf(val, 12, "%d", rows[i].stock);
        val[12] = '\0';
        draw_manager.draw_text(
            rows[i].id_stock,
            val,
            Justify::Left,
            420, y+10,
            COL_TEXT);

        snprintf(val, 12, "%d", rows[i].freight);
        val[12] = '\0';
        draw_manager.draw_text(
            rows[i].id_freight,
            val,
            Justify::Left,
            490, y+10,
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
    draw_manager.draw(
        id(ID::BUTTON_BAR),
        IMG_BR2_EXPORT,
        {20, 460, 0, 0, 1, 1});
}

void Trade::exit() {
    for (int i = 0; i < 8; ++i) {
        draw_manager.release_sprite_id(rows[i].id_stock);
        draw_manager.release_sprite_id(rows[i].id_freight);
    }
}

ExodusMode Trade::update(float delta) {
    Player *p = exostate.get_active_player();

    switch (stage) {
        case Overview:
            {
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

                SpriteClick clk = draw_manager.query_click(id(ID::BUTTON_BAR));
                if (clk.id) {
                    if (clk.x < .25f) {
                        // TODO: Info
                    } else if (clk.x < .5f) {
                        sell = false;
                        // TODO: Open trade panel
                    } else if (clk.x < .75f) {
                        sell = true;
                        // TODO: Open trade panel
                    } else {
                        return ExodusMode::MODE_Pop;
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
                // TODO
                stage = Overview;
            }
            break;
        case DoTrade:
            {
                // TODO
                stage = Overview;
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

void Trade::draw_stock_freight() {
}
