#ifndef GUARD_PLANET_COLONISE_H
#define GUARD_PLANET_COLONISE_H

#include "mode_base.h"

#include "galaxy/planet.h"

class PlanetColonise : ModeBase {
    public:
        PlanetColonise();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        Planet *planet;
        float time;

        enum Stage {
            PlanetOverview0,
            PlanetOverview1,
            Pixelswap,
            LaunchingText,
            Doors,
            Shuttle,
        };

        Stage stage;
};

#endif
