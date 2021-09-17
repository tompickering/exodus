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

void BattleReport::batrpt_draw() {
    LunarBattleParams &b = ephstate.lunar_battle;
    LunarBattleReport &rpt = ephstate.lunar_battle_report;

    Planet *p = exostate.get_active_planet();
    Player *owner = exostate.get_player(p->get_owner());
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

    bool won = (defending ^ rpt.aggressor_won);

    audio_manager.target_music(mpart2mus(won ? 5 : 9));

    draw_manager.draw_text(
        won ? "WON" : "LOST",
        Justify::Left,
        PANEL_X + 4, PANEL_Y + 4,
        COL_TEXT);
}
