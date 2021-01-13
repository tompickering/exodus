#ifndef GUARD_SHIP_EQUIP_H
#define GUARD_SHIP_EQUIP_H

#include "mode_base.h"

#include "draw/draw.h"

struct ShipEquipRow {
    const char* img;
    const char* name;
    int own;
    int max;
    int cost;
    int produce;
    SprID id_produce;
    SprID id_adj;
};

class ShipEquip : ModeBase {
    public:
        ShipEquip();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:
        ShipEquipRow rows[7];
        int total_cost();
};

#endif
