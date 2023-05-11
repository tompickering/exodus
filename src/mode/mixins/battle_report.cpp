#include "battle_report.h"

#include "audio/audio.h"
#include "draw/draw.h"
#include "state/exodus_state.h"
#include "state/ephemeral_state.h"

#include "shared.h"

static const int PANEL_W = 328;
static const int PANEL_H = 120;
static const int PANEL_X = RES_X/2 - (PANEL_W/2);
static const int PANEL_Y = 150;

extern AUDIOMANAGER audio_manager;
extern DRAWMANAGER draw_manager;
extern ExodusState exostate;
extern EphemeralState ephstate;

BattleReport::BattleReport() {
}

bool BattleReport::batrpt_draw(bool reset) {
    if (reset) {
        n = 0;
    }

    LunarBattleParams &b = ephstate.lunar_battle;
    LunarBattleReport &rpt = ephstate.lunar_battle_report;

    Planet *p = exostate.get_active_planet();
    Player *aggressor = nullptr;
    if (b.aggressor_type == AGG_Player) {
        aggressor = exostate.get_player(b.aggressor_idx);
    }

    bool defending = !(aggressor && aggressor->is_human());

    draw_manager.fill({PANEL_X - BORDER, PANEL_Y - BORDER,
                       PANEL_W + 2*BORDER, PANEL_H + 2*BORDER},
                      COL_BORDERS);
    draw_manager.fill_pattern({PANEL_X, PANEL_Y,
                               PANEL_W, PANEL_H});

    // Should be true if active human player won
    bool won = (defending ^ rpt.aggressor_won);

    audio_manager.target_music(mpart2mus(won ? 5 : 9));

    char l0[32];
    char l1[32];
    char l2[32];
    char l3[32];
    char l4[32];
    const char* lines[] = {l0, l1, l2, l3, l4};

    l0[0] = l1[0] = l2[0] = l3[0] = l4[0] = '\0';

    int al = rpt.agg_lost.total();
    int ai = rpt.agg_init.total();
    int dl = rpt.def_lost.total();
    int di = rpt.def_init.total();

    if (n == 0) {
        if (defending) {
            if (won) {
                snprintf(l0, sizeof(l0), "Your army has successfully");
                snprintf(l1, sizeof(l1), "defended the planet %s.", p->get_name());
                snprintf(l3, sizeof(l3), "You lost %d of your %d", dl, di);
                snprintf(l4, sizeof(l4), "defending units.");
                // Orig calls PROCdonotice here - we register NI_PlanetTakeover elsewhere
            } else {
                snprintf(l0, sizeof(l0), "All your %d defending units", di);
                snprintf(l1, sizeof(l1), "have been destroyed.");
                snprintf(l3, sizeof(l3), "You have lost the planet");
                snprintf(l4, sizeof(l4), "%s.", p->get_name());
                // Orig calls PROCdonotice here - we register NI_PlanetTakeover elsewhere
            }
        } else {
            if (won) {
                snprintf(l0, sizeof(l0), "The planet is now yours. You");
                snprintf(l1, sizeof(l1), "lost %d units in this battle.", al);
                snprintf(l3, sizeof(l3), "The remaining %d units will", ai-al);
                snprintf(l4, sizeof(l4), "be stationed at %s.", p->get_name());
            } else {
                snprintf(l0, sizeof(l0), "Your army did not succeed.");
                if (rpt.aggressor_retreated) {
                    snprintf(l2, sizeof(l2), "Your invading troops have");
                    snprintf(l3, sizeof(l3), "been transferred back to the");
                    snprintf(l4, sizeof(l4), "transport ships.");
                } else {
                    snprintf(l2, sizeof(l2), "All your %d invading units", ai);
                    snprintf(l3, sizeof(l3), "have been destroyed. %s", p->get_name());
                    snprintf(l4, sizeof(l4), "remains in the enemy's hand.");
                }
            }
        }
    } else if (n == 1) {
        // Set this false if we display further info
        if (won && rpt.agg_type == AGG_Aliens) {
            snprintf(l0, sizeof(l0), "MESSAGE FROM SCIENTISTS:");
            snprintf(l2, sizeof(l2), "The aliens have used a");
            snprintf(l3, sizeof(l3), "fascinating new technology");
            snprintf(l4, sizeof(l4), "that we are going to analyse.");
        } else {
            return false;
        }
    } else if (n == 2) {
        return false;
    }

    for (int i = 0; i < 5; ++i) {
        int y = PANEL_Y + 8 + i*20;
        draw_manager.draw_text(
            lines[i],
            Justify::Left,
            PANEL_X + 4, y,
            COL_TEXT);
    }

    n++;
    return true;
}
