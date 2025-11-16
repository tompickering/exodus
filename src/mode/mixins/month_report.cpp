#include "month_report.h"

#include "state/exodus_state.h"
#include "draw/draw.h"

#include "util/iter.h"

#define NUM_MONTHREPORT_IDS 11


extern DRAWMANAGER draw_manager;

static SprID monthreport_ids[NUM_MONTHREPORT_IDS];

MonthReport::MonthReport() {
    _monthreport_open = false;
}

bool MonthReport::monthreport_is_open() {
    return _monthreport_open;
}

void MonthReport::monthreport_open() {
    if (monthreport_is_open()) {
        monthreport_close();
    }

    _monthreport_open = true;

    for (int i = 0; i < NUM_MONTHREPORT_IDS; ++i) {
        monthreport_ids[i] = draw_manager.new_sprite_id();
    }

    const int w = 440;
    const int h = 56;
    const int x = RES_X/2 - w/2;
    const int lw = 120;

    // FIXME: Should probably have a 'get_primary_player()' function or similar
    Player *player = exostate().get_player(0);
    int player_idx = exostate().get_player_idx(player);

    for (int row = 0; row < 5; ++row) {
        const int b = BORDER;
        const int real_h = (h+2+2*b);
        const int base_y = (412-(real_h*5))/2;
        const int y = base_y + row*real_h - (row==0?4:0);
        if (row == 0) {
            SprID box_id = monthreport_ids[0];
            draw_manager.fill(box_id, {x-b, y-b, w+b*2, h+b*2}, COL_BORDERS);
            draw_manager.fill(FILL_3D_Out_Hollow, {x-b, y-b, w+b*2, h+b*2}, COL_BORDERS);
            draw_manager.fill_pattern({x, y, w, h});

            char heading[32];
            snprintf(heading, sizeof(heading), "STATUS REPORT: MONTH %d", exostate().get_month());
            draw_manager.draw_text(heading, Justify::Centre, x+w/2, y+16, COL_TEXT);
        } else {
            SprID box_id_l = monthreport_ids[1 + 2*(row-1)];
            SprID box_id_r = monthreport_ids[1 + 2*(row-1) + 1];
            draw_manager.fill(box_id_l, {x-b, y-b, lw+b*2, h+b*2}, COL_BORDERS);
            draw_manager.fill(FILL_3D_Out_Hollow, {x-b, y-b, lw+b*2, h+b*2}, COL_BORDERS);
            draw_manager.fill_pattern({x, y, lw, h});

            const char* section = "Diplomacy";
            if (row == 2) section = "Army";
            if (row == 3) section = "Science";
            if (row == 4) section = "Population";
            draw_manager.draw_text(section, Justify::Left, x+4, y+16, COL_TEXT);

            const int rx = x+lw+b*2+2;
            const int rw = w-lw-2-2*b;
            draw_manager.fill(box_id_r, {rx-b, y-b, rw+2*b, h+b*2}, COL_BORDERS);
            draw_manager.fill(FILL_3D_Out_Hollow, {rx-b, y-b, rw+2*b, h+b*2}, COL_BORDERS);
            draw_manager.fill_pattern({rx, y, rw, h});

            char text[64];

            switch (row) {
                case 1:
                    {
                        int allies = 0;
                        int enemies = 0;
                        for (int i = 0; i < N_PLAYERS; ++i) {
                            Player *other = exostate().get_player(i);
                            if (other == player) {
                                continue;
                            }
                            if (!other->is_participating()) {
                                continue;
                            }
                            if (exostate().is_allied(player_idx, i)) {
                                ++allies;
                            }
                            if (other->is_hostile_to(player_idx)) {
                                ++enemies;
                            }
                        }

                        char n_allies[4];
                        char n_enemies[4];
                        if (allies == 0) {
                            strncpy(n_allies, "no", sizeof(n_allies));
                        } else {
                            snprintf(n_allies, sizeof(n_allies), "%d", allies);
                        }
                        if (enemies == 0) {
                            strncpy(n_enemies, "no", sizeof(n_enemies));
                        } else {
                            snprintf(n_enemies, sizeof(n_enemies), "%d", enemies);
                        }

                        snprintf(text, sizeof(text),
                                 "We have %s all%s and %s",
                                 n_allies, allies==1?"y":"ies", n_enemies);

                        draw_manager.draw_text(text, Justify::Left, rx+4, y+4, COL_TEXT);

                        const char* summary = "A good level";
                        if (allies*2 < enemies*3) {
                            summary = "This is alarming";
                        }

                        snprintf(text, sizeof(text),
                                 "enem%s. %s.",
                                 enemies==1?"y":"ies", summary);

                        draw_manager.draw_text(text, Justify::Left, rx+4, y+24, COL_TEXT);
                    }
                    break;
                case 2:
                    {
                        if (player->get_officer(OFF_Counsellor) == OFFQ_Poor) {
                            draw_manager.draw_text(
                                "Your counsellor is not",
                                Justify::Left,
                                rx+4, y+4,
                                COL_TEXT);
                            draw_manager.draw_text(
                                "qualified in this area.",
                                Justify::Left,
                                rx+4, y+24,
                                COL_TEXT);
                        } else {
                            bool ok = true;
                            int n_planets = 0;
                            for (PlanetIterator piter(player_idx); !piter.complete(); ++piter) {
                                n_planets++;
                                if (piter.get()->get_army_size() < exostate().get_orig_month()) {
                                    ok = false;
                                }
                            }
                            if (ok) {
                                draw_manager.draw_text(
                                    "We seem to have enough",
                                    Justify::Left,
                                    rx+4, y+4,
                                    COL_TEXT);
                            } else {
                                draw_manager.draw_text(
                                    "There is insufficient",
                                    Justify::Left,
                                    rx+4, y+4,
                                    COL_TEXT);
                            }

                            snprintf(text, sizeof(text), "defence at our planet%s.", n_planets==1?"":"s");

                            draw_manager.draw_text(
                                text,
                                Justify::Left,
                                rx+4, y+24,
                                COL_TEXT);
                        }
                    }
                    break;
                case 3:
                    {
                        int n_invs = 0;
                        for (int i = 0; i < (int)INV_MAX; ++i) {
                            if (player->has_invention((Invention)i)) {
                                n_invs++;
                            }
                        }

                        const char* t0 = "";
                        const char* t1 = "";

                        if (FEATURE(EF_REFINED_SCIENCE_REPORTS)) {
                            if (n_invs == (int)INV_MAX) {
                                t0 = "The scientists are very";
                                t1 = "content and not very busy.";
                            } else {
                                OfficerQuality q = player->get_officer(OFF_Science);
                                int qint = (int)q;
                                // Maximum possible value here is 8
                                int threshold = 2*qint - 1 + (player->get_tax() / 20);
                                // Add noise - maximum possible is 9
                                threshold += (1 - (rand() % 3));
                                if (threshold <= 0) {
                                    t0 = "The scientists are desperate";
                                    t1 = "for funding.";
                                    switch (rand() % 3) {
                                        case 0:
                                            t0 = "Not much progress can be made";
                                            t1 = "with such little funding.";
                                            break;
                                        case 1:
                                            t0 = "We have plenty of ideas, but";
                                            t1 = "no means to develop them!";
                                            break;
                                    }
                                    if (onein(4)) {
                                        switch (q) {
                                            case OFFQ_Poor:
                                                t0 = "Our officer does not seem";
                                                t1 = "at all competent.";
                                                break;
                                            case OFFQ_Average:
                                                t0 = "Despite our officer's best";
                                                t1 = "efforts, we're getting nowhere.";
                                                break;
                                            case OFFQ_Good:
                                                t0 = "Even our elite officer cannot";
                                                t1 = "work in this situation.";
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                } else if (threshold == 1) {
                                    t0 = "The scientists are struggling";
                                    t1 = "with such little funding.";
                                    switch (rand() % 3) {
                                        case 0:
                                            t0 = "Progress is painfully";
                                            t1 = "slow.";
                                            break;
                                        case 1:
                                            t0 = "Only the most optimistic of";
                                            t1 = "us hope for results soon.";
                                            break;
                                    }
                                    if (onein(4)) {
                                        switch (q) {
                                            case OFFQ_Poor:
                                                t0 = "Our officer does not seem";
                                                t1 = "to know what they're doing.";
                                                break;
                                            case OFFQ_Average:
                                                t0 = "Our officer is really";
                                                t1 = "trying to drive progress.";
                                                break;
                                            case OFFQ_Good:
                                                t0 = "Our officer is an inspiration";
                                                t1 = "in difficult circumstances.";
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                } else if (threshold <= 3) {
                                    t0 = "The scientists are making";
                                    t1 = "interesting advancements.";
                                    switch (rand() % 3) {
                                        case 0:
                                            t0 = "The scientists are working";
                                            t1 = "on a new invention.";
                                            break;
                                        case 1:
                                            t0 = "We are developing projects";
                                            t1 = "at a steady pace.";
                                            break;
                                    }
                                    if (onein(4)) {
                                        switch (q) {
                                            case OFFQ_Poor:
                                                t0 = "Our officer isn't exactly";
                                                t1 = "helping matters.";
                                                break;
                                            case OFFQ_Average:
                                                t0 = "Our officer is making a";
                                                t1 = "real difference.";
                                                break;
                                            case OFFQ_Good:
                                                t0 = "Our officer is driving";
                                                t1 = "through real change!";
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                } else if (threshold <= 5) {
                                    t0 = "We are making remarkable";
                                    t1 = "progress!";
                                    switch (rand() % 3) {
                                        case 0:
                                            t0 = "Our work is going very";
                                            t1 = "well indeed.";
                                            break;
                                        case 1:
                                            t0 = "We have some fascinating";
                                            t1 = "projects in the pipeline!";
                                            break;
                                    }
                                    if (onein(4)) {
                                        switch (q) {
                                            case OFFQ_Poor:
                                                t0 = "Our officer is managing";
                                                t1 = "to hold things together.";
                                                break;
                                            case OFFQ_Average:
                                                t0 = "Our officer deserves real";
                                                t1 = "credit for this work.";
                                                break;
                                            case OFFQ_Good:
                                                t0 = "We couldn't be making such";
                                                t1 = "progress without our officer!";
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                } else if (threshold <= 7) {
                                    t0 = "We are confident of a";
                                    t1 = "breakthrough soon!";
                                    switch (rand() % 3) {
                                        case 0:
                                            t0 = "We are getting some";
                                            t1 = "incredible results!";
                                            break;
                                        case 1:
                                            t0 = "We have some fascinating";
                                            t1 = "projects in the pipeline!";
                                            break;
                                    }
                                    if (onein(4)) {
                                        switch (q) {
                                            case OFFQ_Poor:
                                                t0 = "We could do even better with";
                                                t1 = "a more capable officer!";
                                                break;
                                            case OFFQ_Average:
                                                t0 = "Our officer deserves a";
                                                t1 = "commendation!";
                                                break;
                                            case OFFQ_Good:
                                                t0 = "Our officer is a credit";
                                                t1 = "to the human race!";
                                                break;
                                            default:
                                                break;
                                        }
                                    }
                                } else {
                                    t0 = "The scientists are thriving,";
                                    t1 = "and extremely excited!";
                                    switch (rand() % 3) {
                                        case 0:
                                            t0 = "We are holding conferences";
                                            t1 = "on a huge scale!";
                                            break;
                                        case 1:
                                            t0 = "We are seeing a new golden";
                                            t1 = "age of science!";
                                            break;
                                    }
                                }
                            }
                        } else {
                            t0 = "The scientists are working";
                            t1 = "on a new invention.";

                            if (n_invs == (int)INV_MAX) {
                                t0 = "The scientists are very";
                                t1 = "content and not very busy.";
                            } else if (player->get_tax() < 5) {
                                t0 = "The scientists have no";
                                t1 = "money for further projects.";
                            } else {
                                switch (rand() % 3) {
                                    case 0:
                                        t0 = "We are making remarkable";
                                        t1 = "advancements!";
                                        break;
                                    case 1:
                                        t0 = "We are making interesting";
                                        t1 = "advancements.";
                                        break;
                                }
                            }
                        }

                        draw_manager.draw_text(
                            t0,
                            Justify::Left,
                            rx+4, y+4,
                            COL_TEXT);
                        draw_manager.draw_text(
                            t1,
                            Justify::Left,
                            rx+4, y+24,
                            COL_TEXT);
                    }
                    break;
                case 4:
                    {
                        int n_planets = 0;
                        int total_unrest = 0;
                        Planet *riot_planet = nullptr;
                        for (PlanetIterator piter(player_idx); !piter.complete(); ++piter) {
                            n_planets++;
                            int unrest = piter.get()->get_unrest();
                            total_unrest += unrest;
                            if (unrest > 8) {
                                riot_planet = piter.get();
                            }
                        }

                        const char *t0 = "";
                        const char *t1 = "";

                        if (riot_planet) {
                            snprintf(text, sizeof(text), "%s!", riot_planet->get_name());
                            t0 = "There are riots at planet";
                            t1 = text;
                        } else {
                            int avg_unrest = 3;
                            if (n_planets > 0) avg_unrest = total_unrest/n_planets;

                            if (avg_unrest > 7) {
                                switch (rand() % 3) {
                                    case 0:
                                        t0 = "The people have destroyed";
                                        snprintf(text, sizeof(text), "a %s monument!", player->get_name());
                                        t1 = text;
                                        break;
                                    case 1:
                                        t0 = "The people are angry.";
                                        t1 = "Your reputation is bad.";
                                        break;
                                    case 2:
                                        t0 = "Voices can be heard";
                                        t1 = "that demand revolution.";
                                        break;
                                }
                            } else if (avg_unrest > 5) {
                                switch (rand() % 5) {
                                    case 0:
                                        t0 = "The people have destroyed";
                                        snprintf(text, sizeof(text), "a %s monument!", player->get_name());
                                        t1 = text;
                                        break;
                                    case 1:
                                        t0 = "The people are unhappy.";
                                        t1 = "They demand a better life.";
                                        break;
                                    case 2:
                                        t0 = "You seem to have enemies";
                                        t1 = "amongst your people.";
                                        break;
                                    case 3:
                                        t0 = "Many people are not";
                                        t1 = "content any more.";
                                        break;
                                    case 4:
                                        t0 = "Your public image is not";
                                        t1 = "very good.";
                                        break;
                                }
                            } else if (avg_unrest >= 2) {
                                switch (rand() % 5) {
                                    case 0:
                                        t0 = "Most people are content.";
                                        t1 = "The fewest are complaining.";
                                        break;
                                    case 1:
                                        t0 = "You are known as a";
                                        t1 = "fair sovereign.";
                                        break;
                                    case 2:
                                        t0 = "Many of your people are";
                                        t1 = "content.";
                                        break;
                                    case 3:
                                        t0 = "The press emphasizes your";
                                        t1 = "well-liked sides.";
                                        break;
                                    case 4:
                                        t0 = "You are an appreciated";
                                        // Orig was 'lord'
                                        snprintf(text, sizeof(text), "%s.", player->get_title());
                                        t1 = text;
                                        break;
                                }
                            } else {
                                switch (rand() % 5) {
                                    case 0:
                                        t0 = "The people have built";
                                        snprintf(text, sizeof(text), "a %s monument!", player->get_name());
                                        t1 = text;
                                        break;
                                    case 1:
                                        t0 = "The press is full of";
                                        t1 = "compliments.";
                                        break;
                                    case 2:
                                        t0 = "The people are happy.";
                                        // Orig was 'lord'
                                        snprintf(text, sizeof(text), "They adore their %s.", player->get_title());
                                        t1 = text;
                                        break;
                                    case 3:
                                        t0 = "You are the idol of";
                                        t1 = "your nation.";
                                        break;
                                    case 4:
                                        t0 = "The people have a high";
                                        t1 = "opinion of you.";
                                        break;
                                }
                            }
                        }

                        draw_manager.draw_text(
                            t0,
                            Justify::Left,
                            rx+4, y+4,
                            COL_TEXT);
                        draw_manager.draw_text(
                            t1,
                            Justify::Left,
                            rx+4, y+24,
                            COL_TEXT);
                    }
                    break;
            }
        }
    }
}

void MonthReport::monthreport_close() {
    _monthreport_open = false;

    for (int i = 0; i < NUM_MONTHREPORT_IDS; ++i) {
        draw_manager.draw(monthreport_ids[i], nullptr);
        draw_manager.release_sprite_id(monthreport_ids[i]);
    }
}
