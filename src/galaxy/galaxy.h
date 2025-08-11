#ifndef GUARD_GALAXY_H
#define GUARD_GALAXY_H

#include "save/saveable.h"

#include "guild.h"
#include "star.h"

#define GALAXY_MAX_STARS 40

#define GALAXY_COLS 27
#define GALAXY_ROWS 17
#define GUILD_POS_X 14
#define GUILD_POS_Y 9

class Galaxy : public Saveable {
    public:
        virtual void save(cJSON* json) const override;
        virtual void load(cJSON* json) override;

        Galaxy() {};
        Galaxy(int);
        Star* get_stars();
        Star* get_stars(int&);
        Guild* get_guild();
    private:
        Guild guild;
        Star stars[GALAXY_MAX_STARS];
        int n_stars;
};

#endif
