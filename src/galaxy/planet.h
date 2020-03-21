#ifndef GUARD_PLANET_H
#define GUARD_PLANET_H

enum PlanetSize {
    PLANET_Small,
    PLANET_Medium,
    PLANET_Large,
};

enum PlanetClass {
    PLANET_Forest,
    PLANET_Desert,
    PLANET_Rock,
    PLANET_Volcano,
    PLANET_Terra,
    PLANET_Ice,
    PLANET_Artificial,
};

class Planet {
    private:
        unsigned int minerals;
};

#endif
