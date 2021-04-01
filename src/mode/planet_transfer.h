#ifndef GUARD_PLANET_TRANSFER_H
#define GUARD_PLANET_TRANSFER_H

#include "mode_base.h"

#include "draw/draw.h"

struct PlanetTransferRow {
    const char* img;
    const char* name;
    SprID id_planet;
    SprID id_freight;
    SprID id_adj;
};

class PlanetTransfer : ModeBase {
    public:
        PlanetTransfer();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        PlanetTransferRow rows[7];
};

#endif
