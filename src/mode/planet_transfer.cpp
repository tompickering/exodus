#include "planet_transfer.h"

#include "assetpaths.h"



static const int PLANET_X = 250;
static const int FREIGHT_X = 350;
static const int ADJ_X = 430;

enum ID {
    CARGO,
    EXIT,
    END,
};

PlanetTransfer::PlanetTransfer() : ModeBase("PlanetTransfer") {
}

void PlanetTransfer::enter() {
    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_PLANETSIDE);
    draw_manager.save_background();
    draw_manager.show_cursor(true);

    input_manager.enable_repeating_clicks(true);

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
        Justify::Centre,
        PLANET_X, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Freight",
        Justify::Centre,
        FREIGHT_X, 40,
        COL_TEXT2);

    for (int i = 0; i < 7; ++i) {
        int y = 80 + 40*i;
        draw_manager.draw(
            rows[i].img,
            {20, y, 0, 0, 1, 1});
        draw_manager.draw_text(
            rows[i].name,
            Justify::Left,
            100, y+10,
            COL_TEXT);
        draw_manager.draw(
            rows[i].id_adj,
            IMG_BR4_EXPORT3,
            {ADJ_X, y+6, 0, 0, 1, 1});
    }

    draw_manager.draw_text(
        "Cargo space:",
        Justify::Left,
        20, 420,
        COL_TEXT);

    draw_manager.fill(
        FILL_3D_Out,
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
    Player *player = exostate().get_active_player();
    Planet *planet = exostate().get_active_planet();

    char cargo[32];
    snprintf(cargo, 32, "%d", player->get_freight_capacity());
    cargo[31] = '\0';
    draw_manager.draw_text(
        id(ID::CARGO),
        cargo,
        Justify::Left,
        150, 420,
        COL_TEXT2);

    // FIXME: It would be nice to have a more general way to approach this...

    int p_lim = planet->get_resource_cap();
    int p_inf, p_gli, p_art;
    int p_min = planet->get_reserves_min();
    int p_fd = planet->get_reserves_food();
    int p_plu = planet->get_reserves_plu();
    int p_rob = planet->get_robots();
    planet->get_army(p_inf, p_gli, p_art);

    const Freight &f = player->get_fleet().freight;
    int f_min = f.minerals;
    int f_fd = f.food;
    int f_plu = f.plutonium;
    int f_inf = f.infantry;
    int f_gli = f.gliders;
    int f_art = f.artillery;
    int f_rob = f.robots;

    int p_capacity = 0;
    int f_capacity = player->get_freight_capacity();

    for (int i = 0; i < 7; ++i) {
        int val_planet = 0;
        int val_freight = 0;

        switch (i) {
            case 0:
                val_planet = p_min;
                val_freight = f_min;
                p_capacity = p_lim - p_min;
                break;
            case 1:
                val_planet = p_fd;
                val_freight = f_fd;
                p_capacity = p_lim - p_fd;
                break;
            case 2:
                val_planet = p_plu;
                val_freight = f_plu;
                p_capacity = p_lim - p_plu;
                break;
            case 3:
                val_planet = p_inf;
                val_freight = f_inf;
                p_capacity = p_lim - p_inf;
                break;
            case 4:
                val_planet = p_gli;
                val_freight = f_gli;
                p_capacity = p_lim - p_gli;
                break;
            case 5:
                val_planet = p_art;
                val_freight = f_art;
                p_capacity = p_lim - p_art;
                break;
            case 6:
                val_planet = p_rob;
                val_freight = f_rob;
                p_capacity = p_lim - p_rob;
                break;
        }

        SpriteClick clk = draw_manager.query_click(rows[i].id_adj);

        if (clk.id) {
            bool to_freight = (clk.x >= 0.5);

            if (to_freight) {
                if (f_capacity <= 0 || val_planet <= 0) {
                    continue;
                }
            } else {
                if (p_capacity <= 0 || val_freight <= 0) {
                    continue;
                }
            }

            int d = to_freight ? 1 : -1;

            switch (i) {
                case 0:
                    planet->adjust_reserves(player->transfer_min(d), 0, 0);
                    break;
                case 1:
                    planet->adjust_reserves(0, player->transfer_fd(d), 0);
                    break;
                case 2:
                    planet->adjust_reserves(0, 0, player->transfer_plu(d));
                    break;
                case 3:
                    planet->adjust_army(player->transfer_inf(d), 0, 0);
                    break;
                case 4:
                    planet->adjust_army(0, player->transfer_gli(d), 0);
                    break;
                case 5:
                    planet->adjust_army(0, 0, player->transfer_art(d));
                    break;
                case 6:
                    planet->adjust_robots(player->transfer_robots(d));
                    break;
            }
        }

        int y = 80 + 40*i;
        char val[13];

        snprintf(val, sizeof(val), "%d", val_planet);
        val[12] = '\0';
        draw_manager.draw_text(
            rows[i].id_planet,
            val,
            Justify::Centre,
            PLANET_X, y+10,
            COL_TEXT);

        snprintf(val, sizeof(val), "%d", val_freight);
        val[12] = '\0';
        draw_manager.draw_text(
            rows[i].id_freight,
            val,
            Justify::Centre,
            FREIGHT_X, y+10,
            COL_TEXT);
    }

    if (draw_manager.query_click(id(ID::EXIT)).id) {
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}
