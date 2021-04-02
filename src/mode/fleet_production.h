#ifndef GUARD_FLEET_PRODUCTION_H
#define GUARD_FLEET_PRODUCTION_H

#include "mode_base.h"

#include "draw/draw.h"

struct FleetProductionRow {
    const char* img;
    const char* name;
    int own;
    int max;
    int cost;
    int produce;
    int y;
    SprID id_produce;
    SprID id_adj;
};

class FleetProduction : ModeBase {
    public:
        FleetProduction();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:
        FleetProductionRow rows[6];
        int total_cost();
};

#endif
