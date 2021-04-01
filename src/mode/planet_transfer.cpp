#include "planet_transfer.h"

#include "assetpaths.h"

extern ExodusState exostate;

enum ID {
    CARGO,
    EXIT,
    END,
};

PlanetTransfer::PlanetTransfer() : ModeBase("PlanetTransfer") {
}

void PlanetTransfer::enter() {
    Player *player = exostate.get_active_player();
    Planet *planet = exostate.get_active_planet();

    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_PLANETSIDE);
    draw_manager.save_background();
    draw_manager.show_cursor(true);

    rows[0].img = IMG_TD2_TR1;
    rows[0].name = "Minerals";

    rows[1].img = IMG_TD2_TR2;
    rows[1].name = "Food";

    rows[2].img = IMG_TD2_TR3;
    rows[2].name = "Plutonium";

    rows[3].img = IMG_TD2_TR4;
    rows[3].name = "Infantry";

    rows[4].img = IMG_TD2_TR5;
    rows[4].name = "Gliders";

    rows[5].img = IMG_TD2_TR6;
    rows[5].name = "Artillery";

    rows[6].img = IMG_TD2_TR7;
    rows[6].name = "Robots";

    for (int i = 0; i < 7; ++i) {
        rows[i].id_planet = draw_manager.new_sprite_id();
        rows[i].id_freight = draw_manager.new_sprite_id();
        rows[i].id_adj = draw_manager.new_sprite_id();
    }

    draw_manager.draw_text(
        "Item",
        Justify::Left,
        100, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Planet",
        Justify::Left,
        280, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Freight",
        Justify::Left,
        350, 40,
        COL_TEXT2);

    draw_manager.draw_text(
        "Cargo space:",
        Justify::Left,
        20, 420,
        COL_TEXT);

    draw_manager.fill(
        {0, 460, RES_X, 26},
        COL_BORDERS);
    draw_manager.fill(
        {18, 460, 122, 26},
        {0, 0, 0});
    draw_manager.draw(
        id(ID::EXIT),
        IMG_BR5_EXPORT4,
        {20, 460, 0, 0, 1, 1});
}

ExodusMode PlanetTransfer::update(float delta) {
    Player *player = exostate.get_active_player();
    Planet *planet = exostate.get_active_planet();

    char cargo[32];
    snprintf(cargo, 32, "%d", player->get_freight_capacity());
    cargo[31] = '\0';
    draw_manager.draw_text(
        id(ID::CARGO),
        cargo,
        Justify::Left,
        150, 420,
        COL_TEXT2);

    for (int i = 0; i < 7; ++i) {
        SpriteClick clk = draw_manager.query_click(rows[i].id_adj);
        if (clk.id) {
            if (clk.x < 0.5) {
                /*
                if (rows[i].produce > 0) {
                    rows[i].produce--;
                }
                */
            } else {
                /*
                if (rows[i].produce < rows[i].max - rows[i].own) {
                    if (p->can_afford(total_cost() + rows[i].cost)) {
                        rows[i].produce++;
                    }
                }
                */
            }
        }
    }

    for (int i = 0; i < 7; ++i) {
        /*
        int y = 80 + 40*i;
        char val[12];
        snprintf(val, 12, "%d", rows[i].produce);
        val[12] = '\0';
        draw_manager.draw_text(
            rows[i].id_produce,
            val,
            Justify::Left,
            490, y+10,
            COL_TEXT);
        */
    }

    if (draw_manager.query_click(id(ID::EXIT)).id) {
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
