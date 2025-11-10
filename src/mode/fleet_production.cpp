#include "fleet_production.h"

#include "assetpaths.h"



enum ID {
    MC,
    EXIT,
    END,
};

FleetProduction::FleetProduction() : ModeBase("FleetProduction") {
}

void FleetProduction::enter() {
    Player *p = exostate().get_active_player();
    Planet *planet = exostate().get_active_planet();
    const Fleet &fleet = p->get_fleet();

    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_INTRO_SPACE);
    draw_manager.save_background();
    draw_manager.show_cursor(true);

    input_manager.enable_repeating_clicks(true);

    rows[0].img = IMG_FL1_TR1;
    rows[0].name = "Scouts";
    rows[0].own = fleet.scouts;
    rows[0].max = 10000;
    rows[0].cost = 10;

    rows[1].img = IMG_FL1_TR2;
    rows[1].name = "Transporters";
    rows[1].own = fleet.transporters;
    rows[1].max = 10000;
    rows[1].cost = 15;

    rows[2].img = IMG_FL1_TR3;
    rows[2].name = "Warships";
    rows[2].own = fleet.warships;
    rows[2].max = 10000;
    rows[2].cost = 20;

    rows[3].img = IMG_FL1_TR4;
    rows[3].name = "Bombers";
    rows[3].own = fleet.bombers;
    rows[3].max = 10000;
    rows[3].cost = 25;

    rows[4].img = IMG_FL1_TR6;
    rows[4].name = "AirDef guns";
    rows[4].own = planet->get_airdef_guns();
    rows[4].max = planet->get_resource_cap();
    rows[4].cost = COST_AIRDEF;

    rows[5].img = IMG_FL1_TR7;
    rows[5].name = "Robots";
    rows[5].own = planet->get_robots();
    rows[5].max = planet->get_robot_cap();
    rows[5].cost = COST_ROBOT;

    for (int i = 0; i < 6; ++i) {
        rows[i].id_produce = draw_manager.new_sprite_id();
        rows[i].id_adj = draw_manager.new_sprite_id();
        rows[i].produce = 0;
        rows[i].y = 80 + 44*(i < 4 ? i : i+1);
    }

    draw_manager.draw_text(
        "FLEET",
        Justify::Left,
        100, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Own",
        Justify::Centre,
        260, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Cost",
        Justify::Centre,
        360, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Produce",
        Justify::Centre,
        460, 40,
        COL_TEXT2);

    char val[13];
    for (int i = 0; i < 6; ++i) {
        draw_manager.draw(
            rows[i].img,
            {20, rows[i].y, 0, 0, 1, 1});
        draw_manager.draw_text(
            rows[i].name,
            Justify::Left,
            100, rows[i].y+10,
            COL_TEXT);

        snprintf(val, 12, "%d", rows[i].own);
        val[12] = '\0';
        draw_manager.draw_text(
            val,
            Justify::Centre,
            260, rows[i].y+10,
            COL_TEXT);

        snprintf(val, 12, "%d", rows[i].cost);
        val[12] = '\0';
        draw_manager.draw_text(
            val,
            Justify::Centre,
            360, rows[i].y+10,
            COL_TEXT);

        draw_manager.draw(
            rows[i].id_adj,
            IMG_BR4_EXPORT3,
            {550, rows[i].y+6, 0, 0, 1, 1});
    }

    draw_manager.draw_text(
        "MC:",
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

void FleetProduction::exit() {
    for (int i = 0; i < 6; ++i) {
        draw_manager.release_sprite_id(rows[i].id_produce);
        draw_manager.release_sprite_id(rows[i].id_adj);
    }

    ModeBase::exit();
}

ExodusMode FleetProduction::update(float delta) {
    Player *p = exostate().get_active_player();
    Planet *planet = exostate().get_active_planet();

    char mc[32];
    snprintf(mc, 32, "%d", p->get_mc() - total_cost());
    mc[31] = '\0';
    draw_manager.draw_text(
        id(ID::MC),
        mc,
        Justify::Left,
        80, 420,
        COL_TEXT2);

    for (int i = 0; i < 6; ++i) {
        SpriteClick clk = draw_manager.query_click(rows[i].id_adj);
        if (clk.id) {
            if (clk.x < 0.5) {
                if (rows[i].produce > 0) {
                    rows[i].produce--;
                }
            } else {
                if (rows[i].max <= 0 || rows[i].produce < rows[i].max - rows[i].own) {
                    if (p->can_afford(total_cost() + rows[i].cost)) {
                        rows[i].produce++;
                    }
                }
            }
        }
    }

    for (int i = 0; i < 6; ++i) {
        char val[13];
        snprintf(val, 12, "%d", rows[i].produce);
        val[12] = '\0';
        draw_manager.draw_text(
            rows[i].id_produce,
            val,
            Justify::Centre,
            460, rows[i].y+10,
            COL_TEXT);
    }

    if (draw_manager.query_click(id(ID::EXIT)).id) {
        if (p->attempt_spend(total_cost(), MC_FleetProd)) {
            Fleet &f = p->get_fleet_nonconst();
            f.scouts += rows[0].produce;
            f.transporters += rows[1].produce;
            f.warships += rows[2].produce;
            f.bombers += rows[3].produce;
            planet->adjust_airdef_guns(rows[4].produce);
            planet->adjust_robots(rows[5].produce);
        } else {
            L.error("Player should not be able to buy more fleet than they can afford");
        }
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}

int FleetProduction::total_cost() {
    int cost = 0;
    for (int i = 0; i < 6; ++i) {
        cost += rows[i].cost * rows[i].produce;
    }
    return cost;
}
