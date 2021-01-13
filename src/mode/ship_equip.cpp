#include "ship_equip.h"

#include "assetpaths.h"

extern ExodusState exostate;

enum ID {
    MC,
    EXIT,
    END,
};

ShipEquip::ShipEquip() : ModeBase("ShipEquip") {
}

void ShipEquip::enter() {
    Player *p = exostate.get_active_player();
    const Starship &s = p->get_starship();

    ModeBase::enter(ID::END);
    draw_manager.draw(IMG_INTRO_SPACE);
    draw_manager.save_background();
    draw_manager.show_cursor(true);

    rows[0].img = IMG_EQ1_TR1;
    rows[0].name = "Shield Generator";
    rows[0].own = s.shield_generators;
    rows[0].max = 2;
    rows[0].cost = 40;

    rows[1].img = IMG_EQ1_TR2;
    rows[1].name = "Laser Gun";
    rows[1].own = s.laser_guns;
    rows[1].max = 4;
    rows[1].cost = 100;

    rows[2].img = IMG_EQ1_TR3;
    rows[2].name = "Missile Launcher";
    rows[2].own = s.missile_launchers;
    rows[2].max = 10;
    rows[2].cost = 30;

    rows[3].img = IMG_EQ1_TR4;
    rows[3].name = "Crew";
    rows[3].own = s.crew;
    rows[3].max = 100;
    rows[3].cost = 5;

    rows[4].img = IMG_EQ1_TR5;
    rows[4].name = "Bionic Probe";
    rows[4].own = s.bionic_probes;
    rows[4].max = 8;
    rows[4].cost = 50;

    rows[5].img = IMG_EQ1_TR6;
    rows[5].name = "Escape Capsule";
    rows[5].own = s.escape_capsule ? 1 : 0;
    rows[5].max = 1;
    rows[5].cost = 200;

    rows[6].img = IMG_EQ1_TR7;
    rows[6].name = "Repair Hangar";
    rows[6].own = s.repair_hangar ? 1 : 0;
    rows[6].max = 1;
    rows[6].cost = 300;

    for (int i = 0; i < 7; ++i) {
        rows[i].id_produce = draw_manager.new_sprite_id();
        rows[i].id_adj = draw_manager.new_sprite_id();
        rows[i].produce = 0;
    }

    draw_manager.draw_text(
        "Equipment",
        Justify::Left,
        100, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Own",
        Justify::Left,
        280, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Max",
        Justify::Left,
        350, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Cost",
        Justify::Left,
        420, 40,
        COL_TEXT2);
    draw_manager.draw_text(
        "Produce",
        Justify::Left,
        490, 40,
        COL_TEXT2);

    char val[13];
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

        snprintf(val, 12, "%d", rows[i].own);
        val[12] = '\0';
        draw_manager.draw_text(
            val,
            Justify::Left,
            280, y+10,
            COL_TEXT);

        snprintf(val, 12, "%d", rows[i].max);
        val[12] = '\0';
        draw_manager.draw_text(
            val,
            Justify::Left,
            350, y+10,
            COL_TEXT);

        snprintf(val, 12, "%d", rows[i].cost);
        val[12] = '\0';
        draw_manager.draw_text(
            val,
            Justify::Left,
            420, y+10,
            COL_TEXT);

        snprintf(val, 12, "%d", rows[i].produce);
        val[12] = '\0';
        draw_manager.draw_text(
            rows[i].id_produce,
            val,
            Justify::Left,
            490, y+10,
            COL_TEXT);

        draw_manager.draw(
            rows[i].id_adj,
            IMG_BR4_EXPORT3,
            {550, y+6, 0, 0, 1, 1});
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

void ShipEquip::exit() {
    for (int i = 0; i < 7; ++i) {
        draw_manager.release_sprite_id(rows[i].id_produce);
        draw_manager.release_sprite_id(rows[i].id_adj);
    }
}

ExodusMode ShipEquip::update(float delta) {
    Player *p = exostate.get_active_player();

    char mc[32];
    snprintf(mc, 32, "%d", p->get_mc() - total_cost());
    mc[31] = '\0';
    draw_manager.draw_text(
        id(ID::MC),
        mc,
        Justify::Left,
        80, 420,
        COL_TEXT2);

    for (int i = 0; i < 7; ++i) {
        SpriteClick clk = draw_manager.query_click(rows[i].id_adj);
        if (clk.id) {
            if (clk.x < 0.5) {
                if (rows[i].produce > 0) {
                    rows[i].produce--;
                }
            } else {
                if (rows[i].produce < rows[i].max - rows[i].own) {
                    if (p->can_afford(total_cost() + rows[i].cost)) {
                        rows[i].produce++;
                    }
                }
            }
        }
    }

    for (int i = 0; i < 7; ++i) {
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
    }

    if (draw_manager.query_click(id(ID::EXIT)).id) {
        if (p->attempt_spend(total_cost())) {
            Starship &s = p->get_starship();
            s.shield_generators += rows[0].produce;
            s.laser_guns += rows[1].produce;
            s.missile_launchers += rows[2].produce;
            s.crew += rows[3].produce;
            s.bionic_probes += rows[4].produce;
            s.escape_capsule |= rows[5].produce == 1;
            s.repair_hangar |= rows[6].produce == 1;
        } else {
            L.error("Player should not be able to buy more than they can afford");
        }
        return ExodusMode::MODE_Pop;
    }

    return ExodusMode::MODE_None;
}

int ShipEquip::total_cost() {
    int cost = 0;
    for (int i = 0; i < 7; ++i) {
        cost += rows[i].cost * rows[i].produce;
    }
    return cost;
}
