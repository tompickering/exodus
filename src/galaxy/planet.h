#ifndef GUARD_PLANET_H
#define GUARD_PLANET_H

#define PLANET_MAX_NAME 20

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

typedef struct {
    const char *panel_icon;
} PlanetSpriteSet;

class Planet {
    public:
        Planet();
        Planet(PlanetClass);
        bool exists();
        PlanetClass get_class();
        int get_income();            // Orig: SIs
        int get_n_cities();          // Orig: SIc
        int get_n_agri();            // Orig: SIna
        const char* get_name();      // Orig: SIn
        void set_name(const char*);
        const PlanetSpriteSet* sprites();
    private:
        void init();

        bool _exists;

        PlanetClass cls;
        char name[PLANET_MAX_NAME + 1];
        int day_hours;         // Orig: pspeed - except we multiply by 4 here.
        int sit;               // Orig: SIt. Trade-related?
        int sid;               // Orig: SId. Size-related?
        int minerals;          // Orig: SIrm.
        int income_adj;        // Adjustment to class value, instead of changing orig SIs
        int sim;               // Orig: SIm. Battle-related?

        int owner;             // Orig: SIi

        bool lunar_base;       // Orig: SIb
        int unrest;            // Orig: SIr
        int army_funding;      // Orig: SIam
        int airdef_guns;       // Orig: SIv
        int robots;            // Orig: SIgr

        int reserves_min;      // Orig: SIg(1)
        int reserves_food;     // Orig: SIg(2)
        int reserves_plu;      // Orig: SIg(3)

        int army_inf;          // Orig: SIk(1)
        int army_gli;          // Orig: SIk(2)
        int army_art;          // Orig: SIk(3)

        // TODO: SIu, SIpo, SIl, SItb
};

#endif
