#ifndef GUARD_PLANET_H
#define GUARD_PLANET_H

enum PlanetSize {
    PLANET_Small,
    PLANET_Medium,
    PLANET_Large,
};

enum PlanetClass {
    Forest,      // 1  -
    Desert,      // 2   |
    Volcano,     // 3   |
    Rock,        // 4    > Original codes for planet types
    Ice,         // 5   |
    Terra,       // 6   |
    Artificial,  // 8  -
};

class Planet {
    private:
        unsigned int minerals;
};

#endif
