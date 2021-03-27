#include "trade.h"

#include "assetpaths.h"

extern ExodusState exostate;

enum ID {
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

    active_row = 0;
    sell = false;

    rows[0].img = IMG_TD2_TR1;
    rows[0].name = "Minerals";
    rows[0].cost = 0;
    rows[0].buy = 0;
    rows[0].stock = 0;
    rows[0].freight = 0;

    rows[1].img = IMG_TD2_TR2;
    rows[1].name = "Food";
    rows[1].cost = 0;
    rows[1].buy = 0;
    rows[1].stock = 0;
    rows[1].freight = 0;

    rows[2].img = IMG_TD2_TR3;
    rows[2].name = "Plutonium";
    rows[2].cost = 0;
    rows[2].buy = 0;
    rows[2].stock = 0;
    rows[2].freight = 0;

    rows[3].img = IMG_TD2_TR4;
    rows[3].name = "Infantry";
    rows[3].cost = 0;
    rows[3].buy = 0;
    rows[3].stock = 0;
    rows[3].freight = 0;

    rows[4].img = IMG_TD2_TR5;
    rows[4].name = "Gliders";
    rows[4].cost = 0;
    rows[4].buy = 0;
    rows[4].stock = 0;
    rows[4].freight = 0;

    rows[5].img = IMG_TD2_TR6;
    rows[5].name = "Artillery";
    rows[5].cost = 0;
    rows[5].buy = 0;
    rows[5].stock = 0;
    rows[5].freight = 0;

    rows[6].img = IMG_TD2_TR7;
    rows[6].name = "Robots";
    rows[6].cost = 0;
    rows[6].buy = 0;
    rows[6].stock = 0;
    rows[6].freight = 0;

    rows[7].img = IMG_TD2_TR8;
    rows[7].name = "Transporters";
    rows[7].cost = 0;
    rows[7].buy = 0;
    rows[7].stock = 0;
    rows[7].freight = 0;

    for (int i = 0; i < 8; ++i) {
        rows[i].id_stock = draw_manager.new_sprite_id();
        rows[i].id_freight = draw_manager.new_sprite_id();
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

    // TODO
    draw_manager.draw_text(
        id(ID::TRANSPORTERS),
        "XXX",
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
    }

    return ExodusMode::MODE_None;
}
