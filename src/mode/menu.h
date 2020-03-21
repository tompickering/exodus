#ifndef GUARD_MENU_H
#define GUARD_MENU_H

#include "mode_base.h"

#include "../platform.h"
#include "../draw/draw.h"

enum GalaxySize {
    GAL_Small,
    GAL_Medium,
    GAL_Large,
};

typedef struct {
    char name[16];
} PlayerInfo;

enum Aim {
    AIM_Might,
    AIM_Money,
    AIM_Civilization,
};

enum EnemyStart {
    ENEMY_None,
    ENEMY_Weak,
    ENEMY_Medium,
    ENEMY_Strong,
};

typedef struct {
    GalaxySize size;
    unsigned char n_players;
    PlayerInfo info[5];
    Aim aim;
    EnemyStart enemy_start;
} GameConfig;

class Menu : ModeBase {
    public:
        Menu();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        enum Stage {
            Main,
            Load,
            Size,
            NPlayers,
            Name,
            Title,
            Flag,
            Aim,
            Difficulty,
            Confirm,
        };

        enum TransitionState {
            None,
            Started,
            Done,
        };

        Stage stage;
        TransitionState trans_state;

        void set_stage(Stage);

        // The purpose of the Menu is to init this game config
        GameConfig config;
};

#endif
