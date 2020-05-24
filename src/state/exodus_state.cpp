#include "exodus_state.h"

#include <cstring>

#include "assetpaths.h"
#include "shared.h"

const char* flags[] = {
    IMG_TS1_FLAG13,
    IMG_TS1_FLAG14,
    IMG_TS1_FLAG15,
    IMG_TS1_FLAG10,
    IMG_TS1_FLAG11,
    IMG_TS1_FLAG12,
    IMG_TS1_FLAG7,
    IMG_TS1_FLAG8,
    IMG_TS1_FLAG9,
    IMG_TS1_FLAG4,
    IMG_TS1_FLAG5,
    IMG_TS1_FLAG6,
    IMG_TS1_FLAG1,
    IMG_TS1_FLAG2,
    IMG_TS1_FLAG3,
};

ExodusState::ExodusState() {
    galaxy_finalised = false;
    active_flytarget_is_guild = false;
}

void ExodusState::init(GameConfig config) {
    galaxy_finalised = false;
    size = config.size;
    n_players = config.n_players;
    month = 1;
    active_player = 0;
    active_star = 0;
    active_planet = -1;
    unsigned int i;

    // PLAYER INIT: Everyone (overridable)
    for (i = 0; i < N_PLAYERS; ++i) {
        players[i].mc = 0;

        players[i].fleet.scouts = 0;
        players[i].fleet.transporters = 0;
        players[i].fleet.warships = 0;
        players[i].fleet.bombers = 0;
        players[i].fleet.freight.minerals = 0;
        players[i].fleet.freight.food = 0;
        players[i].fleet.freight.plutonium = 0;
        players[i].fleet.freight.robots = 0;
        players[i].fleet.freight.infantry = 0;
        players[i].fleet.freight.gliders = 0;
        players[i].fleet.freight.artillery = 0;
    }

    // PLAYER INIT: Human
    for (i = 0; i < n_players; ++i) {
        memcpy(&players[i], &config.players[i], sizeof(Player));
        players[i].race = RACE_Human;
        players[i]._intro_seen = false;
        switch(i) {
            case 0:
                players[i].fleet_marker = IMG_TS1_ICON1;
                break;
            case 1:
                players[i].fleet_marker = IMG_TS1_ICON2;
                break;
            case 2:
                players[i].fleet_marker = IMG_TS1_ICON3;
                break;
            case 3:
                players[i].fleet_marker = IMG_TS1_ICON4;
                break;
            case 4:
                players[i].fleet_marker = IMG_TS1_ICON5;
                break;
        }

        if (config.enemy_start == ENEMY_None) {
            players[i].mc = 1000;
            players[i].fleet.transporters = 50;
            players[i].fleet.warships = 5;
        }

        if (config.enemy_start == ENEMY_Weak) {
            players[i].mc = 300;
            players[i].fleet.transporters = 50;
            players[i].fleet.warships = 5;
        }

        if (config.enemy_start == ENEMY_Medium) {
            players[i].mc = 200;
            players[i].fleet.transporters = 35;
            players[i].fleet.warships = 5;
        }

        if (config.enemy_start == ENEMY_Strong) {
            players[i].mc = 50;
            players[i].fleet.transporters = 20;
            players[i].fleet.freight.minerals = 20;
        }
    }

    // PLAYER INIT: CPU
    for (; i < N_PLAYERS; ++i) {
        players[i].race = (Race)(RACE_Human + RND(4));
        players[i].fleet_marker = nullptr;
        players[i].init_alien_name(i); // Orig: Assigned in PROCclear_memory
        bool flag_assigned = false;
        L.debug("Assigning flag for player %d (CPU)", i);
        while (!flag_assigned) {
            int flag_try = rand() % 15;
            bool flag_ok = true;
            for (unsigned int j = 0; j < i; ++j) {
                if (flag_try == players[j].flag_idx) {
                    flag_ok = false;
                    break;
                }
            }
            if (flag_ok) {
                players[i].flag_idx = flag_try;
                flag_assigned = true;
                L.debug("Assigned flag %d", flag_try);
            }
        }
        unsigned int bonus_mult = 0;
        if (config.enemy_start == ENEMY_Medium) bonus_mult = 1;
        if (config.enemy_start == ENEMY_Strong) bonus_mult = 2;

        players[i].mc = 300 + RND(5)*50 + bonus_mult*150;
        L.debug("Assigned %d MC to CPU player %d", players[i].mc, i);

        players[i].fleet.transporters += bonus_mult * 100;
    }

    // PLAYER INIT: Everyone (not overridable)
    for (i = 0; i < N_PLAYERS; ++i) {
        snprintf(
            players[i].full_name,
            MAX_PLAYER_TITLE + 1 + MAX_PLAYER_NAME + 1,
            "%s %s",
            players[i].title,
            players[i].name);
        players[i].dead = false;
    }

    aim = config.aim;
    enemy_start = config.enemy_start;
    L.debug("ExodusState init");
    L.debug("      Size: %d", size);
    L.debug("   Players: %d", n_players);
    for (unsigned int i = 0; i < n_players; ++i) {
        Player *inf = &players[i];
        L.debug("       %s %s : Flag %d", inf->title, inf->name, inf->flag_idx);
    }
    L.debug("       Aim: %d", aim);
    L.debug("   Enemies: %d", enemy_start);
}

void ExodusState::init_cpu_lords() {
    Galaxy *gal = get_galaxy();
    unsigned int n_stars;
    Star *stars = gal->get_stars(n_stars);

    for (unsigned int i = n_players; i < N_PLAYERS; ++i) {
        // Assign initial planet to CPU lords. Orig: PROCstart_the_lords
        // We follow the original's logic of iterating over stars in the
        // same order for each lord, which affects the probability of
        // neighbouring lords in the same system.
        // N.B. Star indices are not correlated with their positions.
        bool initial_planet_selected = false;
        Star *chosen_star = nullptr;
        int chosen_planet_idx = 0;
        L.debug("Choosing initial planet for CPU lord %d", i);
        while(!initial_planet_selected) {
            int quality = 0;
            for (unsigned int star_idx = 0; star_idx < n_stars; ++star_idx) {
                L.debug("Considering star %d", star_idx);
                Star *s = &stars[star_idx];
                for (int planet_idx = 0; planet_idx < STAR_MAX_PLANETS; ++planet_idx) {
                    Planet *p = s->get_planet(planet_idx);
                    if (p && p->exists() && !p->is_owned()) {
                        L.debug("Considering planet %d", planet_idx);
                        int planet_quality = 0;
                        if (p->get_class() == Forest)  planet_quality = 4;
                        if (p->get_class() == Desert)  planet_quality = 2;
                        if (p->get_class() == Volcano) planet_quality = 1;
                        if (p->get_class() == Rock)    planet_quality = 3;
                        if (p->get_class() == Ice)     planet_quality = 2;
                        if (p->get_class() == Terra)   planet_quality = 5;
                        if (planet_quality > quality && RND(7) != 1) {
                            L.debug("Checking existing ownership");
                            // TODO: Can we just check planet->is_owned() at this point?
                            bool ok = true;
                            for (unsigned int other_idx = n_players; other_idx < i; other_idx++) {
                                Player *other = &players[other_idx];
                                FlyTarget *other_ts = loc2tgt(other->get_location().get_target());
                                int other_tp = other->get_location().get_planet_target();
                                if (other_ts == s && other_tp == planet_idx) {
                                    ok = false;
                                    break;
                                }
                            }
                            if (ok) {
                                L.debug("Updated planet candidate!");
                                quality = planet_quality;
                                chosen_star = s;
                                chosen_planet_idx = planet_idx;
                                players[i].location.set(tgt2loc(chosen_star));
                                players[i].location.set_planet_target(chosen_planet_idx);
                                initial_planet_selected = true;
                                // N.B. we don't break here - we continue iterating
                                // over stars, and are liable to change our mind if
                                // we find a better offer!
                            } else {
                                L.debug("Can't choose this planet");
                            }
                        }
                    }
                }
            }
        }

        // We've settled on a planet - now claim it!
        Planet *chosen_planet = chosen_star->get_planet(chosen_planet_idx);
        chosen_planet->set_owner(i);
        chosen_planet->prepare_for_cpu_lord();

        if (players[i].mc >= 190) {
            players[i].mc -= 190;
        } else {
            players[i].mc = 0;
        }

        chosen_planet->set_name(chosen_planet->get_name_suggestion());

        // TODO: PROCstart_the_lords sets 'firstplanet' here
        // Check the meaning and a suitable representation

        // TODO: PROCstart_the_lords sets TS and TP to 0 here
        // What are the semantics of this?
    }
}

void ExodusState::generate_galaxy() {
    if (galaxy_finalised) {
        L.error("Cannot regenerate galaxy after finalisation");
    }

    unsigned int n_stars = 15;
    if (size == GAL_Medium)
        n_stars = 25;
    if (size == GAL_Large)
        n_stars = 40;

    new(&galaxy) Galaxy(n_stars);
}

void ExodusState::finalise_galaxy() {
    galaxy_finalised = true;
    init_cpu_lords();
}

Galaxy* ExodusState::get_galaxy() {
    return &galaxy;
}

unsigned int ExodusState::get_month() {
    return month;
}

void ExodusState::advance_month() {
    ++month;
}

Player* ExodusState::get_active_player() {
    return &players[active_player];
}

int ExodusState::get_active_player_idx() {
    return (int)active_player;
}

Star* ExodusState::get_active_star() {
    return &(get_galaxy()->get_stars()[active_star]);
}

unsigned int ExodusState::get_active_star_idx() {
    return active_star;
}

Planet* ExodusState::get_active_planet() {
    if (active_planet < 0) {
        return nullptr;
    }

    return get_galaxy()->get_stars()[active_star].get_planet(active_planet);
}

int ExodusState::get_active_planet_idx() {
    return active_planet;
}

FlyTarget* ExodusState::get_active_flytarget() {
    if (active_flytarget_is_guild) {
        return get_galaxy()->get_guild();
    }

    return get_active_star();
}

void ExodusState::set_active_flytarget(FlyTarget* new_target) {
    L.debug("Active target: %s", new_target->name);
    active_planet = -1;

    if (new_target == get_galaxy()->get_guild()) {
        active_flytarget_is_guild = true;
        return;
    }

    active_flytarget_is_guild = false;

    for (int i = 0; i < GALAXY_MAX_STARS; ++i) {
        if (&get_galaxy()->get_stars()[i] == new_target) {
            active_star = i;
        }
    }

}

void ExodusState::set_active_planet(int new_planet) {
    L.debug("Active planet: %d", new_planet);
    active_planet = new_planet;
}

int ExodusState::tgt2loc(FlyTarget* tgt) {
    if (tgt == get_galaxy()->get_guild())
        return -1;
    for (int i = 0; i < GALAXY_MAX_STARS; ++i)
        if (&get_galaxy()->get_stars()[i] == tgt)
            return i;
    L.fatal("Invalid location");
    return -1;
}

FlyTarget* ExodusState::loc2tgt(int loc) {
    if (loc < 0)
        return get_galaxy()->get_guild();
    return &get_galaxy()->get_stars()[loc];
}

Player* ExodusState::get_player(int idx) {
    if (idx < 0 || idx >= N_PLAYERS) {
        L.fatal("Request for invalid player %d", idx);
        return nullptr;
    }

    return &players[idx];
}

int ExodusState::get_player_idx(Player* player) {
    for (int i = 0; i < N_PLAYERS; ++i)
        if (&players[i] == player)
            return i;

    L.fatal("Index request for invalid player");
    return 0;
}

unsigned int ExodusState::get_n_planets(Player* player) {
    Galaxy *gal = get_galaxy();
    unsigned int n_stars;
    Star *stars = gal->get_stars(n_stars);
    unsigned int count = 0;
    for (unsigned int i = 0; i < n_stars; ++i) {
        for (int j = 0; j < STAR_MAX_PLANETS; ++j) {
            Planet *p = stars[i].get_planet(j);
            if (p && p->exists() && p->is_owned() && get_player(p->get_owner()) == player) {
                ++count;
            }
        }
    }
    return count;
}
