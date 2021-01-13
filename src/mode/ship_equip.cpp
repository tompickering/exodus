#include "ship_equip.h"

#include "assetpaths.h"

extern ExodusState exostate;

enum ID {
    MC,
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

    for (int i = 0; i < 7; ++i) {
        int y = 80 + 40*i;
        draw_manager.draw(
            rows[i].id_adj,
            IMG_BR4_EXPORT3,
            {300, y, 0, 0, 1, 1}
        );
    }
}

void ShipEquip::exit() {
    for (int i = 0; i < 7; ++i) {
        draw_manager.release_sprite_id(rows[i].id_produce);
        draw_manager.release_sprite_id(rows[i].id_adj);
    }
}

ExodusMode ShipEquip::update(float delta) {
    return ExodusMode::MODE_None;
}
