#ifndef GUARD_ASSETPATH_H
#define GUARD_ASSETPATH_H

// Can be overly large with no real impact.
// Just a guarantee that we can give when loading resources.
// Includes file extensions (e.g. .png).
#define ASSET_PATH_LEN_MAX 512

extern const char *FONT_AUDIOWIDE;

extern const char *SFX_SHOT;
extern const char *SFX_EXPLOSION;
extern const char *SFX_HEAVYSHOT;
extern const char *SFX_GLIDE_LOW;
extern const char *SFX_GLIDE_MED;
extern const char *SFX_GLIDE_HIGH;
extern const char *SFX_WALK0;
extern const char *SFX_WALK1;
extern const char *SFX_WALK2;
extern const char *SFX_BEEP;

extern const char *MUS_INTRO;
extern const char *MUS_ST1_10;
extern const char *MUS_END;
extern const char *MUS_ST2_E;
extern const char *MUS_OVERVIEW;
extern const char *MUS_ST3_A;
extern const char *MUS_ST3_F;
extern const char *MUS_BATTLE_PREP;
extern const char *MUS_BATTLE_WON;
extern const char *MUS_TRADE;
extern const char *MUS_ST3_27;
extern const char *MUS_INVASION;
extern const char *MUS_BATTLE_LOST;
extern const char *MUS_ACHIEVEMENT;
extern const char *MUS_MINIGAME;
extern const char *MUS_GAMESTART;
extern const char *MUS_OVERWORLD;
extern const char *MUS_ALIENSHIP;
extern const char *MUS_ST3_42;
extern const char *MUS_BATTLEWAIT;
extern const char *MUS_ST3_4D;
extern const char *MUS_CELEBRATE;
extern const char *MUS_GUILD;
extern const char *MUS_OVERWORLD_ANGRY;
extern const char *MUS_ST3_58;

extern const char *IMG_CURSOR;
extern const char *IMG_SHIP_FLIPPED;

extern const char *IMG_INTRO_EARTH;
extern const char *IMG_INTRO_CITY;
extern const char *IMG_INTRO_STARPORT;
extern const char *IMG_INTRO_CORRIDOR;
extern const char *IMG_INTRO_DOOR;
extern const char *IMG_INTRO_KEYPAD;
extern const char *IMG_INTRO_FAIL;
extern const char *IMG_INTRO_LAUNCH;
extern const char *IMG_INTRO_SPACE;
extern const char *IMG_MAP_SOLAR;
extern const char *IMG_BATTLE;
extern const char *IMG_BATTLE_PREP;
extern const char *IMG_LORD_ARRIVE;
extern const char *IMG_GAMEOVER;
extern const char *IMG_PLANETSIDE;
extern const char *IMG_RADAR;
extern const char *IMG_BG_SHUTTLE;
extern const char *IMG_BG_MENU0;
extern const char *IMG_BG_MENU1;
extern const char *IMG_BG_NEWS;
extern const char *IMG_BG_SHIP0;
extern const char *IMG_BG_SHIP1;
extern const char *IMG_BG_STARS0;
extern const char *IMG_BG_STARS1;
extern const char *IMG_BG_STARS2;
extern const char *IMG_BG_STARS3;
extern const char *IMG_BG_STARS4;
extern const char *IMG_BG_STARS5;
extern const char *IMG_BG_STARS6;
extern const char *IMG_BG_STARS7;
extern const char *IMG_PLANET_FOREST;
extern const char *IMG_PLANET_DESERT;
extern const char *IMG_PLANET_VOLCANO;
extern const char *IMG_PLANET_ROCK;
extern const char *IMG_PLANET_ICE;
extern const char *IMG_PLANET_TERRA;
extern const char *IMG_PLANET_ART;
extern const char *IMG_HARBOUR_FOREST;
extern const char *IMG_HARBOUR_DESERT;
extern const char *IMG_HARBOUR_VOLCANO;
extern const char *IMG_HARBOUR_ROCK;
extern const char *IMG_HARBOUR_ICE;
extern const char *IMG_HARBOUR_TERRA;
extern const char *IMG_HARBOUR_ART;
extern const char *IMG_MAKING0;
extern const char *IMG_MAKING1;
extern const char *IMG_MAKING2;
extern const char *IMG_MAKING3;
extern const char *IMG_MAKING4;
extern const char *IMG_MAKING5;
extern const char *IMG_MAKING6;
extern const char *IMG_MAKING7;
extern const char *IMG_MAKING8;
extern const char *IMG_MAKING9;
extern const char *IMG_MAKING10;
extern const char *IMG_MAKING11;
extern const char *IMG_MAKING12;
extern const char *IMG_MAKING13;
extern const char *IMG_MAKING14;

extern const char *IMG_INTRO_PH1_PUSH_5;
extern const char *IMG_INTRO_OD1_BEHIND;
extern const char *IMG_INTRO_PH1_PUSH_9;
extern const char *IMG_INTRO_SH4_MOVEOUT;
extern const char *IMG_INTRO_PH1_PUSH_6;
extern const char *IMG_INTRO_OD1_FRAME;
extern const char *IMG_INTRO_BT1_FALL1;
extern const char *IMG_INTRO_BT1_FALL3;
extern const char *IMG_INTRO_SH4_SHUTTLE2;
extern const char *IMG_INTRO_BT1_FALL5;
extern const char *IMG_INTRO_SH4_SHUTTLE3;
extern const char *IMG_INTRO_SH3_SHUTTLE;
extern const char *IMG_INTRO_FI1_FIRE2;
extern const char *IMG_INTRO_FI1_FIRE3;
extern const char *IMG_INTRO_FI1_FIRE6;
extern const char *IMG_INTRO_OD1_OFF;
extern const char *IMG_INTRO_SH3_CRUISER1;
extern const char *IMG_INTRO_FI1_FIRE5;
extern const char *IMG_INTRO_BT1_FALL7;
extern const char *IMG_INTRO_BT1_FALL6;
extern const char *IMG_INTRO_PH1_PUSH_7;
extern const char *IMG_INTRO_SH4_SHUTTLE4;
extern const char *IMG_INTRO_OD1_DOOR_L;
extern const char *IMG_INTRO_SH4_BLINK3;
extern const char *IMG_INTRO_SH1_SHIP;
extern const char *IMG_INTRO_OD1_DOOR_R;
extern const char *IMG_INTRO_SH4_BLINK1;
extern const char *IMG_INTRO_OD1_ON;
extern const char *IMG_INTRO_SH4_SHUTTLE1;
extern const char *IMG_INTRO_PH1_PUSH_2;
extern const char *IMG_INTRO_FI1_SHOT1;
extern const char *IMG_INTRO_PH1_PUSH_4;
extern const char *IMG_INTRO_SH3_CRUISER;
extern const char *IMG_INTRO_FI1_SHOT4;
extern const char *IMG_INTRO_BT1_FALL2;
extern const char *IMG_INTRO_PH1_PUSH_1;
extern const char *IMG_INTRO_PH1_PUSH_8;
extern const char *IMG_INTRO_SH2_SHIP;
extern const char *IMG_INTRO_FI1_FIRE4;
extern const char *IMG_INTRO_GU1_GUARD;
extern const char *IMG_INTRO_BT1_FALL4;
extern const char *IMG_INTRO_FI1_SHOT3;
extern const char *IMG_INTRO_SH1_SHIP2;
extern const char *IMG_INTRO_SH4_BLINK2;
extern const char *IMG_INTRO_FI1_SHOT5;
extern const char *IMG_INTRO_PH1_PUSH_3;
extern const char *IMG_INTRO_FI1_FIRE1;
extern const char *IMG_INTRO_OD1_SEPP;
extern const char *IMG_INTRO_FI1_SHOT2;
extern const char *IMG_INTRO_SH3_CRUISER2;

extern const char *IMG_FILM_1_SPR_1;
extern const char *IMG_FILM_1_SPR_2_3;
extern const char *IMG_FILM_1_SPR_3_3;

extern const char *IMG_STARTGR_FL4_FLEET;
extern const char *IMG_STARTGR_FL5_10_1;
extern const char *IMG_STARTGR_FL5_10_2;
extern const char *IMG_STARTGR_FL5_10_3;
extern const char *IMG_STARTGR_FL5_10_4;
extern const char *IMG_STARTGR_FL5_10_5;
extern const char *IMG_STARTGR_FL5_11_1;
extern const char *IMG_STARTGR_FL5_11_2;
extern const char *IMG_STARTGR_FL5_11_3;
extern const char *IMG_STARTGR_FL5_11_4;
extern const char *IMG_STARTGR_FL5_11_5;
extern const char *IMG_STARTGR_FL5_1_1;
extern const char *IMG_STARTGR_FL5_12_1;
extern const char *IMG_STARTGR_FL5_12_2;
extern const char *IMG_STARTGR_FL5_12_3;
extern const char *IMG_STARTGR_FL5_12_4;
extern const char *IMG_STARTGR_FL5_12_5;
extern const char *IMG_STARTGR_FL5_1_2;
extern const char *IMG_STARTGR_FL5_1_3;
extern const char *IMG_STARTGR_FL5_1_4;
extern const char *IMG_STARTGR_FL5_1_5;
extern const char *IMG_STARTGR_FL5_2_1;
extern const char *IMG_STARTGR_FL5_2_2;
extern const char *IMG_STARTGR_FL5_2_3;
extern const char *IMG_STARTGR_FL5_2_4;
extern const char *IMG_STARTGR_FL5_2_5;
extern const char *IMG_STARTGR_FL5_3_1;
extern const char *IMG_STARTGR_FL5_3_2;
extern const char *IMG_STARTGR_FL5_3_3;
extern const char *IMG_STARTGR_FL5_3_4;
extern const char *IMG_STARTGR_FL5_3_5;
extern const char *IMG_STARTGR_FL5_4_1;
extern const char *IMG_STARTGR_FL5_4_2;
extern const char *IMG_STARTGR_FL5_4_3;
extern const char *IMG_STARTGR_FL5_4_4;
extern const char *IMG_STARTGR_FL5_4_5;
extern const char *IMG_STARTGR_FL5_5_1;
extern const char *IMG_STARTGR_FL5_5_2;
extern const char *IMG_STARTGR_FL5_5_3;
extern const char *IMG_STARTGR_FL5_5_4;
extern const char *IMG_STARTGR_FL5_5_5;
extern const char *IMG_STARTGR_FL5_6_1;
extern const char *IMG_STARTGR_FL5_6_2;
extern const char *IMG_STARTGR_FL5_6_3;
extern const char *IMG_STARTGR_FL5_6_4;
extern const char *IMG_STARTGR_FL5_6_5;
extern const char *IMG_STARTGR_FL5_7_1;
extern const char *IMG_STARTGR_FL5_7_2;
extern const char *IMG_STARTGR_FL5_7_3;
extern const char *IMG_STARTGR_FL5_7_4;
extern const char *IMG_STARTGR_FL5_7_5;
extern const char *IMG_STARTGR_FL5_8_1;
extern const char *IMG_STARTGR_FL5_8_2;
extern const char *IMG_STARTGR_FL5_8_3;
extern const char *IMG_STARTGR_FL5_8_4;
extern const char *IMG_STARTGR_FL5_8_5;
extern const char *IMG_STARTGR_FL5_9_1;
extern const char *IMG_STARTGR_FL5_9_2;
extern const char *IMG_STARTGR_FL5_9_3;
extern const char *IMG_STARTGR_FL5_9_4;
extern const char *IMG_STARTGR_FL5_9_5;
extern const char *IMG_STARTGR_FL5_FLEET;
extern const char *IMG_STARTGR_FL6_STARS;
extern const char *IMG_STARTGR_GAL_S;
extern const char *IMG_STARTGR_GAL_M;
extern const char *IMG_STARTGR_GAL_L;
extern const char *IMG_STARTGR_AIM_MIGHT;
extern const char *IMG_STARTGR_AIM_MONEY;
extern const char *IMG_STARTGR_AIM_CIV;
extern const char *IMG_STARTGR_STR_WEAK;
extern const char *IMG_STARTGR_STR_MEDIUM;
extern const char *IMG_STARTGR_STR_STRONG;
extern const char *IMG_STARTGR_MPSTR_NONE;
extern const char *IMG_STARTGR_MPSTR_WEAK;
extern const char *IMG_STARTGR_MPSTR_MEDIUM;

extern const char *IMG_AL1_1_2;
extern const char *IMG_AL1_13;
extern const char *IMG_AL1_15;
extern const char *IMG_AL1_1;
extern const char *IMG_AL1_3;
extern const char *IMG_AL1_7;
extern const char *IMG_AL1_9;
extern const char *IMG_AL2_1_2;
extern const char *IMG_AL2_13;
extern const char *IMG_AL2_15;
extern const char *IMG_AL2_1;
extern const char *IMG_AL2_3;
extern const char *IMG_AL2_7;
extern const char *IMG_AL2_9;
extern const char *IMG_AP1_APPROACH;
extern const char *IMG_AP3_APPROACH;
extern const char *IMG_AP8_APPROACH;
extern const char *IMG_BR10_EXPORT;
extern const char *IMG_BR11_DEF;
extern const char *IMG_BR11_LR;
extern const char *IMG_BR11_OK;
extern const char *IMG_BR12_EXPORT;
extern const char *IMG_BR13_FEMALE;
extern const char *IMG_BR13_MALE;
extern const char *IMG_BR14_EXPORT;
extern const char *IMG_BR15_FADE1;
extern const char *IMG_BR15_FADE2;
extern const char *IMG_BR15_FADE3;
extern const char *IMG_BR16_NBAR;
extern const char *IMG_BR1_EXPORT;
extern const char *IMG_BR2_EXPORT;
extern const char *IMG_BR3_EXPORT2;
extern const char *IMG_BR4_EXPORT3;
extern const char *IMG_BR5_EXPORT4;
extern const char *IMG_BR6_OK;
extern const char *IMG_BR7_E1;
extern const char *IMG_BR7_E2;
extern const char *IMG_BR8_EXPORT;
extern const char *IMG_BR9_EXPORT;
extern const char *IMG_BT1_EXPORT;
extern const char *IMG_CS1_C;
extern const char *IMG_CS2_C;
extern const char *IMG_CS3_C;
extern const char *IMG_CS4_C;
extern const char *IMG_CT1_EXPORT;
extern const char *IMG_CT2_EXPORT;
extern const char *IMG_CT3_EXPORT;
extern const char *IMG_CT4_EXPORT;
extern const char *IMG_EN1_DOOR1;
extern const char *IMG_EN1_DOOR2;
extern const char *IMG_EN1_HALL;
extern const char *IMG_EN1_STARS;
extern const char *IMG_EN1_WELCOME;
extern const char *IMG_EN2_SCAPE;
extern const char *IMG_EN2_STS1;
extern const char *IMG_EN2_STS2;
extern const char *IMG_EN2_STS3;
extern const char *IMG_EN2_STS4;
extern const char *IMG_EN2_STS5;
extern const char *IMG_EN2_STS6;
extern const char *IMG_EN2_STS7;
extern const char *IMG_EN2_STS8;
extern const char *IMG_EN2_STS9;
extern const char *IMG_EN2_THEEND;
extern const char *IMG_EQ1_TR0;
extern const char *IMG_EQ1_TR1;
extern const char *IMG_EQ1_TR2;
extern const char *IMG_EQ1_TR3;
extern const char *IMG_EQ1_TR4;
extern const char *IMG_EQ1_TR5;
extern const char *IMG_EQ1_TR6;
extern const char *IMG_EQ1_TR7;
extern const char *IMG_FL1_TR0;
extern const char *IMG_FL1_TR1;
extern const char *IMG_FL1_TR2;
extern const char *IMG_FL1_TR3;
extern const char *IMG_FL1_TR4;
extern const char *IMG_FL1_TR6;
extern const char *IMG_FL1_TR7;
extern const char *IMG_GF1_25;
extern const char *IMG_GF1_30;
extern const char *IMG_GF1_30X;
extern const char *IMG_GF1_31;
extern const char *IMG_GF1_31X;
extern const char *IMG_GF1;
extern const char *IMG_GF2_25;
extern const char *IMG_GF2_30;
extern const char *IMG_GF2_30X;
extern const char *IMG_GF2_31;
extern const char *IMG_GF2_31X;
extern const char *IMG_GF2_MA1;
extern const char *IMG_GF2_MA2;
extern const char *IMG_GF2_MA3;
extern const char *IMG_GF2_MA4;
extern const char *IMG_GF2_MA5;
extern const char *IMG_GF2;
extern const char *IMG_GF3_25;
extern const char *IMG_GF3_30;
extern const char *IMG_GF3_31;
extern const char *IMG_GF3;
extern const char *IMG_GF4_10;
extern const char *IMG_GF4_11;
extern const char *IMG_GF4_1_2;
extern const char *IMG_GF4_12;
extern const char *IMG_GF4_13;
extern const char *IMG_GF4_14;
extern const char *IMG_GF4_15;
extern const char *IMG_GF4_16;
extern const char *IMG_GF4_17;
extern const char *IMG_GF4_18;
extern const char *IMG_GF4_19;
extern const char *IMG_GF4_1;
extern const char *IMG_GF4_20;
extern const char *IMG_GF4_2_1;
extern const char *IMG_GF4_21;
extern const char *IMG_GF4_2_2;
extern const char *IMG_GF4_22;
extern const char *IMG_GF4_23;
extern const char *IMG_GF4_24;
extern const char *IMG_GF4_25_2;
extern const char *IMG_GF4_26;
extern const char *IMG_GF4_27;
extern const char *IMG_GF4_2;
extern const char *IMG_GF4_3;
extern const char *IMG_GF4_4_2;
extern const char *IMG_GF4_4;
extern const char *IMG_GF4_5_1;
extern const char *IMG_GF4_5_2;
extern const char *IMG_GF4_5;
extern const char *IMG_GF4_6;
extern const char *IMG_GF4_7;
extern const char *IMG_GF4_8;
extern const char *IMG_GF4_9;
extern const char *IMG_GF4_BEAM1;
extern const char *IMG_GF4_BEAM2;
extern const char *IMG_GF4_BEAM3;
extern const char *IMG_GF4_BEAM4;
extern const char *IMG_GF4_CLOGO1;
extern const char *IMG_GF4_CLOGO2;
extern const char *IMG_GF4_CLOGO3;
extern const char *IMG_GF4_CLOGO4;
extern const char *IMG_GF4_CLOGO5;
extern const char *IMG_GF4_CLOGO6;
extern const char *IMG_GF4_CLOGO7;
extern const char *IMG_GF4_CLOGO8;
extern const char *IMG_GF4_CLOGO9;
extern const char *IMG_GF4_COVER;
extern const char *IMG_GF4_HMENU1;
extern const char *IMG_GF4_HMENU2;
extern const char *IMG_GF4_HMENU3;
extern const char *IMG_GF4_HMENU4;
extern const char *IMG_GF4_HMENU5;
extern const char *IMG_GF4_MA1;
extern const char *IMG_GF4_MA2;
extern const char *IMG_GF4_MA3;
extern const char *IMG_GF4_MA4;
extern const char *IMG_GF4_MA5;
extern const char *IMG_GF4_SBR;
extern const char *IMG_GF4_SHADE;
extern const char *IMG_GF4_XP1;
extern const char *IMG_GF4_XP2;
extern const char *IMG_GF4_XP3;
extern const char *IMG_GF4_XP4;
extern const char *IMG_GF4_XP5;
extern const char *IMG_GF4_XP6;
extern const char *IMG_GM1_B1;
extern const char *IMG_GM1_B2;
extern const char *IMG_GM1_B3;
extern const char *IMG_GM1_B4;
extern const char *IMG_GM1_EXIT;
extern const char *IMG_GM1_GAMEOVER;
extern const char *IMG_GM1_GO;
extern const char *IMG_GM1_KBON;
extern const char *IMG_GM1_MARKNEW;
extern const char *IMG_GM1_PICTURE;
extern const char *IMG_GM1_S0;
extern const char *IMG_GM1_S1;
extern const char *IMG_GM1_S2;
extern const char *IMG_GM1_S3;
extern const char *IMG_GM1_S4;
extern const char *IMG_GM1_S5;
extern const char *IMG_GM1_S6;
extern const char *IMG_GM1_S7;
extern const char *IMG_GM1_S8;
extern const char *IMG_GM1_S9;
extern const char *IMG_GM1_SBON;
extern const char *IMG_GM1_SHERIFF;
extern const char *IMG_GM1_SHIELD0;
extern const char *IMG_GM1_SHIELD1;
extern const char *IMG_GM1_SHIELD2;
extern const char *IMG_GM1_SHIELD3;
extern const char *IMG_GM1_SHIELD4;
extern const char *IMG_GM1_SHOOT;
extern const char *IMG_GM1_STAGE;
extern const char *IMG_GM1_T10;
extern const char *IMG_GM1_T11;
extern const char *IMG_GM1_T12;
extern const char *IMG_GM1_T1;
extern const char *IMG_GM1_T2;
extern const char *IMG_GM1_T3;
extern const char *IMG_GM1_T4;
extern const char *IMG_GM1_T5;
extern const char *IMG_GM1_T6;
extern const char *IMG_GM1_T7;
extern const char *IMG_GM1_T8;
extern const char *IMG_GM1_T9;
extern const char *IMG_GM1_TFIRE;
extern const char *IMG_GM1_TOKILL;
extern const char *IMG_GM1_XP1;
extern const char *IMG_GM1_XP2;
extern const char *IMG_GM1_XP3;
extern const char *IMG_GM1_XP4;
extern const char *IMG_GM1_XP5;
extern const char *IMG_GM1_XP6;
extern const char *IMG_HF1_PICTURE2;
extern const char *IMG_HF2_PICTURE2;
extern const char *IMG_HF3_PICTURE2;
extern const char *IMG_HF4_PICTURE2;
extern const char *IMG_HF5_PICTURE2;
extern const char *IMG_HF6_PICTURE2;
extern const char *IMG_HF8_PICTURE2;
extern const char *IMG_HP1_HYP1;
extern const char *IMG_HP1_HYP2;
extern const char *IMG_HP1_HYP3;
extern const char *IMG_HP1_HYP4;
extern const char *IMG_HP1_HYP5;
extern const char *IMG_HS1_F10;
extern const char *IMG_HS1_F11;
extern const char *IMG_HS1_F1;
extern const char *IMG_HS1_F2;
extern const char *IMG_HS1_F3;
extern const char *IMG_HS1_F4;
extern const char *IMG_HS1_F5;
extern const char *IMG_HS1_F6;
extern const char *IMG_HS1_F7;
extern const char *IMG_HS1_F8;
extern const char *IMG_HS1_F9;
extern const char *IMG_HS1_P0;
extern const char *IMG_HS1_P1;
extern const char *IMG_HS1_P2;
extern const char *IMG_HS1_P3;
extern const char *IMG_HS1_P4;
extern const char *IMG_HS1_P5;
extern const char *IMG_HS1_P6;
extern const char *IMG_HS1_P7;
extern const char *IMG_HS1_P8;
extern const char *IMG_HS1_S0;
extern const char *IMG_HS1_S1;
extern const char *IMG_HS1_S2;
extern const char *IMG_HS1_S3;
extern const char *IMG_HS1_S4;
extern const char *IMG_HS1_S5;
extern const char *IMG_HS1_S6;
extern const char *IMG_HS1_S7;
extern const char *IMG_HS1_S8;
extern const char *IMG_HS2_C10;
extern const char *IMG_HS2_C1;
extern const char *IMG_HS2_C2;
extern const char *IMG_HS2_C3;
extern const char *IMG_HS2_C4;
extern const char *IMG_HS2_C5;
extern const char *IMG_HS2_C6;
extern const char *IMG_HS2_C7;
extern const char *IMG_HS2_C8;
extern const char *IMG_HS2_C9;
extern const char *IMG_HS2_S;
extern const char *IMG_HS3_G1;
extern const char *IMG_HS3_S1;
extern const char *IMG_HS4_L10;
extern const char *IMG_HS4_L11;
extern const char *IMG_HS4_L12;
extern const char *IMG_HS4_L13;
extern const char *IMG_HS4_L14;
extern const char *IMG_HS4_L15;
extern const char *IMG_HS4_L1;
extern const char *IMG_HS4_L2;
extern const char *IMG_HS4_L3;
extern const char *IMG_HS4_L4;
extern const char *IMG_HS4_L5;
extern const char *IMG_HS4_L6;
extern const char *IMG_HS4_L7;
extern const char *IMG_HS4_L8;
extern const char *IMG_HS4_L9;
extern const char *IMG_LD0_LD0_1;
extern const char *IMG_LD0_LD0_2;
extern const char *IMG_LD0_LD0_3;
extern const char *IMG_LD0_LD0;
extern const char *IMG_LD1_LD1_1;
extern const char *IMG_LD1_LD1_2;
extern const char *IMG_LD1_LD1_3;
extern const char *IMG_LD1_LD1;
extern const char *IMG_LD2_LD2_1;
extern const char *IMG_LD2_LD2_2;
extern const char *IMG_LD2_LD2_3;
extern const char *IMG_LD2_LD2;
extern const char *IMG_LD3_LD3_1;
extern const char *IMG_LD3_LD3_2;
extern const char *IMG_LD3_LD3_3;
extern const char *IMG_LD3_LD3;
extern const char *IMG_LD4_LD4_1;
extern const char *IMG_LD4_LD4_2;
extern const char *IMG_LD4_LD4_3;
extern const char *IMG_LD4_LD4;
extern const char *IMG_LD5_LD5_1;
extern const char *IMG_LD5_LD5_2;
extern const char *IMG_LD5_LD5_3;
extern const char *IMG_LD5_LD5;
extern const char *IMG_LG1_PIC1;
extern const char *IMG_LG1_PIC2;
extern const char *IMG_LG1_PIC3;
extern const char *IMG_LG1_PIC4;
extern const char *IMG_LG1_PIC5;
extern const char *IMG_LP1_LAND10;
extern const char *IMG_LP1_LAND11;
extern const char *IMG_LP1_LAND12;
extern const char *IMG_LP1_LAND13;
extern const char *IMG_LP1_LAND14;
extern const char *IMG_LP1_LAND15;
extern const char *IMG_LP1_LAND16;
extern const char *IMG_LP1_LAND17;
extern const char *IMG_LP1_LAND18;
extern const char *IMG_LP1_LAND19;
extern const char *IMG_LP1_LAND20;
extern const char *IMG_LP1_LAND2;
extern const char *IMG_LP1_LAND3;
extern const char *IMG_LP1_LAND4;
extern const char *IMG_LP1_LAND5;
extern const char *IMG_LP1_LAND6;
extern const char *IMG_LP1_LAND7;
extern const char *IMG_LP1_LAND8;
extern const char *IMG_LP1_LAND9;
extern const char *IMG_ME1_MENU;
extern const char *IMG_ME2_MENU;
extern const char *IMG_ME4_MENU;
extern const char *IMG_ME5_MENU;
extern const char *IMG_ME6_MENU;
extern const char *IMG_ME7_MENU;
extern const char *IMG_OP1_OPEN1;
extern const char *IMG_OP1_OPEN2;
extern const char *IMG_OP1_OPEN3;
extern const char *IMG_OP1_OPEN4;
extern const char *IMG_OP1_OPEN5;
extern const char *IMG_OP1_OPEN6;
extern const char *IMG_PA_ROT;
extern const char *IMG_PL1_PLANET1;
extern const char *IMG_PL2_PLANET2;
extern const char *IMG_PL3_PLANET3;
extern const char *IMG_PL4_PLANET4;
extern const char *IMG_PL5_PLANET5;
extern const char *IMG_PL6_PLANET6;
extern const char *IMG_PL8_PLANET8;
extern const char *IMG_PP1_CONST;
extern const char *IMG_PP2_CONST;
extern const char *IMG_PP3_CONST;
extern const char *IMG_PT1_PATTERN;
extern const char *IMG_PT2_PATTERN;
extern const char *IMG_RD1_A1;
extern const char *IMG_RD1_ACT0;
extern const char *IMG_RD1_ACT1;
extern const char *IMG_RD1_ACT2;
extern const char *IMG_RD1_ACT3;
extern const char *IMG_RD1_ACT4;
extern const char *IMG_RD1_ACT5;
extern const char *IMG_RD1_BOW;
extern const char *IMG_RD1_DEAD10;
extern const char *IMG_RD1_DEAD1;
extern const char *IMG_RD1_DEAD2;
extern const char *IMG_RD1_DEAD3;
extern const char *IMG_RD1_DEAD4;
extern const char *IMG_RD1_DEAD5;
extern const char *IMG_RD1_DEAD6;
extern const char *IMG_RD1_DEAD7;
extern const char *IMG_RD1_DEAD8;
extern const char *IMG_RD1_DEAD9;
extern const char *IMG_RD1_F1;
extern const char *IMG_RD1_FAIL1;
extern const char *IMG_RD1_FAIL2;
extern const char *IMG_RD1_FAIL3;
extern const char *IMG_RD1_HIT;
extern const char *IMG_RD1_MBACK;
extern const char *IMG_RD1_RFAIL;
extern const char *IMG_RD1_ST1;
extern const char *IMG_RD1_ST2;
extern const char *IMG_RD1_ST3;
extern const char *IMG_RD1_ST4;
extern const char *IMG_RD1_STSH;
extern const char *IMG_RD1_SYMBOL1;
extern const char *IMG_RD1_SYMBOL1S;
extern const char *IMG_RD1_SYMBOL2;
extern const char *IMG_RD1_SYMBOL2S;
extern const char *IMG_RD1_SYMBOL3;
extern const char *IMG_RD1_SYMBOL3S;
extern const char *IMG_RD1_TLINE1;
extern const char *IMG_RD1_TLINE2;
extern const char *IMG_RD1_TLINE3;
extern const char *IMG_RD1_TYP1;
extern const char *IMG_RD1_TYP2;
extern const char *IMG_RD1_TYP3;
extern const char *IMG_RD1_TYP4;
extern const char *IMG_RD1_TYP5;
extern const char *IMG_RF1_1_2;
extern const char *IMG_RF1_13;
extern const char *IMG_RF1_1;
extern const char *IMG_RF1_7;
extern const char *IMG_SC1_PICTURE;
extern const char *IMG_SC1_SHUTTLE;
extern const char *IMG_SF1;
extern const char *IMG_SF1_STONE0;
extern const char *IMG_SF1_STONE10;
extern const char *IMG_SF1_STONE19_0;
extern const char *IMG_SF1_STONE2_0;
extern const char *IMG_SF1_STONE29B;
extern const char *IMG_SF1_STONE29;
extern const char *IMG_SF1_STONE2;
extern const char *IMG_SF1_STONE8;
extern const char *IMG_SF1_STONE9;
extern const char *IMG_SF2;
extern const char *IMG_SF2_STONE0;
extern const char *IMG_SF2_STONE10;
extern const char *IMG_SF2_STONE19_0;
extern const char *IMG_SF2_STONE2_0;
extern const char *IMG_SF2_STONE29B;
extern const char *IMG_SF2_STONE29;
extern const char *IMG_SF2_STONE2;
extern const char *IMG_SF2_STONE8;
extern const char *IMG_SF2_STONE9;
extern const char *IMG_SF3;
extern const char *IMG_SF3_STONE0;
extern const char *IMG_SF3_STONE10;
extern const char *IMG_SF3_STONE19_0;
extern const char *IMG_SF3_STONE2_0;
extern const char *IMG_SF3_STONE29B;
extern const char *IMG_SF3_STONE29;
extern const char *IMG_SF3_STONE2;
extern const char *IMG_SF3_STONE8;
extern const char *IMG_SF3_STONE9;
extern const char *IMG_SF4;
extern const char *IMG_SF4_STONE0;
extern const char *IMG_SF4_STONE10;
extern const char *IMG_SF4_STONE19_0;
extern const char *IMG_SF4_STONE2_0;
extern const char *IMG_SF4_STONE29BOLD;
extern const char *IMG_SF4_STONE29B;
extern const char *IMG_SF4_STONE29OLD;
extern const char *IMG_SF4_STONE29;
extern const char *IMG_SF4_STONE2;
extern const char *IMG_SF4_STONE8OLD;
extern const char *IMG_SF4_STONE8;
extern const char *IMG_SF4_STONE9OLD;
extern const char *IMG_SF4_STONE9;
extern const char *IMG_SF5;
extern const char *IMG_SF5_STONE0;
extern const char *IMG_SF5_STONE10;
extern const char *IMG_SF5_STONE19_0;
extern const char *IMG_SF5_STONE2_0;
extern const char *IMG_SF5_STONE29B;
extern const char *IMG_SF5_STONE29;
extern const char *IMG_SF5_STONE2;
extern const char *IMG_SF5_STONE8;
extern const char *IMG_SF5_STONE9;
extern const char *IMG_SF6;
extern const char *IMG_SF6_STONE0;
extern const char *IMG_SF6_STONE10;
extern const char *IMG_SF6_STONE19_0;
extern const char *IMG_SF6_STONE2_0;
extern const char *IMG_SF6_STONE24;
extern const char *IMG_SF6_STONE25;
extern const char *IMG_SF6_STONE29B;
extern const char *IMG_SF6_STONE29;
extern const char *IMG_SF6_STONE2;
extern const char *IMG_SF6_STONE8;
extern const char *IMG_SF6_STONE9;
extern const char *IMG_SF8;
extern const char *IMG_SF8_STONE0;
extern const char *IMG_SF8_STONE10;
extern const char *IMG_SF8_STONE19_0;
extern const char *IMG_SF8_STONE2_0;
extern const char *IMG_SF8_STONE29B;
extern const char *IMG_SF8_STONE29;
extern const char *IMG_SF8_STONE2;
extern const char *IMG_SF8_STONE8;
extern const char *IMG_SF8_STONE9;
extern const char *IMG_SG1_GUILD;
extern const char *IMG_SG1_LGHT1;
extern const char *IMG_SG1_LGHT2;
extern const char *IMG_SG1_LGHT3;
extern const char *IMG_SG1_LGHT4;
extern const char *IMG_SG1_TRANS1;
extern const char *IMG_SG1_TRANS2;
extern const char *IMG_SG1_TRANS3;
extern const char *IMG_SG1_UFO1;
extern const char *IMG_SG1_UFO2;
extern const char *IMG_SG2_EYES2;
extern const char *IMG_SG2_EYES;
extern const char *IMG_SG2_INS1;
extern const char *IMG_SG2_INS2;
extern const char *IMG_SG2_INS3;
extern const char *IMG_SG3_DUST;
extern const char *IMG_SG3_LAMP0;
extern const char *IMG_SG3_LAMP1;
extern const char *IMG_SG3_MIRR0;
extern const char *IMG_SG3_MIRR1;
extern const char *IMG_SG3_MIRR2;
extern const char *IMG_SG3_MIRR3;
extern const char *IMG_SG3_SCREEN1;
extern const char *IMG_SG3_SCREEN2;
extern const char *IMG_SG3_SCREEN3;
extern const char *IMG_SG3_SCREEN4;
extern const char *IMG_SG3_TALK1;
extern const char *IMG_SG3_TALK2;
extern const char *IMG_SG3_TALK3;
extern const char *IMG_SG3_TALK4;
extern const char *IMG_SG3_TALK5;
extern const char *IMG_SG3_TALK6;
extern const char *IMG_SP1_DMG1;
extern const char *IMG_SP1_DMG2;
extern const char *IMG_SP1_DMG3;
extern const char *IMG_SP1_DMG4;
extern const char *IMG_SP1_DMG5;
extern const char *IMG_SP1_DMG6;
extern const char *IMG_SP1_DMG7;
extern const char *IMG_SP1_DMG8;
extern const char *IMG_SP1_SHIP1;
extern const char *IMG_SP1_SHP1;
extern const char *IMG_SP1_SHP2;
extern const char *IMG_SP1_SHP3;
extern const char *IMG_SP1_SHP4;
extern const char *IMG_SP1_SHP5;
extern const char *IMG_ST1_INITSPR;
extern const char *IMG_SU1_ALLOW0;
extern const char *IMG_SU1_ALLOW1;
extern const char *IMG_SU1_B_0;
extern const char *IMG_SU1_B_1;
extern const char *IMG_SU1_BUILD;
extern const char *IMG_SU1_CTA;
extern const char *IMG_SU1_CTB;
extern const char *IMG_SU1_CTC;
extern const char *IMG_SU1_CTRL1;
extern const char *IMG_SU1_CTRL2;
extern const char *IMG_SU1_CTRL3;
extern const char *IMG_SU1_DEAD10;
extern const char *IMG_SU1_DEAD1;
extern const char *IMG_SU1_DEAD2;
extern const char *IMG_SU1_DEAD3;
extern const char *IMG_SU1_DEAD4;
extern const char *IMG_SU1_DEAD5;
extern const char *IMG_SU1_DEAD6;
extern const char *IMG_SU1_DEAD7;
extern const char *IMG_SU1_DEAD8;
extern const char *IMG_SU1_DEAD9;
extern const char *IMG_SU1_EXPORT;
extern const char *IMG_SU1_MARK;
extern const char *IMG_SU1_MENU;
extern const char *IMG_SU1_PARAGRAPH;
extern const char *IMG_SU1_PEOPLE;
extern const char *IMG_SU1_STONE1_0;
extern const char *IMG_SU1_STONE10;
extern const char *IMG_SU1_STONE11_0;
extern const char *IMG_SU1_STONE11_1;
extern const char *IMG_SU1_STONE11_2;
extern const char *IMG_SU1_STONE11_3;
extern const char *IMG_SU1_STONE1_1;
extern const char *IMG_SU1_STONE11;
extern const char *IMG_SU1_STONE12_0;
extern const char *IMG_SU1_STONE12_1;
extern const char *IMG_SU1_STONE12_2;
extern const char *IMG_SU1_STONE12_3;
extern const char *IMG_SU1_STONE1_2;
extern const char *IMG_SU1_STONE12;
extern const char *IMG_SU1_STONE13_0;
extern const char *IMG_SU1_STONE13_1;
extern const char *IMG_SU1_STONE13_2;
extern const char *IMG_SU1_STONE13_3;
extern const char *IMG_SU1_STONE1_3;
extern const char *IMG_SU1_STONE13;
extern const char *IMG_SU1_STONE14_0;
extern const char *IMG_SU1_STONE14_1;
extern const char *IMG_SU1_STONE14_2;
extern const char *IMG_SU1_STONE14;
extern const char *IMG_SU1_STONE15_0;
extern const char *IMG_SU1_STONE15_1;
extern const char *IMG_SU1_STONE15_2;
extern const char *IMG_SU1_STONE15;
extern const char *IMG_SU1_STONE16_0;
extern const char *IMG_SU1_STONE16_1;
extern const char *IMG_SU1_STONE16B;
extern const char *IMG_SU1_STONE16;
extern const char *IMG_SU1_STONE17_0;
extern const char *IMG_SU1_STONE17_1;
extern const char *IMG_SU1_STONE17;
extern const char *IMG_SU1_STONE18;
extern const char *IMG_SU1_STONE19_0;
extern const char *IMG_SU1_STONE1;
extern const char *IMG_SU1_STONE2_0;
extern const char *IMG_SU1_STONE20;
extern const char *IMG_SU1_STONE22;
extern const char *IMG_SU1_STONE23_0;
extern const char *IMG_SU1_STONE23_1;
extern const char *IMG_SU1_STONE23_2;
extern const char *IMG_SU1_STONE23_3;
extern const char *IMG_SU1_STONE23;
extern const char *IMG_SU1_STONE2;
extern const char *IMG_SU1_STONE3_0;
extern const char *IMG_SU1_STONE3_1;
extern const char *IMG_SU1_STONE3_2;
extern const char *IMG_SU1_STONE3;
extern const char *IMG_SU1_STONE4_0;
extern const char *IMG_SU1_STONE4_1;
extern const char *IMG_SU1_STONE4_2;
extern const char *IMG_SU1_STONE4_3;
extern const char *IMG_SU1_STONE4B;
extern const char *IMG_SU1_STONE4;
extern const char *IMG_SU1_STONE5_0;
extern const char *IMG_SU1_STONE5_1;
extern const char *IMG_SU1_STONE5_2;
extern const char *IMG_SU1_STONE5_3;
extern const char *IMG_SU1_STONE5_4;
extern const char *IMG_SU1_STONE5_5;
extern const char *IMG_SU1_STONE5A10;
extern const char *IMG_SU1_STONE5A11;
extern const char *IMG_SU1_STONE5A12;
extern const char *IMG_SU1_STONE5A13;
extern const char *IMG_SU1_STONE5A14;
extern const char *IMG_SU1_STONE5A15;
extern const char *IMG_SU1_STONE5A16;
extern const char *IMG_SU1_STONE5A17;
extern const char *IMG_SU1_STONE5A18;
extern const char *IMG_SU1_STONE5A1;
extern const char *IMG_SU1_STONE5A2;
extern const char *IMG_SU1_STONE5A3;
extern const char *IMG_SU1_STONE5A4;
extern const char *IMG_SU1_STONE5A5;
extern const char *IMG_SU1_STONE5A6;
extern const char *IMG_SU1_STONE5A7;
extern const char *IMG_SU1_STONE5A8;
extern const char *IMG_SU1_STONE5A9;
extern const char *IMG_SU1_STONE5;
extern const char *IMG_SU1_STONE6;
extern const char *IMG_SU1_STONE7;
extern const char *IMG_SU1_STONEXX;
extern const char *IMG_SU1_TRI;
extern const char *IMG_SU2_STONE7;
extern const char *IMG_TD1_TRADE1;
extern const char *IMG_TD1_TRADE2;
extern const char *IMG_TD1_TRADE3;
extern const char *IMG_TD1_TRADE4;
extern const char *IMG_TD1_TRADE5;
extern const char *IMG_TD1_TRADE6;
extern const char *IMG_TD1_TRADE7;
extern const char *IMG_TD1_TRADE8;
extern const char *IMG_TD1_TRADEBACK;
extern const char *IMG_TD2_TR0;
extern const char *IMG_TD2_TR1;
extern const char *IMG_TD2_TR2;
extern const char *IMG_TD2_TR3;
extern const char *IMG_TD2_TR4;
extern const char *IMG_TD2_TR5;
extern const char *IMG_TD2_TR6;
extern const char *IMG_TD2_TR7;
extern const char *IMG_TD2_TR8;
extern const char *IMG_TS1_ALL1;
extern const char *IMG_TS1_ALL2;
extern const char *IMG_TS1_ALL3;
extern const char *IMG_TS1_CLOGO1;
extern const char *IMG_TS1_CLOGO2;
extern const char *IMG_TS1_CLOGO3;
extern const char *IMG_TS1_CLOGO4;
extern const char *IMG_TS1_CLOGO5;
extern const char *IMG_TS1_CLOGO6;
extern const char *IMG_TS1_CLOGO7;
extern const char *IMG_TS1_CLOGO8;
extern const char *IMG_TS1_CLOGO9;
extern const char *IMG_TS1_DEST;
extern const char *IMG_TS1_FLAG0;
extern const char *IMG_TS1_FLAG10;
extern const char *IMG_TS1_FLAG11;
extern const char *IMG_TS1_FLAG12;
extern const char *IMG_TS1_FLAG13;
extern const char *IMG_TS1_FLAG14;
extern const char *IMG_TS1_FLAG15;
extern const char *IMG_TS1_FLAG16;
extern const char *IMG_TS1_FLAG1;
extern const char *IMG_TS1_FLAG2;
extern const char *IMG_TS1_FLAG3;
extern const char *IMG_TS1_FLAG4;
extern const char *IMG_TS1_FLAG5;
extern const char *IMG_TS1_FLAG6;
extern const char *IMG_TS1_FLAG7;
extern const char *IMG_TS1_FLAG8;
extern const char *IMG_TS1_FLAG9;
extern const char *IMG_TS1_HEADER1;
extern const char *IMG_TS1_HEADER2;
extern const char *IMG_TS1_HEADER;
extern const char *IMG_TS1_ICON1;
extern const char *IMG_TS1_ICON2;
extern const char *IMG_TS1_ICON3;
extern const char *IMG_TS1_ICON4;
extern const char *IMG_TS1_ICON5;
extern const char *IMG_TS1_ID1;
extern const char *IMG_TS1_ID2;
extern const char *IMG_TS1_M11;
extern const char *IMG_TS1_M12;
extern const char *IMG_TS1_M13;
extern const char *IMG_TS1_M14;
extern const char *IMG_TS1_M2_1;
extern const char *IMG_TS1_M21;
extern const char *IMG_TS1_M2_2;
extern const char *IMG_TS1_M22;
extern const char *IMG_TS1_M23;
extern const char *IMG_TS1_M24;
extern const char *IMG_TS1_M25;
extern const char *IMG_TS1_M26;
extern const char *IMG_TS1_M27;
extern const char *IMG_TS1_M28;
extern const char *IMG_TS1_MK1;
extern const char *IMG_TS1_MK2;
extern const char *IMG_TS1_MK3;
extern const char *IMG_TS1_QMARK;
extern const char *IMG_TS1_SHICON;
extern const char *IMG_TS1_SML_MENU;
extern const char *IMG_TS1_SPHERE1;
extern const char *IMG_TS1_SPHERE2;
extern const char *IMG_TS1_SPHERE3;
extern const char *IMG_TS1_SPHERE4;
extern const char *IMG_TS1_SPHERE5;
extern const char *IMG_TS1_SPHERE6;
extern const char *IMG_TS1_SPHERE8;
extern const char *IMG_TS1_ST1;
extern const char *IMG_TS1_ST2;
extern const char *IMG_TS1_ST3;
extern const char *IMG_TS1_ST4;
extern const char *IMG_TS1_ST5;
extern const char *IMG_TS1_ST6;
extern const char *IMG_TS1_ST8;
extern const char *IMG_TS1_SUN1;
extern const char *IMG_TS1_SUN2;
extern const char *IMG_TS1_SUN3;
extern const char *IMG_TS1_SUN4;
extern const char *IMG_TS1_SUN5;
extern const char *IMG_TS1_SUN6;
extern const char *IMG_TS1_SUN7;
extern const char *IMG_TS1_SUN8;
extern const char *IMG_TS1_SUN9;
extern const char *IMG_TS1_WORM;
extern const char *IMG_TS2_ALL1;
extern const char *IMG_TS2_ALL2;
extern const char *IMG_TS2_ALL3;
extern const char *IMG_TS2_CLOGO1;
extern const char *IMG_TS2_CLOGO2;
extern const char *IMG_TS2_CLOGO3;
extern const char *IMG_TS2_CLOGO4;
extern const char *IMG_TS2_CLOGO5;
extern const char *IMG_TS2_CLOGO6;
extern const char *IMG_TS2_CLOGO7;
extern const char *IMG_TS2_CLOGO8;
extern const char *IMG_TS2_CLOGO9;
extern const char *IMG_TS2_M2_1;
extern const char *IMG_TS2_M2_2;
extern const char *IMG_TS2_PATTERN;
extern const char *IMG_TS2_SHICON;
extern const char *IMG_TS2_SML_MENU;
extern const char *IMG_TS2_SPHERE1;
extern const char *IMG_TS2_SPHERE2;
extern const char *IMG_TS2_SPHERE3;
extern const char *IMG_TS2_SPHERE4;
extern const char *IMG_TS2_SPHERE5;
extern const char *IMG_TS2_SPHERE6;
extern const char *IMG_TS2_SPHERE8;
extern const char *IMG_VS0_VPIC;
extern const char *IMG_VS1_VESSEL;
extern const char *IMG_VS2_VESSEL;
extern const char *IMG_VS3_VESSEL;
extern const char *IMG_VS4_VESSEL;
extern const char *IMG_VS5_VESSEL;
extern const char *IMG_VS6_VESSEL;
extern const char *IMG_WD1_DONE;
extern const char *IMG_WD1_WELL;

extern const char *INIT_IMG;
extern const char *CURSOR_IMG;

extern const char *ASSETS_SFX[];
extern const char *ASSETS_MUS[];
extern const char *ASSETS_IMG[];

#endif
