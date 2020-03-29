#ifdef LINUX

#define D_SUB "assets/img/sub/"

const char *FONT_AUDIOWIDE = "assets/font/audiowide/Audiowide-Regular.ttf";

#define D_SFX "assets/sfx/"
const char *SFX_SHOT            = D_SFX "1_2000";
const char *SFX_EXPLOSION       = D_SFX "2_2000";
const char *SFX_HEAVYSHOT       = D_SFX "3_500";
const char *SFX_GLIDE_LOW       = D_SFX "4_800";
const char *SFX_GLIDE_MED       = D_SFX "4_1000";
const char *SFX_GLIDE_HIGH      = D_SFX "4_1500";
const char *SFX_WALK0           = D_SFX "5_1C00";
const char *SFX_WALK1           = D_SFX "6_1C00";
const char *SFX_WALK2           = D_SFX "7_1C00";
const char *SFX_BEEP            = D_SFX "8_2500";

#define D_MUS "assets/music/"
const char *MUS_INTRO           = D_MUS "ST1_1";
const char *MUS_ST1_10          = D_MUS "ST1_10";
const char *MUS_END             = D_MUS "ST2_1";
const char *MUS_ST2_E           = D_MUS "ST2_E";
const char *MUS_OVERVIEW        = D_MUS "ST3_0";
const char *MUS_ST3_A           = D_MUS "ST3_A";
const char *MUS_ST3_F           = D_MUS "ST3_F";
const char *MUS_BATTLE_PREP     = D_MUS "ST3_13";
const char *MUS_BATTLE_WON      = D_MUS "ST3_17";
const char *MUS_TRADE           = D_MUS "ST3_23";
const char *MUS_ST3_27          = D_MUS "ST3_27";
const char *MUS_INVASION        = D_MUS "ST3_2A";
const char *MUS_BATTLE_LOST     = D_MUS "ST3_2B";
const char *MUS_ACHIEVEMENT     = D_MUS "ST3_2C";
const char *MUS_MINIGAME        = D_MUS "ST3_2E";
const char *MUS_GAMESTART       = D_MUS "ST3_34";
const char *MUS_OVERWORLD       = D_MUS "ST3_36";
const char *MUS_ALIENSHIP       = D_MUS "ST3_3C";
const char *MUS_ST3_42          = D_MUS "ST3_42";
const char *MUS_BATTLEWAIT      = D_MUS "ST3_48";
const char *MUS_ST3_4D          = D_MUS "ST3_4D";
const char *MUS_CELEBRATE       = D_MUS "ST3_51";
const char *MUS_GUILD           = D_MUS "ST3_52";
const char *MUS_OVERWORLD_ANGRY = D_MUS "ST3_55";
const char *MUS_ST3_58          = D_MUS "ST3_58";

const char *IMG_CURSOR          = "assets/img/cursor";
const char *IMG_SHIP_FLIPPED    = "assets/img/SH3_cruiser1_flipped";

#define D_PIC "assets/img/pictures/"
const char *IMG_INTRO_EARTH     = D_PIC "ea1";
const char *IMG_INTRO_CITY      = D_PIC "ct1";
const char *IMG_INTRO_STARPORT  = D_PIC "cn1";
const char *IMG_INTRO_CORRIDOR  = D_PIC "cr1";
const char *IMG_INTRO_DOOR      = D_PIC "dr1";
const char *IMG_INTRO_KEYPAD    = D_PIC "ts1";
const char *IMG_INTRO_FAIL      = D_PIC "go1";
const char *IMG_INTRO_LAUNCH    = D_PIC "ho1";
const char *IMG_INTRO_SPACE     = D_PIC "js1";
const char *IMG_MAP_SOLAR       = D_PIC "mp2";
const char *IMG_BATTLE          = D_PIC "lb1";
const char *IMG_BATTLE_PREP     = D_PIC "ms1";
const char *IMG_LORD_ARRIVE     = D_PIC "lpl";
const char *IMG_GAMEOVER        = D_PIC "en1";
const char *IMG_PLANETSIDE      = D_PIC "ps1";
const char *IMG_RADAR           = D_PIC "rd1";
const char *IMG_BG_SHUTTLE      = D_PIC "sl1";
const char *IMG_BG_MENU0        = D_PIC "ey1";
const char *IMG_BG_MENU1        = D_PIC "ey2";
const char *IMG_BG_NEWS         = D_PIC "nb1";
const char *IMG_BG_SHIP0        = D_PIC "ss1";
const char *IMG_BG_SHIP1        = D_PIC "ss2";
const char *IMG_BG_STARS0       = D_PIC "st1";
const char *IMG_BG_STARS1       = D_PIC "st2";
const char *IMG_BG_STARS2       = D_PIC "st3";
const char *IMG_BG_STARS3       = D_PIC "st6";
const char *IMG_BG_STARS4       = D_PIC "st8";
const char *IMG_BG_STARS5       = D_PIC "st9";
const char *IMG_BG_STARS6       = D_PIC "st10";
const char *IMG_BG_STARS7       = D_PIC "st11";
const char *IMG_PLANET_FOREST   = D_PIC "pl1";
const char *IMG_PLANET_DESERT   = D_PIC "pl2";
const char *IMG_PLANET_VOLCANO  = D_PIC "pl3";
const char *IMG_PLANET_ROCK     = D_PIC "pl4";
const char *IMG_PLANET_ICE      = D_PIC "pl5";
const char *IMG_PLANET_TERRA    = D_PIC "pl6";
const char *IMG_PLANET_ART      = D_PIC "pl8";
const char *IMG_HARBOUR_FOREST  = D_PIC "sh1";
const char *IMG_HARBOUR_DESERT  = D_PIC "sh2";
const char *IMG_HARBOUR_VOLCANO = D_PIC "sh3";
const char *IMG_HARBOUR_ROCK    = D_PIC "sh4";
const char *IMG_HARBOUR_ICE     = D_PIC "sh5";
const char *IMG_HARBOUR_TERRA   = D_PIC "sh6";
const char *IMG_HARBOUR_ART     = D_PIC "sh8";
const char *IMG_MAKING0         = D_PIC "mk1";
const char *IMG_MAKING1         = D_PIC "mk2";
const char *IMG_MAKING2         = D_PIC "mk3";
const char *IMG_MAKING3         = D_PIC "mk4";
const char *IMG_MAKING4         = D_PIC "mk5";
const char *IMG_MAKING5         = D_PIC "mk6";
const char *IMG_MAKING6         = D_PIC "mk7";
const char *IMG_MAKING7         = D_PIC "mk8";
const char *IMG_MAKING8         = D_PIC "mk9";
const char *IMG_MAKING9         = D_PIC "mk10";
const char *IMG_MAKING10        = D_PIC "mk11";
const char *IMG_MAKING11        = D_PIC "mk12";
const char *IMG_MAKING12        = D_PIC "mk13";
const char *IMG_MAKING13        = D_PIC "mk14";
const char *IMG_MAKING14        = D_PIC "mk15";

#define D_INTRO "assets/img/intro/"
const char *IMG_INTRO_BT1_FALL1    = D_INTRO "BT1_fall1";
const char *IMG_INTRO_BT1_FALL2    = D_INTRO "BT1_fall2";
const char *IMG_INTRO_BT1_FALL3    = D_INTRO "BT1_fall3";
const char *IMG_INTRO_BT1_FALL4    = D_INTRO "BT1_fall4";
const char *IMG_INTRO_BT1_FALL5    = D_INTRO "BT1_fall5";
const char *IMG_INTRO_BT1_FALL6    = D_INTRO "BT1_fall6";
const char *IMG_INTRO_BT1_FALL7    = D_INTRO "BT1_fall7";
const char *IMG_INTRO_FI1_FIRE1    = D_INTRO "FI1_fire1";
const char *IMG_INTRO_FI1_FIRE2    = D_INTRO "FI1_fire2";
const char *IMG_INTRO_FI1_FIRE3    = D_INTRO "FI1_fire3";
const char *IMG_INTRO_FI1_FIRE4    = D_INTRO "FI1_fire4";
const char *IMG_INTRO_FI1_FIRE5    = D_INTRO "FI1_fire5";
const char *IMG_INTRO_FI1_FIRE6    = D_INTRO "FI1_fire6";
const char *IMG_INTRO_FI1_SHOT1    = D_INTRO "FI1_shot1";
const char *IMG_INTRO_FI1_SHOT2    = D_INTRO "FI1_shot2";
const char *IMG_INTRO_FI1_SHOT3    = D_INTRO "FI1_shot3";
const char *IMG_INTRO_FI1_SHOT4    = D_INTRO "FI1_shot4";
const char *IMG_INTRO_FI1_SHOT5    = D_INTRO "FI1_shot5";
const char *IMG_INTRO_GU1_GUARD    = D_INTRO "GU1_guard";
const char *IMG_INTRO_OD1_BEHIND   = D_INTRO "OD1_behind";
const char *IMG_INTRO_OD1_DOOR_L   = D_INTRO "OD1_door_l";
const char *IMG_INTRO_OD1_DOOR_R   = D_INTRO "OD1_door_r";
const char *IMG_INTRO_OD1_FRAME    = D_INTRO "OD1_frame";
const char *IMG_INTRO_OD1_OFF      = D_INTRO "OD1_off";
const char *IMG_INTRO_OD1_ON       = D_INTRO "OD1_on";
const char *IMG_INTRO_OD1_SEPP     = D_INTRO "OD1_sepp";
const char *IMG_INTRO_PH1_PUSH_1   = D_INTRO "PH1_push_1";
const char *IMG_INTRO_PH1_PUSH_2   = D_INTRO "PH1_push_2";
const char *IMG_INTRO_PH1_PUSH_3   = D_INTRO "PH1_push_3";
const char *IMG_INTRO_PH1_PUSH_4   = D_INTRO "PH1_push_4";
const char *IMG_INTRO_PH1_PUSH_5   = D_INTRO "PH1_push_5";
const char *IMG_INTRO_PH1_PUSH_6   = D_INTRO "PH1_push_6";
const char *IMG_INTRO_PH1_PUSH_7   = D_INTRO "PH1_push_7";
const char *IMG_INTRO_PH1_PUSH_8   = D_INTRO "PH1_push_8";
const char *IMG_INTRO_PH1_PUSH_9   = D_INTRO "PH1_push_9";
const char *IMG_INTRO_SH1_SHIP     = D_INTRO "SH1_ship";
const char *IMG_INTRO_SH1_SHIP2    = D_INTRO "SH1_ship2";
const char *IMG_INTRO_SH2_SHIP     = D_INTRO "SH2_ship";
const char *IMG_INTRO_SH3_CRUISER  = D_INTRO "SH3_cruiser";
const char *IMG_INTRO_SH3_CRUISER1 = D_INTRO "SH3_cruiser1";
const char *IMG_INTRO_SH3_CRUISER2 = D_INTRO "SH3_cruiser2";
const char *IMG_INTRO_SH3_SHUTTLE  = D_INTRO "SH3_shuttle";
const char *IMG_INTRO_SH4_BLINK1   = D_INTRO "SH4_blink1";
const char *IMG_INTRO_SH4_BLINK2   = D_INTRO "SH4_blink2";
const char *IMG_INTRO_SH4_BLINK3   = D_INTRO "SH4_blink3";
const char *IMG_INTRO_SH4_MOVEOUT  = D_INTRO "SH4_moveout";
const char *IMG_INTRO_SH4_SHUTTLE1 = D_INTRO "SH4_shuttle1";
const char *IMG_INTRO_SH4_SHUTTLE2 = D_INTRO "SH4_shuttle2";
const char *IMG_INTRO_SH4_SHUTTLE3 = D_INTRO "SH4_shuttle3";
const char *IMG_INTRO_SH4_SHUTTLE4 = D_INTRO "SH4_shuttle4";

#define D_FLM "assets/img/film/"
const char *IMG_FILM_1_SPR_1          = D_FLM "1_spr_1";
const char *IMG_FILM_1_SPR_2_3        = D_FLM "1_spr_2-3";
const char *IMG_FILM_1_SPR_3_3        = D_FLM "1_spr_3-3";

#define D_SGR "assets/img/startgr/"
const char *IMG_STARTGR_FL4_FLEET     = D_SGR "fl4_fleet";
const char *IMG_STARTGR_FL5_10_1      = D_SGR "fl5_10-1";
const char *IMG_STARTGR_FL5_10_2      = D_SGR "fl5_10-2";
const char *IMG_STARTGR_FL5_10_3      = D_SGR "fl5_10-3";
const char *IMG_STARTGR_FL5_10_4      = D_SGR "fl5_10-4";
const char *IMG_STARTGR_FL5_10_5      = D_SGR "fl5_10-5";
const char *IMG_STARTGR_FL5_11_1      = D_SGR "fl5_11-1";
const char *IMG_STARTGR_FL5_11_2      = D_SGR "fl5_11-2";
const char *IMG_STARTGR_FL5_11_3      = D_SGR "fl5_11-3";
const char *IMG_STARTGR_FL5_11_4      = D_SGR "fl5_11-4";
const char *IMG_STARTGR_FL5_11_5      = D_SGR "fl5_11-5";
const char *IMG_STARTGR_FL5_1_1       = D_SGR "fl5_1-1";
const char *IMG_STARTGR_FL5_12_1      = D_SGR "fl5_12-1";
const char *IMG_STARTGR_FL5_12_2      = D_SGR "fl5_12-2";
const char *IMG_STARTGR_FL5_12_3      = D_SGR "fl5_12-3";
const char *IMG_STARTGR_FL5_12_4      = D_SGR "fl5_12-4";
const char *IMG_STARTGR_FL5_12_5      = D_SGR "fl5_12-5";
const char *IMG_STARTGR_FL5_1_2       = D_SGR "fl5_1-2";
const char *IMG_STARTGR_FL5_1_3       = D_SGR "fl5_1-3";
const char *IMG_STARTGR_FL5_1_4       = D_SGR "fl5_1-4";
const char *IMG_STARTGR_FL5_1_5       = D_SGR "fl5_1-5";
const char *IMG_STARTGR_FL5_2_1       = D_SGR "fl5_2-1";
const char *IMG_STARTGR_FL5_2_2       = D_SGR "fl5_2-2";
const char *IMG_STARTGR_FL5_2_3       = D_SGR "fl5_2-3";
const char *IMG_STARTGR_FL5_2_4       = D_SGR "fl5_2-4";
const char *IMG_STARTGR_FL5_2_5       = D_SGR "fl5_2-5";
const char *IMG_STARTGR_FL5_3_1       = D_SGR "fl5_3-1";
const char *IMG_STARTGR_FL5_3_2       = D_SGR "fl5_3-2";
const char *IMG_STARTGR_FL5_3_3       = D_SGR "fl5_3-3";
const char *IMG_STARTGR_FL5_3_4       = D_SGR "fl5_3-4";
const char *IMG_STARTGR_FL5_3_5       = D_SGR "fl5_3-5";
const char *IMG_STARTGR_FL5_4_1       = D_SGR "fl5_4-1";
const char *IMG_STARTGR_FL5_4_2       = D_SGR "fl5_4-2";
const char *IMG_STARTGR_FL5_4_3       = D_SGR "fl5_4-3";
const char *IMG_STARTGR_FL5_4_4       = D_SGR "fl5_4-4";
const char *IMG_STARTGR_FL5_4_5       = D_SGR "fl5_4-5";
const char *IMG_STARTGR_FL5_5_1       = D_SGR "fl5_5-1";
const char *IMG_STARTGR_FL5_5_2       = D_SGR "fl5_5-2";
const char *IMG_STARTGR_FL5_5_3       = D_SGR "fl5_5-3";
const char *IMG_STARTGR_FL5_5_4       = D_SGR "fl5_5-4";
const char *IMG_STARTGR_FL5_5_5       = D_SGR "fl5_5-5";
const char *IMG_STARTGR_FL5_6_1       = D_SGR "fl5_6-1";
const char *IMG_STARTGR_FL5_6_2       = D_SGR "fl5_6-2";
const char *IMG_STARTGR_FL5_6_3       = D_SGR "fl5_6-3";
const char *IMG_STARTGR_FL5_6_4       = D_SGR "fl5_6-4";
const char *IMG_STARTGR_FL5_6_5       = D_SGR "fl5_6-5";
const char *IMG_STARTGR_FL5_7_1       = D_SGR "fl5_7-1";
const char *IMG_STARTGR_FL5_7_2       = D_SGR "fl5_7-2";
const char *IMG_STARTGR_FL5_7_3       = D_SGR "fl5_7-3";
const char *IMG_STARTGR_FL5_7_4       = D_SGR "fl5_7-4";
const char *IMG_STARTGR_FL5_7_5       = D_SGR "fl5_7-5";
const char *IMG_STARTGR_FL5_8_1       = D_SGR "fl5_8-1";
const char *IMG_STARTGR_FL5_8_2       = D_SGR "fl5_8-2";
const char *IMG_STARTGR_FL5_8_3       = D_SGR "fl5_8-3";
const char *IMG_STARTGR_FL5_8_4       = D_SGR "fl5_8-4";
const char *IMG_STARTGR_FL5_8_5       = D_SGR "fl5_8-5";
const char *IMG_STARTGR_FL5_9_1       = D_SGR "fl5_9-1";
const char *IMG_STARTGR_FL5_9_2       = D_SGR "fl5_9-2";
const char *IMG_STARTGR_FL5_9_3       = D_SGR "fl5_9-3";
const char *IMG_STARTGR_FL5_9_4       = D_SGR "fl5_9-4";
const char *IMG_STARTGR_FL5_9_5       = D_SGR "fl5_9-5";
const char *IMG_STARTGR_FL5_FLEET     = D_SGR "fl5_fleet";
const char *IMG_STARTGR_FL6_STARS     = D_SGR "fl6_stars";
const char *IMG_STARTGR_GAL_S         = D_SGR "gr1_piccy1";
const char *IMG_STARTGR_GAL_M         = D_SGR "gr1_piccy2";
const char *IMG_STARTGR_GAL_L         = D_SGR "gr1_piccy3";
const char *IMG_STARTGR_AIM_MIGHT     = D_SGR "gr3_piccy1";
const char *IMG_STARTGR_AIM_MONEY     = D_SGR "gr3_piccy2";
const char *IMG_STARTGR_AIM_CIV       = D_SGR "gr3_piccy3";
const char *IMG_STARTGR_STR_WEAK      = D_SGR "gr5_piccy1";
const char *IMG_STARTGR_STR_MEDIUM    = D_SGR "gr5_piccy2";
const char *IMG_STARTGR_STR_STRONG    = D_SGR "gr5_piccy3";
const char *IMG_STARTGR_MPSTR_NONE    = D_SGR "gr7_piccy1";
const char *IMG_STARTGR_MPSTR_WEAK    = D_SGR "gr7_piccy2";
const char *IMG_STARTGR_MPSTR_MEDIUM  = D_SGR "gr7_piccy3";

#define D_GRA "assets/img/graphic/"
const char *IMG_AL1_1_2          = D_GRA "al1_1-2";
const char *IMG_AL1_13           = D_GRA "al1_13";
const char *IMG_AL1_15           = D_GRA "al1_15";
const char *IMG_AL1_1            = D_GRA "al1_1";
const char *IMG_AL1_3            = D_GRA "al1_3";
const char *IMG_AL1_7            = D_GRA "al1_7";
const char *IMG_AL1_9            = D_GRA "al1_9";
const char *IMG_AL2_1_2          = D_GRA "al2_1-2";
const char *IMG_AL2_13           = D_GRA "al2_13";
const char *IMG_AL2_15           = D_GRA "al2_15";
const char *IMG_AL2_1            = D_GRA "al2_1";
const char *IMG_AL2_3            = D_GRA "al2_3";
const char *IMG_AL2_7            = D_GRA "al2_7";
const char *IMG_AL2_9            = D_GRA "al2_9";
const char *IMG_AP1_APPROACH     = D_GRA "ap1_approach";
const char *IMG_AP3_APPROACH     = D_GRA "ap3_approach";
const char *IMG_AP8_APPROACH     = D_GRA "ap8_approach";
const char *IMG_BR10_EXPORT      = D_GRA "br10_export";
const char *IMG_BR11_DEF         = D_GRA "br11_def";
const char *IMG_BR11_LR          = D_GRA "br11_lr";
const char *IMG_BR11_OK          = D_GRA "br11_ok";
const char *IMG_BR12_EXPORT      = D_GRA "br12_export";
const char *IMG_BR13_FEMALE      = D_GRA "br13_female";
const char *IMG_BR13_MALE        = D_GRA "br13_male";
const char *IMG_BR14_EXPORT      = D_GRA "br14_export";
const char *IMG_BR15_FADE1       = D_GRA "br15_fade1";
const char *IMG_BR15_FADE2       = D_GRA "br15_fade2";
const char *IMG_BR15_FADE3       = D_GRA "br15_fade3";
const char *IMG_BR16_NBAR        = D_GRA "br16_nbar";
const char *IMG_BR1_EXPORT       = D_GRA "br1_export";
const char *IMG_BR2_EXPORT       = D_GRA "br2_export";
const char *IMG_BR3_EXPORT2      = D_GRA "br3_export2";
const char *IMG_BR4_EXPORT3      = D_GRA "br4_export3";
const char *IMG_BR5_EXPORT4      = D_GRA "br5_export4";
const char *IMG_BR6_OK           = D_GRA "br6_ok";
const char *IMG_BR7_E1           = D_GRA "br7_e1";
const char *IMG_BR7_E2           = D_GRA "br7_e2";
const char *IMG_BR8_EXPORT       = D_GRA "br8_export";
const char *IMG_BR9_EXPORT       = D_GRA "br9_export";
const char *IMG_BT1_EXPORT       = D_GRA "bt1_export";
const char *IMG_CS1_C            = D_GRA "cs1_c";
const char *IMG_CS2_C            = D_GRA "cs2_c";
const char *IMG_CS3_C            = D_GRA "cs3_c";
const char *IMG_CS4_C            = D_GRA "cs4_c";
const char *IMG_CT1_EXPORT       = D_GRA "ct1_export";
const char *IMG_CT2_EXPORT       = D_GRA "ct2_export";
const char *IMG_CT3_EXPORT       = D_GRA "ct3_export";
const char *IMG_CT4_EXPORT       = D_GRA "ct4_export";
const char *IMG_EN1_DOOR1        = D_GRA "en1_door1";
const char *IMG_EN1_DOOR2        = D_GRA "en1_door2";
const char *IMG_EN1_HALL         = D_GRA "en1_hall";
const char *IMG_EN1_STARS        = D_GRA "en1_stars";
const char *IMG_EN1_WELCOME      = D_GRA "en1_welcome";
const char *IMG_EN2_SCAPE        = D_GRA "en2_scape";
const char *IMG_EN2_STS1         = D_GRA "en2_sts1";
const char *IMG_EN2_STS2         = D_GRA "en2_sts2";
const char *IMG_EN2_STS3         = D_GRA "en2_sts3";
const char *IMG_EN2_STS4         = D_GRA "en2_sts4";
const char *IMG_EN2_STS5         = D_GRA "en2_sts5";
const char *IMG_EN2_STS6         = D_GRA "en2_sts6";
const char *IMG_EN2_STS7         = D_GRA "en2_sts7";
const char *IMG_EN2_STS8         = D_GRA "en2_sts8";
const char *IMG_EN2_STS9         = D_GRA "en2_sts9";
const char *IMG_EN2_THEEND       = D_GRA "en2_theend";
const char *IMG_EQ1_TR0          = D_GRA "eq1_tr0";
const char *IMG_EQ1_TR1          = D_GRA "eq1_tr1";
const char *IMG_EQ1_TR2          = D_GRA "eq1_tr2";
const char *IMG_EQ1_TR3          = D_GRA "eq1_tr3";
const char *IMG_EQ1_TR4          = D_GRA "eq1_tr4";
const char *IMG_EQ1_TR5          = D_GRA "eq1_tr5";
const char *IMG_EQ1_TR6          = D_GRA "eq1_tr6";
const char *IMG_EQ1_TR7          = D_GRA "eq1_tr7";
const char *IMG_FL1_TR0          = D_GRA "fl1_tr0";
const char *IMG_FL1_TR1          = D_GRA "fl1_tr1";
const char *IMG_FL1_TR2          = D_GRA "fl1_tr2";
const char *IMG_FL1_TR3          = D_GRA "fl1_tr3";
const char *IMG_FL1_TR4          = D_GRA "fl1_tr4";
const char *IMG_FL1_TR6          = D_GRA "fl1_tr6";
const char *IMG_FL1_TR7          = D_GRA "fl1_tr7";
const char *IMG_GF1_25           = D_GRA "gf1_25";
const char *IMG_GF1_30           = D_GRA "gf1_30";
const char *IMG_GF1_30X          = D_GRA "gf1_30x";
const char *IMG_GF1_31           = D_GRA "gf1_31";
const char *IMG_GF1_31X          = D_GRA "gf1_31x";
const char *IMG_GF1_PART00       = D_GRA "gf1_part00";
const char *IMG_GF1_PART010      = D_GRA "gf1_part010";
const char *IMG_GF1_PART011      = D_GRA "gf1_part011";
const char *IMG_GF1_PART01       = D_GRA "gf1_part01";
const char *IMG_GF1_PART02       = D_GRA "gf1_part02";
const char *IMG_GF1_PART03       = D_GRA "gf1_part03";
const char *IMG_GF1_PART04       = D_GRA "gf1_part04";
const char *IMG_GF1_PART05       = D_GRA "gf1_part05";
const char *IMG_GF1_PART06       = D_GRA "gf1_part06";
const char *IMG_GF1_PART07       = D_GRA "gf1_part07";
const char *IMG_GF1_PART08       = D_GRA "gf1_part08";
const char *IMG_GF1_PART09       = D_GRA "gf1_part09";
const char *IMG_GF1_PART100      = D_GRA "gf1_part100";
const char *IMG_GF1_PART1010     = D_GRA "gf1_part1010";
const char *IMG_GF1_PART1011     = D_GRA "gf1_part1011";
const char *IMG_GF1_PART101      = D_GRA "gf1_part101";
const char *IMG_GF1_PART102      = D_GRA "gf1_part102";
const char *IMG_GF1_PART103      = D_GRA "gf1_part103";
const char *IMG_GF1_PART104      = D_GRA "gf1_part104";
const char *IMG_GF1_PART105      = D_GRA "gf1_part105";
const char *IMG_GF1_PART106      = D_GRA "gf1_part106";
const char *IMG_GF1_PART107      = D_GRA "gf1_part107";
const char *IMG_GF1_PART108      = D_GRA "gf1_part108";
const char *IMG_GF1_PART109      = D_GRA "gf1_part109";
const char *IMG_GF1_PART10       = D_GRA "gf1_part10";
const char *IMG_GF1_PART110      = D_GRA "gf1_part110";
const char *IMG_GF1_PART1110     = D_GRA "gf1_part1110";
const char *IMG_GF1_PART1111     = D_GRA "gf1_part1111";
const char *IMG_GF1_PART111      = D_GRA "gf1_part111";
const char *IMG_GF1_PART112      = D_GRA "gf1_part112";
const char *IMG_GF1_PART113      = D_GRA "gf1_part113";
const char *IMG_GF1_PART114      = D_GRA "gf1_part114";
const char *IMG_GF1_PART115      = D_GRA "gf1_part115";
const char *IMG_GF1_PART116      = D_GRA "gf1_part116";
const char *IMG_GF1_PART117      = D_GRA "gf1_part117";
const char *IMG_GF1_PART118      = D_GRA "gf1_part118";
const char *IMG_GF1_PART119      = D_GRA "gf1_part119";
const char *IMG_GF1_PART11       = D_GRA "gf1_part11";
const char *IMG_GF1_PART120      = D_GRA "gf1_part120";
const char *IMG_GF1_PART1210     = D_GRA "gf1_part1210";
const char *IMG_GF1_PART1211     = D_GRA "gf1_part1211";
const char *IMG_GF1_PART121      = D_GRA "gf1_part121";
const char *IMG_GF1_PART122      = D_GRA "gf1_part122";
const char *IMG_GF1_PART123      = D_GRA "gf1_part123";
const char *IMG_GF1_PART124      = D_GRA "gf1_part124";
const char *IMG_GF1_PART125      = D_GRA "gf1_part125";
const char *IMG_GF1_PART126      = D_GRA "gf1_part126";
const char *IMG_GF1_PART127      = D_GRA "gf1_part127";
const char *IMG_GF1_PART128      = D_GRA "gf1_part128";
const char *IMG_GF1_PART129      = D_GRA "gf1_part129";
const char *IMG_GF1_PART12       = D_GRA "gf1_part12";
const char *IMG_GF1_PART130      = D_GRA "gf1_part130";
const char *IMG_GF1_PART1310     = D_GRA "gf1_part1310";
const char *IMG_GF1_PART1311     = D_GRA "gf1_part1311";
const char *IMG_GF1_PART131      = D_GRA "gf1_part131";
const char *IMG_GF1_PART132      = D_GRA "gf1_part132";
const char *IMG_GF1_PART133      = D_GRA "gf1_part133";
const char *IMG_GF1_PART134      = D_GRA "gf1_part134";
const char *IMG_GF1_PART135      = D_GRA "gf1_part135";
const char *IMG_GF1_PART136      = D_GRA "gf1_part136";
const char *IMG_GF1_PART137      = D_GRA "gf1_part137";
const char *IMG_GF1_PART138      = D_GRA "gf1_part138";
const char *IMG_GF1_PART139      = D_GRA "gf1_part139";
const char *IMG_GF1_PART13       = D_GRA "gf1_part13";
const char *IMG_GF1_PART140      = D_GRA "gf1_part140";
const char *IMG_GF1_PART1410     = D_GRA "gf1_part1410";
const char *IMG_GF1_PART1411     = D_GRA "gf1_part1411";
const char *IMG_GF1_PART141      = D_GRA "gf1_part141";
const char *IMG_GF1_PART142      = D_GRA "gf1_part142";
const char *IMG_GF1_PART143      = D_GRA "gf1_part143";
const char *IMG_GF1_PART144      = D_GRA "gf1_part144";
const char *IMG_GF1_PART145      = D_GRA "gf1_part145";
const char *IMG_GF1_PART146      = D_GRA "gf1_part146";
const char *IMG_GF1_PART147      = D_GRA "gf1_part147";
const char *IMG_GF1_PART148      = D_GRA "gf1_part148";
const char *IMG_GF1_PART149      = D_GRA "gf1_part149";
const char *IMG_GF1_PART14       = D_GRA "gf1_part14";
const char *IMG_GF1_PART150      = D_GRA "gf1_part150";
const char *IMG_GF1_PART1510     = D_GRA "gf1_part1510";
const char *IMG_GF1_PART1511     = D_GRA "gf1_part1511";
const char *IMG_GF1_PART151      = D_GRA "gf1_part151";
const char *IMG_GF1_PART152      = D_GRA "gf1_part152";
const char *IMG_GF1_PART153      = D_GRA "gf1_part153";
const char *IMG_GF1_PART154      = D_GRA "gf1_part154";
const char *IMG_GF1_PART155      = D_GRA "gf1_part155";
const char *IMG_GF1_PART156      = D_GRA "gf1_part156";
const char *IMG_GF1_PART157      = D_GRA "gf1_part157";
const char *IMG_GF1_PART158      = D_GRA "gf1_part158";
const char *IMG_GF1_PART159      = D_GRA "gf1_part159";
const char *IMG_GF1_PART15       = D_GRA "gf1_part15";
const char *IMG_GF1_PART16       = D_GRA "gf1_part16";
const char *IMG_GF1_PART17       = D_GRA "gf1_part17";
const char *IMG_GF1_PART18       = D_GRA "gf1_part18";
const char *IMG_GF1_PART19       = D_GRA "gf1_part19";
const char *IMG_GF1_PART20       = D_GRA "gf1_part20";
const char *IMG_GF1_PART210      = D_GRA "gf1_part210";
const char *IMG_GF1_PART211      = D_GRA "gf1_part211";
const char *IMG_GF1_PART21       = D_GRA "gf1_part21";
const char *IMG_GF1_PART22       = D_GRA "gf1_part22";
const char *IMG_GF1_PART23       = D_GRA "gf1_part23";
const char *IMG_GF1_PART24       = D_GRA "gf1_part24";
const char *IMG_GF1_PART25       = D_GRA "gf1_part25";
const char *IMG_GF1_PART26       = D_GRA "gf1_part26";
const char *IMG_GF1_PART27       = D_GRA "gf1_part27";
const char *IMG_GF1_PART28       = D_GRA "gf1_part28";
const char *IMG_GF1_PART29       = D_GRA "gf1_part29";
const char *IMG_GF1_PART30       = D_GRA "gf1_part30";
const char *IMG_GF1_PART310      = D_GRA "gf1_part310";
const char *IMG_GF1_PART311      = D_GRA "gf1_part311";
const char *IMG_GF1_PART31       = D_GRA "gf1_part31";
const char *IMG_GF1_PART32       = D_GRA "gf1_part32";
const char *IMG_GF1_PART33       = D_GRA "gf1_part33";
const char *IMG_GF1_PART34       = D_GRA "gf1_part34";
const char *IMG_GF1_PART35       = D_GRA "gf1_part35";
const char *IMG_GF1_PART36       = D_GRA "gf1_part36";
const char *IMG_GF1_PART37       = D_GRA "gf1_part37";
const char *IMG_GF1_PART38       = D_GRA "gf1_part38";
const char *IMG_GF1_PART39       = D_GRA "gf1_part39";
const char *IMG_GF1_PART40       = D_GRA "gf1_part40";
const char *IMG_GF1_PART410      = D_GRA "gf1_part410";
const char *IMG_GF1_PART411      = D_GRA "gf1_part411";
const char *IMG_GF1_PART41       = D_GRA "gf1_part41";
const char *IMG_GF1_PART42       = D_GRA "gf1_part42";
const char *IMG_GF1_PART43       = D_GRA "gf1_part43";
const char *IMG_GF1_PART44       = D_GRA "gf1_part44";
const char *IMG_GF1_PART45       = D_GRA "gf1_part45";
const char *IMG_GF1_PART46       = D_GRA "gf1_part46";
const char *IMG_GF1_PART47       = D_GRA "gf1_part47";
const char *IMG_GF1_PART48       = D_GRA "gf1_part48";
const char *IMG_GF1_PART49       = D_GRA "gf1_part49";
const char *IMG_GF1_PART50       = D_GRA "gf1_part50";
const char *IMG_GF1_PART510      = D_GRA "gf1_part510";
const char *IMG_GF1_PART511      = D_GRA "gf1_part511";
const char *IMG_GF1_PART51       = D_GRA "gf1_part51";
const char *IMG_GF1_PART52       = D_GRA "gf1_part52";
const char *IMG_GF1_PART53       = D_GRA "gf1_part53";
const char *IMG_GF1_PART54       = D_GRA "gf1_part54";
const char *IMG_GF1_PART55       = D_GRA "gf1_part55";
const char *IMG_GF1_PART56       = D_GRA "gf1_part56";
const char *IMG_GF1_PART57       = D_GRA "gf1_part57";
const char *IMG_GF1_PART58       = D_GRA "gf1_part58";
const char *IMG_GF1_PART59       = D_GRA "gf1_part59";
const char *IMG_GF1_PART60       = D_GRA "gf1_part60";
const char *IMG_GF1_PART610      = D_GRA "gf1_part610";
const char *IMG_GF1_PART611      = D_GRA "gf1_part611";
const char *IMG_GF1_PART61       = D_GRA "gf1_part61";
const char *IMG_GF1_PART62       = D_GRA "gf1_part62";
const char *IMG_GF1_PART63       = D_GRA "gf1_part63";
const char *IMG_GF1_PART64       = D_GRA "gf1_part64";
const char *IMG_GF1_PART65       = D_GRA "gf1_part65";
const char *IMG_GF1_PART66       = D_GRA "gf1_part66";
const char *IMG_GF1_PART67       = D_GRA "gf1_part67";
const char *IMG_GF1_PART68       = D_GRA "gf1_part68";
const char *IMG_GF1_PART69       = D_GRA "gf1_part69";
const char *IMG_GF1_PART70       = D_GRA "gf1_part70";
const char *IMG_GF1_PART710      = D_GRA "gf1_part710";
const char *IMG_GF1_PART711      = D_GRA "gf1_part711";
const char *IMG_GF1_PART71       = D_GRA "gf1_part71";
const char *IMG_GF1_PART72       = D_GRA "gf1_part72";
const char *IMG_GF1_PART73       = D_GRA "gf1_part73";
const char *IMG_GF1_PART74       = D_GRA "gf1_part74";
const char *IMG_GF1_PART75       = D_GRA "gf1_part75";
const char *IMG_GF1_PART76       = D_GRA "gf1_part76";
const char *IMG_GF1_PART77       = D_GRA "gf1_part77";
const char *IMG_GF1_PART78       = D_GRA "gf1_part78";
const char *IMG_GF1_PART79       = D_GRA "gf1_part79";
const char *IMG_GF1_PART80       = D_GRA "gf1_part80";
const char *IMG_GF1_PART810      = D_GRA "gf1_part810";
const char *IMG_GF1_PART811      = D_GRA "gf1_part811";
const char *IMG_GF1_PART81       = D_GRA "gf1_part81";
const char *IMG_GF1_PART82       = D_GRA "gf1_part82";
const char *IMG_GF1_PART83       = D_GRA "gf1_part83";
const char *IMG_GF1_PART84       = D_GRA "gf1_part84";
const char *IMG_GF1_PART85       = D_GRA "gf1_part85";
const char *IMG_GF1_PART86       = D_GRA "gf1_part86";
const char *IMG_GF1_PART87       = D_GRA "gf1_part87";
const char *IMG_GF1_PART88       = D_GRA "gf1_part88";
const char *IMG_GF1_PART89       = D_GRA "gf1_part89";
const char *IMG_GF1_PART90       = D_GRA "gf1_part90";
const char *IMG_GF1_PART910      = D_GRA "gf1_part910";
const char *IMG_GF1_PART911      = D_GRA "gf1_part911";
const char *IMG_GF1_PART91       = D_GRA "gf1_part91";
const char *IMG_GF1_PART92       = D_GRA "gf1_part92";
const char *IMG_GF1_PART93       = D_GRA "gf1_part93";
const char *IMG_GF1_PART94       = D_GRA "gf1_part94";
const char *IMG_GF1_PART95       = D_GRA "gf1_part95";
const char *IMG_GF1_PART96       = D_GRA "gf1_part96";
const char *IMG_GF1_PART97       = D_GRA "gf1_part97";
const char *IMG_GF1_PART98       = D_GRA "gf1_part98";
const char *IMG_GF1_PART99       = D_GRA "gf1_part99";
const char *IMG_GF2_25           = D_GRA "gf2_25";
const char *IMG_GF2_30           = D_GRA "gf2_30";
const char *IMG_GF2_30X          = D_GRA "gf2_30x";
const char *IMG_GF2_31           = D_GRA "gf2_31";
const char *IMG_GF2_31X          = D_GRA "gf2_31x";
const char *IMG_GF2_MA1          = D_GRA "gf2_ma1";
const char *IMG_GF2_MA2          = D_GRA "gf2_ma2";
const char *IMG_GF2_MA3          = D_GRA "gf2_ma3";
const char *IMG_GF2_MA4          = D_GRA "gf2_ma4";
const char *IMG_GF2_MA5          = D_GRA "gf2_ma5";
const char *IMG_GF2_PART00       = D_GRA "gf2_part00";
const char *IMG_GF2_PART010      = D_GRA "gf2_part010";
const char *IMG_GF2_PART011      = D_GRA "gf2_part011";
const char *IMG_GF2_PART01       = D_GRA "gf2_part01";
const char *IMG_GF2_PART02       = D_GRA "gf2_part02";
const char *IMG_GF2_PART03       = D_GRA "gf2_part03";
const char *IMG_GF2_PART04       = D_GRA "gf2_part04";
const char *IMG_GF2_PART05       = D_GRA "gf2_part05";
const char *IMG_GF2_PART06       = D_GRA "gf2_part06";
const char *IMG_GF2_PART07       = D_GRA "gf2_part07";
const char *IMG_GF2_PART08       = D_GRA "gf2_part08";
const char *IMG_GF2_PART09       = D_GRA "gf2_part09";
const char *IMG_GF2_PART100      = D_GRA "gf2_part100";
const char *IMG_GF2_PART1010     = D_GRA "gf2_part1010";
const char *IMG_GF2_PART1011     = D_GRA "gf2_part1011";
const char *IMG_GF2_PART101      = D_GRA "gf2_part101";
const char *IMG_GF2_PART102      = D_GRA "gf2_part102";
const char *IMG_GF2_PART103      = D_GRA "gf2_part103";
const char *IMG_GF2_PART104      = D_GRA "gf2_part104";
const char *IMG_GF2_PART105      = D_GRA "gf2_part105";
const char *IMG_GF2_PART106      = D_GRA "gf2_part106";
const char *IMG_GF2_PART107      = D_GRA "gf2_part107";
const char *IMG_GF2_PART108      = D_GRA "gf2_part108";
const char *IMG_GF2_PART109      = D_GRA "gf2_part109";
const char *IMG_GF2_PART10       = D_GRA "gf2_part10";
const char *IMG_GF2_PART110      = D_GRA "gf2_part110";
const char *IMG_GF2_PART1110     = D_GRA "gf2_part1110";
const char *IMG_GF2_PART1111     = D_GRA "gf2_part1111";
const char *IMG_GF2_PART111      = D_GRA "gf2_part111";
const char *IMG_GF2_PART112      = D_GRA "gf2_part112";
const char *IMG_GF2_PART113      = D_GRA "gf2_part113";
const char *IMG_GF2_PART114      = D_GRA "gf2_part114";
const char *IMG_GF2_PART115      = D_GRA "gf2_part115";
const char *IMG_GF2_PART116      = D_GRA "gf2_part116";
const char *IMG_GF2_PART117      = D_GRA "gf2_part117";
const char *IMG_GF2_PART118      = D_GRA "gf2_part118";
const char *IMG_GF2_PART119      = D_GRA "gf2_part119";
const char *IMG_GF2_PART11       = D_GRA "gf2_part11";
const char *IMG_GF2_PART120      = D_GRA "gf2_part120";
const char *IMG_GF2_PART1210     = D_GRA "gf2_part1210";
const char *IMG_GF2_PART1211     = D_GRA "gf2_part1211";
const char *IMG_GF2_PART121      = D_GRA "gf2_part121";
const char *IMG_GF2_PART122      = D_GRA "gf2_part122";
const char *IMG_GF2_PART123      = D_GRA "gf2_part123";
const char *IMG_GF2_PART124      = D_GRA "gf2_part124";
const char *IMG_GF2_PART125      = D_GRA "gf2_part125";
const char *IMG_GF2_PART126      = D_GRA "gf2_part126";
const char *IMG_GF2_PART127      = D_GRA "gf2_part127";
const char *IMG_GF2_PART128      = D_GRA "gf2_part128";
const char *IMG_GF2_PART129      = D_GRA "gf2_part129";
const char *IMG_GF2_PART12       = D_GRA "gf2_part12";
const char *IMG_GF2_PART130      = D_GRA "gf2_part130";
const char *IMG_GF2_PART1310     = D_GRA "gf2_part1310";
const char *IMG_GF2_PART1311     = D_GRA "gf2_part1311";
const char *IMG_GF2_PART131      = D_GRA "gf2_part131";
const char *IMG_GF2_PART132      = D_GRA "gf2_part132";
const char *IMG_GF2_PART133      = D_GRA "gf2_part133";
const char *IMG_GF2_PART134      = D_GRA "gf2_part134";
const char *IMG_GF2_PART135      = D_GRA "gf2_part135";
const char *IMG_GF2_PART136      = D_GRA "gf2_part136";
const char *IMG_GF2_PART137      = D_GRA "gf2_part137";
const char *IMG_GF2_PART138      = D_GRA "gf2_part138";
const char *IMG_GF2_PART139      = D_GRA "gf2_part139";
const char *IMG_GF2_PART13       = D_GRA "gf2_part13";
const char *IMG_GF2_PART140      = D_GRA "gf2_part140";
const char *IMG_GF2_PART1410     = D_GRA "gf2_part1410";
const char *IMG_GF2_PART1411     = D_GRA "gf2_part1411";
const char *IMG_GF2_PART141      = D_GRA "gf2_part141";
const char *IMG_GF2_PART142      = D_GRA "gf2_part142";
const char *IMG_GF2_PART143      = D_GRA "gf2_part143";
const char *IMG_GF2_PART144      = D_GRA "gf2_part144";
const char *IMG_GF2_PART145      = D_GRA "gf2_part145";
const char *IMG_GF2_PART146      = D_GRA "gf2_part146";
const char *IMG_GF2_PART147      = D_GRA "gf2_part147";
const char *IMG_GF2_PART148      = D_GRA "gf2_part148";
const char *IMG_GF2_PART149      = D_GRA "gf2_part149";
const char *IMG_GF2_PART14       = D_GRA "gf2_part14";
const char *IMG_GF2_PART150      = D_GRA "gf2_part150";
const char *IMG_GF2_PART1510     = D_GRA "gf2_part1510";
const char *IMG_GF2_PART1511     = D_GRA "gf2_part1511";
const char *IMG_GF2_PART151      = D_GRA "gf2_part151";
const char *IMG_GF2_PART152      = D_GRA "gf2_part152";
const char *IMG_GF2_PART153      = D_GRA "gf2_part153";
const char *IMG_GF2_PART154      = D_GRA "gf2_part154";
const char *IMG_GF2_PART155      = D_GRA "gf2_part155";
const char *IMG_GF2_PART156      = D_GRA "gf2_part156";
const char *IMG_GF2_PART157      = D_GRA "gf2_part157";
const char *IMG_GF2_PART158      = D_GRA "gf2_part158";
const char *IMG_GF2_PART159      = D_GRA "gf2_part159";
const char *IMG_GF2_PART15       = D_GRA "gf2_part15";
const char *IMG_GF2_PART16       = D_GRA "gf2_part16";
const char *IMG_GF2_PART17       = D_GRA "gf2_part17";
const char *IMG_GF2_PART18       = D_GRA "gf2_part18";
const char *IMG_GF2_PART19       = D_GRA "gf2_part19";
const char *IMG_GF2_PART20       = D_GRA "gf2_part20";
const char *IMG_GF2_PART210      = D_GRA "gf2_part210";
const char *IMG_GF2_PART211      = D_GRA "gf2_part211";
const char *IMG_GF2_PART21       = D_GRA "gf2_part21";
const char *IMG_GF2_PART22       = D_GRA "gf2_part22";
const char *IMG_GF2_PART23       = D_GRA "gf2_part23";
const char *IMG_GF2_PART24       = D_GRA "gf2_part24";
const char *IMG_GF2_PART25       = D_GRA "gf2_part25";
const char *IMG_GF2_PART26       = D_GRA "gf2_part26";
const char *IMG_GF2_PART27       = D_GRA "gf2_part27";
const char *IMG_GF2_PART28       = D_GRA "gf2_part28";
const char *IMG_GF2_PART29       = D_GRA "gf2_part29";
const char *IMG_GF2_PART30       = D_GRA "gf2_part30";
const char *IMG_GF2_PART310      = D_GRA "gf2_part310";
const char *IMG_GF2_PART311      = D_GRA "gf2_part311";
const char *IMG_GF2_PART31       = D_GRA "gf2_part31";
const char *IMG_GF2_PART32       = D_GRA "gf2_part32";
const char *IMG_GF2_PART33       = D_GRA "gf2_part33";
const char *IMG_GF2_PART34       = D_GRA "gf2_part34";
const char *IMG_GF2_PART35       = D_GRA "gf2_part35";
const char *IMG_GF2_PART36       = D_GRA "gf2_part36";
const char *IMG_GF2_PART37       = D_GRA "gf2_part37";
const char *IMG_GF2_PART38       = D_GRA "gf2_part38";
const char *IMG_GF2_PART39       = D_GRA "gf2_part39";
const char *IMG_GF2_PART40       = D_GRA "gf2_part40";
const char *IMG_GF2_PART410      = D_GRA "gf2_part410";
const char *IMG_GF2_PART411      = D_GRA "gf2_part411";
const char *IMG_GF2_PART41       = D_GRA "gf2_part41";
const char *IMG_GF2_PART42       = D_GRA "gf2_part42";
const char *IMG_GF2_PART43       = D_GRA "gf2_part43";
const char *IMG_GF2_PART44       = D_GRA "gf2_part44";
const char *IMG_GF2_PART45       = D_GRA "gf2_part45";
const char *IMG_GF2_PART46       = D_GRA "gf2_part46";
const char *IMG_GF2_PART47       = D_GRA "gf2_part47";
const char *IMG_GF2_PART48       = D_GRA "gf2_part48";
const char *IMG_GF2_PART49       = D_GRA "gf2_part49";
const char *IMG_GF2_PART50       = D_GRA "gf2_part50";
const char *IMG_GF2_PART510      = D_GRA "gf2_part510";
const char *IMG_GF2_PART511      = D_GRA "gf2_part511";
const char *IMG_GF2_PART51       = D_GRA "gf2_part51";
const char *IMG_GF2_PART52       = D_GRA "gf2_part52";
const char *IMG_GF2_PART53       = D_GRA "gf2_part53";
const char *IMG_GF2_PART54       = D_GRA "gf2_part54";
const char *IMG_GF2_PART55       = D_GRA "gf2_part55";
const char *IMG_GF2_PART56       = D_GRA "gf2_part56";
const char *IMG_GF2_PART57       = D_GRA "gf2_part57";
const char *IMG_GF2_PART58       = D_GRA "gf2_part58";
const char *IMG_GF2_PART59       = D_GRA "gf2_part59";
const char *IMG_GF2_PART60       = D_GRA "gf2_part60";
const char *IMG_GF2_PART610      = D_GRA "gf2_part610";
const char *IMG_GF2_PART611      = D_GRA "gf2_part611";
const char *IMG_GF2_PART61       = D_GRA "gf2_part61";
const char *IMG_GF2_PART62       = D_GRA "gf2_part62";
const char *IMG_GF2_PART63       = D_GRA "gf2_part63";
const char *IMG_GF2_PART64       = D_GRA "gf2_part64";
const char *IMG_GF2_PART65       = D_GRA "gf2_part65";
const char *IMG_GF2_PART66       = D_GRA "gf2_part66";
const char *IMG_GF2_PART67       = D_GRA "gf2_part67";
const char *IMG_GF2_PART68       = D_GRA "gf2_part68";
const char *IMG_GF2_PART69       = D_GRA "gf2_part69";
const char *IMG_GF2_PART70       = D_GRA "gf2_part70";
const char *IMG_GF2_PART710      = D_GRA "gf2_part710";
const char *IMG_GF2_PART711      = D_GRA "gf2_part711";
const char *IMG_GF2_PART71       = D_GRA "gf2_part71";
const char *IMG_GF2_PART72       = D_GRA "gf2_part72";
const char *IMG_GF2_PART73       = D_GRA "gf2_part73";
const char *IMG_GF2_PART74       = D_GRA "gf2_part74";
const char *IMG_GF2_PART75       = D_GRA "gf2_part75";
const char *IMG_GF2_PART76       = D_GRA "gf2_part76";
const char *IMG_GF2_PART77       = D_GRA "gf2_part77";
const char *IMG_GF2_PART78       = D_GRA "gf2_part78";
const char *IMG_GF2_PART79       = D_GRA "gf2_part79";
const char *IMG_GF2_PART80       = D_GRA "gf2_part80";
const char *IMG_GF2_PART810      = D_GRA "gf2_part810";
const char *IMG_GF2_PART811      = D_GRA "gf2_part811";
const char *IMG_GF2_PART81       = D_GRA "gf2_part81";
const char *IMG_GF2_PART82       = D_GRA "gf2_part82";
const char *IMG_GF2_PART83       = D_GRA "gf2_part83";
const char *IMG_GF2_PART84       = D_GRA "gf2_part84";
const char *IMG_GF2_PART85       = D_GRA "gf2_part85";
const char *IMG_GF2_PART86       = D_GRA "gf2_part86";
const char *IMG_GF2_PART87       = D_GRA "gf2_part87";
const char *IMG_GF2_PART88       = D_GRA "gf2_part88";
const char *IMG_GF2_PART89       = D_GRA "gf2_part89";
const char *IMG_GF2_PART90       = D_GRA "gf2_part90";
const char *IMG_GF2_PART910      = D_GRA "gf2_part910";
const char *IMG_GF2_PART911      = D_GRA "gf2_part911";
const char *IMG_GF2_PART91       = D_GRA "gf2_part91";
const char *IMG_GF2_PART92       = D_GRA "gf2_part92";
const char *IMG_GF2_PART93       = D_GRA "gf2_part93";
const char *IMG_GF2_PART94       = D_GRA "gf2_part94";
const char *IMG_GF2_PART95       = D_GRA "gf2_part95";
const char *IMG_GF2_PART96       = D_GRA "gf2_part96";
const char *IMG_GF2_PART97       = D_GRA "gf2_part97";
const char *IMG_GF2_PART98       = D_GRA "gf2_part98";
const char *IMG_GF2_PART99       = D_GRA "gf2_part99";
const char *IMG_GF3_25           = D_GRA "gf3_25";
const char *IMG_GF3_30           = D_GRA "gf3_30";
const char *IMG_GF3_31           = D_GRA "gf3_31";
const char *IMG_GF3_PART00       = D_GRA "gf3_part00";
const char *IMG_GF3_PART010      = D_GRA "gf3_part010";
const char *IMG_GF3_PART011      = D_GRA "gf3_part011";
const char *IMG_GF3_PART01       = D_GRA "gf3_part01";
const char *IMG_GF3_PART02       = D_GRA "gf3_part02";
const char *IMG_GF3_PART03       = D_GRA "gf3_part03";
const char *IMG_GF3_PART04       = D_GRA "gf3_part04";
const char *IMG_GF3_PART05       = D_GRA "gf3_part05";
const char *IMG_GF3_PART06       = D_GRA "gf3_part06";
const char *IMG_GF3_PART07       = D_GRA "gf3_part07";
const char *IMG_GF3_PART08       = D_GRA "gf3_part08";
const char *IMG_GF3_PART09       = D_GRA "gf3_part09";
const char *IMG_GF3_PART100      = D_GRA "gf3_part100";
const char *IMG_GF3_PART1010     = D_GRA "gf3_part1010";
const char *IMG_GF3_PART1011     = D_GRA "gf3_part1011";
const char *IMG_GF3_PART101      = D_GRA "gf3_part101";
const char *IMG_GF3_PART102      = D_GRA "gf3_part102";
const char *IMG_GF3_PART103      = D_GRA "gf3_part103";
const char *IMG_GF3_PART104      = D_GRA "gf3_part104";
const char *IMG_GF3_PART105      = D_GRA "gf3_part105";
const char *IMG_GF3_PART106      = D_GRA "gf3_part106";
const char *IMG_GF3_PART107      = D_GRA "gf3_part107";
const char *IMG_GF3_PART108      = D_GRA "gf3_part108";
const char *IMG_GF3_PART109      = D_GRA "gf3_part109";
const char *IMG_GF3_PART10       = D_GRA "gf3_part10";
const char *IMG_GF3_PART110      = D_GRA "gf3_part110";
const char *IMG_GF3_PART1110     = D_GRA "gf3_part1110";
const char *IMG_GF3_PART1111     = D_GRA "gf3_part1111";
const char *IMG_GF3_PART111      = D_GRA "gf3_part111";
const char *IMG_GF3_PART112      = D_GRA "gf3_part112";
const char *IMG_GF3_PART113      = D_GRA "gf3_part113";
const char *IMG_GF3_PART114      = D_GRA "gf3_part114";
const char *IMG_GF3_PART115      = D_GRA "gf3_part115";
const char *IMG_GF3_PART116      = D_GRA "gf3_part116";
const char *IMG_GF3_PART117      = D_GRA "gf3_part117";
const char *IMG_GF3_PART118      = D_GRA "gf3_part118";
const char *IMG_GF3_PART119      = D_GRA "gf3_part119";
const char *IMG_GF3_PART11       = D_GRA "gf3_part11";
const char *IMG_GF3_PART120      = D_GRA "gf3_part120";
const char *IMG_GF3_PART1210     = D_GRA "gf3_part1210";
const char *IMG_GF3_PART1211     = D_GRA "gf3_part1211";
const char *IMG_GF3_PART121      = D_GRA "gf3_part121";
const char *IMG_GF3_PART122      = D_GRA "gf3_part122";
const char *IMG_GF3_PART123      = D_GRA "gf3_part123";
const char *IMG_GF3_PART124      = D_GRA "gf3_part124";
const char *IMG_GF3_PART125      = D_GRA "gf3_part125";
const char *IMG_GF3_PART126      = D_GRA "gf3_part126";
const char *IMG_GF3_PART127      = D_GRA "gf3_part127";
const char *IMG_GF3_PART128      = D_GRA "gf3_part128";
const char *IMG_GF3_PART129      = D_GRA "gf3_part129";
const char *IMG_GF3_PART12       = D_GRA "gf3_part12";
const char *IMG_GF3_PART130      = D_GRA "gf3_part130";
const char *IMG_GF3_PART1310     = D_GRA "gf3_part1310";
const char *IMG_GF3_PART1311     = D_GRA "gf3_part1311";
const char *IMG_GF3_PART131      = D_GRA "gf3_part131";
const char *IMG_GF3_PART132      = D_GRA "gf3_part132";
const char *IMG_GF3_PART133      = D_GRA "gf3_part133";
const char *IMG_GF3_PART134      = D_GRA "gf3_part134";
const char *IMG_GF3_PART135      = D_GRA "gf3_part135";
const char *IMG_GF3_PART136      = D_GRA "gf3_part136";
const char *IMG_GF3_PART137      = D_GRA "gf3_part137";
const char *IMG_GF3_PART138      = D_GRA "gf3_part138";
const char *IMG_GF3_PART139      = D_GRA "gf3_part139";
const char *IMG_GF3_PART13       = D_GRA "gf3_part13";
const char *IMG_GF3_PART140      = D_GRA "gf3_part140";
const char *IMG_GF3_PART1410     = D_GRA "gf3_part1410";
const char *IMG_GF3_PART1411     = D_GRA "gf3_part1411";
const char *IMG_GF3_PART141      = D_GRA "gf3_part141";
const char *IMG_GF3_PART142      = D_GRA "gf3_part142";
const char *IMG_GF3_PART143      = D_GRA "gf3_part143";
const char *IMG_GF3_PART144      = D_GRA "gf3_part144";
const char *IMG_GF3_PART145      = D_GRA "gf3_part145";
const char *IMG_GF3_PART146      = D_GRA "gf3_part146";
const char *IMG_GF3_PART147      = D_GRA "gf3_part147";
const char *IMG_GF3_PART148      = D_GRA "gf3_part148";
const char *IMG_GF3_PART149      = D_GRA "gf3_part149";
const char *IMG_GF3_PART14       = D_GRA "gf3_part14";
const char *IMG_GF3_PART150      = D_GRA "gf3_part150";
const char *IMG_GF3_PART1510     = D_GRA "gf3_part1510";
const char *IMG_GF3_PART1511     = D_GRA "gf3_part1511";
const char *IMG_GF3_PART151      = D_GRA "gf3_part151";
const char *IMG_GF3_PART152      = D_GRA "gf3_part152";
const char *IMG_GF3_PART153      = D_GRA "gf3_part153";
const char *IMG_GF3_PART154      = D_GRA "gf3_part154";
const char *IMG_GF3_PART155      = D_GRA "gf3_part155";
const char *IMG_GF3_PART156      = D_GRA "gf3_part156";
const char *IMG_GF3_PART157      = D_GRA "gf3_part157";
const char *IMG_GF3_PART158      = D_GRA "gf3_part158";
const char *IMG_GF3_PART159      = D_GRA "gf3_part159";
const char *IMG_GF3_PART15       = D_GRA "gf3_part15";
const char *IMG_GF3_PART16       = D_GRA "gf3_part16";
const char *IMG_GF3_PART17       = D_GRA "gf3_part17";
const char *IMG_GF3_PART18       = D_GRA "gf3_part18";
const char *IMG_GF3_PART19       = D_GRA "gf3_part19";
const char *IMG_GF3_PART20       = D_GRA "gf3_part20";
const char *IMG_GF3_PART210      = D_GRA "gf3_part210";
const char *IMG_GF3_PART211      = D_GRA "gf3_part211";
const char *IMG_GF3_PART21       = D_GRA "gf3_part21";
const char *IMG_GF3_PART22       = D_GRA "gf3_part22";
const char *IMG_GF3_PART23       = D_GRA "gf3_part23";
const char *IMG_GF3_PART24       = D_GRA "gf3_part24";
const char *IMG_GF3_PART25       = D_GRA "gf3_part25";
const char *IMG_GF3_PART26       = D_GRA "gf3_part26";
const char *IMG_GF3_PART27       = D_GRA "gf3_part27";
const char *IMG_GF3_PART28       = D_GRA "gf3_part28";
const char *IMG_GF3_PART29       = D_GRA "gf3_part29";
const char *IMG_GF3_PART30       = D_GRA "gf3_part30";
const char *IMG_GF3_PART310      = D_GRA "gf3_part310";
const char *IMG_GF3_PART311      = D_GRA "gf3_part311";
const char *IMG_GF3_PART31       = D_GRA "gf3_part31";
const char *IMG_GF3_PART32       = D_GRA "gf3_part32";
const char *IMG_GF3_PART33       = D_GRA "gf3_part33";
const char *IMG_GF3_PART34       = D_GRA "gf3_part34";
const char *IMG_GF3_PART35       = D_GRA "gf3_part35";
const char *IMG_GF3_PART36       = D_GRA "gf3_part36";
const char *IMG_GF3_PART37       = D_GRA "gf3_part37";
const char *IMG_GF3_PART38       = D_GRA "gf3_part38";
const char *IMG_GF3_PART39       = D_GRA "gf3_part39";
const char *IMG_GF3_PART40       = D_GRA "gf3_part40";
const char *IMG_GF3_PART410      = D_GRA "gf3_part410";
const char *IMG_GF3_PART411      = D_GRA "gf3_part411";
const char *IMG_GF3_PART41       = D_GRA "gf3_part41";
const char *IMG_GF3_PART42       = D_GRA "gf3_part42";
const char *IMG_GF3_PART43       = D_GRA "gf3_part43";
const char *IMG_GF3_PART44       = D_GRA "gf3_part44";
const char *IMG_GF3_PART45       = D_GRA "gf3_part45";
const char *IMG_GF3_PART46       = D_GRA "gf3_part46";
const char *IMG_GF3_PART47       = D_GRA "gf3_part47";
const char *IMG_GF3_PART48       = D_GRA "gf3_part48";
const char *IMG_GF3_PART49       = D_GRA "gf3_part49";
const char *IMG_GF3_PART50       = D_GRA "gf3_part50";
const char *IMG_GF3_PART510      = D_GRA "gf3_part510";
const char *IMG_GF3_PART511      = D_GRA "gf3_part511";
const char *IMG_GF3_PART51       = D_GRA "gf3_part51";
const char *IMG_GF3_PART52       = D_GRA "gf3_part52";
const char *IMG_GF3_PART53       = D_GRA "gf3_part53";
const char *IMG_GF3_PART54       = D_GRA "gf3_part54";
const char *IMG_GF3_PART55       = D_GRA "gf3_part55";
const char *IMG_GF3_PART56       = D_GRA "gf3_part56";
const char *IMG_GF3_PART57       = D_GRA "gf3_part57";
const char *IMG_GF3_PART58       = D_GRA "gf3_part58";
const char *IMG_GF3_PART59       = D_GRA "gf3_part59";
const char *IMG_GF3_PART60       = D_GRA "gf3_part60";
const char *IMG_GF3_PART610      = D_GRA "gf3_part610";
const char *IMG_GF3_PART611      = D_GRA "gf3_part611";
const char *IMG_GF3_PART61       = D_GRA "gf3_part61";
const char *IMG_GF3_PART62       = D_GRA "gf3_part62";
const char *IMG_GF3_PART63       = D_GRA "gf3_part63";
const char *IMG_GF3_PART64       = D_GRA "gf3_part64";
const char *IMG_GF3_PART65       = D_GRA "gf3_part65";
const char *IMG_GF3_PART66       = D_GRA "gf3_part66";
const char *IMG_GF3_PART67       = D_GRA "gf3_part67";
const char *IMG_GF3_PART68       = D_GRA "gf3_part68";
const char *IMG_GF3_PART69       = D_GRA "gf3_part69";
const char *IMG_GF3_PART70       = D_GRA "gf3_part70";
const char *IMG_GF3_PART710      = D_GRA "gf3_part710";
const char *IMG_GF3_PART711      = D_GRA "gf3_part711";
const char *IMG_GF3_PART71       = D_GRA "gf3_part71";
const char *IMG_GF3_PART72       = D_GRA "gf3_part72";
const char *IMG_GF3_PART73       = D_GRA "gf3_part73";
const char *IMG_GF3_PART74       = D_GRA "gf3_part74";
const char *IMG_GF3_PART75       = D_GRA "gf3_part75";
const char *IMG_GF3_PART76       = D_GRA "gf3_part76";
const char *IMG_GF3_PART77       = D_GRA "gf3_part77";
const char *IMG_GF3_PART78       = D_GRA "gf3_part78";
const char *IMG_GF3_PART79       = D_GRA "gf3_part79";
const char *IMG_GF3_PART80       = D_GRA "gf3_part80";
const char *IMG_GF3_PART810      = D_GRA "gf3_part810";
const char *IMG_GF3_PART811      = D_GRA "gf3_part811";
const char *IMG_GF3_PART81       = D_GRA "gf3_part81";
const char *IMG_GF3_PART82       = D_GRA "gf3_part82";
const char *IMG_GF3_PART83       = D_GRA "gf3_part83";
const char *IMG_GF3_PART84       = D_GRA "gf3_part84";
const char *IMG_GF3_PART85       = D_GRA "gf3_part85";
const char *IMG_GF3_PART86       = D_GRA "gf3_part86";
const char *IMG_GF3_PART87       = D_GRA "gf3_part87";
const char *IMG_GF3_PART88       = D_GRA "gf3_part88";
const char *IMG_GF3_PART89       = D_GRA "gf3_part89";
const char *IMG_GF3_PART90       = D_GRA "gf3_part90";
const char *IMG_GF3_PART910      = D_GRA "gf3_part910";
const char *IMG_GF3_PART911      = D_GRA "gf3_part911";
const char *IMG_GF3_PART91       = D_GRA "gf3_part91";
const char *IMG_GF3_PART92       = D_GRA "gf3_part92";
const char *IMG_GF3_PART93       = D_GRA "gf3_part93";
const char *IMG_GF3_PART94       = D_GRA "gf3_part94";
const char *IMG_GF3_PART95       = D_GRA "gf3_part95";
const char *IMG_GF3_PART96       = D_GRA "gf3_part96";
const char *IMG_GF3_PART97       = D_GRA "gf3_part97";
const char *IMG_GF3_PART98       = D_GRA "gf3_part98";
const char *IMG_GF3_PART99       = D_GRA "gf3_part99";
const char *IMG_GF4_10           = D_GRA "gf4_10";
const char *IMG_GF4_11           = D_GRA "gf4_11";
const char *IMG_GF4_1_2          = D_GRA "gf4_1-2";
const char *IMG_GF4_12           = D_GRA "gf4_12";
const char *IMG_GF4_13           = D_GRA "gf4_13";
const char *IMG_GF4_14           = D_GRA "gf4_14";
const char *IMG_GF4_15           = D_GRA "gf4_15";
const char *IMG_GF4_16           = D_GRA "gf4_16";
const char *IMG_GF4_17           = D_GRA "gf4_17";
const char *IMG_GF4_18           = D_GRA "gf4_18";
const char *IMG_GF4_19           = D_GRA "gf4_19";
const char *IMG_GF4_1            = D_GRA "gf4_1";
const char *IMG_GF4_20           = D_GRA "gf4_20";
const char *IMG_GF4_2_1          = D_GRA "gf4_2-1";
const char *IMG_GF4_21           = D_GRA "gf4_21";
const char *IMG_GF4_2_2          = D_GRA "gf4_2-2";
const char *IMG_GF4_22           = D_GRA "gf4_22";
const char *IMG_GF4_23           = D_GRA "gf4_23";
const char *IMG_GF4_24           = D_GRA "gf4_24";
const char *IMG_GF4_25_2         = D_GRA "gf4_25-2";
const char *IMG_GF4_26           = D_GRA "gf4_26";
const char *IMG_GF4_27           = D_GRA "gf4_27";
const char *IMG_GF4_2            = D_GRA "gf4_2";
const char *IMG_GF4_3            = D_GRA "gf4_3";
const char *IMG_GF4_4_2          = D_GRA "gf4_4-2";
const char *IMG_GF4_4            = D_GRA "gf4_4";
const char *IMG_GF4_5_1          = D_GRA "gf4_5-1";
const char *IMG_GF4_5_2          = D_GRA "gf4_5-2";
const char *IMG_GF4_5            = D_GRA "gf4_5";
const char *IMG_GF4_6            = D_GRA "gf4_6";
const char *IMG_GF4_7            = D_GRA "gf4_7";
const char *IMG_GF4_8            = D_GRA "gf4_8";
const char *IMG_GF4_9            = D_GRA "gf4_9";
const char *IMG_GF4_BEAM1        = D_GRA "gf4_beam1";
const char *IMG_GF4_BEAM2        = D_GRA "gf4_beam2";
const char *IMG_GF4_BEAM3        = D_GRA "gf4_beam3";
const char *IMG_GF4_BEAM4        = D_GRA "gf4_beam4";
const char *IMG_GF4_CLOGO1       = D_GRA "gf4_clogo1";
const char *IMG_GF4_CLOGO2       = D_GRA "gf4_clogo2";
const char *IMG_GF4_CLOGO3       = D_GRA "gf4_clogo3";
const char *IMG_GF4_CLOGO4       = D_GRA "gf4_clogo4";
const char *IMG_GF4_CLOGO5       = D_GRA "gf4_clogo5";
const char *IMG_GF4_CLOGO6       = D_GRA "gf4_clogo6";
const char *IMG_GF4_CLOGO7       = D_GRA "gf4_clogo7";
const char *IMG_GF4_CLOGO8       = D_GRA "gf4_clogo8";
const char *IMG_GF4_CLOGO9       = D_GRA "gf4_clogo9";
const char *IMG_GF4_COVER        = D_GRA "gf4_cover";
const char *IMG_GF4_HMENU1       = D_GRA "gf4_hmenu1";
const char *IMG_GF4_HMENU2       = D_GRA "gf4_hmenu2";
const char *IMG_GF4_HMENU3       = D_GRA "gf4_hmenu3";
const char *IMG_GF4_HMENU4       = D_GRA "gf4_hmenu4";
const char *IMG_GF4_HMENU5       = D_GRA "gf4_hmenu5";
const char *IMG_GF4_MA1          = D_GRA "gf4_ma1";
const char *IMG_GF4_MA2          = D_GRA "gf4_ma2";
const char *IMG_GF4_MA3          = D_GRA "gf4_ma3";
const char *IMG_GF4_MA4          = D_GRA "gf4_ma4";
const char *IMG_GF4_MA5          = D_GRA "gf4_ma5";
const char *IMG_GF4_SBR          = D_GRA "gf4_sbr";
const char *IMG_GF4_SHADE        = D_GRA "gf4_shade";
const char *IMG_GF4_XP1          = D_GRA "gf4_xp1";
const char *IMG_GF4_XP2          = D_GRA "gf4_xp2";
const char *IMG_GF4_XP3          = D_GRA "gf4_xp3";
const char *IMG_GF4_XP4          = D_GRA "gf4_xp4";
const char *IMG_GF4_XP5          = D_GRA "gf4_xp5";
const char *IMG_GF4_XP6          = D_GRA "gf4_xp6";
const char *IMG_GM1_B1           = D_GRA "gm1_b1";
const char *IMG_GM1_B2           = D_GRA "gm1_b2";
const char *IMG_GM1_B3           = D_GRA "gm1_b3";
const char *IMG_GM1_B4           = D_GRA "gm1_b4";
const char *IMG_GM1_EXIT         = D_GRA "gm1_exit";
const char *IMG_GM1_GAMEOVER     = D_GRA "gm1_gameover";
const char *IMG_GM1_GO           = D_GRA "gm1_go";
const char *IMG_GM1_KBON         = D_GRA "gm1_kbon";
const char *IMG_GM1_MARKNEW      = D_GRA "gm1_marknew";
const char *IMG_GM1_PICTURE      = D_GRA "gm1_picture";
const char *IMG_GM1_S0           = D_GRA "gm1_s0";
const char *IMG_GM1_S1           = D_GRA "gm1_s1";
const char *IMG_GM1_S2           = D_GRA "gm1_s2";
const char *IMG_GM1_S3           = D_GRA "gm1_s3";
const char *IMG_GM1_S4           = D_GRA "gm1_s4";
const char *IMG_GM1_S5           = D_GRA "gm1_s5";
const char *IMG_GM1_S6           = D_GRA "gm1_s6";
const char *IMG_GM1_S7           = D_GRA "gm1_s7";
const char *IMG_GM1_S8           = D_GRA "gm1_s8";
const char *IMG_GM1_S9           = D_GRA "gm1_s9";
const char *IMG_GM1_SBON         = D_GRA "gm1_sbon";
const char *IMG_GM1_SHERIFF      = D_GRA "gm1_sheriff";
const char *IMG_GM1_SHIELD0      = D_GRA "gm1_shield0";
const char *IMG_GM1_SHIELD1      = D_GRA "gm1_shield1";
const char *IMG_GM1_SHIELD2      = D_GRA "gm1_shield2";
const char *IMG_GM1_SHIELD3      = D_GRA "gm1_shield3";
const char *IMG_GM1_SHIELD4      = D_GRA "gm1_shield4";
const char *IMG_GM1_SHOOT        = D_GRA "gm1_shoot";
const char *IMG_GM1_STAGE        = D_GRA "gm1_stage";
const char *IMG_GM1_T10          = D_GRA "gm1_t10";
const char *IMG_GM1_T11          = D_GRA "gm1_t11";
const char *IMG_GM1_T12          = D_GRA "gm1_t12";
const char *IMG_GM1_T1           = D_GRA "gm1_t1";
const char *IMG_GM1_T2           = D_GRA "gm1_t2";
const char *IMG_GM1_T3           = D_GRA "gm1_t3";
const char *IMG_GM1_T4           = D_GRA "gm1_t4";
const char *IMG_GM1_T5           = D_GRA "gm1_t5";
const char *IMG_GM1_T6           = D_GRA "gm1_t6";
const char *IMG_GM1_T7           = D_GRA "gm1_t7";
const char *IMG_GM1_T8           = D_GRA "gm1_t8";
const char *IMG_GM1_T9           = D_GRA "gm1_t9";
const char *IMG_GM1_TFIRE        = D_GRA "gm1_tfire";
const char *IMG_GM1_TOKILL       = D_GRA "gm1_tokill";
const char *IMG_GM1_XP1          = D_GRA "gm1_xp1";
const char *IMG_GM1_XP2          = D_GRA "gm1_xp2";
const char *IMG_GM1_XP3          = D_GRA "gm1_xp3";
const char *IMG_GM1_XP4          = D_GRA "gm1_xp4";
const char *IMG_GM1_XP5          = D_GRA "gm1_xp5";
const char *IMG_GM1_XP6          = D_GRA "gm1_xp6";
const char *IMG_HF1_PICTURE2     = D_GRA "hf1_picture2";
const char *IMG_HF2_PICTURE2     = D_GRA "hf2_picture2";
const char *IMG_HF3_PICTURE2     = D_GRA "hf3_picture2";
const char *IMG_HF4_PICTURE2     = D_GRA "hf4_picture2";
const char *IMG_HF5_PICTURE2     = D_GRA "hf5_picture2";
const char *IMG_HF6_PICTURE2     = D_GRA "hf6_picture2";
const char *IMG_HF8_PICTURE2     = D_GRA "hf8_picture2";
const char *IMG_HP1_HYP1         = D_SUB "hp1_hyp1";
const char *IMG_HP1_HYP2         = D_SUB "hp1_hyp2";
const char *IMG_HP1_HYP3         = D_SUB "hp1_hyp3";
const char *IMG_HP1_HYP4         = D_SUB "hp1_hyp4";
const char *IMG_HP1_HYP5         = D_SUB "hp1_hyp5";
const char *IMG_HS1_F10          = D_GRA "hs1_f10";
const char *IMG_HS1_F11          = D_GRA "hs1_f11";
const char *IMG_HS1_F1           = D_GRA "hs1_f1";
const char *IMG_HS1_F2           = D_GRA "hs1_f2";
const char *IMG_HS1_F3           = D_GRA "hs1_f3";
const char *IMG_HS1_F4           = D_GRA "hs1_f4";
const char *IMG_HS1_F5           = D_GRA "hs1_f5";
const char *IMG_HS1_F6           = D_GRA "hs1_f6";
const char *IMG_HS1_F7           = D_GRA "hs1_f7";
const char *IMG_HS1_F8           = D_GRA "hs1_f8";
const char *IMG_HS1_F9           = D_GRA "hs1_f9";
const char *IMG_HS1_P0           = D_GRA "hs1_p0";
const char *IMG_HS1_P1           = D_GRA "hs1_p1";
const char *IMG_HS1_P2           = D_GRA "hs1_p2";
const char *IMG_HS1_P3           = D_GRA "hs1_p3";
const char *IMG_HS1_P4           = D_GRA "hs1_p4";
const char *IMG_HS1_P5           = D_GRA "hs1_p5";
const char *IMG_HS1_P6           = D_GRA "hs1_p6";
const char *IMG_HS1_P7           = D_GRA "hs1_p7";
const char *IMG_HS1_P8           = D_GRA "hs1_p8";
const char *IMG_HS1_S0           = D_GRA "hs1_s0";
const char *IMG_HS1_S1           = D_GRA "hs1_s1";
const char *IMG_HS1_S2           = D_GRA "hs1_s2";
const char *IMG_HS1_S3           = D_GRA "hs1_s3";
const char *IMG_HS1_S4           = D_GRA "hs1_s4";
const char *IMG_HS1_S5           = D_GRA "hs1_s5";
const char *IMG_HS1_S6           = D_GRA "hs1_s6";
const char *IMG_HS1_S7           = D_GRA "hs1_s7";
const char *IMG_HS1_S8           = D_GRA "hs1_s8";
const char *IMG_HS2_C10          = D_GRA "hs2_c10";
const char *IMG_HS2_C1           = D_GRA "hs2_c1";
const char *IMG_HS2_C2           = D_GRA "hs2_c2";
const char *IMG_HS2_C3           = D_GRA "hs2_c3";
const char *IMG_HS2_C4           = D_GRA "hs2_c4";
const char *IMG_HS2_C5           = D_GRA "hs2_c5";
const char *IMG_HS2_C6           = D_GRA "hs2_c6";
const char *IMG_HS2_C7           = D_GRA "hs2_c7";
const char *IMG_HS2_C8           = D_GRA "hs2_c8";
const char *IMG_HS2_C9           = D_GRA "hs2_c9";
const char *IMG_HS2_S            = D_GRA "hs2_s";
const char *IMG_HS3_G1           = D_GRA "hs3_g1";
const char *IMG_HS3_S1           = D_GRA "hs3_s1";
const char *IMG_HS4_L10          = D_GRA "hs4_l10";
const char *IMG_HS4_L11          = D_GRA "hs4_l11";
const char *IMG_HS4_L12          = D_GRA "hs4_l12";
const char *IMG_HS4_L13          = D_GRA "hs4_l13";
const char *IMG_HS4_L14          = D_GRA "hs4_l14";
const char *IMG_HS4_L15          = D_GRA "hs4_l15";
const char *IMG_HS4_L1           = D_GRA "hs4_l1";
const char *IMG_HS4_L2           = D_GRA "hs4_l2";
const char *IMG_HS4_L3           = D_GRA "hs4_l3";
const char *IMG_HS4_L4           = D_GRA "hs4_l4";
const char *IMG_HS4_L5           = D_GRA "hs4_l5";
const char *IMG_HS4_L6           = D_GRA "hs4_l6";
const char *IMG_HS4_L7           = D_GRA "hs4_l7";
const char *IMG_HS4_L8           = D_GRA "hs4_l8";
const char *IMG_HS4_L9           = D_GRA "hs4_l9";
const char *IMG_LD0_LD0_1        = D_GRA "ld0_ld0-1";
const char *IMG_LD0_LD0_2        = D_GRA "ld0_ld0-2";
const char *IMG_LD0_LD0_3        = D_GRA "ld0_ld0-3";
const char *IMG_LD0_LD0          = D_GRA "ld0_ld0";
const char *IMG_LD1_LD1_1        = D_GRA "ld1_ld1-1";
const char *IMG_LD1_LD1_2        = D_GRA "ld1_ld1-2";
const char *IMG_LD1_LD1_3        = D_GRA "ld1_ld1-3";
const char *IMG_LD1_LD1          = D_GRA "ld1_ld1";
const char *IMG_LD2_LD2_1        = D_GRA "ld2_ld2-1";
const char *IMG_LD2_LD2_2        = D_GRA "ld2_ld2-2";
const char *IMG_LD2_LD2_3        = D_GRA "ld2_ld2-3";
const char *IMG_LD2_LD2          = D_GRA "ld2_ld2";
const char *IMG_LD3_LD3_1        = D_GRA "ld3_ld3-1";
const char *IMG_LD3_LD3_2        = D_GRA "ld3_ld3-2";
const char *IMG_LD3_LD3_3        = D_GRA "ld3_ld3-3";
const char *IMG_LD3_LD3          = D_GRA "ld3_ld3";
const char *IMG_LD4_LD4_1        = D_GRA "ld4_ld4-1";
const char *IMG_LD4_LD4_2        = D_GRA "ld4_ld4-2";
const char *IMG_LD4_LD4_3        = D_GRA "ld4_ld4-3";
const char *IMG_LD4_LD4          = D_GRA "ld4_ld4";
const char *IMG_LD5_LD5_1        = D_GRA "ld5_ld5-1";
const char *IMG_LD5_LD5_2        = D_GRA "ld5_ld5-2";
const char *IMG_LD5_LD5_3        = D_GRA "ld5_ld5-3";
const char *IMG_LD5_LD5          = D_GRA "ld5_ld5";
const char *IMG_LG1_PIC1         = D_GRA "lg1_pic1";
const char *IMG_LG1_PIC2         = D_GRA "lg1_pic2";
const char *IMG_LG1_PIC3         = D_GRA "lg1_pic3";
const char *IMG_LG1_PIC4         = D_GRA "lg1_pic4";
const char *IMG_LG1_PIC5         = D_GRA "lg1_pic5";
const char *IMG_LP1_LAND10       = D_GRA "lp1_land10";
const char *IMG_LP1_LAND11       = D_GRA "lp1_land11";
const char *IMG_LP1_LAND12       = D_GRA "lp1_land12";
const char *IMG_LP1_LAND13       = D_GRA "lp1_land13";
const char *IMG_LP1_LAND14       = D_GRA "lp1_land14";
const char *IMG_LP1_LAND15       = D_GRA "lp1_land15";
const char *IMG_LP1_LAND16       = D_GRA "lp1_land16";
const char *IMG_LP1_LAND17       = D_GRA "lp1_land17";
const char *IMG_LP1_LAND18       = D_GRA "lp1_land18";
const char *IMG_LP1_LAND19       = D_GRA "lp1_land19";
const char *IMG_LP1_LAND20       = D_GRA "lp1_land20";
const char *IMG_LP1_LAND2        = D_GRA "lp1_land2";
const char *IMG_LP1_LAND3        = D_GRA "lp1_land3";
const char *IMG_LP1_LAND4        = D_GRA "lp1_land4";
const char *IMG_LP1_LAND5        = D_GRA "lp1_land5";
const char *IMG_LP1_LAND6        = D_GRA "lp1_land6";
const char *IMG_LP1_LAND7        = D_GRA "lp1_land7";
const char *IMG_LP1_LAND8        = D_GRA "lp1_land8";
const char *IMG_LP1_LAND9        = D_GRA "lp1_land9";
const char *IMG_ME1_MENU         = D_GRA "me1_menu";
const char *IMG_ME2_MENU         = D_GRA "me2_menu";
const char *IMG_ME4_MENU         = D_GRA "me4_menu";
const char *IMG_ME5_MENU         = D_GRA "me5_menu";
const char *IMG_ME6_MENU         = D_GRA "me6_menu";
const char *IMG_ME7_MENU         = D_GRA "me7_menu";
const char *IMG_OP1_OPEN1        = D_GRA "op1_open1";
const char *IMG_OP1_OPEN2        = D_GRA "op1_open2";
const char *IMG_OP1_OPEN3        = D_GRA "op1_open3";
const char *IMG_OP1_OPEN4        = D_GRA "op1_open4";
const char *IMG_OP1_OPEN5        = D_GRA "op1_open5";
const char *IMG_OP1_OPEN6        = D_GRA "op1_open6";
const char *IMG_PA_ROT           = D_SUB "pa_rot";
const char *IMG_PL1_PLANET1      = D_GRA "pl1_planet1";
const char *IMG_PL2_PLANET2      = D_GRA "pl2_planet2";
const char *IMG_PL3_PLANET3      = D_GRA "pl3_planet3";
const char *IMG_PL4_PLANET4      = D_GRA "pl4_planet4";
const char *IMG_PL5_PLANET5      = D_GRA "pl5_planet5";
const char *IMG_PL6_PLANET6      = D_GRA "pl6_planet6";
const char *IMG_PL8_PLANET8      = D_GRA "pl8_planet8";
const char *IMG_PP1_CONST        = D_GRA "pp1_const";
const char *IMG_PP2_CONST        = D_GRA "pp2_const";
const char *IMG_PP3_CONST        = D_GRA "pp3_const";
const char *IMG_PT1_PATTERN      = D_GRA "pt1_pattern";
const char *IMG_PT2_PATTERN      = D_GRA "pt2_pattern";
const char *IMG_RD1_A1           = D_GRA "rd1_a1";
const char *IMG_RD1_ACT0         = D_GRA "rd1_act0";
const char *IMG_RD1_ACT1         = D_GRA "rd1_act1";
const char *IMG_RD1_ACT2         = D_GRA "rd1_act2";
const char *IMG_RD1_ACT3         = D_GRA "rd1_act3";
const char *IMG_RD1_ACT4         = D_GRA "rd1_act4";
const char *IMG_RD1_ACT5         = D_GRA "rd1_act5";
const char *IMG_RD1_BOW          = D_GRA "rd1_bow";
const char *IMG_RD1_DEAD10       = D_GRA "rd1_dead10";
const char *IMG_RD1_DEAD1        = D_GRA "rd1_dead1";
const char *IMG_RD1_DEAD2        = D_GRA "rd1_dead2";
const char *IMG_RD1_DEAD3        = D_GRA "rd1_dead3";
const char *IMG_RD1_DEAD4        = D_GRA "rd1_dead4";
const char *IMG_RD1_DEAD5        = D_GRA "rd1_dead5";
const char *IMG_RD1_DEAD6        = D_GRA "rd1_dead6";
const char *IMG_RD1_DEAD7        = D_GRA "rd1_dead7";
const char *IMG_RD1_DEAD8        = D_GRA "rd1_dead8";
const char *IMG_RD1_DEAD9        = D_GRA "rd1_dead9";
const char *IMG_RD1_F1           = D_GRA "rd1_f1";
const char *IMG_RD1_FAIL1        = D_GRA "rd1_fail1";
const char *IMG_RD1_FAIL2        = D_GRA "rd1_fail2";
const char *IMG_RD1_FAIL3        = D_GRA "rd1_fail3";
const char *IMG_RD1_HIT          = D_GRA "rd1_hit";
const char *IMG_RD1_MBACK        = D_GRA "rd1_mback";
const char *IMG_RD1_RFAIL        = D_GRA "rd1_rfail";
const char *IMG_RD1_ST1          = D_GRA "rd1_st1";
const char *IMG_RD1_ST2          = D_GRA "rd1_st2";
const char *IMG_RD1_ST3          = D_GRA "rd1_st3";
const char *IMG_RD1_ST4          = D_GRA "rd1_st4";
const char *IMG_RD1_STSH         = D_GRA "rd1_stsh";
const char *IMG_RD1_SYMBOL1      = D_GRA "rd1_symbol1";
const char *IMG_RD1_SYMBOL1S     = D_GRA "rd1_symbol1s";
const char *IMG_RD1_SYMBOL2      = D_GRA "rd1_symbol2";
const char *IMG_RD1_SYMBOL2S     = D_GRA "rd1_symbol2s";
const char *IMG_RD1_SYMBOL3      = D_GRA "rd1_symbol3";
const char *IMG_RD1_SYMBOL3S     = D_GRA "rd1_symbol3s";
const char *IMG_RD1_TLINE1       = D_GRA "rd1_tline1";
const char *IMG_RD1_TLINE2       = D_GRA "rd1_tline2";
const char *IMG_RD1_TLINE3       = D_GRA "rd1_tline3";
const char *IMG_RD1_TYP1         = D_GRA "rd1_typ1";
const char *IMG_RD1_TYP2         = D_GRA "rd1_typ2";
const char *IMG_RD1_TYP3         = D_GRA "rd1_typ3";
const char *IMG_RD1_TYP4         = D_GRA "rd1_typ4";
const char *IMG_RD1_TYP5         = D_GRA "rd1_typ5";
const char *IMG_RF1_1_2          = D_GRA "rf1_1-2";
const char *IMG_RF1_13           = D_GRA "rf1_13";
const char *IMG_RF1_1            = D_GRA "rf1_1";
const char *IMG_RF1_7            = D_GRA "rf1_7";
const char *IMG_SC1_PICTURE      = D_GRA "sc1_picture";
const char *IMG_SC1_SHUTTLE      = D_GRA "sc1_shuttle";
const char *IMG_SF1_SPART00      = D_GRA "sf1_spart00";
const char *IMG_SF1_SPART01      = D_GRA "sf1_spart01";
const char *IMG_SF1_SPART02      = D_GRA "sf1_spart02";
const char *IMG_SF1_SPART03      = D_GRA "sf1_spart03";
const char *IMG_SF1_SPART04      = D_GRA "sf1_spart04";
const char *IMG_SF1_SPART05      = D_GRA "sf1_spart05";
const char *IMG_SF1_SPART06      = D_GRA "sf1_spart06";
const char *IMG_SF1_SPART07      = D_GRA "sf1_spart07";
const char *IMG_SF1_SPART08      = D_GRA "sf1_spart08";
const char *IMG_SF1_SPART09      = D_GRA "sf1_spart09";
const char *IMG_SF1_SPART0C      = D_GRA "sf1_spart0C";
const char *IMG_SF1_SPART0E      = D_GRA "sf1_spart0E";
const char *IMG_SF1_SPART0G      = D_GRA "sf1_spart0G";
const char *IMG_SF1_SPART0L      = D_GRA "sf1_spart0L";
const char *IMG_SF1_SPART0Q      = D_GRA "sf1_spart0Q";
const char *IMG_SF1_SPART0S      = D_GRA "sf1_spart0S";
const char *IMG_SF1_SPART10      = D_GRA "sf1_spart10";
const char *IMG_SF1_SPART11      = D_GRA "sf1_spart11";
const char *IMG_SF1_SPART12      = D_GRA "sf1_spart12";
const char *IMG_SF1_SPART13      = D_GRA "sf1_spart13";
const char *IMG_SF1_SPART14      = D_GRA "sf1_spart14";
const char *IMG_SF1_SPART15      = D_GRA "sf1_spart15";
const char *IMG_SF1_SPART16      = D_GRA "sf1_spart16";
const char *IMG_SF1_SPART17      = D_GRA "sf1_spart17";
const char *IMG_SF1_SPART18      = D_GRA "sf1_spart18";
const char *IMG_SF1_SPART19      = D_GRA "sf1_spart19";
const char *IMG_SF1_SPART1C      = D_GRA "sf1_spart1C";
const char *IMG_SF1_SPART1E      = D_GRA "sf1_spart1E";
const char *IMG_SF1_SPART1G      = D_GRA "sf1_spart1G";
const char *IMG_SF1_SPART1L      = D_GRA "sf1_spart1L";
const char *IMG_SF1_SPART1Q      = D_GRA "sf1_spart1Q";
const char *IMG_SF1_SPART1S      = D_GRA "sf1_spart1S";
const char *IMG_SF1_SPART20      = D_GRA "sf1_spart20";
const char *IMG_SF1_SPART21      = D_GRA "sf1_spart21";
const char *IMG_SF1_SPART22      = D_GRA "sf1_spart22";
const char *IMG_SF1_SPART23      = D_GRA "sf1_spart23";
const char *IMG_SF1_SPART24      = D_GRA "sf1_spart24";
const char *IMG_SF1_SPART25      = D_GRA "sf1_spart25";
const char *IMG_SF1_SPART26      = D_GRA "sf1_spart26";
const char *IMG_SF1_SPART27      = D_GRA "sf1_spart27";
const char *IMG_SF1_SPART28      = D_GRA "sf1_spart28";
const char *IMG_SF1_SPART29      = D_GRA "sf1_spart29";
const char *IMG_SF1_SPART2C      = D_GRA "sf1_spart2C";
const char *IMG_SF1_SPART2E      = D_GRA "sf1_spart2E";
const char *IMG_SF1_SPART2G      = D_GRA "sf1_spart2G";
const char *IMG_SF1_SPART2L      = D_GRA "sf1_spart2L";
const char *IMG_SF1_SPART2Q      = D_GRA "sf1_spart2Q";
const char *IMG_SF1_SPART2S      = D_GRA "sf1_spart2S";
const char *IMG_SF1_SPART30      = D_GRA "sf1_spart30";
const char *IMG_SF1_SPART31      = D_GRA "sf1_spart31";
const char *IMG_SF1_SPART32      = D_GRA "sf1_spart32";
const char *IMG_SF1_SPART33      = D_GRA "sf1_spart33";
const char *IMG_SF1_SPART34      = D_GRA "sf1_spart34";
const char *IMG_SF1_SPART35      = D_GRA "sf1_spart35";
const char *IMG_SF1_SPART36      = D_GRA "sf1_spart36";
const char *IMG_SF1_SPART37      = D_GRA "sf1_spart37";
const char *IMG_SF1_SPART38      = D_GRA "sf1_spart38";
const char *IMG_SF1_SPART39      = D_GRA "sf1_spart39";
const char *IMG_SF1_SPART3C      = D_GRA "sf1_spart3C";
const char *IMG_SF1_SPART3E      = D_GRA "sf1_spart3E";
const char *IMG_SF1_SPART3G      = D_GRA "sf1_spart3G";
const char *IMG_SF1_SPART3L      = D_GRA "sf1_spart3L";
const char *IMG_SF1_SPART3Q      = D_GRA "sf1_spart3Q";
const char *IMG_SF1_SPART3S      = D_GRA "sf1_spart3S";
const char *IMG_SF1_SPART40      = D_GRA "sf1_spart40";
const char *IMG_SF1_SPART41      = D_GRA "sf1_spart41";
const char *IMG_SF1_SPART42      = D_GRA "sf1_spart42";
const char *IMG_SF1_SPART43      = D_GRA "sf1_spart43";
const char *IMG_SF1_SPART44      = D_GRA "sf1_spart44";
const char *IMG_SF1_SPART45      = D_GRA "sf1_spart45";
const char *IMG_SF1_SPART46      = D_GRA "sf1_spart46";
const char *IMG_SF1_SPART47      = D_GRA "sf1_spart47";
const char *IMG_SF1_SPART48      = D_GRA "sf1_spart48";
const char *IMG_SF1_SPART49      = D_GRA "sf1_spart49";
const char *IMG_SF1_SPART4C      = D_GRA "sf1_spart4C";
const char *IMG_SF1_SPART4E      = D_GRA "sf1_spart4E";
const char *IMG_SF1_SPART4G      = D_GRA "sf1_spart4G";
const char *IMG_SF1_SPART4L      = D_GRA "sf1_spart4L";
const char *IMG_SF1_SPART4Q      = D_GRA "sf1_spart4Q";
const char *IMG_SF1_SPART4S      = D_GRA "sf1_spart4S";
const char *IMG_SF1_SPART50      = D_GRA "sf1_spart50";
const char *IMG_SF1_SPART51      = D_GRA "sf1_spart51";
const char *IMG_SF1_SPART52      = D_GRA "sf1_spart52";
const char *IMG_SF1_SPART53      = D_GRA "sf1_spart53";
const char *IMG_SF1_SPART54      = D_GRA "sf1_spart54";
const char *IMG_SF1_SPART55      = D_GRA "sf1_spart55";
const char *IMG_SF1_SPART56      = D_GRA "sf1_spart56";
const char *IMG_SF1_SPART57      = D_GRA "sf1_spart57";
const char *IMG_SF1_SPART58      = D_GRA "sf1_spart58";
const char *IMG_SF1_SPART59      = D_GRA "sf1_spart59";
const char *IMG_SF1_SPART5C      = D_GRA "sf1_spart5C";
const char *IMG_SF1_SPART5E      = D_GRA "sf1_spart5E";
const char *IMG_SF1_SPART5G      = D_GRA "sf1_spart5G";
const char *IMG_SF1_SPART5L      = D_GRA "sf1_spart5L";
const char *IMG_SF1_SPART5Q      = D_GRA "sf1_spart5Q";
const char *IMG_SF1_SPART5S      = D_GRA "sf1_spart5S";
const char *IMG_SF1_SPART60      = D_GRA "sf1_spart60";
const char *IMG_SF1_SPART61      = D_GRA "sf1_spart61";
const char *IMG_SF1_SPART62      = D_GRA "sf1_spart62";
const char *IMG_SF1_SPART63      = D_GRA "sf1_spart63";
const char *IMG_SF1_SPART64      = D_GRA "sf1_spart64";
const char *IMG_SF1_SPART65      = D_GRA "sf1_spart65";
const char *IMG_SF1_SPART66      = D_GRA "sf1_spart66";
const char *IMG_SF1_SPART67      = D_GRA "sf1_spart67";
const char *IMG_SF1_SPART68      = D_GRA "sf1_spart68";
const char *IMG_SF1_SPART69      = D_GRA "sf1_spart69";
const char *IMG_SF1_SPART6C      = D_GRA "sf1_spart6C";
const char *IMG_SF1_SPART6E      = D_GRA "sf1_spart6E";
const char *IMG_SF1_SPART6G      = D_GRA "sf1_spart6G";
const char *IMG_SF1_SPART6L      = D_GRA "sf1_spart6L";
const char *IMG_SF1_SPART6Q      = D_GRA "sf1_spart6Q";
const char *IMG_SF1_SPART6S      = D_GRA "sf1_spart6S";
const char *IMG_SF1_SPART70      = D_GRA "sf1_spart70";
const char *IMG_SF1_SPART71      = D_GRA "sf1_spart71";
const char *IMG_SF1_SPART72      = D_GRA "sf1_spart72";
const char *IMG_SF1_SPART73      = D_GRA "sf1_spart73";
const char *IMG_SF1_SPART74      = D_GRA "sf1_spart74";
const char *IMG_SF1_SPART75      = D_GRA "sf1_spart75";
const char *IMG_SF1_SPART76      = D_GRA "sf1_spart76";
const char *IMG_SF1_SPART77      = D_GRA "sf1_spart77";
const char *IMG_SF1_SPART78      = D_GRA "sf1_spart78";
const char *IMG_SF1_SPART79      = D_GRA "sf1_spart79";
const char *IMG_SF1_SPART7C      = D_GRA "sf1_spart7C";
const char *IMG_SF1_SPART7E      = D_GRA "sf1_spart7E";
const char *IMG_SF1_SPART7G      = D_GRA "sf1_spart7G";
const char *IMG_SF1_SPART7L      = D_GRA "sf1_spart7L";
const char *IMG_SF1_SPART7Q      = D_GRA "sf1_spart7Q";
const char *IMG_SF1_SPART7S      = D_GRA "sf1_spart7S";
const char *IMG_SF1_SPART80      = D_GRA "sf1_spart80";
const char *IMG_SF1_SPART81      = D_GRA "sf1_spart81";
const char *IMG_SF1_SPART82      = D_GRA "sf1_spart82";
const char *IMG_SF1_SPART83      = D_GRA "sf1_spart83";
const char *IMG_SF1_SPART84      = D_GRA "sf1_spart84";
const char *IMG_SF1_SPART85      = D_GRA "sf1_spart85";
const char *IMG_SF1_SPART86      = D_GRA "sf1_spart86";
const char *IMG_SF1_SPART87      = D_GRA "sf1_spart87";
const char *IMG_SF1_SPART88      = D_GRA "sf1_spart88";
const char *IMG_SF1_SPART89      = D_GRA "sf1_spart89";
const char *IMG_SF1_SPART8C      = D_GRA "sf1_spart8C";
const char *IMG_SF1_SPART8E      = D_GRA "sf1_spart8E";
const char *IMG_SF1_SPART8G      = D_GRA "sf1_spart8G";
const char *IMG_SF1_SPART8L      = D_GRA "sf1_spart8L";
const char *IMG_SF1_SPART8Q      = D_GRA "sf1_spart8Q";
const char *IMG_SF1_SPART8S      = D_GRA "sf1_spart8S";
const char *IMG_SF1_SPART90      = D_GRA "sf1_spart90";
const char *IMG_SF1_SPART91      = D_GRA "sf1_spart91";
const char *IMG_SF1_SPART92      = D_GRA "sf1_spart92";
const char *IMG_SF1_SPART93      = D_GRA "sf1_spart93";
const char *IMG_SF1_SPART94      = D_GRA "sf1_spart94";
const char *IMG_SF1_SPART95      = D_GRA "sf1_spart95";
const char *IMG_SF1_SPART96      = D_GRA "sf1_spart96";
const char *IMG_SF1_SPART97      = D_GRA "sf1_spart97";
const char *IMG_SF1_SPART98      = D_GRA "sf1_spart98";
const char *IMG_SF1_SPART99      = D_GRA "sf1_spart99";
const char *IMG_SF1_SPART9C      = D_GRA "sf1_spart9C";
const char *IMG_SF1_SPART9E      = D_GRA "sf1_spart9E";
const char *IMG_SF1_SPART9G      = D_GRA "sf1_spart9G";
const char *IMG_SF1_SPART9L      = D_GRA "sf1_spart9L";
const char *IMG_SF1_SPART9Q      = D_GRA "sf1_spart9Q";
const char *IMG_SF1_SPART9S      = D_GRA "sf1_spart9S";
const char *IMG_SF1_SPARTC0      = D_GRA "sf1_spartC0";
const char *IMG_SF1_SPARTC1      = D_GRA "sf1_spartC1";
const char *IMG_SF1_SPARTC2      = D_GRA "sf1_spartC2";
const char *IMG_SF1_SPARTC3      = D_GRA "sf1_spartC3";
const char *IMG_SF1_SPARTC4      = D_GRA "sf1_spartC4";
const char *IMG_SF1_SPARTC5      = D_GRA "sf1_spartC5";
const char *IMG_SF1_SPARTC6      = D_GRA "sf1_spartC6";
const char *IMG_SF1_SPARTC7      = D_GRA "sf1_spartC7";
const char *IMG_SF1_SPARTC8      = D_GRA "sf1_spartC8";
const char *IMG_SF1_SPARTC9      = D_GRA "sf1_spartC9";
const char *IMG_SF1_SPARTCC      = D_GRA "sf1_spartCC";
const char *IMG_SF1_SPARTCE      = D_GRA "sf1_spartCE";
const char *IMG_SF1_SPARTCG      = D_GRA "sf1_spartCG";
const char *IMG_SF1_SPARTCL      = D_GRA "sf1_spartCL";
const char *IMG_SF1_SPARTCQ      = D_GRA "sf1_spartCQ";
const char *IMG_SF1_SPARTCS      = D_GRA "sf1_spartCS";
const char *IMG_SF1_SPARTE0      = D_GRA "sf1_spartE0";
const char *IMG_SF1_SPARTE1      = D_GRA "sf1_spartE1";
const char *IMG_SF1_SPARTE2      = D_GRA "sf1_spartE2";
const char *IMG_SF1_SPARTE3      = D_GRA "sf1_spartE3";
const char *IMG_SF1_SPARTE4      = D_GRA "sf1_spartE4";
const char *IMG_SF1_SPARTE5      = D_GRA "sf1_spartE5";
const char *IMG_SF1_SPARTE6      = D_GRA "sf1_spartE6";
const char *IMG_SF1_SPARTE7      = D_GRA "sf1_spartE7";
const char *IMG_SF1_SPARTE8      = D_GRA "sf1_spartE8";
const char *IMG_SF1_SPARTE9      = D_GRA "sf1_spartE9";
const char *IMG_SF1_SPARTEC      = D_GRA "sf1_spartEC";
const char *IMG_SF1_SPARTEE      = D_GRA "sf1_spartEE";
const char *IMG_SF1_SPARTEG      = D_GRA "sf1_spartEG";
const char *IMG_SF1_SPARTEL      = D_GRA "sf1_spartEL";
const char *IMG_SF1_SPARTEQ      = D_GRA "sf1_spartEQ";
const char *IMG_SF1_SPARTES      = D_GRA "sf1_spartES";
const char *IMG_SF1_SPARTG0      = D_GRA "sf1_spartG0";
const char *IMG_SF1_SPARTG1      = D_GRA "sf1_spartG1";
const char *IMG_SF1_SPARTG2      = D_GRA "sf1_spartG2";
const char *IMG_SF1_SPARTG3      = D_GRA "sf1_spartG3";
const char *IMG_SF1_SPARTG4      = D_GRA "sf1_spartG4";
const char *IMG_SF1_SPARTG5      = D_GRA "sf1_spartG5";
const char *IMG_SF1_SPARTG6      = D_GRA "sf1_spartG6";
const char *IMG_SF1_SPARTG7      = D_GRA "sf1_spartG7";
const char *IMG_SF1_SPARTG8      = D_GRA "sf1_spartG8";
const char *IMG_SF1_SPARTG9      = D_GRA "sf1_spartG9";
const char *IMG_SF1_SPARTGC      = D_GRA "sf1_spartGC";
const char *IMG_SF1_SPARTGE      = D_GRA "sf1_spartGE";
const char *IMG_SF1_SPARTGG      = D_GRA "sf1_spartGG";
const char *IMG_SF1_SPARTGL      = D_GRA "sf1_spartGL";
const char *IMG_SF1_SPARTGQ      = D_GRA "sf1_spartGQ";
const char *IMG_SF1_SPARTGS      = D_GRA "sf1_spartGS";
const char *IMG_SF1_SPARTL0      = D_GRA "sf1_spartL0";
const char *IMG_SF1_SPARTL1      = D_GRA "sf1_spartL1";
const char *IMG_SF1_SPARTL2      = D_GRA "sf1_spartL2";
const char *IMG_SF1_SPARTL3      = D_GRA "sf1_spartL3";
const char *IMG_SF1_SPARTL4      = D_GRA "sf1_spartL4";
const char *IMG_SF1_SPARTL5      = D_GRA "sf1_spartL5";
const char *IMG_SF1_SPARTL6      = D_GRA "sf1_spartL6";
const char *IMG_SF1_SPARTL7      = D_GRA "sf1_spartL7";
const char *IMG_SF1_SPARTL8      = D_GRA "sf1_spartL8";
const char *IMG_SF1_SPARTL9      = D_GRA "sf1_spartL9";
const char *IMG_SF1_SPARTLC      = D_GRA "sf1_spartLC";
const char *IMG_SF1_SPARTLE      = D_GRA "sf1_spartLE";
const char *IMG_SF1_SPARTLG      = D_GRA "sf1_spartLG";
const char *IMG_SF1_SPARTLL      = D_GRA "sf1_spartLL";
const char *IMG_SF1_SPARTLQ      = D_GRA "sf1_spartLQ";
const char *IMG_SF1_SPARTLS      = D_GRA "sf1_spartLS";
const char *IMG_SF1_SPARTQ0      = D_GRA "sf1_spartQ0";
const char *IMG_SF1_SPARTQ1      = D_GRA "sf1_spartQ1";
const char *IMG_SF1_SPARTQ2      = D_GRA "sf1_spartQ2";
const char *IMG_SF1_SPARTQ3      = D_GRA "sf1_spartQ3";
const char *IMG_SF1_SPARTQ4      = D_GRA "sf1_spartQ4";
const char *IMG_SF1_SPARTQ5      = D_GRA "sf1_spartQ5";
const char *IMG_SF1_SPARTQ6      = D_GRA "sf1_spartQ6";
const char *IMG_SF1_SPARTQ7      = D_GRA "sf1_spartQ7";
const char *IMG_SF1_SPARTQ8      = D_GRA "sf1_spartQ8";
const char *IMG_SF1_SPARTQ9      = D_GRA "sf1_spartQ9";
const char *IMG_SF1_SPARTQC      = D_GRA "sf1_spartQC";
const char *IMG_SF1_SPARTQE      = D_GRA "sf1_spartQE";
const char *IMG_SF1_SPARTQG      = D_GRA "sf1_spartQG";
const char *IMG_SF1_SPARTQL      = D_GRA "sf1_spartQL";
const char *IMG_SF1_SPARTQQ      = D_GRA "sf1_spartQQ";
const char *IMG_SF1_SPARTQS      = D_GRA "sf1_spartQS";
const char *IMG_SF1_SPARTS0      = D_GRA "sf1_spartS0";
const char *IMG_SF1_SPARTS1      = D_GRA "sf1_spartS1";
const char *IMG_SF1_SPARTS2      = D_GRA "sf1_spartS2";
const char *IMG_SF1_SPARTS3      = D_GRA "sf1_spartS3";
const char *IMG_SF1_SPARTS4      = D_GRA "sf1_spartS4";
const char *IMG_SF1_SPARTS5      = D_GRA "sf1_spartS5";
const char *IMG_SF1_SPARTS6      = D_GRA "sf1_spartS6";
const char *IMG_SF1_SPARTS7      = D_GRA "sf1_spartS7";
const char *IMG_SF1_SPARTS8      = D_GRA "sf1_spartS8";
const char *IMG_SF1_SPARTS9      = D_GRA "sf1_spartS9";
const char *IMG_SF1_SPARTSC      = D_GRA "sf1_spartSC";
const char *IMG_SF1_SPARTSE      = D_GRA "sf1_spartSE";
const char *IMG_SF1_SPARTSG      = D_GRA "sf1_spartSG";
const char *IMG_SF1_SPARTSL      = D_GRA "sf1_spartSL";
const char *IMG_SF1_SPARTSQ      = D_GRA "sf1_spartSQ";
const char *IMG_SF1_SPARTSS      = D_GRA "sf1_spartSS";
const char *IMG_SF1_STONE0       = D_GRA "sf1_stone0";
const char *IMG_SF1_STONE10      = D_GRA "sf1_stone10";
const char *IMG_SF1_STONE19_0    = D_GRA "sf1_stone19-0";
const char *IMG_SF1_STONE2_0     = D_GRA "sf1_stone2-0";
const char *IMG_SF1_STONE29B     = D_GRA "sf1_stone29b";
const char *IMG_SF1_STONE29      = D_GRA "sf1_stone29";
const char *IMG_SF1_STONE2       = D_GRA "sf1_stone2";
const char *IMG_SF1_STONE8       = D_GRA "sf1_stone8";
const char *IMG_SF1_STONE9       = D_GRA "sf1_stone9";
const char *IMG_SF2_SPART00      = D_GRA "sf2_spart00";
const char *IMG_SF2_SPART01      = D_GRA "sf2_spart01";
const char *IMG_SF2_SPART02      = D_GRA "sf2_spart02";
const char *IMG_SF2_SPART03      = D_GRA "sf2_spart03";
const char *IMG_SF2_SPART04      = D_GRA "sf2_spart04";
const char *IMG_SF2_SPART05      = D_GRA "sf2_spart05";
const char *IMG_SF2_SPART06      = D_GRA "sf2_spart06";
const char *IMG_SF2_SPART07      = D_GRA "sf2_spart07";
const char *IMG_SF2_SPART08      = D_GRA "sf2_spart08";
const char *IMG_SF2_SPART09      = D_GRA "sf2_spart09";
const char *IMG_SF2_SPART0C      = D_GRA "sf2_spart0C";
const char *IMG_SF2_SPART0E      = D_GRA "sf2_spart0E";
const char *IMG_SF2_SPART0G      = D_GRA "sf2_spart0G";
const char *IMG_SF2_SPART0L      = D_GRA "sf2_spart0L";
const char *IMG_SF2_SPART0Q      = D_GRA "sf2_spart0Q";
const char *IMG_SF2_SPART0S      = D_GRA "sf2_spart0S";
const char *IMG_SF2_SPART10      = D_GRA "sf2_spart10";
const char *IMG_SF2_SPART11      = D_GRA "sf2_spart11";
const char *IMG_SF2_SPART12      = D_GRA "sf2_spart12";
const char *IMG_SF2_SPART13      = D_GRA "sf2_spart13";
const char *IMG_SF2_SPART14      = D_GRA "sf2_spart14";
const char *IMG_SF2_SPART15      = D_GRA "sf2_spart15";
const char *IMG_SF2_SPART16      = D_GRA "sf2_spart16";
const char *IMG_SF2_SPART17      = D_GRA "sf2_spart17";
const char *IMG_SF2_SPART18      = D_GRA "sf2_spart18";
const char *IMG_SF2_SPART19      = D_GRA "sf2_spart19";
const char *IMG_SF2_SPART1C      = D_GRA "sf2_spart1C";
const char *IMG_SF2_SPART1E      = D_GRA "sf2_spart1E";
const char *IMG_SF2_SPART1G      = D_GRA "sf2_spart1G";
const char *IMG_SF2_SPART1L      = D_GRA "sf2_spart1L";
const char *IMG_SF2_SPART1Q      = D_GRA "sf2_spart1Q";
const char *IMG_SF2_SPART1S      = D_GRA "sf2_spart1S";
const char *IMG_SF2_SPART20      = D_GRA "sf2_spart20";
const char *IMG_SF2_SPART21      = D_GRA "sf2_spart21";
const char *IMG_SF2_SPART22      = D_GRA "sf2_spart22";
const char *IMG_SF2_SPART23      = D_GRA "sf2_spart23";
const char *IMG_SF2_SPART24      = D_GRA "sf2_spart24";
const char *IMG_SF2_SPART25      = D_GRA "sf2_spart25";
const char *IMG_SF2_SPART26      = D_GRA "sf2_spart26";
const char *IMG_SF2_SPART27      = D_GRA "sf2_spart27";
const char *IMG_SF2_SPART28      = D_GRA "sf2_spart28";
const char *IMG_SF2_SPART29      = D_GRA "sf2_spart29";
const char *IMG_SF2_SPART2C      = D_GRA "sf2_spart2C";
const char *IMG_SF2_SPART2E      = D_GRA "sf2_spart2E";
const char *IMG_SF2_SPART2G      = D_GRA "sf2_spart2G";
const char *IMG_SF2_SPART2L      = D_GRA "sf2_spart2L";
const char *IMG_SF2_SPART2Q      = D_GRA "sf2_spart2Q";
const char *IMG_SF2_SPART2S      = D_GRA "sf2_spart2S";
const char *IMG_SF2_SPART30      = D_GRA "sf2_spart30";
const char *IMG_SF2_SPART31      = D_GRA "sf2_spart31";
const char *IMG_SF2_SPART32      = D_GRA "sf2_spart32";
const char *IMG_SF2_SPART33      = D_GRA "sf2_spart33";
const char *IMG_SF2_SPART34      = D_GRA "sf2_spart34";
const char *IMG_SF2_SPART35      = D_GRA "sf2_spart35";
const char *IMG_SF2_SPART36      = D_GRA "sf2_spart36";
const char *IMG_SF2_SPART37      = D_GRA "sf2_spart37";
const char *IMG_SF2_SPART38      = D_GRA "sf2_spart38";
const char *IMG_SF2_SPART39      = D_GRA "sf2_spart39";
const char *IMG_SF2_SPART3C      = D_GRA "sf2_spart3C";
const char *IMG_SF2_SPART3E      = D_GRA "sf2_spart3E";
const char *IMG_SF2_SPART3G      = D_GRA "sf2_spart3G";
const char *IMG_SF2_SPART3L      = D_GRA "sf2_spart3L";
const char *IMG_SF2_SPART3Q      = D_GRA "sf2_spart3Q";
const char *IMG_SF2_SPART3S      = D_GRA "sf2_spart3S";
const char *IMG_SF2_SPART40      = D_GRA "sf2_spart40";
const char *IMG_SF2_SPART41      = D_GRA "sf2_spart41";
const char *IMG_SF2_SPART42      = D_GRA "sf2_spart42";
const char *IMG_SF2_SPART43      = D_GRA "sf2_spart43";
const char *IMG_SF2_SPART44      = D_GRA "sf2_spart44";
const char *IMG_SF2_SPART45      = D_GRA "sf2_spart45";
const char *IMG_SF2_SPART46      = D_GRA "sf2_spart46";
const char *IMG_SF2_SPART47      = D_GRA "sf2_spart47";
const char *IMG_SF2_SPART48      = D_GRA "sf2_spart48";
const char *IMG_SF2_SPART49      = D_GRA "sf2_spart49";
const char *IMG_SF2_SPART4C      = D_GRA "sf2_spart4C";
const char *IMG_SF2_SPART4E      = D_GRA "sf2_spart4E";
const char *IMG_SF2_SPART4G      = D_GRA "sf2_spart4G";
const char *IMG_SF2_SPART4L      = D_GRA "sf2_spart4L";
const char *IMG_SF2_SPART4Q      = D_GRA "sf2_spart4Q";
const char *IMG_SF2_SPART4S      = D_GRA "sf2_spart4S";
const char *IMG_SF2_SPART50      = D_GRA "sf2_spart50";
const char *IMG_SF2_SPART51      = D_GRA "sf2_spart51";
const char *IMG_SF2_SPART52      = D_GRA "sf2_spart52";
const char *IMG_SF2_SPART53      = D_GRA "sf2_spart53";
const char *IMG_SF2_SPART54      = D_GRA "sf2_spart54";
const char *IMG_SF2_SPART55      = D_GRA "sf2_spart55";
const char *IMG_SF2_SPART56      = D_GRA "sf2_spart56";
const char *IMG_SF2_SPART57      = D_GRA "sf2_spart57";
const char *IMG_SF2_SPART58      = D_GRA "sf2_spart58";
const char *IMG_SF2_SPART59      = D_GRA "sf2_spart59";
const char *IMG_SF2_SPART5C      = D_GRA "sf2_spart5C";
const char *IMG_SF2_SPART5E      = D_GRA "sf2_spart5E";
const char *IMG_SF2_SPART5G      = D_GRA "sf2_spart5G";
const char *IMG_SF2_SPART5L      = D_GRA "sf2_spart5L";
const char *IMG_SF2_SPART5Q      = D_GRA "sf2_spart5Q";
const char *IMG_SF2_SPART5S      = D_GRA "sf2_spart5S";
const char *IMG_SF2_SPART60      = D_GRA "sf2_spart60";
const char *IMG_SF2_SPART61      = D_GRA "sf2_spart61";
const char *IMG_SF2_SPART62      = D_GRA "sf2_spart62";
const char *IMG_SF2_SPART63      = D_GRA "sf2_spart63";
const char *IMG_SF2_SPART64      = D_GRA "sf2_spart64";
const char *IMG_SF2_SPART65      = D_GRA "sf2_spart65";
const char *IMG_SF2_SPART66      = D_GRA "sf2_spart66";
const char *IMG_SF2_SPART67      = D_GRA "sf2_spart67";
const char *IMG_SF2_SPART68      = D_GRA "sf2_spart68";
const char *IMG_SF2_SPART69      = D_GRA "sf2_spart69";
const char *IMG_SF2_SPART6C      = D_GRA "sf2_spart6C";
const char *IMG_SF2_SPART6E      = D_GRA "sf2_spart6E";
const char *IMG_SF2_SPART6G      = D_GRA "sf2_spart6G";
const char *IMG_SF2_SPART6L      = D_GRA "sf2_spart6L";
const char *IMG_SF2_SPART6Q      = D_GRA "sf2_spart6Q";
const char *IMG_SF2_SPART6S      = D_GRA "sf2_spart6S";
const char *IMG_SF2_SPART70      = D_GRA "sf2_spart70";
const char *IMG_SF2_SPART71      = D_GRA "sf2_spart71";
const char *IMG_SF2_SPART72      = D_GRA "sf2_spart72";
const char *IMG_SF2_SPART73      = D_GRA "sf2_spart73";
const char *IMG_SF2_SPART74      = D_GRA "sf2_spart74";
const char *IMG_SF2_SPART75      = D_GRA "sf2_spart75";
const char *IMG_SF2_SPART76      = D_GRA "sf2_spart76";
const char *IMG_SF2_SPART77      = D_GRA "sf2_spart77";
const char *IMG_SF2_SPART78      = D_GRA "sf2_spart78";
const char *IMG_SF2_SPART79      = D_GRA "sf2_spart79";
const char *IMG_SF2_SPART7C      = D_GRA "sf2_spart7C";
const char *IMG_SF2_SPART7E      = D_GRA "sf2_spart7E";
const char *IMG_SF2_SPART7G      = D_GRA "sf2_spart7G";
const char *IMG_SF2_SPART7L      = D_GRA "sf2_spart7L";
const char *IMG_SF2_SPART7Q      = D_GRA "sf2_spart7Q";
const char *IMG_SF2_SPART7S      = D_GRA "sf2_spart7S";
const char *IMG_SF2_SPART80      = D_GRA "sf2_spart80";
const char *IMG_SF2_SPART81      = D_GRA "sf2_spart81";
const char *IMG_SF2_SPART82      = D_GRA "sf2_spart82";
const char *IMG_SF2_SPART83      = D_GRA "sf2_spart83";
const char *IMG_SF2_SPART84      = D_GRA "sf2_spart84";
const char *IMG_SF2_SPART85      = D_GRA "sf2_spart85";
const char *IMG_SF2_SPART86      = D_GRA "sf2_spart86";
const char *IMG_SF2_SPART87      = D_GRA "sf2_spart87";
const char *IMG_SF2_SPART88      = D_GRA "sf2_spart88";
const char *IMG_SF2_SPART89      = D_GRA "sf2_spart89";
const char *IMG_SF2_SPART8C      = D_GRA "sf2_spart8C";
const char *IMG_SF2_SPART8E      = D_GRA "sf2_spart8E";
const char *IMG_SF2_SPART8G      = D_GRA "sf2_spart8G";
const char *IMG_SF2_SPART8L      = D_GRA "sf2_spart8L";
const char *IMG_SF2_SPART8Q      = D_GRA "sf2_spart8Q";
const char *IMG_SF2_SPART8S      = D_GRA "sf2_spart8S";
const char *IMG_SF2_SPART90      = D_GRA "sf2_spart90";
const char *IMG_SF2_SPART91      = D_GRA "sf2_spart91";
const char *IMG_SF2_SPART92      = D_GRA "sf2_spart92";
const char *IMG_SF2_SPART93      = D_GRA "sf2_spart93";
const char *IMG_SF2_SPART94      = D_GRA "sf2_spart94";
const char *IMG_SF2_SPART95      = D_GRA "sf2_spart95";
const char *IMG_SF2_SPART96      = D_GRA "sf2_spart96";
const char *IMG_SF2_SPART97      = D_GRA "sf2_spart97";
const char *IMG_SF2_SPART98      = D_GRA "sf2_spart98";
const char *IMG_SF2_SPART99      = D_GRA "sf2_spart99";
const char *IMG_SF2_SPART9C      = D_GRA "sf2_spart9C";
const char *IMG_SF2_SPART9E      = D_GRA "sf2_spart9E";
const char *IMG_SF2_SPART9G      = D_GRA "sf2_spart9G";
const char *IMG_SF2_SPART9L      = D_GRA "sf2_spart9L";
const char *IMG_SF2_SPART9Q      = D_GRA "sf2_spart9Q";
const char *IMG_SF2_SPART9S      = D_GRA "sf2_spart9S";
const char *IMG_SF2_SPARTC0      = D_GRA "sf2_spartC0";
const char *IMG_SF2_SPARTC1      = D_GRA "sf2_spartC1";
const char *IMG_SF2_SPARTC2      = D_GRA "sf2_spartC2";
const char *IMG_SF2_SPARTC3      = D_GRA "sf2_spartC3";
const char *IMG_SF2_SPARTC4      = D_GRA "sf2_spartC4";
const char *IMG_SF2_SPARTC5      = D_GRA "sf2_spartC5";
const char *IMG_SF2_SPARTC6      = D_GRA "sf2_spartC6";
const char *IMG_SF2_SPARTC7      = D_GRA "sf2_spartC7";
const char *IMG_SF2_SPARTC8      = D_GRA "sf2_spartC8";
const char *IMG_SF2_SPARTC9      = D_GRA "sf2_spartC9";
const char *IMG_SF2_SPARTCC      = D_GRA "sf2_spartCC";
const char *IMG_SF2_SPARTCE      = D_GRA "sf2_spartCE";
const char *IMG_SF2_SPARTCG      = D_GRA "sf2_spartCG";
const char *IMG_SF2_SPARTCL      = D_GRA "sf2_spartCL";
const char *IMG_SF2_SPARTCQ      = D_GRA "sf2_spartCQ";
const char *IMG_SF2_SPARTCS      = D_GRA "sf2_spartCS";
const char *IMG_SF2_SPARTE0      = D_GRA "sf2_spartE0";
const char *IMG_SF2_SPARTE1      = D_GRA "sf2_spartE1";
const char *IMG_SF2_SPARTE2      = D_GRA "sf2_spartE2";
const char *IMG_SF2_SPARTE3      = D_GRA "sf2_spartE3";
const char *IMG_SF2_SPARTE4      = D_GRA "sf2_spartE4";
const char *IMG_SF2_SPARTE5      = D_GRA "sf2_spartE5";
const char *IMG_SF2_SPARTE6      = D_GRA "sf2_spartE6";
const char *IMG_SF2_SPARTE7      = D_GRA "sf2_spartE7";
const char *IMG_SF2_SPARTE8      = D_GRA "sf2_spartE8";
const char *IMG_SF2_SPARTE9      = D_GRA "sf2_spartE9";
const char *IMG_SF2_SPARTEC      = D_GRA "sf2_spartEC";
const char *IMG_SF2_SPARTEE      = D_GRA "sf2_spartEE";
const char *IMG_SF2_SPARTEG      = D_GRA "sf2_spartEG";
const char *IMG_SF2_SPARTEL      = D_GRA "sf2_spartEL";
const char *IMG_SF2_SPARTEQ      = D_GRA "sf2_spartEQ";
const char *IMG_SF2_SPARTES      = D_GRA "sf2_spartES";
const char *IMG_SF2_SPARTG0      = D_GRA "sf2_spartG0";
const char *IMG_SF2_SPARTG1      = D_GRA "sf2_spartG1";
const char *IMG_SF2_SPARTG2      = D_GRA "sf2_spartG2";
const char *IMG_SF2_SPARTG3      = D_GRA "sf2_spartG3";
const char *IMG_SF2_SPARTG4      = D_GRA "sf2_spartG4";
const char *IMG_SF2_SPARTG5      = D_GRA "sf2_spartG5";
const char *IMG_SF2_SPARTG6      = D_GRA "sf2_spartG6";
const char *IMG_SF2_SPARTG7      = D_GRA "sf2_spartG7";
const char *IMG_SF2_SPARTG8      = D_GRA "sf2_spartG8";
const char *IMG_SF2_SPARTG9      = D_GRA "sf2_spartG9";
const char *IMG_SF2_SPARTGC      = D_GRA "sf2_spartGC";
const char *IMG_SF2_SPARTGE      = D_GRA "sf2_spartGE";
const char *IMG_SF2_SPARTGG      = D_GRA "sf2_spartGG";
const char *IMG_SF2_SPARTGL      = D_GRA "sf2_spartGL";
const char *IMG_SF2_SPARTGQ      = D_GRA "sf2_spartGQ";
const char *IMG_SF2_SPARTGS      = D_GRA "sf2_spartGS";
const char *IMG_SF2_SPARTL0      = D_GRA "sf2_spartL0";
const char *IMG_SF2_SPARTL1      = D_GRA "sf2_spartL1";
const char *IMG_SF2_SPARTL2      = D_GRA "sf2_spartL2";
const char *IMG_SF2_SPARTL3      = D_GRA "sf2_spartL3";
const char *IMG_SF2_SPARTL4      = D_GRA "sf2_spartL4";
const char *IMG_SF2_SPARTL5      = D_GRA "sf2_spartL5";
const char *IMG_SF2_SPARTL6      = D_GRA "sf2_spartL6";
const char *IMG_SF2_SPARTL7      = D_GRA "sf2_spartL7";
const char *IMG_SF2_SPARTL8      = D_GRA "sf2_spartL8";
const char *IMG_SF2_SPARTL9      = D_GRA "sf2_spartL9";
const char *IMG_SF2_SPARTLC      = D_GRA "sf2_spartLC";
const char *IMG_SF2_SPARTLE      = D_GRA "sf2_spartLE";
const char *IMG_SF2_SPARTLG      = D_GRA "sf2_spartLG";
const char *IMG_SF2_SPARTLL      = D_GRA "sf2_spartLL";
const char *IMG_SF2_SPARTLQ      = D_GRA "sf2_spartLQ";
const char *IMG_SF2_SPARTLS      = D_GRA "sf2_spartLS";
const char *IMG_SF2_SPARTQ0      = D_GRA "sf2_spartQ0";
const char *IMG_SF2_SPARTQ1      = D_GRA "sf2_spartQ1";
const char *IMG_SF2_SPARTQ2      = D_GRA "sf2_spartQ2";
const char *IMG_SF2_SPARTQ3      = D_GRA "sf2_spartQ3";
const char *IMG_SF2_SPARTQ4      = D_GRA "sf2_spartQ4";
const char *IMG_SF2_SPARTQ5      = D_GRA "sf2_spartQ5";
const char *IMG_SF2_SPARTQ6      = D_GRA "sf2_spartQ6";
const char *IMG_SF2_SPARTQ7      = D_GRA "sf2_spartQ7";
const char *IMG_SF2_SPARTQ8      = D_GRA "sf2_spartQ8";
const char *IMG_SF2_SPARTQ9      = D_GRA "sf2_spartQ9";
const char *IMG_SF2_SPARTQC      = D_GRA "sf2_spartQC";
const char *IMG_SF2_SPARTQE      = D_GRA "sf2_spartQE";
const char *IMG_SF2_SPARTQG      = D_GRA "sf2_spartQG";
const char *IMG_SF2_SPARTQL      = D_GRA "sf2_spartQL";
const char *IMG_SF2_SPARTQQ      = D_GRA "sf2_spartQQ";
const char *IMG_SF2_SPARTQS      = D_GRA "sf2_spartQS";
const char *IMG_SF2_SPARTS0      = D_GRA "sf2_spartS0";
const char *IMG_SF2_SPARTS1      = D_GRA "sf2_spartS1";
const char *IMG_SF2_SPARTS2      = D_GRA "sf2_spartS2";
const char *IMG_SF2_SPARTS3      = D_GRA "sf2_spartS3";
const char *IMG_SF2_SPARTS4      = D_GRA "sf2_spartS4";
const char *IMG_SF2_SPARTS5      = D_GRA "sf2_spartS5";
const char *IMG_SF2_SPARTS6      = D_GRA "sf2_spartS6";
const char *IMG_SF2_SPARTS7      = D_GRA "sf2_spartS7";
const char *IMG_SF2_SPARTS8      = D_GRA "sf2_spartS8";
const char *IMG_SF2_SPARTS9      = D_GRA "sf2_spartS9";
const char *IMG_SF2_SPARTSC      = D_GRA "sf2_spartSC";
const char *IMG_SF2_SPARTSE      = D_GRA "sf2_spartSE";
const char *IMG_SF2_SPARTSG      = D_GRA "sf2_spartSG";
const char *IMG_SF2_SPARTSL      = D_GRA "sf2_spartSL";
const char *IMG_SF2_SPARTSQ      = D_GRA "sf2_spartSQ";
const char *IMG_SF2_SPARTSS      = D_GRA "sf2_spartSS";
const char *IMG_SF2_STONE0       = D_GRA "sf2_stone0";
const char *IMG_SF2_STONE10      = D_GRA "sf2_stone10";
const char *IMG_SF2_STONE19_0    = D_GRA "sf2_stone19-0";
const char *IMG_SF2_STONE2_0     = D_GRA "sf2_stone2-0";
const char *IMG_SF2_STONE29B     = D_GRA "sf2_stone29b";
const char *IMG_SF2_STONE29      = D_GRA "sf2_stone29";
const char *IMG_SF2_STONE2       = D_GRA "sf2_stone2";
const char *IMG_SF2_STONE8       = D_GRA "sf2_stone8";
const char *IMG_SF2_STONE9       = D_GRA "sf2_stone9";
const char *IMG_SF3_SPART00      = D_GRA "sf3_spart00";
const char *IMG_SF3_SPART01      = D_GRA "sf3_spart01";
const char *IMG_SF3_SPART02      = D_GRA "sf3_spart02";
const char *IMG_SF3_SPART03      = D_GRA "sf3_spart03";
const char *IMG_SF3_SPART04      = D_GRA "sf3_spart04";
const char *IMG_SF3_SPART05      = D_GRA "sf3_spart05";
const char *IMG_SF3_SPART06      = D_GRA "sf3_spart06";
const char *IMG_SF3_SPART07      = D_GRA "sf3_spart07";
const char *IMG_SF3_SPART08      = D_GRA "sf3_spart08";
const char *IMG_SF3_SPART09      = D_GRA "sf3_spart09";
const char *IMG_SF3_SPART0C      = D_GRA "sf3_spart0C";
const char *IMG_SF3_SPART0E      = D_GRA "sf3_spart0E";
const char *IMG_SF3_SPART0G      = D_GRA "sf3_spart0G";
const char *IMG_SF3_SPART0L      = D_GRA "sf3_spart0L";
const char *IMG_SF3_SPART0Q      = D_GRA "sf3_spart0Q";
const char *IMG_SF3_SPART0S      = D_GRA "sf3_spart0S";
const char *IMG_SF3_SPART10      = D_GRA "sf3_spart10";
const char *IMG_SF3_SPART11      = D_GRA "sf3_spart11";
const char *IMG_SF3_SPART12      = D_GRA "sf3_spart12";
const char *IMG_SF3_SPART13      = D_GRA "sf3_spart13";
const char *IMG_SF3_SPART14      = D_GRA "sf3_spart14";
const char *IMG_SF3_SPART15      = D_GRA "sf3_spart15";
const char *IMG_SF3_SPART16      = D_GRA "sf3_spart16";
const char *IMG_SF3_SPART17      = D_GRA "sf3_spart17";
const char *IMG_SF3_SPART18      = D_GRA "sf3_spart18";
const char *IMG_SF3_SPART19      = D_GRA "sf3_spart19";
const char *IMG_SF3_SPART1C      = D_GRA "sf3_spart1C";
const char *IMG_SF3_SPART1E      = D_GRA "sf3_spart1E";
const char *IMG_SF3_SPART1G      = D_GRA "sf3_spart1G";
const char *IMG_SF3_SPART1L      = D_GRA "sf3_spart1L";
const char *IMG_SF3_SPART1Q      = D_GRA "sf3_spart1Q";
const char *IMG_SF3_SPART1S      = D_GRA "sf3_spart1S";
const char *IMG_SF3_SPART20      = D_GRA "sf3_spart20";
const char *IMG_SF3_SPART21      = D_GRA "sf3_spart21";
const char *IMG_SF3_SPART22      = D_GRA "sf3_spart22";
const char *IMG_SF3_SPART23      = D_GRA "sf3_spart23";
const char *IMG_SF3_SPART24      = D_GRA "sf3_spart24";
const char *IMG_SF3_SPART25      = D_GRA "sf3_spart25";
const char *IMG_SF3_SPART26      = D_GRA "sf3_spart26";
const char *IMG_SF3_SPART27      = D_GRA "sf3_spart27";
const char *IMG_SF3_SPART28      = D_GRA "sf3_spart28";
const char *IMG_SF3_SPART29      = D_GRA "sf3_spart29";
const char *IMG_SF3_SPART2C      = D_GRA "sf3_spart2C";
const char *IMG_SF3_SPART2E      = D_GRA "sf3_spart2E";
const char *IMG_SF3_SPART2G      = D_GRA "sf3_spart2G";
const char *IMG_SF3_SPART2L      = D_GRA "sf3_spart2L";
const char *IMG_SF3_SPART2Q      = D_GRA "sf3_spart2Q";
const char *IMG_SF3_SPART2S      = D_GRA "sf3_spart2S";
const char *IMG_SF3_SPART30      = D_GRA "sf3_spart30";
const char *IMG_SF3_SPART31      = D_GRA "sf3_spart31";
const char *IMG_SF3_SPART32      = D_GRA "sf3_spart32";
const char *IMG_SF3_SPART33      = D_GRA "sf3_spart33";
const char *IMG_SF3_SPART34      = D_GRA "sf3_spart34";
const char *IMG_SF3_SPART35      = D_GRA "sf3_spart35";
const char *IMG_SF3_SPART36      = D_GRA "sf3_spart36";
const char *IMG_SF3_SPART37      = D_GRA "sf3_spart37";
const char *IMG_SF3_SPART38      = D_GRA "sf3_spart38";
const char *IMG_SF3_SPART39      = D_GRA "sf3_spart39";
const char *IMG_SF3_SPART3C      = D_GRA "sf3_spart3C";
const char *IMG_SF3_SPART3E      = D_GRA "sf3_spart3E";
const char *IMG_SF3_SPART3G      = D_GRA "sf3_spart3G";
const char *IMG_SF3_SPART3L      = D_GRA "sf3_spart3L";
const char *IMG_SF3_SPART3Q      = D_GRA "sf3_spart3Q";
const char *IMG_SF3_SPART3S      = D_GRA "sf3_spart3S";
const char *IMG_SF3_SPART40      = D_GRA "sf3_spart40";
const char *IMG_SF3_SPART41      = D_GRA "sf3_spart41";
const char *IMG_SF3_SPART42      = D_GRA "sf3_spart42";
const char *IMG_SF3_SPART43      = D_GRA "sf3_spart43";
const char *IMG_SF3_SPART44      = D_GRA "sf3_spart44";
const char *IMG_SF3_SPART45      = D_GRA "sf3_spart45";
const char *IMG_SF3_SPART46      = D_GRA "sf3_spart46";
const char *IMG_SF3_SPART47      = D_GRA "sf3_spart47";
const char *IMG_SF3_SPART48      = D_GRA "sf3_spart48";
const char *IMG_SF3_SPART49      = D_GRA "sf3_spart49";
const char *IMG_SF3_SPART4C      = D_GRA "sf3_spart4C";
const char *IMG_SF3_SPART4E      = D_GRA "sf3_spart4E";
const char *IMG_SF3_SPART4G      = D_GRA "sf3_spart4G";
const char *IMG_SF3_SPART4L      = D_GRA "sf3_spart4L";
const char *IMG_SF3_SPART4Q      = D_GRA "sf3_spart4Q";
const char *IMG_SF3_SPART4S      = D_GRA "sf3_spart4S";
const char *IMG_SF3_SPART50      = D_GRA "sf3_spart50";
const char *IMG_SF3_SPART51      = D_GRA "sf3_spart51";
const char *IMG_SF3_SPART52      = D_GRA "sf3_spart52";
const char *IMG_SF3_SPART53      = D_GRA "sf3_spart53";
const char *IMG_SF3_SPART54      = D_GRA "sf3_spart54";
const char *IMG_SF3_SPART55      = D_GRA "sf3_spart55";
const char *IMG_SF3_SPART56      = D_GRA "sf3_spart56";
const char *IMG_SF3_SPART57      = D_GRA "sf3_spart57";
const char *IMG_SF3_SPART58      = D_GRA "sf3_spart58";
const char *IMG_SF3_SPART59      = D_GRA "sf3_spart59";
const char *IMG_SF3_SPART5C      = D_GRA "sf3_spart5C";
const char *IMG_SF3_SPART5E      = D_GRA "sf3_spart5E";
const char *IMG_SF3_SPART5G      = D_GRA "sf3_spart5G";
const char *IMG_SF3_SPART5L      = D_GRA "sf3_spart5L";
const char *IMG_SF3_SPART5Q      = D_GRA "sf3_spart5Q";
const char *IMG_SF3_SPART5S      = D_GRA "sf3_spart5S";
const char *IMG_SF3_SPART60      = D_GRA "sf3_spart60";
const char *IMG_SF3_SPART61      = D_GRA "sf3_spart61";
const char *IMG_SF3_SPART62      = D_GRA "sf3_spart62";
const char *IMG_SF3_SPART63      = D_GRA "sf3_spart63";
const char *IMG_SF3_SPART64      = D_GRA "sf3_spart64";
const char *IMG_SF3_SPART65      = D_GRA "sf3_spart65";
const char *IMG_SF3_SPART66      = D_GRA "sf3_spart66";
const char *IMG_SF3_SPART67      = D_GRA "sf3_spart67";
const char *IMG_SF3_SPART68      = D_GRA "sf3_spart68";
const char *IMG_SF3_SPART69      = D_GRA "sf3_spart69";
const char *IMG_SF3_SPART6C      = D_GRA "sf3_spart6C";
const char *IMG_SF3_SPART6E      = D_GRA "sf3_spart6E";
const char *IMG_SF3_SPART6G      = D_GRA "sf3_spart6G";
const char *IMG_SF3_SPART6L      = D_GRA "sf3_spart6L";
const char *IMG_SF3_SPART6Q      = D_GRA "sf3_spart6Q";
const char *IMG_SF3_SPART6S      = D_GRA "sf3_spart6S";
const char *IMG_SF3_SPART70      = D_GRA "sf3_spart70";
const char *IMG_SF3_SPART71      = D_GRA "sf3_spart71";
const char *IMG_SF3_SPART72      = D_GRA "sf3_spart72";
const char *IMG_SF3_SPART73      = D_GRA "sf3_spart73";
const char *IMG_SF3_SPART74      = D_GRA "sf3_spart74";
const char *IMG_SF3_SPART75      = D_GRA "sf3_spart75";
const char *IMG_SF3_SPART76      = D_GRA "sf3_spart76";
const char *IMG_SF3_SPART77      = D_GRA "sf3_spart77";
const char *IMG_SF3_SPART78      = D_GRA "sf3_spart78";
const char *IMG_SF3_SPART79      = D_GRA "sf3_spart79";
const char *IMG_SF3_SPART7C      = D_GRA "sf3_spart7C";
const char *IMG_SF3_SPART7E      = D_GRA "sf3_spart7E";
const char *IMG_SF3_SPART7G      = D_GRA "sf3_spart7G";
const char *IMG_SF3_SPART7L      = D_GRA "sf3_spart7L";
const char *IMG_SF3_SPART7Q      = D_GRA "sf3_spart7Q";
const char *IMG_SF3_SPART7S      = D_GRA "sf3_spart7S";
const char *IMG_SF3_SPART80      = D_GRA "sf3_spart80";
const char *IMG_SF3_SPART81      = D_GRA "sf3_spart81";
const char *IMG_SF3_SPART82      = D_GRA "sf3_spart82";
const char *IMG_SF3_SPART83      = D_GRA "sf3_spart83";
const char *IMG_SF3_SPART84      = D_GRA "sf3_spart84";
const char *IMG_SF3_SPART85      = D_GRA "sf3_spart85";
const char *IMG_SF3_SPART86      = D_GRA "sf3_spart86";
const char *IMG_SF3_SPART87      = D_GRA "sf3_spart87";
const char *IMG_SF3_SPART88      = D_GRA "sf3_spart88";
const char *IMG_SF3_SPART89      = D_GRA "sf3_spart89";
const char *IMG_SF3_SPART8C      = D_GRA "sf3_spart8C";
const char *IMG_SF3_SPART8E      = D_GRA "sf3_spart8E";
const char *IMG_SF3_SPART8G      = D_GRA "sf3_spart8G";
const char *IMG_SF3_SPART8L      = D_GRA "sf3_spart8L";
const char *IMG_SF3_SPART8Q      = D_GRA "sf3_spart8Q";
const char *IMG_SF3_SPART8S      = D_GRA "sf3_spart8S";
const char *IMG_SF3_SPART90      = D_GRA "sf3_spart90";
const char *IMG_SF3_SPART91      = D_GRA "sf3_spart91";
const char *IMG_SF3_SPART92      = D_GRA "sf3_spart92";
const char *IMG_SF3_SPART93      = D_GRA "sf3_spart93";
const char *IMG_SF3_SPART94      = D_GRA "sf3_spart94";
const char *IMG_SF3_SPART95      = D_GRA "sf3_spart95";
const char *IMG_SF3_SPART96      = D_GRA "sf3_spart96";
const char *IMG_SF3_SPART97      = D_GRA "sf3_spart97";
const char *IMG_SF3_SPART98      = D_GRA "sf3_spart98";
const char *IMG_SF3_SPART99      = D_GRA "sf3_spart99";
const char *IMG_SF3_SPART9C      = D_GRA "sf3_spart9C";
const char *IMG_SF3_SPART9E      = D_GRA "sf3_spart9E";
const char *IMG_SF3_SPART9G      = D_GRA "sf3_spart9G";
const char *IMG_SF3_SPART9L      = D_GRA "sf3_spart9L";
const char *IMG_SF3_SPART9Q      = D_GRA "sf3_spart9Q";
const char *IMG_SF3_SPART9S      = D_GRA "sf3_spart9S";
const char *IMG_SF3_SPARTC0      = D_GRA "sf3_spartC0";
const char *IMG_SF3_SPARTC1      = D_GRA "sf3_spartC1";
const char *IMG_SF3_SPARTC2      = D_GRA "sf3_spartC2";
const char *IMG_SF3_SPARTC3      = D_GRA "sf3_spartC3";
const char *IMG_SF3_SPARTC4      = D_GRA "sf3_spartC4";
const char *IMG_SF3_SPARTC5      = D_GRA "sf3_spartC5";
const char *IMG_SF3_SPARTC6      = D_GRA "sf3_spartC6";
const char *IMG_SF3_SPARTC7      = D_GRA "sf3_spartC7";
const char *IMG_SF3_SPARTC8      = D_GRA "sf3_spartC8";
const char *IMG_SF3_SPARTC9      = D_GRA "sf3_spartC9";
const char *IMG_SF3_SPARTCC      = D_GRA "sf3_spartCC";
const char *IMG_SF3_SPARTCE      = D_GRA "sf3_spartCE";
const char *IMG_SF3_SPARTCG      = D_GRA "sf3_spartCG";
const char *IMG_SF3_SPARTCL      = D_GRA "sf3_spartCL";
const char *IMG_SF3_SPARTCQ      = D_GRA "sf3_spartCQ";
const char *IMG_SF3_SPARTCS      = D_GRA "sf3_spartCS";
const char *IMG_SF3_SPARTE0      = D_GRA "sf3_spartE0";
const char *IMG_SF3_SPARTE1      = D_GRA "sf3_spartE1";
const char *IMG_SF3_SPARTE2      = D_GRA "sf3_spartE2";
const char *IMG_SF3_SPARTE3      = D_GRA "sf3_spartE3";
const char *IMG_SF3_SPARTE4      = D_GRA "sf3_spartE4";
const char *IMG_SF3_SPARTE5      = D_GRA "sf3_spartE5";
const char *IMG_SF3_SPARTE6      = D_GRA "sf3_spartE6";
const char *IMG_SF3_SPARTE7      = D_GRA "sf3_spartE7";
const char *IMG_SF3_SPARTE8      = D_GRA "sf3_spartE8";
const char *IMG_SF3_SPARTE9      = D_GRA "sf3_spartE9";
const char *IMG_SF3_SPARTEC      = D_GRA "sf3_spartEC";
const char *IMG_SF3_SPARTEE      = D_GRA "sf3_spartEE";
const char *IMG_SF3_SPARTEG      = D_GRA "sf3_spartEG";
const char *IMG_SF3_SPARTEL      = D_GRA "sf3_spartEL";
const char *IMG_SF3_SPARTEQ      = D_GRA "sf3_spartEQ";
const char *IMG_SF3_SPARTES      = D_GRA "sf3_spartES";
const char *IMG_SF3_SPARTG0      = D_GRA "sf3_spartG0";
const char *IMG_SF3_SPARTG1      = D_GRA "sf3_spartG1";
const char *IMG_SF3_SPARTG2      = D_GRA "sf3_spartG2";
const char *IMG_SF3_SPARTG3      = D_GRA "sf3_spartG3";
const char *IMG_SF3_SPARTG4      = D_GRA "sf3_spartG4";
const char *IMG_SF3_SPARTG5      = D_GRA "sf3_spartG5";
const char *IMG_SF3_SPARTG6      = D_GRA "sf3_spartG6";
const char *IMG_SF3_SPARTG7      = D_GRA "sf3_spartG7";
const char *IMG_SF3_SPARTG8      = D_GRA "sf3_spartG8";
const char *IMG_SF3_SPARTG9      = D_GRA "sf3_spartG9";
const char *IMG_SF3_SPARTGC      = D_GRA "sf3_spartGC";
const char *IMG_SF3_SPARTGE      = D_GRA "sf3_spartGE";
const char *IMG_SF3_SPARTGG      = D_GRA "sf3_spartGG";
const char *IMG_SF3_SPARTGL      = D_GRA "sf3_spartGL";
const char *IMG_SF3_SPARTGQ      = D_GRA "sf3_spartGQ";
const char *IMG_SF3_SPARTGS      = D_GRA "sf3_spartGS";
const char *IMG_SF3_SPARTL0      = D_GRA "sf3_spartL0";
const char *IMG_SF3_SPARTL1      = D_GRA "sf3_spartL1";
const char *IMG_SF3_SPARTL2      = D_GRA "sf3_spartL2";
const char *IMG_SF3_SPARTL3      = D_GRA "sf3_spartL3";
const char *IMG_SF3_SPARTL4      = D_GRA "sf3_spartL4";
const char *IMG_SF3_SPARTL5      = D_GRA "sf3_spartL5";
const char *IMG_SF3_SPARTL6      = D_GRA "sf3_spartL6";
const char *IMG_SF3_SPARTL7      = D_GRA "sf3_spartL7";
const char *IMG_SF3_SPARTL8      = D_GRA "sf3_spartL8";
const char *IMG_SF3_SPARTL9      = D_GRA "sf3_spartL9";
const char *IMG_SF3_SPARTLC      = D_GRA "sf3_spartLC";
const char *IMG_SF3_SPARTLE      = D_GRA "sf3_spartLE";
const char *IMG_SF3_SPARTLG      = D_GRA "sf3_spartLG";
const char *IMG_SF3_SPARTLL      = D_GRA "sf3_spartLL";
const char *IMG_SF3_SPARTLQ      = D_GRA "sf3_spartLQ";
const char *IMG_SF3_SPARTLS      = D_GRA "sf3_spartLS";
const char *IMG_SF3_SPARTQ0      = D_GRA "sf3_spartQ0";
const char *IMG_SF3_SPARTQ1      = D_GRA "sf3_spartQ1";
const char *IMG_SF3_SPARTQ2      = D_GRA "sf3_spartQ2";
const char *IMG_SF3_SPARTQ3      = D_GRA "sf3_spartQ3";
const char *IMG_SF3_SPARTQ4      = D_GRA "sf3_spartQ4";
const char *IMG_SF3_SPARTQ5      = D_GRA "sf3_spartQ5";
const char *IMG_SF3_SPARTQ6      = D_GRA "sf3_spartQ6";
const char *IMG_SF3_SPARTQ7      = D_GRA "sf3_spartQ7";
const char *IMG_SF3_SPARTQ8      = D_GRA "sf3_spartQ8";
const char *IMG_SF3_SPARTQ9      = D_GRA "sf3_spartQ9";
const char *IMG_SF3_SPARTQC      = D_GRA "sf3_spartQC";
const char *IMG_SF3_SPARTQE      = D_GRA "sf3_spartQE";
const char *IMG_SF3_SPARTQG      = D_GRA "sf3_spartQG";
const char *IMG_SF3_SPARTQL      = D_GRA "sf3_spartQL";
const char *IMG_SF3_SPARTQQ      = D_GRA "sf3_spartQQ";
const char *IMG_SF3_SPARTQS      = D_GRA "sf3_spartQS";
const char *IMG_SF3_SPARTS0      = D_GRA "sf3_spartS0";
const char *IMG_SF3_SPARTS1      = D_GRA "sf3_spartS1";
const char *IMG_SF3_SPARTS2      = D_GRA "sf3_spartS2";
const char *IMG_SF3_SPARTS3      = D_GRA "sf3_spartS3";
const char *IMG_SF3_SPARTS4      = D_GRA "sf3_spartS4";
const char *IMG_SF3_SPARTS5      = D_GRA "sf3_spartS5";
const char *IMG_SF3_SPARTS6      = D_GRA "sf3_spartS6";
const char *IMG_SF3_SPARTS7      = D_GRA "sf3_spartS7";
const char *IMG_SF3_SPARTS8      = D_GRA "sf3_spartS8";
const char *IMG_SF3_SPARTS9      = D_GRA "sf3_spartS9";
const char *IMG_SF3_SPARTSC      = D_GRA "sf3_spartSC";
const char *IMG_SF3_SPARTSE      = D_GRA "sf3_spartSE";
const char *IMG_SF3_SPARTSG      = D_GRA "sf3_spartSG";
const char *IMG_SF3_SPARTSL      = D_GRA "sf3_spartSL";
const char *IMG_SF3_SPARTSQ      = D_GRA "sf3_spartSQ";
const char *IMG_SF3_SPARTSS      = D_GRA "sf3_spartSS";
const char *IMG_SF3_STONE0       = D_GRA "sf3_stone0";
const char *IMG_SF3_STONE10      = D_GRA "sf3_stone10";
const char *IMG_SF3_STONE19_0    = D_GRA "sf3_stone19-0";
const char *IMG_SF3_STONE2_0     = D_GRA "sf3_stone2-0";
const char *IMG_SF3_STONE29B     = D_GRA "sf3_stone29b";
const char *IMG_SF3_STONE29      = D_GRA "sf3_stone29";
const char *IMG_SF3_STONE2       = D_GRA "sf3_stone2";
const char *IMG_SF3_STONE8       = D_GRA "sf3_stone8";
const char *IMG_SF3_STONE9       = D_GRA "sf3_stone9";
const char *IMG_SF4_SPART00      = D_GRA "sf4_spart00";
const char *IMG_SF4_SPART01      = D_GRA "sf4_spart01";
const char *IMG_SF4_SPART02      = D_GRA "sf4_spart02";
const char *IMG_SF4_SPART03      = D_GRA "sf4_spart03";
const char *IMG_SF4_SPART04      = D_GRA "sf4_spart04";
const char *IMG_SF4_SPART05      = D_GRA "sf4_spart05";
const char *IMG_SF4_SPART06      = D_GRA "sf4_spart06";
const char *IMG_SF4_SPART07      = D_GRA "sf4_spart07";
const char *IMG_SF4_SPART08      = D_GRA "sf4_spart08";
const char *IMG_SF4_SPART09      = D_GRA "sf4_spart09";
const char *IMG_SF4_SPART0C      = D_GRA "sf4_spart0C";
const char *IMG_SF4_SPART0E      = D_GRA "sf4_spart0E";
const char *IMG_SF4_SPART0G      = D_GRA "sf4_spart0G";
const char *IMG_SF4_SPART0L      = D_GRA "sf4_spart0L";
const char *IMG_SF4_SPART0Q      = D_GRA "sf4_spart0Q";
const char *IMG_SF4_SPART0S      = D_GRA "sf4_spart0S";
const char *IMG_SF4_SPART10      = D_GRA "sf4_spart10";
const char *IMG_SF4_SPART11      = D_GRA "sf4_spart11";
const char *IMG_SF4_SPART12      = D_GRA "sf4_spart12";
const char *IMG_SF4_SPART13      = D_GRA "sf4_spart13";
const char *IMG_SF4_SPART14      = D_GRA "sf4_spart14";
const char *IMG_SF4_SPART15      = D_GRA "sf4_spart15";
const char *IMG_SF4_SPART16      = D_GRA "sf4_spart16";
const char *IMG_SF4_SPART17      = D_GRA "sf4_spart17";
const char *IMG_SF4_SPART18      = D_GRA "sf4_spart18";
const char *IMG_SF4_SPART19      = D_GRA "sf4_spart19";
const char *IMG_SF4_SPART1C      = D_GRA "sf4_spart1C";
const char *IMG_SF4_SPART1E      = D_GRA "sf4_spart1E";
const char *IMG_SF4_SPART1G      = D_GRA "sf4_spart1G";
const char *IMG_SF4_SPART1L      = D_GRA "sf4_spart1L";
const char *IMG_SF4_SPART1Q      = D_GRA "sf4_spart1Q";
const char *IMG_SF4_SPART1S      = D_GRA "sf4_spart1S";
const char *IMG_SF4_SPART20      = D_GRA "sf4_spart20";
const char *IMG_SF4_SPART21      = D_GRA "sf4_spart21";
const char *IMG_SF4_SPART22      = D_GRA "sf4_spart22";
const char *IMG_SF4_SPART23      = D_GRA "sf4_spart23";
const char *IMG_SF4_SPART24      = D_GRA "sf4_spart24";
const char *IMG_SF4_SPART25      = D_GRA "sf4_spart25";
const char *IMG_SF4_SPART26      = D_GRA "sf4_spart26";
const char *IMG_SF4_SPART27      = D_GRA "sf4_spart27";
const char *IMG_SF4_SPART28      = D_GRA "sf4_spart28";
const char *IMG_SF4_SPART29      = D_GRA "sf4_spart29";
const char *IMG_SF4_SPART2C      = D_GRA "sf4_spart2C";
const char *IMG_SF4_SPART2E      = D_GRA "sf4_spart2E";
const char *IMG_SF4_SPART2G      = D_GRA "sf4_spart2G";
const char *IMG_SF4_SPART2L      = D_GRA "sf4_spart2L";
const char *IMG_SF4_SPART2Q      = D_GRA "sf4_spart2Q";
const char *IMG_SF4_SPART2S      = D_GRA "sf4_spart2S";
const char *IMG_SF4_SPART30      = D_GRA "sf4_spart30";
const char *IMG_SF4_SPART31      = D_GRA "sf4_spart31";
const char *IMG_SF4_SPART32      = D_GRA "sf4_spart32";
const char *IMG_SF4_SPART33      = D_GRA "sf4_spart33";
const char *IMG_SF4_SPART34      = D_GRA "sf4_spart34";
const char *IMG_SF4_SPART35      = D_GRA "sf4_spart35";
const char *IMG_SF4_SPART36      = D_GRA "sf4_spart36";
const char *IMG_SF4_SPART37      = D_GRA "sf4_spart37";
const char *IMG_SF4_SPART38      = D_GRA "sf4_spart38";
const char *IMG_SF4_SPART39      = D_GRA "sf4_spart39";
const char *IMG_SF4_SPART3C      = D_GRA "sf4_spart3C";
const char *IMG_SF4_SPART3E      = D_GRA "sf4_spart3E";
const char *IMG_SF4_SPART3G      = D_GRA "sf4_spart3G";
const char *IMG_SF4_SPART3L      = D_GRA "sf4_spart3L";
const char *IMG_SF4_SPART3Q      = D_GRA "sf4_spart3Q";
const char *IMG_SF4_SPART3S      = D_GRA "sf4_spart3S";
const char *IMG_SF4_SPART40      = D_GRA "sf4_spart40";
const char *IMG_SF4_SPART41      = D_GRA "sf4_spart41";
const char *IMG_SF4_SPART42      = D_GRA "sf4_spart42";
const char *IMG_SF4_SPART43      = D_GRA "sf4_spart43";
const char *IMG_SF4_SPART44      = D_GRA "sf4_spart44";
const char *IMG_SF4_SPART45      = D_GRA "sf4_spart45";
const char *IMG_SF4_SPART46      = D_GRA "sf4_spart46";
const char *IMG_SF4_SPART47      = D_GRA "sf4_spart47";
const char *IMG_SF4_SPART48      = D_GRA "sf4_spart48";
const char *IMG_SF4_SPART49      = D_GRA "sf4_spart49";
const char *IMG_SF4_SPART4C      = D_GRA "sf4_spart4C";
const char *IMG_SF4_SPART4E      = D_GRA "sf4_spart4E";
const char *IMG_SF4_SPART4G      = D_GRA "sf4_spart4G";
const char *IMG_SF4_SPART4L      = D_GRA "sf4_spart4L";
const char *IMG_SF4_SPART4Q      = D_GRA "sf4_spart4Q";
const char *IMG_SF4_SPART4S      = D_GRA "sf4_spart4S";
const char *IMG_SF4_SPART50      = D_GRA "sf4_spart50";
const char *IMG_SF4_SPART51      = D_GRA "sf4_spart51";
const char *IMG_SF4_SPART52      = D_GRA "sf4_spart52";
const char *IMG_SF4_SPART53      = D_GRA "sf4_spart53";
const char *IMG_SF4_SPART54      = D_GRA "sf4_spart54";
const char *IMG_SF4_SPART55      = D_GRA "sf4_spart55";
const char *IMG_SF4_SPART56      = D_GRA "sf4_spart56";
const char *IMG_SF4_SPART57      = D_GRA "sf4_spart57";
const char *IMG_SF4_SPART58      = D_GRA "sf4_spart58";
const char *IMG_SF4_SPART59      = D_GRA "sf4_spart59";
const char *IMG_SF4_SPART5C      = D_GRA "sf4_spart5C";
const char *IMG_SF4_SPART5E      = D_GRA "sf4_spart5E";
const char *IMG_SF4_SPART5G      = D_GRA "sf4_spart5G";
const char *IMG_SF4_SPART5L      = D_GRA "sf4_spart5L";
const char *IMG_SF4_SPART5Q      = D_GRA "sf4_spart5Q";
const char *IMG_SF4_SPART5S      = D_GRA "sf4_spart5S";
const char *IMG_SF4_SPART60      = D_GRA "sf4_spart60";
const char *IMG_SF4_SPART61      = D_GRA "sf4_spart61";
const char *IMG_SF4_SPART62      = D_GRA "sf4_spart62";
const char *IMG_SF4_SPART63      = D_GRA "sf4_spart63";
const char *IMG_SF4_SPART64      = D_GRA "sf4_spart64";
const char *IMG_SF4_SPART65      = D_GRA "sf4_spart65";
const char *IMG_SF4_SPART66      = D_GRA "sf4_spart66";
const char *IMG_SF4_SPART67      = D_GRA "sf4_spart67";
const char *IMG_SF4_SPART68      = D_GRA "sf4_spart68";
const char *IMG_SF4_SPART69      = D_GRA "sf4_spart69";
const char *IMG_SF4_SPART6C      = D_GRA "sf4_spart6C";
const char *IMG_SF4_SPART6E      = D_GRA "sf4_spart6E";
const char *IMG_SF4_SPART6G      = D_GRA "sf4_spart6G";
const char *IMG_SF4_SPART6L      = D_GRA "sf4_spart6L";
const char *IMG_SF4_SPART6Q      = D_GRA "sf4_spart6Q";
const char *IMG_SF4_SPART6S      = D_GRA "sf4_spart6S";
const char *IMG_SF4_SPART70      = D_GRA "sf4_spart70";
const char *IMG_SF4_SPART71      = D_GRA "sf4_spart71";
const char *IMG_SF4_SPART72      = D_GRA "sf4_spart72";
const char *IMG_SF4_SPART73      = D_GRA "sf4_spart73";
const char *IMG_SF4_SPART74      = D_GRA "sf4_spart74";
const char *IMG_SF4_SPART75      = D_GRA "sf4_spart75";
const char *IMG_SF4_SPART76      = D_GRA "sf4_spart76";
const char *IMG_SF4_SPART77      = D_GRA "sf4_spart77";
const char *IMG_SF4_SPART78      = D_GRA "sf4_spart78";
const char *IMG_SF4_SPART79      = D_GRA "sf4_spart79";
const char *IMG_SF4_SPART7C      = D_GRA "sf4_spart7C";
const char *IMG_SF4_SPART7E      = D_GRA "sf4_spart7E";
const char *IMG_SF4_SPART7G      = D_GRA "sf4_spart7G";
const char *IMG_SF4_SPART7L      = D_GRA "sf4_spart7L";
const char *IMG_SF4_SPART7Q      = D_GRA "sf4_spart7Q";
const char *IMG_SF4_SPART7S      = D_GRA "sf4_spart7S";
const char *IMG_SF4_SPART80      = D_GRA "sf4_spart80";
const char *IMG_SF4_SPART81      = D_GRA "sf4_spart81";
const char *IMG_SF4_SPART82      = D_GRA "sf4_spart82";
const char *IMG_SF4_SPART83      = D_GRA "sf4_spart83";
const char *IMG_SF4_SPART84      = D_GRA "sf4_spart84";
const char *IMG_SF4_SPART85      = D_GRA "sf4_spart85";
const char *IMG_SF4_SPART86      = D_GRA "sf4_spart86";
const char *IMG_SF4_SPART87      = D_GRA "sf4_spart87";
const char *IMG_SF4_SPART88      = D_GRA "sf4_spart88";
const char *IMG_SF4_SPART89      = D_GRA "sf4_spart89";
const char *IMG_SF4_SPART8C      = D_GRA "sf4_spart8C";
const char *IMG_SF4_SPART8E      = D_GRA "sf4_spart8E";
const char *IMG_SF4_SPART8G      = D_GRA "sf4_spart8G";
const char *IMG_SF4_SPART8L      = D_GRA "sf4_spart8L";
const char *IMG_SF4_SPART8Q      = D_GRA "sf4_spart8Q";
const char *IMG_SF4_SPART8S      = D_GRA "sf4_spart8S";
const char *IMG_SF4_SPART90      = D_GRA "sf4_spart90";
const char *IMG_SF4_SPART91      = D_GRA "sf4_spart91";
const char *IMG_SF4_SPART92      = D_GRA "sf4_spart92";
const char *IMG_SF4_SPART93      = D_GRA "sf4_spart93";
const char *IMG_SF4_SPART94      = D_GRA "sf4_spart94";
const char *IMG_SF4_SPART95      = D_GRA "sf4_spart95";
const char *IMG_SF4_SPART96      = D_GRA "sf4_spart96";
const char *IMG_SF4_SPART97      = D_GRA "sf4_spart97";
const char *IMG_SF4_SPART98      = D_GRA "sf4_spart98";
const char *IMG_SF4_SPART99      = D_GRA "sf4_spart99";
const char *IMG_SF4_SPART9C      = D_GRA "sf4_spart9C";
const char *IMG_SF4_SPART9E      = D_GRA "sf4_spart9E";
const char *IMG_SF4_SPART9G      = D_GRA "sf4_spart9G";
const char *IMG_SF4_SPART9L      = D_GRA "sf4_spart9L";
const char *IMG_SF4_SPART9Q      = D_GRA "sf4_spart9Q";
const char *IMG_SF4_SPART9S      = D_GRA "sf4_spart9S";
const char *IMG_SF4_SPARTC0      = D_GRA "sf4_spartC0";
const char *IMG_SF4_SPARTC1      = D_GRA "sf4_spartC1";
const char *IMG_SF4_SPARTC2      = D_GRA "sf4_spartC2";
const char *IMG_SF4_SPARTC3      = D_GRA "sf4_spartC3";
const char *IMG_SF4_SPARTC4      = D_GRA "sf4_spartC4";
const char *IMG_SF4_SPARTC5      = D_GRA "sf4_spartC5";
const char *IMG_SF4_SPARTC6      = D_GRA "sf4_spartC6";
const char *IMG_SF4_SPARTC7      = D_GRA "sf4_spartC7";
const char *IMG_SF4_SPARTC8      = D_GRA "sf4_spartC8";
const char *IMG_SF4_SPARTC9      = D_GRA "sf4_spartC9";
const char *IMG_SF4_SPARTCC      = D_GRA "sf4_spartCC";
const char *IMG_SF4_SPARTCE      = D_GRA "sf4_spartCE";
const char *IMG_SF4_SPARTCG      = D_GRA "sf4_spartCG";
const char *IMG_SF4_SPARTCL      = D_GRA "sf4_spartCL";
const char *IMG_SF4_SPARTCQ      = D_GRA "sf4_spartCQ";
const char *IMG_SF4_SPARTCS      = D_GRA "sf4_spartCS";
const char *IMG_SF4_SPARTE0      = D_GRA "sf4_spartE0";
const char *IMG_SF4_SPARTE1      = D_GRA "sf4_spartE1";
const char *IMG_SF4_SPARTE2      = D_GRA "sf4_spartE2";
const char *IMG_SF4_SPARTE3      = D_GRA "sf4_spartE3";
const char *IMG_SF4_SPARTE4      = D_GRA "sf4_spartE4";
const char *IMG_SF4_SPARTE5      = D_GRA "sf4_spartE5";
const char *IMG_SF4_SPARTE6      = D_GRA "sf4_spartE6";
const char *IMG_SF4_SPARTE7      = D_GRA "sf4_spartE7";
const char *IMG_SF4_SPARTE8      = D_GRA "sf4_spartE8";
const char *IMG_SF4_SPARTE9      = D_GRA "sf4_spartE9";
const char *IMG_SF4_SPARTEC      = D_GRA "sf4_spartEC";
const char *IMG_SF4_SPARTEE      = D_GRA "sf4_spartEE";
const char *IMG_SF4_SPARTEG      = D_GRA "sf4_spartEG";
const char *IMG_SF4_SPARTEL      = D_GRA "sf4_spartEL";
const char *IMG_SF4_SPARTEQ      = D_GRA "sf4_spartEQ";
const char *IMG_SF4_SPARTES      = D_GRA "sf4_spartES";
const char *IMG_SF4_SPARTG0      = D_GRA "sf4_spartG0";
const char *IMG_SF4_SPARTG1      = D_GRA "sf4_spartG1";
const char *IMG_SF4_SPARTG2      = D_GRA "sf4_spartG2";
const char *IMG_SF4_SPARTG3      = D_GRA "sf4_spartG3";
const char *IMG_SF4_SPARTG4      = D_GRA "sf4_spartG4";
const char *IMG_SF4_SPARTG5      = D_GRA "sf4_spartG5";
const char *IMG_SF4_SPARTG6      = D_GRA "sf4_spartG6";
const char *IMG_SF4_SPARTG7      = D_GRA "sf4_spartG7";
const char *IMG_SF4_SPARTG8      = D_GRA "sf4_spartG8";
const char *IMG_SF4_SPARTG9      = D_GRA "sf4_spartG9";
const char *IMG_SF4_SPARTGC      = D_GRA "sf4_spartGC";
const char *IMG_SF4_SPARTGE      = D_GRA "sf4_spartGE";
const char *IMG_SF4_SPARTGG      = D_GRA "sf4_spartGG";
const char *IMG_SF4_SPARTGL      = D_GRA "sf4_spartGL";
const char *IMG_SF4_SPARTGQ      = D_GRA "sf4_spartGQ";
const char *IMG_SF4_SPARTGS      = D_GRA "sf4_spartGS";
const char *IMG_SF4_SPARTL0      = D_GRA "sf4_spartL0";
const char *IMG_SF4_SPARTL1      = D_GRA "sf4_spartL1";
const char *IMG_SF4_SPARTL2      = D_GRA "sf4_spartL2";
const char *IMG_SF4_SPARTL3      = D_GRA "sf4_spartL3";
const char *IMG_SF4_SPARTL4      = D_GRA "sf4_spartL4";
const char *IMG_SF4_SPARTL5      = D_GRA "sf4_spartL5";
const char *IMG_SF4_SPARTL6      = D_GRA "sf4_spartL6";
const char *IMG_SF4_SPARTL7      = D_GRA "sf4_spartL7";
const char *IMG_SF4_SPARTL8      = D_GRA "sf4_spartL8";
const char *IMG_SF4_SPARTL9      = D_GRA "sf4_spartL9";
const char *IMG_SF4_SPARTLC      = D_GRA "sf4_spartLC";
const char *IMG_SF4_SPARTLE      = D_GRA "sf4_spartLE";
const char *IMG_SF4_SPARTLG      = D_GRA "sf4_spartLG";
const char *IMG_SF4_SPARTLL      = D_GRA "sf4_spartLL";
const char *IMG_SF4_SPARTLQ      = D_GRA "sf4_spartLQ";
const char *IMG_SF4_SPARTLS      = D_GRA "sf4_spartLS";
const char *IMG_SF4_SPARTQ0      = D_GRA "sf4_spartQ0";
const char *IMG_SF4_SPARTQ1      = D_GRA "sf4_spartQ1";
const char *IMG_SF4_SPARTQ2      = D_GRA "sf4_spartQ2";
const char *IMG_SF4_SPARTQ3      = D_GRA "sf4_spartQ3";
const char *IMG_SF4_SPARTQ4      = D_GRA "sf4_spartQ4";
const char *IMG_SF4_SPARTQ5      = D_GRA "sf4_spartQ5";
const char *IMG_SF4_SPARTQ6      = D_GRA "sf4_spartQ6";
const char *IMG_SF4_SPARTQ7      = D_GRA "sf4_spartQ7";
const char *IMG_SF4_SPARTQ8      = D_GRA "sf4_spartQ8";
const char *IMG_SF4_SPARTQ9      = D_GRA "sf4_spartQ9";
const char *IMG_SF4_SPARTQC      = D_GRA "sf4_spartQC";
const char *IMG_SF4_SPARTQE      = D_GRA "sf4_spartQE";
const char *IMG_SF4_SPARTQG      = D_GRA "sf4_spartQG";
const char *IMG_SF4_SPARTQL      = D_GRA "sf4_spartQL";
const char *IMG_SF4_SPARTQQ      = D_GRA "sf4_spartQQ";
const char *IMG_SF4_SPARTQS      = D_GRA "sf4_spartQS";
const char *IMG_SF4_SPARTS0      = D_GRA "sf4_spartS0";
const char *IMG_SF4_SPARTS1      = D_GRA "sf4_spartS1";
const char *IMG_SF4_SPARTS2      = D_GRA "sf4_spartS2";
const char *IMG_SF4_SPARTS3      = D_GRA "sf4_spartS3";
const char *IMG_SF4_SPARTS4      = D_GRA "sf4_spartS4";
const char *IMG_SF4_SPARTS5      = D_GRA "sf4_spartS5";
const char *IMG_SF4_SPARTS6      = D_GRA "sf4_spartS6";
const char *IMG_SF4_SPARTS7      = D_GRA "sf4_spartS7";
const char *IMG_SF4_SPARTS8      = D_GRA "sf4_spartS8";
const char *IMG_SF4_SPARTS9      = D_GRA "sf4_spartS9";
const char *IMG_SF4_SPARTSC      = D_GRA "sf4_spartSC";
const char *IMG_SF4_SPARTSE      = D_GRA "sf4_spartSE";
const char *IMG_SF4_SPARTSG      = D_GRA "sf4_spartSG";
const char *IMG_SF4_SPARTSL      = D_GRA "sf4_spartSL";
const char *IMG_SF4_SPARTSQ      = D_GRA "sf4_spartSQ";
const char *IMG_SF4_SPARTSS      = D_GRA "sf4_spartSS";
const char *IMG_SF4_STONE0       = D_GRA "sf4_stone0";
const char *IMG_SF4_STONE10      = D_GRA "sf4_stone10";
const char *IMG_SF4_STONE19_0    = D_GRA "sf4_stone19-0";
const char *IMG_SF4_STONE2_0     = D_GRA "sf4_stone2-0";
const char *IMG_SF4_STONE29BOLD  = D_GRA "sf4_stone29bold";
const char *IMG_SF4_STONE29B     = D_GRA "sf4_stone29b";
const char *IMG_SF4_STONE29OLD   = D_GRA "sf4_stone29old";
const char *IMG_SF4_STONE29      = D_GRA "sf4_stone29";
const char *IMG_SF4_STONE2       = D_GRA "sf4_stone2";
const char *IMG_SF4_STONE8OLD    = D_GRA "sf4_stone8old";
const char *IMG_SF4_STONE8       = D_GRA "sf4_stone8";
const char *IMG_SF4_STONE9OLD    = D_GRA "sf4_stone9old";
const char *IMG_SF4_STONE9       = D_GRA "sf4_stone9";
const char *IMG_SF5_SPART00      = D_GRA "sf5_spart00";
const char *IMG_SF5_SPART01      = D_GRA "sf5_spart01";
const char *IMG_SF5_SPART02      = D_GRA "sf5_spart02";
const char *IMG_SF5_SPART03      = D_GRA "sf5_spart03";
const char *IMG_SF5_SPART04      = D_GRA "sf5_spart04";
const char *IMG_SF5_SPART05      = D_GRA "sf5_spart05";
const char *IMG_SF5_SPART06      = D_GRA "sf5_spart06";
const char *IMG_SF5_SPART07      = D_GRA "sf5_spart07";
const char *IMG_SF5_SPART08      = D_GRA "sf5_spart08";
const char *IMG_SF5_SPART09      = D_GRA "sf5_spart09";
const char *IMG_SF5_SPART0C      = D_GRA "sf5_spart0C";
const char *IMG_SF5_SPART0E      = D_GRA "sf5_spart0E";
const char *IMG_SF5_SPART0G      = D_GRA "sf5_spart0G";
const char *IMG_SF5_SPART0L      = D_GRA "sf5_spart0L";
const char *IMG_SF5_SPART0Q      = D_GRA "sf5_spart0Q";
const char *IMG_SF5_SPART0S      = D_GRA "sf5_spart0S";
const char *IMG_SF5_SPART10      = D_GRA "sf5_spart10";
const char *IMG_SF5_SPART11      = D_GRA "sf5_spart11";
const char *IMG_SF5_SPART12      = D_GRA "sf5_spart12";
const char *IMG_SF5_SPART13      = D_GRA "sf5_spart13";
const char *IMG_SF5_SPART14      = D_GRA "sf5_spart14";
const char *IMG_SF5_SPART15      = D_GRA "sf5_spart15";
const char *IMG_SF5_SPART16      = D_GRA "sf5_spart16";
const char *IMG_SF5_SPART17      = D_GRA "sf5_spart17";
const char *IMG_SF5_SPART18      = D_GRA "sf5_spart18";
const char *IMG_SF5_SPART19      = D_GRA "sf5_spart19";
const char *IMG_SF5_SPART1C      = D_GRA "sf5_spart1C";
const char *IMG_SF5_SPART1E      = D_GRA "sf5_spart1E";
const char *IMG_SF5_SPART1G      = D_GRA "sf5_spart1G";
const char *IMG_SF5_SPART1L      = D_GRA "sf5_spart1L";
const char *IMG_SF5_SPART1Q      = D_GRA "sf5_spart1Q";
const char *IMG_SF5_SPART1S      = D_GRA "sf5_spart1S";
const char *IMG_SF5_SPART20      = D_GRA "sf5_spart20";
const char *IMG_SF5_SPART21      = D_GRA "sf5_spart21";
const char *IMG_SF5_SPART22      = D_GRA "sf5_spart22";
const char *IMG_SF5_SPART23      = D_GRA "sf5_spart23";
const char *IMG_SF5_SPART24      = D_GRA "sf5_spart24";
const char *IMG_SF5_SPART25      = D_GRA "sf5_spart25";
const char *IMG_SF5_SPART26      = D_GRA "sf5_spart26";
const char *IMG_SF5_SPART27      = D_GRA "sf5_spart27";
const char *IMG_SF5_SPART28      = D_GRA "sf5_spart28";
const char *IMG_SF5_SPART29      = D_GRA "sf5_spart29";
const char *IMG_SF5_SPART2C      = D_GRA "sf5_spart2C";
const char *IMG_SF5_SPART2E      = D_GRA "sf5_spart2E";
const char *IMG_SF5_SPART2G      = D_GRA "sf5_spart2G";
const char *IMG_SF5_SPART2L      = D_GRA "sf5_spart2L";
const char *IMG_SF5_SPART2Q      = D_GRA "sf5_spart2Q";
const char *IMG_SF5_SPART2S      = D_GRA "sf5_spart2S";
const char *IMG_SF5_SPART30      = D_GRA "sf5_spart30";
const char *IMG_SF5_SPART31      = D_GRA "sf5_spart31";
const char *IMG_SF5_SPART32      = D_GRA "sf5_spart32";
const char *IMG_SF5_SPART33      = D_GRA "sf5_spart33";
const char *IMG_SF5_SPART34      = D_GRA "sf5_spart34";
const char *IMG_SF5_SPART35      = D_GRA "sf5_spart35";
const char *IMG_SF5_SPART36      = D_GRA "sf5_spart36";
const char *IMG_SF5_SPART37      = D_GRA "sf5_spart37";
const char *IMG_SF5_SPART38      = D_GRA "sf5_spart38";
const char *IMG_SF5_SPART39      = D_GRA "sf5_spart39";
const char *IMG_SF5_SPART3C      = D_GRA "sf5_spart3C";
const char *IMG_SF5_SPART3E      = D_GRA "sf5_spart3E";
const char *IMG_SF5_SPART3G      = D_GRA "sf5_spart3G";
const char *IMG_SF5_SPART3L      = D_GRA "sf5_spart3L";
const char *IMG_SF5_SPART3Q      = D_GRA "sf5_spart3Q";
const char *IMG_SF5_SPART3S      = D_GRA "sf5_spart3S";
const char *IMG_SF5_SPART40      = D_GRA "sf5_spart40";
const char *IMG_SF5_SPART41      = D_GRA "sf5_spart41";
const char *IMG_SF5_SPART42      = D_GRA "sf5_spart42";
const char *IMG_SF5_SPART43      = D_GRA "sf5_spart43";
const char *IMG_SF5_SPART44      = D_GRA "sf5_spart44";
const char *IMG_SF5_SPART45      = D_GRA "sf5_spart45";
const char *IMG_SF5_SPART46      = D_GRA "sf5_spart46";
const char *IMG_SF5_SPART47      = D_GRA "sf5_spart47";
const char *IMG_SF5_SPART48      = D_GRA "sf5_spart48";
const char *IMG_SF5_SPART49      = D_GRA "sf5_spart49";
const char *IMG_SF5_SPART4C      = D_GRA "sf5_spart4C";
const char *IMG_SF5_SPART4E      = D_GRA "sf5_spart4E";
const char *IMG_SF5_SPART4G      = D_GRA "sf5_spart4G";
const char *IMG_SF5_SPART4L      = D_GRA "sf5_spart4L";
const char *IMG_SF5_SPART4Q      = D_GRA "sf5_spart4Q";
const char *IMG_SF5_SPART4S      = D_GRA "sf5_spart4S";
const char *IMG_SF5_SPART50      = D_GRA "sf5_spart50";
const char *IMG_SF5_SPART51      = D_GRA "sf5_spart51";
const char *IMG_SF5_SPART52      = D_GRA "sf5_spart52";
const char *IMG_SF5_SPART53      = D_GRA "sf5_spart53";
const char *IMG_SF5_SPART54      = D_GRA "sf5_spart54";
const char *IMG_SF5_SPART55      = D_GRA "sf5_spart55";
const char *IMG_SF5_SPART56      = D_GRA "sf5_spart56";
const char *IMG_SF5_SPART57      = D_GRA "sf5_spart57";
const char *IMG_SF5_SPART58      = D_GRA "sf5_spart58";
const char *IMG_SF5_SPART59      = D_GRA "sf5_spart59";
const char *IMG_SF5_SPART5C      = D_GRA "sf5_spart5C";
const char *IMG_SF5_SPART5E      = D_GRA "sf5_spart5E";
const char *IMG_SF5_SPART5G      = D_GRA "sf5_spart5G";
const char *IMG_SF5_SPART5L      = D_GRA "sf5_spart5L";
const char *IMG_SF5_SPART5Q      = D_GRA "sf5_spart5Q";
const char *IMG_SF5_SPART5S      = D_GRA "sf5_spart5S";
const char *IMG_SF5_SPART60      = D_GRA "sf5_spart60";
const char *IMG_SF5_SPART61      = D_GRA "sf5_spart61";
const char *IMG_SF5_SPART62      = D_GRA "sf5_spart62";
const char *IMG_SF5_SPART63      = D_GRA "sf5_spart63";
const char *IMG_SF5_SPART64      = D_GRA "sf5_spart64";
const char *IMG_SF5_SPART65      = D_GRA "sf5_spart65";
const char *IMG_SF5_SPART66      = D_GRA "sf5_spart66";
const char *IMG_SF5_SPART67      = D_GRA "sf5_spart67";
const char *IMG_SF5_SPART68      = D_GRA "sf5_spart68";
const char *IMG_SF5_SPART69      = D_GRA "sf5_spart69";
const char *IMG_SF5_SPART6C      = D_GRA "sf5_spart6C";
const char *IMG_SF5_SPART6E      = D_GRA "sf5_spart6E";
const char *IMG_SF5_SPART6G      = D_GRA "sf5_spart6G";
const char *IMG_SF5_SPART6L      = D_GRA "sf5_spart6L";
const char *IMG_SF5_SPART6Q      = D_GRA "sf5_spart6Q";
const char *IMG_SF5_SPART6S      = D_GRA "sf5_spart6S";
const char *IMG_SF5_SPART70      = D_GRA "sf5_spart70";
const char *IMG_SF5_SPART71      = D_GRA "sf5_spart71";
const char *IMG_SF5_SPART72      = D_GRA "sf5_spart72";
const char *IMG_SF5_SPART73      = D_GRA "sf5_spart73";
const char *IMG_SF5_SPART74      = D_GRA "sf5_spart74";
const char *IMG_SF5_SPART75      = D_GRA "sf5_spart75";
const char *IMG_SF5_SPART76      = D_GRA "sf5_spart76";
const char *IMG_SF5_SPART77      = D_GRA "sf5_spart77";
const char *IMG_SF5_SPART78      = D_GRA "sf5_spart78";
const char *IMG_SF5_SPART79      = D_GRA "sf5_spart79";
const char *IMG_SF5_SPART7C      = D_GRA "sf5_spart7C";
const char *IMG_SF5_SPART7E      = D_GRA "sf5_spart7E";
const char *IMG_SF5_SPART7G      = D_GRA "sf5_spart7G";
const char *IMG_SF5_SPART7L      = D_GRA "sf5_spart7L";
const char *IMG_SF5_SPART7Q      = D_GRA "sf5_spart7Q";
const char *IMG_SF5_SPART7S      = D_GRA "sf5_spart7S";
const char *IMG_SF5_SPART80      = D_GRA "sf5_spart80";
const char *IMG_SF5_SPART81      = D_GRA "sf5_spart81";
const char *IMG_SF5_SPART82      = D_GRA "sf5_spart82";
const char *IMG_SF5_SPART83      = D_GRA "sf5_spart83";
const char *IMG_SF5_SPART84      = D_GRA "sf5_spart84";
const char *IMG_SF5_SPART85      = D_GRA "sf5_spart85";
const char *IMG_SF5_SPART86      = D_GRA "sf5_spart86";
const char *IMG_SF5_SPART87      = D_GRA "sf5_spart87";
const char *IMG_SF5_SPART88      = D_GRA "sf5_spart88";
const char *IMG_SF5_SPART89      = D_GRA "sf5_spart89";
const char *IMG_SF5_SPART8C      = D_GRA "sf5_spart8C";
const char *IMG_SF5_SPART8E      = D_GRA "sf5_spart8E";
const char *IMG_SF5_SPART8G      = D_GRA "sf5_spart8G";
const char *IMG_SF5_SPART8L      = D_GRA "sf5_spart8L";
const char *IMG_SF5_SPART8Q      = D_GRA "sf5_spart8Q";
const char *IMG_SF5_SPART8S      = D_GRA "sf5_spart8S";
const char *IMG_SF5_SPART90      = D_GRA "sf5_spart90";
const char *IMG_SF5_SPART91      = D_GRA "sf5_spart91";
const char *IMG_SF5_SPART92      = D_GRA "sf5_spart92";
const char *IMG_SF5_SPART93      = D_GRA "sf5_spart93";
const char *IMG_SF5_SPART94      = D_GRA "sf5_spart94";
const char *IMG_SF5_SPART95      = D_GRA "sf5_spart95";
const char *IMG_SF5_SPART96      = D_GRA "sf5_spart96";
const char *IMG_SF5_SPART97      = D_GRA "sf5_spart97";
const char *IMG_SF5_SPART98      = D_GRA "sf5_spart98";
const char *IMG_SF5_SPART99      = D_GRA "sf5_spart99";
const char *IMG_SF5_SPART9C      = D_GRA "sf5_spart9C";
const char *IMG_SF5_SPART9E      = D_GRA "sf5_spart9E";
const char *IMG_SF5_SPART9G      = D_GRA "sf5_spart9G";
const char *IMG_SF5_SPART9L      = D_GRA "sf5_spart9L";
const char *IMG_SF5_SPART9Q      = D_GRA "sf5_spart9Q";
const char *IMG_SF5_SPART9S      = D_GRA "sf5_spart9S";
const char *IMG_SF5_SPARTC0      = D_GRA "sf5_spartC0";
const char *IMG_SF5_SPARTC1      = D_GRA "sf5_spartC1";
const char *IMG_SF5_SPARTC2      = D_GRA "sf5_spartC2";
const char *IMG_SF5_SPARTC3      = D_GRA "sf5_spartC3";
const char *IMG_SF5_SPARTC4      = D_GRA "sf5_spartC4";
const char *IMG_SF5_SPARTC5      = D_GRA "sf5_spartC5";
const char *IMG_SF5_SPARTC6      = D_GRA "sf5_spartC6";
const char *IMG_SF5_SPARTC7      = D_GRA "sf5_spartC7";
const char *IMG_SF5_SPARTC8      = D_GRA "sf5_spartC8";
const char *IMG_SF5_SPARTC9      = D_GRA "sf5_spartC9";
const char *IMG_SF5_SPARTCC      = D_GRA "sf5_spartCC";
const char *IMG_SF5_SPARTCE      = D_GRA "sf5_spartCE";
const char *IMG_SF5_SPARTCG      = D_GRA "sf5_spartCG";
const char *IMG_SF5_SPARTCL      = D_GRA "sf5_spartCL";
const char *IMG_SF5_SPARTCQ      = D_GRA "sf5_spartCQ";
const char *IMG_SF5_SPARTCS      = D_GRA "sf5_spartCS";
const char *IMG_SF5_SPARTE0      = D_GRA "sf5_spartE0";
const char *IMG_SF5_SPARTE1      = D_GRA "sf5_spartE1";
const char *IMG_SF5_SPARTE2      = D_GRA "sf5_spartE2";
const char *IMG_SF5_SPARTE3      = D_GRA "sf5_spartE3";
const char *IMG_SF5_SPARTE4      = D_GRA "sf5_spartE4";
const char *IMG_SF5_SPARTE5      = D_GRA "sf5_spartE5";
const char *IMG_SF5_SPARTE6      = D_GRA "sf5_spartE6";
const char *IMG_SF5_SPARTE7      = D_GRA "sf5_spartE7";
const char *IMG_SF5_SPARTE8      = D_GRA "sf5_spartE8";
const char *IMG_SF5_SPARTE9      = D_GRA "sf5_spartE9";
const char *IMG_SF5_SPARTEC      = D_GRA "sf5_spartEC";
const char *IMG_SF5_SPARTEE      = D_GRA "sf5_spartEE";
const char *IMG_SF5_SPARTEG      = D_GRA "sf5_spartEG";
const char *IMG_SF5_SPARTEL      = D_GRA "sf5_spartEL";
const char *IMG_SF5_SPARTEQ      = D_GRA "sf5_spartEQ";
const char *IMG_SF5_SPARTES      = D_GRA "sf5_spartES";
const char *IMG_SF5_SPARTG0      = D_GRA "sf5_spartG0";
const char *IMG_SF5_SPARTG1      = D_GRA "sf5_spartG1";
const char *IMG_SF5_SPARTG2      = D_GRA "sf5_spartG2";
const char *IMG_SF5_SPARTG3      = D_GRA "sf5_spartG3";
const char *IMG_SF5_SPARTG4      = D_GRA "sf5_spartG4";
const char *IMG_SF5_SPARTG5      = D_GRA "sf5_spartG5";
const char *IMG_SF5_SPARTG6      = D_GRA "sf5_spartG6";
const char *IMG_SF5_SPARTG7      = D_GRA "sf5_spartG7";
const char *IMG_SF5_SPARTG8      = D_GRA "sf5_spartG8";
const char *IMG_SF5_SPARTG9      = D_GRA "sf5_spartG9";
const char *IMG_SF5_SPARTGC      = D_GRA "sf5_spartGC";
const char *IMG_SF5_SPARTGE      = D_GRA "sf5_spartGE";
const char *IMG_SF5_SPARTGG      = D_GRA "sf5_spartGG";
const char *IMG_SF5_SPARTGL      = D_GRA "sf5_spartGL";
const char *IMG_SF5_SPARTGQ      = D_GRA "sf5_spartGQ";
const char *IMG_SF5_SPARTGS      = D_GRA "sf5_spartGS";
const char *IMG_SF5_SPARTL0      = D_GRA "sf5_spartL0";
const char *IMG_SF5_SPARTL1      = D_GRA "sf5_spartL1";
const char *IMG_SF5_SPARTL2      = D_GRA "sf5_spartL2";
const char *IMG_SF5_SPARTL3      = D_GRA "sf5_spartL3";
const char *IMG_SF5_SPARTL4      = D_GRA "sf5_spartL4";
const char *IMG_SF5_SPARTL5      = D_GRA "sf5_spartL5";
const char *IMG_SF5_SPARTL6      = D_GRA "sf5_spartL6";
const char *IMG_SF5_SPARTL7      = D_GRA "sf5_spartL7";
const char *IMG_SF5_SPARTL8      = D_GRA "sf5_spartL8";
const char *IMG_SF5_SPARTL9      = D_GRA "sf5_spartL9";
const char *IMG_SF5_SPARTLC      = D_GRA "sf5_spartLC";
const char *IMG_SF5_SPARTLE      = D_GRA "sf5_spartLE";
const char *IMG_SF5_SPARTLG      = D_GRA "sf5_spartLG";
const char *IMG_SF5_SPARTLL      = D_GRA "sf5_spartLL";
const char *IMG_SF5_SPARTLQ      = D_GRA "sf5_spartLQ";
const char *IMG_SF5_SPARTLS      = D_GRA "sf5_spartLS";
const char *IMG_SF5_SPARTQ0      = D_GRA "sf5_spartQ0";
const char *IMG_SF5_SPARTQ1      = D_GRA "sf5_spartQ1";
const char *IMG_SF5_SPARTQ2      = D_GRA "sf5_spartQ2";
const char *IMG_SF5_SPARTQ3      = D_GRA "sf5_spartQ3";
const char *IMG_SF5_SPARTQ4      = D_GRA "sf5_spartQ4";
const char *IMG_SF5_SPARTQ5      = D_GRA "sf5_spartQ5";
const char *IMG_SF5_SPARTQ6      = D_GRA "sf5_spartQ6";
const char *IMG_SF5_SPARTQ7      = D_GRA "sf5_spartQ7";
const char *IMG_SF5_SPARTQ8      = D_GRA "sf5_spartQ8";
const char *IMG_SF5_SPARTQ9      = D_GRA "sf5_spartQ9";
const char *IMG_SF5_SPARTQC      = D_GRA "sf5_spartQC";
const char *IMG_SF5_SPARTQE      = D_GRA "sf5_spartQE";
const char *IMG_SF5_SPARTQG      = D_GRA "sf5_spartQG";
const char *IMG_SF5_SPARTQL      = D_GRA "sf5_spartQL";
const char *IMG_SF5_SPARTQQ      = D_GRA "sf5_spartQQ";
const char *IMG_SF5_SPARTQS      = D_GRA "sf5_spartQS";
const char *IMG_SF5_SPARTS0      = D_GRA "sf5_spartS0";
const char *IMG_SF5_SPARTS1      = D_GRA "sf5_spartS1";
const char *IMG_SF5_SPARTS2      = D_GRA "sf5_spartS2";
const char *IMG_SF5_SPARTS3      = D_GRA "sf5_spartS3";
const char *IMG_SF5_SPARTS4      = D_GRA "sf5_spartS4";
const char *IMG_SF5_SPARTS5      = D_GRA "sf5_spartS5";
const char *IMG_SF5_SPARTS6      = D_GRA "sf5_spartS6";
const char *IMG_SF5_SPARTS7      = D_GRA "sf5_spartS7";
const char *IMG_SF5_SPARTS8      = D_GRA "sf5_spartS8";
const char *IMG_SF5_SPARTS9      = D_GRA "sf5_spartS9";
const char *IMG_SF5_SPARTSC      = D_GRA "sf5_spartSC";
const char *IMG_SF5_SPARTSE      = D_GRA "sf5_spartSE";
const char *IMG_SF5_SPARTSG      = D_GRA "sf5_spartSG";
const char *IMG_SF5_SPARTSL      = D_GRA "sf5_spartSL";
const char *IMG_SF5_SPARTSQ      = D_GRA "sf5_spartSQ";
const char *IMG_SF5_SPARTSS      = D_GRA "sf5_spartSS";
const char *IMG_SF5_STONE0       = D_GRA "sf5_stone0";
const char *IMG_SF5_STONE10      = D_GRA "sf5_stone10";
const char *IMG_SF5_STONE19_0    = D_GRA "sf5_stone19-0";
const char *IMG_SF5_STONE2_0     = D_GRA "sf5_stone2-0";
const char *IMG_SF5_STONE29B     = D_GRA "sf5_stone29b";
const char *IMG_SF5_STONE29      = D_GRA "sf5_stone29";
const char *IMG_SF5_STONE2       = D_GRA "sf5_stone2";
const char *IMG_SF5_STONE8       = D_GRA "sf5_stone8";
const char *IMG_SF5_STONE9       = D_GRA "sf5_stone9";
const char *IMG_SF6_SPART00      = D_GRA "sf6_spart00";
const char *IMG_SF6_SPART01      = D_GRA "sf6_spart01";
const char *IMG_SF6_SPART02      = D_GRA "sf6_spart02";
const char *IMG_SF6_SPART03      = D_GRA "sf6_spart03";
const char *IMG_SF6_SPART04      = D_GRA "sf6_spart04";
const char *IMG_SF6_SPART05      = D_GRA "sf6_spart05";
const char *IMG_SF6_SPART06      = D_GRA "sf6_spart06";
const char *IMG_SF6_SPART07      = D_GRA "sf6_spart07";
const char *IMG_SF6_SPART08      = D_GRA "sf6_spart08";
const char *IMG_SF6_SPART09      = D_GRA "sf6_spart09";
const char *IMG_SF6_SPART0C      = D_GRA "sf6_spart0C";
const char *IMG_SF6_SPART0E      = D_GRA "sf6_spart0E";
const char *IMG_SF6_SPART0G      = D_GRA "sf6_spart0G";
const char *IMG_SF6_SPART0L      = D_GRA "sf6_spart0L";
const char *IMG_SF6_SPART0Q      = D_GRA "sf6_spart0Q";
const char *IMG_SF6_SPART0S      = D_GRA "sf6_spart0S";
const char *IMG_SF6_SPART10      = D_GRA "sf6_spart10";
const char *IMG_SF6_SPART11      = D_GRA "sf6_spart11";
const char *IMG_SF6_SPART12      = D_GRA "sf6_spart12";
const char *IMG_SF6_SPART13      = D_GRA "sf6_spart13";
const char *IMG_SF6_SPART14      = D_GRA "sf6_spart14";
const char *IMG_SF6_SPART15      = D_GRA "sf6_spart15";
const char *IMG_SF6_SPART16      = D_GRA "sf6_spart16";
const char *IMG_SF6_SPART17      = D_GRA "sf6_spart17";
const char *IMG_SF6_SPART18      = D_GRA "sf6_spart18";
const char *IMG_SF6_SPART19      = D_GRA "sf6_spart19";
const char *IMG_SF6_SPART1C      = D_GRA "sf6_spart1C";
const char *IMG_SF6_SPART1E      = D_GRA "sf6_spart1E";
const char *IMG_SF6_SPART1G      = D_GRA "sf6_spart1G";
const char *IMG_SF6_SPART1L      = D_GRA "sf6_spart1L";
const char *IMG_SF6_SPART1Q      = D_GRA "sf6_spart1Q";
const char *IMG_SF6_SPART1S      = D_GRA "sf6_spart1S";
const char *IMG_SF6_SPART20      = D_GRA "sf6_spart20";
const char *IMG_SF6_SPART21      = D_GRA "sf6_spart21";
const char *IMG_SF6_SPART22      = D_GRA "sf6_spart22";
const char *IMG_SF6_SPART23      = D_GRA "sf6_spart23";
const char *IMG_SF6_SPART24      = D_GRA "sf6_spart24";
const char *IMG_SF6_SPART25      = D_GRA "sf6_spart25";
const char *IMG_SF6_SPART26      = D_GRA "sf6_spart26";
const char *IMG_SF6_SPART27      = D_GRA "sf6_spart27";
const char *IMG_SF6_SPART28      = D_GRA "sf6_spart28";
const char *IMG_SF6_SPART29      = D_GRA "sf6_spart29";
const char *IMG_SF6_SPART2C      = D_GRA "sf6_spart2C";
const char *IMG_SF6_SPART2E      = D_GRA "sf6_spart2E";
const char *IMG_SF6_SPART2G      = D_GRA "sf6_spart2G";
const char *IMG_SF6_SPART2L      = D_GRA "sf6_spart2L";
const char *IMG_SF6_SPART2Q      = D_GRA "sf6_spart2Q";
const char *IMG_SF6_SPART2S      = D_GRA "sf6_spart2S";
const char *IMG_SF6_SPART30      = D_GRA "sf6_spart30";
const char *IMG_SF6_SPART31      = D_GRA "sf6_spart31";
const char *IMG_SF6_SPART32      = D_GRA "sf6_spart32";
const char *IMG_SF6_SPART33      = D_GRA "sf6_spart33";
const char *IMG_SF6_SPART34      = D_GRA "sf6_spart34";
const char *IMG_SF6_SPART35      = D_GRA "sf6_spart35";
const char *IMG_SF6_SPART36      = D_GRA "sf6_spart36";
const char *IMG_SF6_SPART37      = D_GRA "sf6_spart37";
const char *IMG_SF6_SPART38      = D_GRA "sf6_spart38";
const char *IMG_SF6_SPART39      = D_GRA "sf6_spart39";
const char *IMG_SF6_SPART3C      = D_GRA "sf6_spart3C";
const char *IMG_SF6_SPART3E      = D_GRA "sf6_spart3E";
const char *IMG_SF6_SPART3G      = D_GRA "sf6_spart3G";
const char *IMG_SF6_SPART3L      = D_GRA "sf6_spart3L";
const char *IMG_SF6_SPART3Q      = D_GRA "sf6_spart3Q";
const char *IMG_SF6_SPART3S      = D_GRA "sf6_spart3S";
const char *IMG_SF6_SPART40      = D_GRA "sf6_spart40";
const char *IMG_SF6_SPART41      = D_GRA "sf6_spart41";
const char *IMG_SF6_SPART42      = D_GRA "sf6_spart42";
const char *IMG_SF6_SPART43      = D_GRA "sf6_spart43";
const char *IMG_SF6_SPART44      = D_GRA "sf6_spart44";
const char *IMG_SF6_SPART45      = D_GRA "sf6_spart45";
const char *IMG_SF6_SPART46      = D_GRA "sf6_spart46";
const char *IMG_SF6_SPART47      = D_GRA "sf6_spart47";
const char *IMG_SF6_SPART48      = D_GRA "sf6_spart48";
const char *IMG_SF6_SPART49      = D_GRA "sf6_spart49";
const char *IMG_SF6_SPART4C      = D_GRA "sf6_spart4C";
const char *IMG_SF6_SPART4E      = D_GRA "sf6_spart4E";
const char *IMG_SF6_SPART4G      = D_GRA "sf6_spart4G";
const char *IMG_SF6_SPART4L      = D_GRA "sf6_spart4L";
const char *IMG_SF6_SPART4Q      = D_GRA "sf6_spart4Q";
const char *IMG_SF6_SPART4S      = D_GRA "sf6_spart4S";
const char *IMG_SF6_SPART50      = D_GRA "sf6_spart50";
const char *IMG_SF6_SPART51      = D_GRA "sf6_spart51";
const char *IMG_SF6_SPART52      = D_GRA "sf6_spart52";
const char *IMG_SF6_SPART53      = D_GRA "sf6_spart53";
const char *IMG_SF6_SPART54      = D_GRA "sf6_spart54";
const char *IMG_SF6_SPART55      = D_GRA "sf6_spart55";
const char *IMG_SF6_SPART56      = D_GRA "sf6_spart56";
const char *IMG_SF6_SPART57      = D_GRA "sf6_spart57";
const char *IMG_SF6_SPART58      = D_GRA "sf6_spart58";
const char *IMG_SF6_SPART59      = D_GRA "sf6_spart59";
const char *IMG_SF6_SPART5C      = D_GRA "sf6_spart5C";
const char *IMG_SF6_SPART5E      = D_GRA "sf6_spart5E";
const char *IMG_SF6_SPART5G      = D_GRA "sf6_spart5G";
const char *IMG_SF6_SPART5L      = D_GRA "sf6_spart5L";
const char *IMG_SF6_SPART5Q      = D_GRA "sf6_spart5Q";
const char *IMG_SF6_SPART5S      = D_GRA "sf6_spart5S";
const char *IMG_SF6_SPART60      = D_GRA "sf6_spart60";
const char *IMG_SF6_SPART61      = D_GRA "sf6_spart61";
const char *IMG_SF6_SPART62      = D_GRA "sf6_spart62";
const char *IMG_SF6_SPART63      = D_GRA "sf6_spart63";
const char *IMG_SF6_SPART64      = D_GRA "sf6_spart64";
const char *IMG_SF6_SPART65      = D_GRA "sf6_spart65";
const char *IMG_SF6_SPART66      = D_GRA "sf6_spart66";
const char *IMG_SF6_SPART67      = D_GRA "sf6_spart67";
const char *IMG_SF6_SPART68      = D_GRA "sf6_spart68";
const char *IMG_SF6_SPART69      = D_GRA "sf6_spart69";
const char *IMG_SF6_SPART6C      = D_GRA "sf6_spart6C";
const char *IMG_SF6_SPART6E      = D_GRA "sf6_spart6E";
const char *IMG_SF6_SPART6G      = D_GRA "sf6_spart6G";
const char *IMG_SF6_SPART6L      = D_GRA "sf6_spart6L";
const char *IMG_SF6_SPART6Q      = D_GRA "sf6_spart6Q";
const char *IMG_SF6_SPART6S      = D_GRA "sf6_spart6S";
const char *IMG_SF6_SPART70      = D_GRA "sf6_spart70";
const char *IMG_SF6_SPART71      = D_GRA "sf6_spart71";
const char *IMG_SF6_SPART72      = D_GRA "sf6_spart72";
const char *IMG_SF6_SPART73      = D_GRA "sf6_spart73";
const char *IMG_SF6_SPART74      = D_GRA "sf6_spart74";
const char *IMG_SF6_SPART75      = D_GRA "sf6_spart75";
const char *IMG_SF6_SPART76      = D_GRA "sf6_spart76";
const char *IMG_SF6_SPART77      = D_GRA "sf6_spart77";
const char *IMG_SF6_SPART78      = D_GRA "sf6_spart78";
const char *IMG_SF6_SPART79      = D_GRA "sf6_spart79";
const char *IMG_SF6_SPART7C      = D_GRA "sf6_spart7C";
const char *IMG_SF6_SPART7E      = D_GRA "sf6_spart7E";
const char *IMG_SF6_SPART7G      = D_GRA "sf6_spart7G";
const char *IMG_SF6_SPART7L      = D_GRA "sf6_spart7L";
const char *IMG_SF6_SPART7Q      = D_GRA "sf6_spart7Q";
const char *IMG_SF6_SPART7S      = D_GRA "sf6_spart7S";
const char *IMG_SF6_SPART80      = D_GRA "sf6_spart80";
const char *IMG_SF6_SPART81      = D_GRA "sf6_spart81";
const char *IMG_SF6_SPART82      = D_GRA "sf6_spart82";
const char *IMG_SF6_SPART83      = D_GRA "sf6_spart83";
const char *IMG_SF6_SPART84      = D_GRA "sf6_spart84";
const char *IMG_SF6_SPART85      = D_GRA "sf6_spart85";
const char *IMG_SF6_SPART86      = D_GRA "sf6_spart86";
const char *IMG_SF6_SPART87      = D_GRA "sf6_spart87";
const char *IMG_SF6_SPART88      = D_GRA "sf6_spart88";
const char *IMG_SF6_SPART89      = D_GRA "sf6_spart89";
const char *IMG_SF6_SPART8C      = D_GRA "sf6_spart8C";
const char *IMG_SF6_SPART8E      = D_GRA "sf6_spart8E";
const char *IMG_SF6_SPART8G      = D_GRA "sf6_spart8G";
const char *IMG_SF6_SPART8L      = D_GRA "sf6_spart8L";
const char *IMG_SF6_SPART8Q      = D_GRA "sf6_spart8Q";
const char *IMG_SF6_SPART8S      = D_GRA "sf6_spart8S";
const char *IMG_SF6_SPART90      = D_GRA "sf6_spart90";
const char *IMG_SF6_SPART91      = D_GRA "sf6_spart91";
const char *IMG_SF6_SPART92      = D_GRA "sf6_spart92";
const char *IMG_SF6_SPART93      = D_GRA "sf6_spart93";
const char *IMG_SF6_SPART94      = D_GRA "sf6_spart94";
const char *IMG_SF6_SPART95      = D_GRA "sf6_spart95";
const char *IMG_SF6_SPART96      = D_GRA "sf6_spart96";
const char *IMG_SF6_SPART97      = D_GRA "sf6_spart97";
const char *IMG_SF6_SPART98      = D_GRA "sf6_spart98";
const char *IMG_SF6_SPART99      = D_GRA "sf6_spart99";
const char *IMG_SF6_SPART9C      = D_GRA "sf6_spart9C";
const char *IMG_SF6_SPART9E      = D_GRA "sf6_spart9E";
const char *IMG_SF6_SPART9G      = D_GRA "sf6_spart9G";
const char *IMG_SF6_SPART9L      = D_GRA "sf6_spart9L";
const char *IMG_SF6_SPART9Q      = D_GRA "sf6_spart9Q";
const char *IMG_SF6_SPART9S      = D_GRA "sf6_spart9S";
const char *IMG_SF6_SPARTC0      = D_GRA "sf6_spartC0";
const char *IMG_SF6_SPARTC1      = D_GRA "sf6_spartC1";
const char *IMG_SF6_SPARTC2      = D_GRA "sf6_spartC2";
const char *IMG_SF6_SPARTC3      = D_GRA "sf6_spartC3";
const char *IMG_SF6_SPARTC4      = D_GRA "sf6_spartC4";
const char *IMG_SF6_SPARTC5      = D_GRA "sf6_spartC5";
const char *IMG_SF6_SPARTC6      = D_GRA "sf6_spartC6";
const char *IMG_SF6_SPARTC7      = D_GRA "sf6_spartC7";
const char *IMG_SF6_SPARTC8      = D_GRA "sf6_spartC8";
const char *IMG_SF6_SPARTC9      = D_GRA "sf6_spartC9";
const char *IMG_SF6_SPARTCC      = D_GRA "sf6_spartCC";
const char *IMG_SF6_SPARTCE      = D_GRA "sf6_spartCE";
const char *IMG_SF6_SPARTCG      = D_GRA "sf6_spartCG";
const char *IMG_SF6_SPARTCL      = D_GRA "sf6_spartCL";
const char *IMG_SF6_SPARTCQ      = D_GRA "sf6_spartCQ";
const char *IMG_SF6_SPARTCS      = D_GRA "sf6_spartCS";
const char *IMG_SF6_SPARTE0      = D_GRA "sf6_spartE0";
const char *IMG_SF6_SPARTE1      = D_GRA "sf6_spartE1";
const char *IMG_SF6_SPARTE2      = D_GRA "sf6_spartE2";
const char *IMG_SF6_SPARTE3      = D_GRA "sf6_spartE3";
const char *IMG_SF6_SPARTE4      = D_GRA "sf6_spartE4";
const char *IMG_SF6_SPARTE5      = D_GRA "sf6_spartE5";
const char *IMG_SF6_SPARTE6      = D_GRA "sf6_spartE6";
const char *IMG_SF6_SPARTE7      = D_GRA "sf6_spartE7";
const char *IMG_SF6_SPARTE8      = D_GRA "sf6_spartE8";
const char *IMG_SF6_SPARTE9      = D_GRA "sf6_spartE9";
const char *IMG_SF6_SPARTEC      = D_GRA "sf6_spartEC";
const char *IMG_SF6_SPARTEE      = D_GRA "sf6_spartEE";
const char *IMG_SF6_SPARTEG      = D_GRA "sf6_spartEG";
const char *IMG_SF6_SPARTEL      = D_GRA "sf6_spartEL";
const char *IMG_SF6_SPARTEQ      = D_GRA "sf6_spartEQ";
const char *IMG_SF6_SPARTES      = D_GRA "sf6_spartES";
const char *IMG_SF6_SPARTG0      = D_GRA "sf6_spartG0";
const char *IMG_SF6_SPARTG1      = D_GRA "sf6_spartG1";
const char *IMG_SF6_SPARTG2      = D_GRA "sf6_spartG2";
const char *IMG_SF6_SPARTG3      = D_GRA "sf6_spartG3";
const char *IMG_SF6_SPARTG4      = D_GRA "sf6_spartG4";
const char *IMG_SF6_SPARTG5      = D_GRA "sf6_spartG5";
const char *IMG_SF6_SPARTG6      = D_GRA "sf6_spartG6";
const char *IMG_SF6_SPARTG7      = D_GRA "sf6_spartG7";
const char *IMG_SF6_SPARTG8      = D_GRA "sf6_spartG8";
const char *IMG_SF6_SPARTG9      = D_GRA "sf6_spartG9";
const char *IMG_SF6_SPARTGC      = D_GRA "sf6_spartGC";
const char *IMG_SF6_SPARTGE      = D_GRA "sf6_spartGE";
const char *IMG_SF6_SPARTGG      = D_GRA "sf6_spartGG";
const char *IMG_SF6_SPARTGL      = D_GRA "sf6_spartGL";
const char *IMG_SF6_SPARTGQ      = D_GRA "sf6_spartGQ";
const char *IMG_SF6_SPARTGS      = D_GRA "sf6_spartGS";
const char *IMG_SF6_SPARTL0      = D_GRA "sf6_spartL0";
const char *IMG_SF6_SPARTL1      = D_GRA "sf6_spartL1";
const char *IMG_SF6_SPARTL2      = D_GRA "sf6_spartL2";
const char *IMG_SF6_SPARTL3      = D_GRA "sf6_spartL3";
const char *IMG_SF6_SPARTL4      = D_GRA "sf6_spartL4";
const char *IMG_SF6_SPARTL5      = D_GRA "sf6_spartL5";
const char *IMG_SF6_SPARTL6      = D_GRA "sf6_spartL6";
const char *IMG_SF6_SPARTL7      = D_GRA "sf6_spartL7";
const char *IMG_SF6_SPARTL8      = D_GRA "sf6_spartL8";
const char *IMG_SF6_SPARTL9      = D_GRA "sf6_spartL9";
const char *IMG_SF6_SPARTLC      = D_GRA "sf6_spartLC";
const char *IMG_SF6_SPARTLE      = D_GRA "sf6_spartLE";
const char *IMG_SF6_SPARTLG      = D_GRA "sf6_spartLG";
const char *IMG_SF6_SPARTLL      = D_GRA "sf6_spartLL";
const char *IMG_SF6_SPARTLQ      = D_GRA "sf6_spartLQ";
const char *IMG_SF6_SPARTLS      = D_GRA "sf6_spartLS";
const char *IMG_SF6_SPARTQ0      = D_GRA "sf6_spartQ0";
const char *IMG_SF6_SPARTQ1      = D_GRA "sf6_spartQ1";
const char *IMG_SF6_SPARTQ2      = D_GRA "sf6_spartQ2";
const char *IMG_SF6_SPARTQ3      = D_GRA "sf6_spartQ3";
const char *IMG_SF6_SPARTQ4      = D_GRA "sf6_spartQ4";
const char *IMG_SF6_SPARTQ5      = D_GRA "sf6_spartQ5";
const char *IMG_SF6_SPARTQ6      = D_GRA "sf6_spartQ6";
const char *IMG_SF6_SPARTQ7      = D_GRA "sf6_spartQ7";
const char *IMG_SF6_SPARTQ8      = D_GRA "sf6_spartQ8";
const char *IMG_SF6_SPARTQ9      = D_GRA "sf6_spartQ9";
const char *IMG_SF6_SPARTQC      = D_GRA "sf6_spartQC";
const char *IMG_SF6_SPARTQE      = D_GRA "sf6_spartQE";
const char *IMG_SF6_SPARTQG      = D_GRA "sf6_spartQG";
const char *IMG_SF6_SPARTQL      = D_GRA "sf6_spartQL";
const char *IMG_SF6_SPARTQQ      = D_GRA "sf6_spartQQ";
const char *IMG_SF6_SPARTQS      = D_GRA "sf6_spartQS";
const char *IMG_SF6_SPARTS0      = D_GRA "sf6_spartS0";
const char *IMG_SF6_SPARTS1      = D_GRA "sf6_spartS1";
const char *IMG_SF6_SPARTS2      = D_GRA "sf6_spartS2";
const char *IMG_SF6_SPARTS3      = D_GRA "sf6_spartS3";
const char *IMG_SF6_SPARTS4      = D_GRA "sf6_spartS4";
const char *IMG_SF6_SPARTS5      = D_GRA "sf6_spartS5";
const char *IMG_SF6_SPARTS6      = D_GRA "sf6_spartS6";
const char *IMG_SF6_SPARTS7      = D_GRA "sf6_spartS7";
const char *IMG_SF6_SPARTS8      = D_GRA "sf6_spartS8";
const char *IMG_SF6_SPARTS9      = D_GRA "sf6_spartS9";
const char *IMG_SF6_SPARTSC      = D_GRA "sf6_spartSC";
const char *IMG_SF6_SPARTSE      = D_GRA "sf6_spartSE";
const char *IMG_SF6_SPARTSG      = D_GRA "sf6_spartSG";
const char *IMG_SF6_SPARTSL      = D_GRA "sf6_spartSL";
const char *IMG_SF6_SPARTSQ      = D_GRA "sf6_spartSQ";
const char *IMG_SF6_SPARTSS      = D_GRA "sf6_spartSS";
const char *IMG_SF6_STONE0       = D_GRA "sf6_stone0";
const char *IMG_SF6_STONE10      = D_GRA "sf6_stone10";
const char *IMG_SF6_STONE19_0    = D_GRA "sf6_stone19-0";
const char *IMG_SF6_STONE2_0     = D_GRA "sf6_stone2-0";
const char *IMG_SF6_STONE24      = D_GRA "sf6_stone24";
const char *IMG_SF6_STONE25      = D_GRA "sf6_stone25";
const char *IMG_SF6_STONE29B     = D_GRA "sf6_stone29b";
const char *IMG_SF6_STONE29      = D_GRA "sf6_stone29";
const char *IMG_SF6_STONE2       = D_GRA "sf6_stone2";
const char *IMG_SF6_STONE8       = D_GRA "sf6_stone8";
const char *IMG_SF6_STONE9       = D_GRA "sf6_stone9";
const char *IMG_SF8_SPART00      = D_GRA "sf8_spart00";
const char *IMG_SF8_SPART01      = D_GRA "sf8_spart01";
const char *IMG_SF8_SPART02      = D_GRA "sf8_spart02";
const char *IMG_SF8_SPART03      = D_GRA "sf8_spart03";
const char *IMG_SF8_SPART04      = D_GRA "sf8_spart04";
const char *IMG_SF8_SPART05      = D_GRA "sf8_spart05";
const char *IMG_SF8_SPART06      = D_GRA "sf8_spart06";
const char *IMG_SF8_SPART07      = D_GRA "sf8_spart07";
const char *IMG_SF8_SPART08      = D_GRA "sf8_spart08";
const char *IMG_SF8_SPART09      = D_GRA "sf8_spart09";
const char *IMG_SF8_SPART0C      = D_GRA "sf8_spart0C";
const char *IMG_SF8_SPART0E      = D_GRA "sf8_spart0E";
const char *IMG_SF8_SPART0G      = D_GRA "sf8_spart0G";
const char *IMG_SF8_SPART0L      = D_GRA "sf8_spart0L";
const char *IMG_SF8_SPART0Q      = D_GRA "sf8_spart0Q";
const char *IMG_SF8_SPART0S      = D_GRA "sf8_spart0S";
const char *IMG_SF8_SPART10      = D_GRA "sf8_spart10";
const char *IMG_SF8_SPART11      = D_GRA "sf8_spart11";
const char *IMG_SF8_SPART12      = D_GRA "sf8_spart12";
const char *IMG_SF8_SPART13      = D_GRA "sf8_spart13";
const char *IMG_SF8_SPART14      = D_GRA "sf8_spart14";
const char *IMG_SF8_SPART15      = D_GRA "sf8_spart15";
const char *IMG_SF8_SPART16      = D_GRA "sf8_spart16";
const char *IMG_SF8_SPART17      = D_GRA "sf8_spart17";
const char *IMG_SF8_SPART18      = D_GRA "sf8_spart18";
const char *IMG_SF8_SPART19      = D_GRA "sf8_spart19";
const char *IMG_SF8_SPART1C      = D_GRA "sf8_spart1C";
const char *IMG_SF8_SPART1E      = D_GRA "sf8_spart1E";
const char *IMG_SF8_SPART1G      = D_GRA "sf8_spart1G";
const char *IMG_SF8_SPART1L      = D_GRA "sf8_spart1L";
const char *IMG_SF8_SPART1Q      = D_GRA "sf8_spart1Q";
const char *IMG_SF8_SPART1S      = D_GRA "sf8_spart1S";
const char *IMG_SF8_SPART20      = D_GRA "sf8_spart20";
const char *IMG_SF8_SPART21      = D_GRA "sf8_spart21";
const char *IMG_SF8_SPART22      = D_GRA "sf8_spart22";
const char *IMG_SF8_SPART23      = D_GRA "sf8_spart23";
const char *IMG_SF8_SPART24      = D_GRA "sf8_spart24";
const char *IMG_SF8_SPART25      = D_GRA "sf8_spart25";
const char *IMG_SF8_SPART26      = D_GRA "sf8_spart26";
const char *IMG_SF8_SPART27      = D_GRA "sf8_spart27";
const char *IMG_SF8_SPART28      = D_GRA "sf8_spart28";
const char *IMG_SF8_SPART29      = D_GRA "sf8_spart29";
const char *IMG_SF8_SPART2C      = D_GRA "sf8_spart2C";
const char *IMG_SF8_SPART2E      = D_GRA "sf8_spart2E";
const char *IMG_SF8_SPART2G      = D_GRA "sf8_spart2G";
const char *IMG_SF8_SPART2L      = D_GRA "sf8_spart2L";
const char *IMG_SF8_SPART2Q      = D_GRA "sf8_spart2Q";
const char *IMG_SF8_SPART2S      = D_GRA "sf8_spart2S";
const char *IMG_SF8_SPART30      = D_GRA "sf8_spart30";
const char *IMG_SF8_SPART31      = D_GRA "sf8_spart31";
const char *IMG_SF8_SPART32      = D_GRA "sf8_spart32";
const char *IMG_SF8_SPART33      = D_GRA "sf8_spart33";
const char *IMG_SF8_SPART34      = D_GRA "sf8_spart34";
const char *IMG_SF8_SPART35      = D_GRA "sf8_spart35";
const char *IMG_SF8_SPART36      = D_GRA "sf8_spart36";
const char *IMG_SF8_SPART37      = D_GRA "sf8_spart37";
const char *IMG_SF8_SPART38      = D_GRA "sf8_spart38";
const char *IMG_SF8_SPART39      = D_GRA "sf8_spart39";
const char *IMG_SF8_SPART3C      = D_GRA "sf8_spart3C";
const char *IMG_SF8_SPART3E      = D_GRA "sf8_spart3E";
const char *IMG_SF8_SPART3G      = D_GRA "sf8_spart3G";
const char *IMG_SF8_SPART3L      = D_GRA "sf8_spart3L";
const char *IMG_SF8_SPART3Q      = D_GRA "sf8_spart3Q";
const char *IMG_SF8_SPART3S      = D_GRA "sf8_spart3S";
const char *IMG_SF8_SPART40      = D_GRA "sf8_spart40";
const char *IMG_SF8_SPART41      = D_GRA "sf8_spart41";
const char *IMG_SF8_SPART42      = D_GRA "sf8_spart42";
const char *IMG_SF8_SPART43      = D_GRA "sf8_spart43";
const char *IMG_SF8_SPART44      = D_GRA "sf8_spart44";
const char *IMG_SF8_SPART45      = D_GRA "sf8_spart45";
const char *IMG_SF8_SPART46      = D_GRA "sf8_spart46";
const char *IMG_SF8_SPART47      = D_GRA "sf8_spart47";
const char *IMG_SF8_SPART48      = D_GRA "sf8_spart48";
const char *IMG_SF8_SPART49      = D_GRA "sf8_spart49";
const char *IMG_SF8_SPART4C      = D_GRA "sf8_spart4C";
const char *IMG_SF8_SPART4E      = D_GRA "sf8_spart4E";
const char *IMG_SF8_SPART4G      = D_GRA "sf8_spart4G";
const char *IMG_SF8_SPART4L      = D_GRA "sf8_spart4L";
const char *IMG_SF8_SPART4Q      = D_GRA "sf8_spart4Q";
const char *IMG_SF8_SPART4S      = D_GRA "sf8_spart4S";
const char *IMG_SF8_SPART50      = D_GRA "sf8_spart50";
const char *IMG_SF8_SPART51      = D_GRA "sf8_spart51";
const char *IMG_SF8_SPART52      = D_GRA "sf8_spart52";
const char *IMG_SF8_SPART53      = D_GRA "sf8_spart53";
const char *IMG_SF8_SPART54      = D_GRA "sf8_spart54";
const char *IMG_SF8_SPART55      = D_GRA "sf8_spart55";
const char *IMG_SF8_SPART56      = D_GRA "sf8_spart56";
const char *IMG_SF8_SPART57      = D_GRA "sf8_spart57";
const char *IMG_SF8_SPART58      = D_GRA "sf8_spart58";
const char *IMG_SF8_SPART59      = D_GRA "sf8_spart59";
const char *IMG_SF8_SPART5C      = D_GRA "sf8_spart5C";
const char *IMG_SF8_SPART5E      = D_GRA "sf8_spart5E";
const char *IMG_SF8_SPART5G      = D_GRA "sf8_spart5G";
const char *IMG_SF8_SPART5L      = D_GRA "sf8_spart5L";
const char *IMG_SF8_SPART5Q      = D_GRA "sf8_spart5Q";
const char *IMG_SF8_SPART5S      = D_GRA "sf8_spart5S";
const char *IMG_SF8_SPART60      = D_GRA "sf8_spart60";
const char *IMG_SF8_SPART61      = D_GRA "sf8_spart61";
const char *IMG_SF8_SPART62      = D_GRA "sf8_spart62";
const char *IMG_SF8_SPART63      = D_GRA "sf8_spart63";
const char *IMG_SF8_SPART64      = D_GRA "sf8_spart64";
const char *IMG_SF8_SPART65      = D_GRA "sf8_spart65";
const char *IMG_SF8_SPART66      = D_GRA "sf8_spart66";
const char *IMG_SF8_SPART67      = D_GRA "sf8_spart67";
const char *IMG_SF8_SPART68      = D_GRA "sf8_spart68";
const char *IMG_SF8_SPART69      = D_GRA "sf8_spart69";
const char *IMG_SF8_SPART6C      = D_GRA "sf8_spart6C";
const char *IMG_SF8_SPART6E      = D_GRA "sf8_spart6E";
const char *IMG_SF8_SPART6G      = D_GRA "sf8_spart6G";
const char *IMG_SF8_SPART6L      = D_GRA "sf8_spart6L";
const char *IMG_SF8_SPART6Q      = D_GRA "sf8_spart6Q";
const char *IMG_SF8_SPART6S      = D_GRA "sf8_spart6S";
const char *IMG_SF8_SPART70      = D_GRA "sf8_spart70";
const char *IMG_SF8_SPART71      = D_GRA "sf8_spart71";
const char *IMG_SF8_SPART72      = D_GRA "sf8_spart72";
const char *IMG_SF8_SPART73      = D_GRA "sf8_spart73";
const char *IMG_SF8_SPART74      = D_GRA "sf8_spart74";
const char *IMG_SF8_SPART75      = D_GRA "sf8_spart75";
const char *IMG_SF8_SPART76      = D_GRA "sf8_spart76";
const char *IMG_SF8_SPART77      = D_GRA "sf8_spart77";
const char *IMG_SF8_SPART78      = D_GRA "sf8_spart78";
const char *IMG_SF8_SPART79      = D_GRA "sf8_spart79";
const char *IMG_SF8_SPART7C      = D_GRA "sf8_spart7C";
const char *IMG_SF8_SPART7E      = D_GRA "sf8_spart7E";
const char *IMG_SF8_SPART7G      = D_GRA "sf8_spart7G";
const char *IMG_SF8_SPART7L      = D_GRA "sf8_spart7L";
const char *IMG_SF8_SPART7Q      = D_GRA "sf8_spart7Q";
const char *IMG_SF8_SPART7S      = D_GRA "sf8_spart7S";
const char *IMG_SF8_SPART80      = D_GRA "sf8_spart80";
const char *IMG_SF8_SPART81      = D_GRA "sf8_spart81";
const char *IMG_SF8_SPART82      = D_GRA "sf8_spart82";
const char *IMG_SF8_SPART83      = D_GRA "sf8_spart83";
const char *IMG_SF8_SPART84      = D_GRA "sf8_spart84";
const char *IMG_SF8_SPART85      = D_GRA "sf8_spart85";
const char *IMG_SF8_SPART86      = D_GRA "sf8_spart86";
const char *IMG_SF8_SPART87      = D_GRA "sf8_spart87";
const char *IMG_SF8_SPART88      = D_GRA "sf8_spart88";
const char *IMG_SF8_SPART89      = D_GRA "sf8_spart89";
const char *IMG_SF8_SPART8C      = D_GRA "sf8_spart8C";
const char *IMG_SF8_SPART8E      = D_GRA "sf8_spart8E";
const char *IMG_SF8_SPART8G      = D_GRA "sf8_spart8G";
const char *IMG_SF8_SPART8L      = D_GRA "sf8_spart8L";
const char *IMG_SF8_SPART8Q      = D_GRA "sf8_spart8Q";
const char *IMG_SF8_SPART8S      = D_GRA "sf8_spart8S";
const char *IMG_SF8_SPART90      = D_GRA "sf8_spart90";
const char *IMG_SF8_SPART91      = D_GRA "sf8_spart91";
const char *IMG_SF8_SPART92      = D_GRA "sf8_spart92";
const char *IMG_SF8_SPART93      = D_GRA "sf8_spart93";
const char *IMG_SF8_SPART94      = D_GRA "sf8_spart94";
const char *IMG_SF8_SPART95      = D_GRA "sf8_spart95";
const char *IMG_SF8_SPART96      = D_GRA "sf8_spart96";
const char *IMG_SF8_SPART97      = D_GRA "sf8_spart97";
const char *IMG_SF8_SPART98      = D_GRA "sf8_spart98";
const char *IMG_SF8_SPART99      = D_GRA "sf8_spart99";
const char *IMG_SF8_SPART9C      = D_GRA "sf8_spart9C";
const char *IMG_SF8_SPART9E      = D_GRA "sf8_spart9E";
const char *IMG_SF8_SPART9G      = D_GRA "sf8_spart9G";
const char *IMG_SF8_SPART9L      = D_GRA "sf8_spart9L";
const char *IMG_SF8_SPART9Q      = D_GRA "sf8_spart9Q";
const char *IMG_SF8_SPART9S      = D_GRA "sf8_spart9S";
const char *IMG_SF8_SPARTC0      = D_GRA "sf8_spartC0";
const char *IMG_SF8_SPARTC1      = D_GRA "sf8_spartC1";
const char *IMG_SF8_SPARTC2      = D_GRA "sf8_spartC2";
const char *IMG_SF8_SPARTC3      = D_GRA "sf8_spartC3";
const char *IMG_SF8_SPARTC4      = D_GRA "sf8_spartC4";
const char *IMG_SF8_SPARTC5      = D_GRA "sf8_spartC5";
const char *IMG_SF8_SPARTC6      = D_GRA "sf8_spartC6";
const char *IMG_SF8_SPARTC7      = D_GRA "sf8_spartC7";
const char *IMG_SF8_SPARTC8      = D_GRA "sf8_spartC8";
const char *IMG_SF8_SPARTC9      = D_GRA "sf8_spartC9";
const char *IMG_SF8_SPARTCC      = D_GRA "sf8_spartCC";
const char *IMG_SF8_SPARTCE      = D_GRA "sf8_spartCE";
const char *IMG_SF8_SPARTCG      = D_GRA "sf8_spartCG";
const char *IMG_SF8_SPARTCL      = D_GRA "sf8_spartCL";
const char *IMG_SF8_SPARTCQ      = D_GRA "sf8_spartCQ";
const char *IMG_SF8_SPARTCS      = D_GRA "sf8_spartCS";
const char *IMG_SF8_SPARTE0      = D_GRA "sf8_spartE0";
const char *IMG_SF8_SPARTE1      = D_GRA "sf8_spartE1";
const char *IMG_SF8_SPARTE2      = D_GRA "sf8_spartE2";
const char *IMG_SF8_SPARTE3      = D_GRA "sf8_spartE3";
const char *IMG_SF8_SPARTE4      = D_GRA "sf8_spartE4";
const char *IMG_SF8_SPARTE5      = D_GRA "sf8_spartE5";
const char *IMG_SF8_SPARTE6      = D_GRA "sf8_spartE6";
const char *IMG_SF8_SPARTE7      = D_GRA "sf8_spartE7";
const char *IMG_SF8_SPARTE8      = D_GRA "sf8_spartE8";
const char *IMG_SF8_SPARTE9      = D_GRA "sf8_spartE9";
const char *IMG_SF8_SPARTEC      = D_GRA "sf8_spartEC";
const char *IMG_SF8_SPARTEE      = D_GRA "sf8_spartEE";
const char *IMG_SF8_SPARTEG      = D_GRA "sf8_spartEG";
const char *IMG_SF8_SPARTEL      = D_GRA "sf8_spartEL";
const char *IMG_SF8_SPARTEQ      = D_GRA "sf8_spartEQ";
const char *IMG_SF8_SPARTES      = D_GRA "sf8_spartES";
const char *IMG_SF8_SPARTG0      = D_GRA "sf8_spartG0";
const char *IMG_SF8_SPARTG1      = D_GRA "sf8_spartG1";
const char *IMG_SF8_SPARTG2      = D_GRA "sf8_spartG2";
const char *IMG_SF8_SPARTG3      = D_GRA "sf8_spartG3";
const char *IMG_SF8_SPARTG4      = D_GRA "sf8_spartG4";
const char *IMG_SF8_SPARTG5      = D_GRA "sf8_spartG5";
const char *IMG_SF8_SPARTG6      = D_GRA "sf8_spartG6";
const char *IMG_SF8_SPARTG7      = D_GRA "sf8_spartG7";
const char *IMG_SF8_SPARTG8      = D_GRA "sf8_spartG8";
const char *IMG_SF8_SPARTG9      = D_GRA "sf8_spartG9";
const char *IMG_SF8_SPARTGC      = D_GRA "sf8_spartGC";
const char *IMG_SF8_SPARTGE      = D_GRA "sf8_spartGE";
const char *IMG_SF8_SPARTGG      = D_GRA "sf8_spartGG";
const char *IMG_SF8_SPARTGL      = D_GRA "sf8_spartGL";
const char *IMG_SF8_SPARTGQ      = D_GRA "sf8_spartGQ";
const char *IMG_SF8_SPARTGS      = D_GRA "sf8_spartGS";
const char *IMG_SF8_SPARTL0      = D_GRA "sf8_spartL0";
const char *IMG_SF8_SPARTL1      = D_GRA "sf8_spartL1";
const char *IMG_SF8_SPARTL2      = D_GRA "sf8_spartL2";
const char *IMG_SF8_SPARTL3      = D_GRA "sf8_spartL3";
const char *IMG_SF8_SPARTL4      = D_GRA "sf8_spartL4";
const char *IMG_SF8_SPARTL5      = D_GRA "sf8_spartL5";
const char *IMG_SF8_SPARTL6      = D_GRA "sf8_spartL6";
const char *IMG_SF8_SPARTL7      = D_GRA "sf8_spartL7";
const char *IMG_SF8_SPARTL8      = D_GRA "sf8_spartL8";
const char *IMG_SF8_SPARTL9      = D_GRA "sf8_spartL9";
const char *IMG_SF8_SPARTLC      = D_GRA "sf8_spartLC";
const char *IMG_SF8_SPARTLE      = D_GRA "sf8_spartLE";
const char *IMG_SF8_SPARTLG      = D_GRA "sf8_spartLG";
const char *IMG_SF8_SPARTLL      = D_GRA "sf8_spartLL";
const char *IMG_SF8_SPARTLQ      = D_GRA "sf8_spartLQ";
const char *IMG_SF8_SPARTLS      = D_GRA "sf8_spartLS";
const char *IMG_SF8_SPARTQ0      = D_GRA "sf8_spartQ0";
const char *IMG_SF8_SPARTQ1      = D_GRA "sf8_spartQ1";
const char *IMG_SF8_SPARTQ2      = D_GRA "sf8_spartQ2";
const char *IMG_SF8_SPARTQ3      = D_GRA "sf8_spartQ3";
const char *IMG_SF8_SPARTQ4      = D_GRA "sf8_spartQ4";
const char *IMG_SF8_SPARTQ5      = D_GRA "sf8_spartQ5";
const char *IMG_SF8_SPARTQ6      = D_GRA "sf8_spartQ6";
const char *IMG_SF8_SPARTQ7      = D_GRA "sf8_spartQ7";
const char *IMG_SF8_SPARTQ8      = D_GRA "sf8_spartQ8";
const char *IMG_SF8_SPARTQ9      = D_GRA "sf8_spartQ9";
const char *IMG_SF8_SPARTQC      = D_GRA "sf8_spartQC";
const char *IMG_SF8_SPARTQE      = D_GRA "sf8_spartQE";
const char *IMG_SF8_SPARTQG      = D_GRA "sf8_spartQG";
const char *IMG_SF8_SPARTQL      = D_GRA "sf8_spartQL";
const char *IMG_SF8_SPARTQQ      = D_GRA "sf8_spartQQ";
const char *IMG_SF8_SPARTQS      = D_GRA "sf8_spartQS";
const char *IMG_SF8_SPARTS0      = D_GRA "sf8_spartS0";
const char *IMG_SF8_SPARTS1      = D_GRA "sf8_spartS1";
const char *IMG_SF8_SPARTS2      = D_GRA "sf8_spartS2";
const char *IMG_SF8_SPARTS3      = D_GRA "sf8_spartS3";
const char *IMG_SF8_SPARTS4      = D_GRA "sf8_spartS4";
const char *IMG_SF8_SPARTS5      = D_GRA "sf8_spartS5";
const char *IMG_SF8_SPARTS6      = D_GRA "sf8_spartS6";
const char *IMG_SF8_SPARTS7      = D_GRA "sf8_spartS7";
const char *IMG_SF8_SPARTS8      = D_GRA "sf8_spartS8";
const char *IMG_SF8_SPARTS9      = D_GRA "sf8_spartS9";
const char *IMG_SF8_SPARTSC      = D_GRA "sf8_spartSC";
const char *IMG_SF8_SPARTSE      = D_GRA "sf8_spartSE";
const char *IMG_SF8_SPARTSG      = D_GRA "sf8_spartSG";
const char *IMG_SF8_SPARTSL      = D_GRA "sf8_spartSL";
const char *IMG_SF8_SPARTSQ      = D_GRA "sf8_spartSQ";
const char *IMG_SF8_SPARTSS      = D_GRA "sf8_spartSS";
const char *IMG_SF8_STONE0       = D_GRA "sf8_stone0";
const char *IMG_SF8_STONE10      = D_GRA "sf8_stone10";
const char *IMG_SF8_STONE19_0    = D_GRA "sf8_stone19-0";
const char *IMG_SF8_STONE2_0     = D_GRA "sf8_stone2-0";
const char *IMG_SF8_STONE29B     = D_GRA "sf8_stone29b";
const char *IMG_SF8_STONE29      = D_GRA "sf8_stone29";
const char *IMG_SF8_STONE2       = D_GRA "sf8_stone2";
const char *IMG_SF8_STONE8       = D_GRA "sf8_stone8";
const char *IMG_SF8_STONE9       = D_GRA "sf8_stone9";
const char *IMG_SG1_GUILD        = D_GRA "sg1_guild";
const char *IMG_SG1_LGHT1        = D_GRA "sg1_lght1";
const char *IMG_SG1_LGHT2        = D_GRA "sg1_lght2";
const char *IMG_SG1_LGHT3        = D_GRA "sg1_lght3";
const char *IMG_SG1_LGHT4        = D_GRA "sg1_lght4";
const char *IMG_SG1_TRANS1       = D_GRA "sg1_trans1";
const char *IMG_SG1_TRANS2       = D_GRA "sg1_trans2";
const char *IMG_SG1_TRANS3       = D_GRA "sg1_trans3";
const char *IMG_SG1_UFO1         = D_GRA "sg1_ufo1";
const char *IMG_SG1_UFO2         = D_GRA "sg1_ufo2";
const char *IMG_SG2_EYES2        = D_GRA "sg2_eyes2";
const char *IMG_SG2_EYES         = D_GRA "sg2_eyes";
const char *IMG_SG2_INS1         = D_GRA "sg2_ins1";
const char *IMG_SG2_INS2         = D_GRA "sg2_ins2";
const char *IMG_SG2_INS3         = D_GRA "sg2_ins3";
const char *IMG_SG3_DUST         = D_GRA "sg3_dust";
const char *IMG_SG3_LAMP0        = D_GRA "sg3_lamp0";
const char *IMG_SG3_LAMP1        = D_GRA "sg3_lamp1";
const char *IMG_SG3_MIRR0        = D_GRA "sg3_mirr0";
const char *IMG_SG3_MIRR1        = D_GRA "sg3_mirr1";
const char *IMG_SG3_MIRR2        = D_GRA "sg3_mirr2";
const char *IMG_SG3_MIRR3        = D_GRA "sg3_mirr3";
const char *IMG_SG3_SCREEN1      = D_GRA "sg3_screen1";
const char *IMG_SG3_SCREEN2      = D_GRA "sg3_screen2";
const char *IMG_SG3_SCREEN3      = D_GRA "sg3_screen3";
const char *IMG_SG3_SCREEN4      = D_GRA "sg3_screen4";
const char *IMG_SG3_TALK1        = D_GRA "sg3_talk1";
const char *IMG_SG3_TALK2        = D_GRA "sg3_talk2";
const char *IMG_SG3_TALK3        = D_GRA "sg3_talk3";
const char *IMG_SG3_TALK4        = D_GRA "sg3_talk4";
const char *IMG_SG3_TALK5        = D_GRA "sg3_talk5";
const char *IMG_SG3_TALK6        = D_GRA "sg3_talk6";
const char *IMG_SP1_DMG1         = D_GRA "sp1_dmg1";
const char *IMG_SP1_DMG2         = D_GRA "sp1_dmg2";
const char *IMG_SP1_DMG3         = D_GRA "sp1_dmg3";
const char *IMG_SP1_DMG4         = D_GRA "sp1_dmg4";
const char *IMG_SP1_DMG5         = D_GRA "sp1_dmg5";
const char *IMG_SP1_DMG6         = D_GRA "sp1_dmg6";
const char *IMG_SP1_DMG7         = D_GRA "sp1_dmg7";
const char *IMG_SP1_DMG8         = D_GRA "sp1_dmg8";
const char *IMG_SP1_SHIP1        = D_GRA "sp1_ship1";
const char *IMG_SP1_SHP1         = D_GRA "sp1_shp1";
const char *IMG_SP1_SHP2         = D_GRA "sp1_shp2";
const char *IMG_SP1_SHP3         = D_GRA "sp1_shp3";
const char *IMG_SP1_SHP4         = D_GRA "sp1_shp4";
const char *IMG_SP1_SHP5         = D_GRA "sp1_shp5";
const char *IMG_ST1_INITSPR      = D_GRA "st1_initspr";
const char *IMG_SU1_ALLOW0       = D_GRA "su1_allow0";
const char *IMG_SU1_ALLOW1       = D_GRA "su1_allow1";
const char *IMG_SU1_B_0          = D_GRA "su1_b_0";
const char *IMG_SU1_B_1          = D_GRA "su1_b_1";
const char *IMG_SU1_BUILD        = D_GRA "su1_build";
const char *IMG_SU1_CTA          = D_GRA "su1_cta";
const char *IMG_SU1_CTB          = D_GRA "su1_ctb";
const char *IMG_SU1_CTC          = D_GRA "su1_ctc";
const char *IMG_SU1_CTRL1        = D_GRA "su1_ctrl1";
const char *IMG_SU1_CTRL2        = D_GRA "su1_ctrl2";
const char *IMG_SU1_CTRL3        = D_GRA "su1_ctrl3";
const char *IMG_SU1_DEAD10       = D_GRA "su1_dead10";
const char *IMG_SU1_DEAD1        = D_GRA "su1_dead1";
const char *IMG_SU1_DEAD2        = D_GRA "su1_dead2";
const char *IMG_SU1_DEAD3        = D_GRA "su1_dead3";
const char *IMG_SU1_DEAD4        = D_GRA "su1_dead4";
const char *IMG_SU1_DEAD5        = D_GRA "su1_dead5";
const char *IMG_SU1_DEAD6        = D_GRA "su1_dead6";
const char *IMG_SU1_DEAD7        = D_GRA "su1_dead7";
const char *IMG_SU1_DEAD8        = D_GRA "su1_dead8";
const char *IMG_SU1_DEAD9        = D_GRA "su1_dead9";
const char *IMG_SU1_EXPORT       = D_GRA "su1_export";
const char *IMG_SU1_MARK         = D_GRA "su1_mark";
const char *IMG_SU1_MENU         = D_GRA "su1_menu";
const char *IMG_SU1_PARAGRAPH    = D_GRA "su1_paragraph";
const char *IMG_SU1_PEOPLE       = D_GRA "su1_people";
const char *IMG_SU1_STONE1_0     = D_GRA "su1_stone1-0";
const char *IMG_SU1_STONE10      = D_GRA "su1_stone10";
const char *IMG_SU1_STONE11_0    = D_GRA "su1_stone11-0";
const char *IMG_SU1_STONE11_1    = D_GRA "su1_stone11-1";
const char *IMG_SU1_STONE11_2    = D_GRA "su1_stone11-2";
const char *IMG_SU1_STONE11_3    = D_GRA "su1_stone11-3";
const char *IMG_SU1_STONE1_1     = D_GRA "su1_stone1-1";
const char *IMG_SU1_STONE11      = D_GRA "su1_stone11";
const char *IMG_SU1_STONE12_0    = D_GRA "su1_stone12-0";
const char *IMG_SU1_STONE12_1    = D_GRA "su1_stone12-1";
const char *IMG_SU1_STONE12_2    = D_GRA "su1_stone12-2";
const char *IMG_SU1_STONE12_3    = D_GRA "su1_stone12-3";
const char *IMG_SU1_STONE1_2     = D_GRA "su1_stone1-2";
const char *IMG_SU1_STONE12      = D_GRA "su1_stone12";
const char *IMG_SU1_STONE13_0    = D_GRA "su1_stone13-0";
const char *IMG_SU1_STONE13_1    = D_GRA "su1_stone13-1";
const char *IMG_SU1_STONE13_2    = D_GRA "su1_stone13-2";
const char *IMG_SU1_STONE13_3    = D_GRA "su1_stone13-3";
const char *IMG_SU1_STONE1_3     = D_GRA "su1_stone1-3";
const char *IMG_SU1_STONE13      = D_GRA "su1_stone13";
const char *IMG_SU1_STONE14_0    = D_GRA "su1_stone14-0";
const char *IMG_SU1_STONE14_1    = D_GRA "su1_stone14-1";
const char *IMG_SU1_STONE14_2    = D_GRA "su1_stone14-2";
const char *IMG_SU1_STONE14      = D_GRA "su1_stone14";
const char *IMG_SU1_STONE15_0    = D_GRA "su1_stone15-0";
const char *IMG_SU1_STONE15_1    = D_GRA "su1_stone15-1";
const char *IMG_SU1_STONE15_2    = D_GRA "su1_stone15-2";
const char *IMG_SU1_STONE15      = D_GRA "su1_stone15";
const char *IMG_SU1_STONE16_0    = D_GRA "su1_stone16-0";
const char *IMG_SU1_STONE16_1    = D_GRA "su1_stone16-1";
const char *IMG_SU1_STONE16B     = D_GRA "su1_stone16b";
const char *IMG_SU1_STONE16      = D_GRA "su1_stone16";
const char *IMG_SU1_STONE17_0    = D_GRA "su1_stone17-0";
const char *IMG_SU1_STONE17_1    = D_GRA "su1_stone17-1";
const char *IMG_SU1_STONE17      = D_GRA "su1_stone17";
const char *IMG_SU1_STONE18      = D_GRA "su1_stone18";
const char *IMG_SU1_STONE19_0    = D_GRA "su1_stone19-0";
const char *IMG_SU1_STONE1       = D_GRA "su1_stone1";
const char *IMG_SU1_STONE2_0     = D_GRA "su1_stone2-0";
const char *IMG_SU1_STONE20      = D_GRA "su1_stone20";
const char *IMG_SU1_STONE22      = D_GRA "su1_stone22";
const char *IMG_SU1_STONE23_0    = D_GRA "su1_stone23-0";
const char *IMG_SU1_STONE23_1    = D_GRA "su1_stone23-1";
const char *IMG_SU1_STONE23_2    = D_GRA "su1_stone23-2";
const char *IMG_SU1_STONE23_3    = D_GRA "su1_stone23-3";
const char *IMG_SU1_STONE23      = D_GRA "su1_stone23";
const char *IMG_SU1_STONE2       = D_GRA "su1_stone2";
const char *IMG_SU1_STONE3_0     = D_GRA "su1_stone3-0";
const char *IMG_SU1_STONE3_1     = D_GRA "su1_stone3-1";
const char *IMG_SU1_STONE3_2     = D_GRA "su1_stone3-2";
const char *IMG_SU1_STONE3       = D_GRA "su1_stone3";
const char *IMG_SU1_STONE4_0     = D_GRA "su1_stone4-0";
const char *IMG_SU1_STONE4_1     = D_GRA "su1_stone4-1";
const char *IMG_SU1_STONE4_2     = D_GRA "su1_stone4-2";
const char *IMG_SU1_STONE4_3     = D_GRA "su1_stone4-3";
const char *IMG_SU1_STONE4B      = D_GRA "su1_stone4b";
const char *IMG_SU1_STONE4       = D_GRA "su1_stone4";
const char *IMG_SU1_STONE5_0     = D_GRA "su1_stone5-0";
const char *IMG_SU1_STONE5_1     = D_GRA "su1_stone5-1";
const char *IMG_SU1_STONE5_2     = D_GRA "su1_stone5-2";
const char *IMG_SU1_STONE5_3     = D_GRA "su1_stone5-3";
const char *IMG_SU1_STONE5_4     = D_GRA "su1_stone5-4";
const char *IMG_SU1_STONE5_5     = D_GRA "su1_stone5-5";
const char *IMG_SU1_STONE5A10    = D_GRA "su1_stone5a10";
const char *IMG_SU1_STONE5A11    = D_GRA "su1_stone5a11";
const char *IMG_SU1_STONE5A12    = D_GRA "su1_stone5a12";
const char *IMG_SU1_STONE5A13    = D_GRA "su1_stone5a13";
const char *IMG_SU1_STONE5A14    = D_GRA "su1_stone5a14";
const char *IMG_SU1_STONE5A15    = D_GRA "su1_stone5a15";
const char *IMG_SU1_STONE5A16    = D_GRA "su1_stone5a16";
const char *IMG_SU1_STONE5A17    = D_GRA "su1_stone5a17";
const char *IMG_SU1_STONE5A18    = D_GRA "su1_stone5a18";
const char *IMG_SU1_STONE5A1     = D_GRA "su1_stone5a1";
const char *IMG_SU1_STONE5A2     = D_GRA "su1_stone5a2";
const char *IMG_SU1_STONE5A3     = D_GRA "su1_stone5a3";
const char *IMG_SU1_STONE5A4     = D_GRA "su1_stone5a4";
const char *IMG_SU1_STONE5A5     = D_GRA "su1_stone5a5";
const char *IMG_SU1_STONE5A6     = D_GRA "su1_stone5a6";
const char *IMG_SU1_STONE5A7     = D_GRA "su1_stone5a7";
const char *IMG_SU1_STONE5A8     = D_GRA "su1_stone5a8";
const char *IMG_SU1_STONE5A9     = D_GRA "su1_stone5a9";
const char *IMG_SU1_STONE5       = D_GRA "su1_stone5";
const char *IMG_SU1_STONE6       = D_GRA "su1_stone6";
const char *IMG_SU1_STONE7       = D_GRA "su1_stone7";
const char *IMG_SU1_STONEXX      = D_GRA "su1_stonexx";
const char *IMG_SU1_TRI          = D_GRA "su1_tri";
const char *IMG_SU2_STONE7       = D_GRA "su2_stone7";
const char *IMG_TD1_TRADE1       = D_GRA "td1_trade1";
const char *IMG_TD1_TRADE2       = D_GRA "td1_trade2";
const char *IMG_TD1_TRADE3       = D_GRA "td1_trade3";
const char *IMG_TD1_TRADE4       = D_GRA "td1_trade4";
const char *IMG_TD1_TRADE5       = D_GRA "td1_trade5";
const char *IMG_TD1_TRADE6       = D_GRA "td1_trade6";
const char *IMG_TD1_TRADE7       = D_GRA "td1_trade7";
const char *IMG_TD1_TRADE8       = D_GRA "td1_trade8";
const char *IMG_TD1_TRADEBACK    = D_GRA "td1_tradeback";
const char *IMG_TD2_TR0          = D_GRA "td2_tr0";
const char *IMG_TD2_TR1          = D_GRA "td2_tr1";
const char *IMG_TD2_TR2          = D_GRA "td2_tr2";
const char *IMG_TD2_TR3          = D_GRA "td2_tr3";
const char *IMG_TD2_TR4          = D_GRA "td2_tr4";
const char *IMG_TD2_TR5          = D_GRA "td2_tr5";
const char *IMG_TD2_TR6          = D_GRA "td2_tr6";
const char *IMG_TD2_TR7          = D_GRA "td2_tr7";
const char *IMG_TD2_TR8          = D_GRA "td2_tr8";
const char *IMG_TS1_ALL1         = D_GRA "ts1_all1";
const char *IMG_TS1_ALL2         = D_GRA "ts1_all2";
const char *IMG_TS1_ALL3         = D_GRA "ts1_all3";
const char *IMG_TS1_CLOGO1       = D_GRA "ts1_clogo1";
const char *IMG_TS1_CLOGO2       = D_GRA "ts1_clogo2";
const char *IMG_TS1_CLOGO3       = D_GRA "ts1_clogo3";
const char *IMG_TS1_CLOGO4       = D_GRA "ts1_clogo4";
const char *IMG_TS1_CLOGO5       = D_GRA "ts1_clogo5";
const char *IMG_TS1_CLOGO6       = D_GRA "ts1_clogo6";
const char *IMG_TS1_CLOGO7       = D_GRA "ts1_clogo7";
const char *IMG_TS1_CLOGO8       = D_GRA "ts1_clogo8";
const char *IMG_TS1_CLOGO9       = D_GRA "ts1_clogo9";
const char *IMG_TS1_DEST         = D_GRA "ts1_dest";
const char *IMG_TS1_FLAG0        = D_GRA "ts1_flag0";
const char *IMG_TS1_FLAG10       = D_GRA "ts1_flag10";
const char *IMG_TS1_FLAG11       = D_GRA "ts1_flag11";
const char *IMG_TS1_FLAG12       = D_GRA "ts1_flag12";
const char *IMG_TS1_FLAG13       = D_GRA "ts1_flag13";
const char *IMG_TS1_FLAG14       = D_GRA "ts1_flag14";
const char *IMG_TS1_FLAG15       = D_GRA "ts1_flag15";
const char *IMG_TS1_FLAG16       = D_GRA "ts1_flag16";
const char *IMG_TS1_FLAG1        = D_GRA "ts1_flag1";
const char *IMG_TS1_FLAG2        = D_GRA "ts1_flag2";
const char *IMG_TS1_FLAG3        = D_GRA "ts1_flag3";
const char *IMG_TS1_FLAG4        = D_GRA "ts1_flag4";
const char *IMG_TS1_FLAG5        = D_GRA "ts1_flag5";
const char *IMG_TS1_FLAG6        = D_GRA "ts1_flag6";
const char *IMG_TS1_FLAG7        = D_GRA "ts1_flag7";
const char *IMG_TS1_FLAG8        = D_GRA "ts1_flag8";
const char *IMG_TS1_FLAG9        = D_GRA "ts1_flag9";
const char *IMG_TS1_HEADER1      = D_GRA "ts1_header1";
const char *IMG_TS1_HEADER2      = D_GRA "ts1_header2";
const char *IMG_TS1_HEADER       = D_GRA "ts1_header";
const char *IMG_TS1_ICON1        = D_GRA "ts1_icon1";
const char *IMG_TS1_ICON2        = D_GRA "ts1_icon2";
const char *IMG_TS1_ICON3        = D_GRA "ts1_icon3";
const char *IMG_TS1_ICON4        = D_GRA "ts1_icon4";
const char *IMG_TS1_ICON5        = D_GRA "ts1_icon5";
const char *IMG_TS1_ID1          = D_GRA "ts1_id1";
const char *IMG_TS1_ID2          = D_GRA "ts1_id2";
const char *IMG_TS1_M11          = D_GRA "ts1_m11";
const char *IMG_TS1_M12          = D_GRA "ts1_m12";
const char *IMG_TS1_M13          = D_GRA "ts1_m13";
const char *IMG_TS1_M14          = D_GRA "ts1_m14";
const char *IMG_TS1_M2_1         = D_GRA "ts1_m2_1";
const char *IMG_TS1_M21          = D_GRA "ts1_m21";
const char *IMG_TS1_M2_2         = D_GRA "ts1_m2_2";
const char *IMG_TS1_M22          = D_GRA "ts1_m22";
const char *IMG_TS1_M23          = D_GRA "ts1_m23";
const char *IMG_TS1_M24          = D_GRA "ts1_m24";
const char *IMG_TS1_M25          = D_GRA "ts1_m25";
const char *IMG_TS1_M26          = D_GRA "ts1_m26";
const char *IMG_TS1_M27          = D_GRA "ts1_m27";
const char *IMG_TS1_M28          = D_GRA "ts1_m28";
const char *IMG_TS1_MK1          = D_GRA "ts1_mk1";
const char *IMG_TS1_MK2          = D_GRA "ts1_mk2";
const char *IMG_TS1_MK3          = D_GRA "ts1_mk3";
const char *IMG_TS1_QMARK        = D_GRA "ts1_qmark";
const char *IMG_TS1_SHICON       = D_GRA "ts1_shicon";
const char *IMG_TS1_SML_MENU     = D_GRA "ts1_sml_menu";
const char *IMG_TS1_SPHERE1      = D_GRA "ts1_sphere1";
const char *IMG_TS1_SPHERE2      = D_GRA "ts1_sphere2";
const char *IMG_TS1_SPHERE3      = D_GRA "ts1_sphere3";
const char *IMG_TS1_SPHERE4      = D_GRA "ts1_sphere4";
const char *IMG_TS1_SPHERE5      = D_GRA "ts1_sphere5";
const char *IMG_TS1_SPHERE6      = D_GRA "ts1_sphere6";
const char *IMG_TS1_SPHERE8      = D_GRA "ts1_sphere8";
const char *IMG_TS1_ST1          = D_GRA "ts1_st1";
const char *IMG_TS1_ST2          = D_GRA "ts1_st2";
const char *IMG_TS1_ST3          = D_GRA "ts1_st3";
const char *IMG_TS1_ST4          = D_GRA "ts1_st4";
const char *IMG_TS1_ST5          = D_GRA "ts1_st5";
const char *IMG_TS1_ST6          = D_GRA "ts1_st6";
const char *IMG_TS1_ST8          = D_GRA "ts1_st8";
const char *IMG_TS1_SUN1         = D_GRA "ts1_sun1";
const char *IMG_TS1_SUN2         = D_GRA "ts1_sun2";
const char *IMG_TS1_SUN3         = D_GRA "ts1_sun3";
const char *IMG_TS1_SUN4         = D_GRA "ts1_sun4";
const char *IMG_TS1_SUN5         = D_GRA "ts1_sun5";
const char *IMG_TS1_SUN6         = D_GRA "ts1_sun6";
const char *IMG_TS1_SUN7         = D_GRA "ts1_sun7";
const char *IMG_TS1_SUN8         = D_GRA "ts1_sun8";
const char *IMG_TS1_SUN9         = D_GRA "ts1_sun9";
const char *IMG_TS1_WORM         = D_GRA "ts1_worm";
const char *IMG_TS2_ALL1         = D_GRA "ts2_all1";
const char *IMG_TS2_ALL2         = D_GRA "ts2_all2";
const char *IMG_TS2_ALL3         = D_GRA "ts2_all3";
const char *IMG_TS2_CLOGO1       = D_GRA "ts2_clogo1";
const char *IMG_TS2_CLOGO2       = D_GRA "ts2_clogo2";
const char *IMG_TS2_CLOGO3       = D_GRA "ts2_clogo3";
const char *IMG_TS2_CLOGO4       = D_GRA "ts2_clogo4";
const char *IMG_TS2_CLOGO5       = D_GRA "ts2_clogo5";
const char *IMG_TS2_CLOGO6       = D_GRA "ts2_clogo6";
const char *IMG_TS2_CLOGO7       = D_GRA "ts2_clogo7";
const char *IMG_TS2_CLOGO8       = D_GRA "ts2_clogo8";
const char *IMG_TS2_CLOGO9       = D_GRA "ts2_clogo9";
const char *IMG_TS2_M2_1         = D_GRA "ts2_m2_1";
const char *IMG_TS2_M2_2         = D_GRA "ts2_m2_2";
const char *IMG_TS2_PATTERN      = D_GRA "ts2_pattern";
const char *IMG_TS2_SHICON       = D_GRA "ts2_shicon";
const char *IMG_TS2_SML_MENU     = D_GRA "ts2_sml_menu";
const char *IMG_TS2_SPHERE1      = D_GRA "ts2_sphere1";
const char *IMG_TS2_SPHERE2      = D_GRA "ts2_sphere2";
const char *IMG_TS2_SPHERE3      = D_GRA "ts2_sphere3";
const char *IMG_TS2_SPHERE4      = D_GRA "ts2_sphere4";
const char *IMG_TS2_SPHERE5      = D_GRA "ts2_sphere5";
const char *IMG_TS2_SPHERE6      = D_GRA "ts2_sphere6";
const char *IMG_TS2_SPHERE8      = D_GRA "ts2_sphere8";
const char *IMG_VS0_VPIC         = D_GRA "vs0_vpic";
const char *IMG_VS1_VESSEL       = D_GRA "vs1_vessel";
const char *IMG_VS2_VESSEL       = D_GRA "vs2_vessel";
const char *IMG_VS3_VESSEL       = D_GRA "vs3_vessel";
const char *IMG_VS4_VESSEL       = D_GRA "vs4_vessel";
const char *IMG_VS5_VESSEL       = D_GRA "vs5_vessel";
const char *IMG_VS6_VESSEL       = D_GRA "vs6_vessel";
const char *IMG_WD1_DONE         = D_GRA "wd1_done";
const char *IMG_WD1_WELL         = D_GRA "wd1_well";

#endif  // LINUX

// Special cases
// These don't need to be present in ASSETS_IMG.
// We load and display this image whilst the rest are loading.
const char *INIT_IMG = IMG_ST1_INITSPR;
// This is given special handling by the draw manager
const char *CURSOR_IMG = IMG_CURSOR;

const char *ASSETS_SFX[] = {
    SFX_SHOT,
    SFX_EXPLOSION,
    SFX_HEAVYSHOT,
    SFX_GLIDE_LOW,
    SFX_GLIDE_MED,
    SFX_GLIDE_HIGH,
    SFX_WALK0,
    SFX_WALK1,
    SFX_WALK2,
    SFX_BEEP,

    "",
};

const char *ASSETS_MUS[] = {
    MUS_INTRO,
    MUS_ST1_10,
    MUS_END,
    MUS_ST2_E,
    MUS_OVERVIEW,
    MUS_ST3_A,
    MUS_ST3_F,
    MUS_BATTLE_PREP,
    MUS_BATTLE_WON,
    MUS_TRADE,
    MUS_ST3_27,
    MUS_INVASION,
    MUS_BATTLE_LOST,
    MUS_ACHIEVEMENT,
    MUS_MINIGAME,
    MUS_GAMESTART,
    MUS_OVERWORLD,
    MUS_ALIENSHIP,
    MUS_ST3_42,
    MUS_BATTLEWAIT,
    MUS_ST3_4D,
    MUS_CELEBRATE,
    MUS_GUILD,
    MUS_OVERWORLD_ANGRY,
    MUS_ST3_58,

    "",
};

const char *ASSETS_IMG[] = {
    IMG_SHIP_FLIPPED,

    IMG_INTRO_EARTH,
    IMG_INTRO_CITY,
    IMG_INTRO_STARPORT,
    IMG_INTRO_CORRIDOR,
    IMG_INTRO_DOOR,
    IMG_INTRO_KEYPAD,
    IMG_INTRO_FAIL,
    IMG_INTRO_LAUNCH,
    IMG_INTRO_SPACE,
    IMG_MAP_SOLAR,
    IMG_BATTLE,
    IMG_BATTLE_PREP,
    IMG_LORD_ARRIVE,
    IMG_GAMEOVER,
    IMG_PLANETSIDE,
    IMG_RADAR,
    IMG_BG_SHUTTLE,
    IMG_BG_MENU0,
    IMG_BG_MENU1,
    IMG_BG_NEWS,
    IMG_BG_SHIP0,
    IMG_BG_SHIP1,
    IMG_BG_STARS0,
    IMG_BG_STARS1,
    IMG_BG_STARS2,
    IMG_BG_STARS3,
    IMG_BG_STARS4,
    IMG_BG_STARS5,
    IMG_BG_STARS6,
    IMG_BG_STARS7,
    IMG_PLANET_FOREST,
    IMG_PLANET_DESERT,
    IMG_PLANET_VOLCANO,
    IMG_PLANET_ROCK,
    IMG_PLANET_ICE,
    IMG_PLANET_TERRA,
    IMG_PLANET_ART,
    IMG_HARBOUR_FOREST,
    IMG_HARBOUR_DESERT,
    IMG_HARBOUR_VOLCANO,
    IMG_HARBOUR_ROCK,
    IMG_HARBOUR_ICE,
    IMG_HARBOUR_TERRA,
    IMG_HARBOUR_ART,
    IMG_MAKING0,
    IMG_MAKING1,
    IMG_MAKING2,
    IMG_MAKING3,
    IMG_MAKING4,
    IMG_MAKING5,
    IMG_MAKING6,
    IMG_MAKING7,
    IMG_MAKING8,
    IMG_MAKING9,
    IMG_MAKING10,
    IMG_MAKING11,
    IMG_MAKING12,
    IMG_MAKING13,
    IMG_MAKING14,

    IMG_INTRO_BT1_FALL1,
    IMG_INTRO_BT1_FALL2,
    IMG_INTRO_BT1_FALL3,
    IMG_INTRO_BT1_FALL4,
    IMG_INTRO_BT1_FALL5,
    IMG_INTRO_BT1_FALL6,
    IMG_INTRO_BT1_FALL7,
    IMG_INTRO_FI1_FIRE1,
    IMG_INTRO_FI1_FIRE2,
    IMG_INTRO_FI1_FIRE3,
    IMG_INTRO_FI1_FIRE4,
    IMG_INTRO_FI1_FIRE5,
    IMG_INTRO_FI1_FIRE6,
    IMG_INTRO_FI1_SHOT1,
    IMG_INTRO_FI1_SHOT2,
    IMG_INTRO_FI1_SHOT3,
    IMG_INTRO_FI1_SHOT4,
    IMG_INTRO_FI1_SHOT5,
    IMG_INTRO_GU1_GUARD,
    IMG_INTRO_OD1_BEHIND,
    IMG_INTRO_OD1_DOOR_L,
    IMG_INTRO_OD1_DOOR_R,
    IMG_INTRO_OD1_FRAME,
    IMG_INTRO_OD1_OFF,
    IMG_INTRO_OD1_ON,
    IMG_INTRO_OD1_SEPP,
    IMG_INTRO_PH1_PUSH_1,
    IMG_INTRO_PH1_PUSH_2,
    IMG_INTRO_PH1_PUSH_3,
    IMG_INTRO_PH1_PUSH_4,
    IMG_INTRO_PH1_PUSH_5,
    IMG_INTRO_PH1_PUSH_6,
    IMG_INTRO_PH1_PUSH_7,
    IMG_INTRO_PH1_PUSH_8,
    IMG_INTRO_PH1_PUSH_9,
    IMG_INTRO_SH1_SHIP,
    IMG_INTRO_SH1_SHIP2,
    IMG_INTRO_SH2_SHIP,
    IMG_INTRO_SH3_CRUISER,
    IMG_INTRO_SH3_CRUISER1,
    IMG_INTRO_SH3_CRUISER2,
    IMG_INTRO_SH3_SHUTTLE,
    IMG_INTRO_SH4_BLINK1,
    IMG_INTRO_SH4_BLINK2,
    IMG_INTRO_SH4_BLINK3,
    IMG_INTRO_SH4_MOVEOUT,
    IMG_INTRO_SH4_SHUTTLE1,
    IMG_INTRO_SH4_SHUTTLE2,
    IMG_INTRO_SH4_SHUTTLE3,
    IMG_INTRO_SH4_SHUTTLE4,

    IMG_FILM_1_SPR_1,
    IMG_FILM_1_SPR_2_3,
    IMG_FILM_1_SPR_3_3,

    IMG_STARTGR_FL4_FLEET,
    IMG_STARTGR_FL5_10_1,
    IMG_STARTGR_FL5_10_2,
    IMG_STARTGR_FL5_10_3,
    IMG_STARTGR_FL5_10_4,
    IMG_STARTGR_FL5_10_5,
    IMG_STARTGR_FL5_11_1,
    IMG_STARTGR_FL5_11_2,
    IMG_STARTGR_FL5_11_3,
    IMG_STARTGR_FL5_11_4,
    IMG_STARTGR_FL5_11_5,
    IMG_STARTGR_FL5_1_1,
    IMG_STARTGR_FL5_12_1,
    IMG_STARTGR_FL5_12_2,
    IMG_STARTGR_FL5_12_3,
    IMG_STARTGR_FL5_12_4,
    IMG_STARTGR_FL5_12_5,
    IMG_STARTGR_FL5_1_2,
    IMG_STARTGR_FL5_1_3,
    IMG_STARTGR_FL5_1_4,
    IMG_STARTGR_FL5_1_5,
    IMG_STARTGR_FL5_2_1,
    IMG_STARTGR_FL5_2_2,
    IMG_STARTGR_FL5_2_3,
    IMG_STARTGR_FL5_2_4,
    IMG_STARTGR_FL5_2_5,
    IMG_STARTGR_FL5_3_1,
    IMG_STARTGR_FL5_3_2,
    IMG_STARTGR_FL5_3_3,
    IMG_STARTGR_FL5_3_4,
    IMG_STARTGR_FL5_3_5,
    IMG_STARTGR_FL5_4_1,
    IMG_STARTGR_FL5_4_2,
    IMG_STARTGR_FL5_4_3,
    IMG_STARTGR_FL5_4_4,
    IMG_STARTGR_FL5_4_5,
    IMG_STARTGR_FL5_5_1,
    IMG_STARTGR_FL5_5_2,
    IMG_STARTGR_FL5_5_3,
    IMG_STARTGR_FL5_5_4,
    IMG_STARTGR_FL5_5_5,
    IMG_STARTGR_FL5_6_1,
    IMG_STARTGR_FL5_6_2,
    IMG_STARTGR_FL5_6_3,
    IMG_STARTGR_FL5_6_4,
    IMG_STARTGR_FL5_6_5,
    IMG_STARTGR_FL5_7_1,
    IMG_STARTGR_FL5_7_2,
    IMG_STARTGR_FL5_7_3,
    IMG_STARTGR_FL5_7_4,
    IMG_STARTGR_FL5_7_5,
    IMG_STARTGR_FL5_8_1,
    IMG_STARTGR_FL5_8_2,
    IMG_STARTGR_FL5_8_3,
    IMG_STARTGR_FL5_8_4,
    IMG_STARTGR_FL5_8_5,
    IMG_STARTGR_FL5_9_1,
    IMG_STARTGR_FL5_9_2,
    IMG_STARTGR_FL5_9_3,
    IMG_STARTGR_FL5_9_4,
    IMG_STARTGR_FL5_9_5,
    IMG_STARTGR_FL5_FLEET,
    IMG_STARTGR_FL6_STARS,
    IMG_STARTGR_GAL_S,
    IMG_STARTGR_GAL_M,
    IMG_STARTGR_GAL_L,
    IMG_STARTGR_AIM_MIGHT,
    IMG_STARTGR_AIM_MONEY,
    IMG_STARTGR_AIM_CIV,
    IMG_STARTGR_STR_WEAK,
    IMG_STARTGR_STR_MEDIUM,
    IMG_STARTGR_STR_STRONG,
    IMG_STARTGR_MPSTR_NONE,
    IMG_STARTGR_MPSTR_WEAK,
    IMG_STARTGR_MPSTR_MEDIUM,

    IMG_AL1_1_2,
    IMG_AL1_13,
    IMG_AL1_15,
    IMG_AL1_1,
    IMG_AL1_3,
    IMG_AL1_7,
    IMG_AL1_9,
    IMG_AL2_1_2,
    IMG_AL2_13,
    IMG_AL2_15,
    IMG_AL2_1,
    IMG_AL2_3,
    IMG_AL2_7,
    IMG_AL2_9,
    IMG_AP1_APPROACH,
    IMG_AP3_APPROACH,
    IMG_AP8_APPROACH,
    IMG_BR10_EXPORT,
    IMG_BR11_DEF,
    IMG_BR11_LR,
    IMG_BR11_OK,
    IMG_BR12_EXPORT,
    IMG_BR13_FEMALE,
    IMG_BR13_MALE,
    IMG_BR14_EXPORT,
    IMG_BR15_FADE1,
    IMG_BR15_FADE2,
    IMG_BR15_FADE3,
    IMG_BR16_NBAR,
    IMG_BR1_EXPORT,
    IMG_BR2_EXPORT,
    IMG_BR3_EXPORT2,
    IMG_BR4_EXPORT3,
    IMG_BR5_EXPORT4,
    IMG_BR6_OK,
    IMG_BR7_E1,
    IMG_BR7_E2,
    IMG_BR8_EXPORT,
    IMG_BR9_EXPORT,
    IMG_BT1_EXPORT,
    IMG_CS1_C,
    IMG_CS2_C,
    IMG_CS3_C,
    IMG_CS4_C,
    IMG_CT1_EXPORT,
    IMG_CT2_EXPORT,
    IMG_CT3_EXPORT,
    IMG_CT4_EXPORT,
    IMG_EN1_DOOR1,
    IMG_EN1_DOOR2,
    IMG_EN1_HALL,
    IMG_EN1_STARS,
    IMG_EN1_WELCOME,
    IMG_EN2_SCAPE,
    IMG_EN2_STS1,
    IMG_EN2_STS2,
    IMG_EN2_STS3,
    IMG_EN2_STS4,
    IMG_EN2_STS5,
    IMG_EN2_STS6,
    IMG_EN2_STS7,
    IMG_EN2_STS8,
    IMG_EN2_STS9,
    IMG_EN2_THEEND,
    IMG_EQ1_TR0,
    IMG_EQ1_TR1,
    IMG_EQ1_TR2,
    IMG_EQ1_TR3,
    IMG_EQ1_TR4,
    IMG_EQ1_TR5,
    IMG_EQ1_TR6,
    IMG_EQ1_TR7,
    IMG_FL1_TR0,
    IMG_FL1_TR1,
    IMG_FL1_TR2,
    IMG_FL1_TR3,
    IMG_FL1_TR4,
    IMG_FL1_TR6,
    IMG_FL1_TR7,
    IMG_GF1_25,
    IMG_GF1_30,
    IMG_GF1_30X,
    IMG_GF1_31,
    IMG_GF1_31X,
    IMG_GF1_PART00,
    IMG_GF1_PART010,
    IMG_GF1_PART011,
    IMG_GF1_PART01,
    IMG_GF1_PART02,
    IMG_GF1_PART03,
    IMG_GF1_PART04,
    IMG_GF1_PART05,
    IMG_GF1_PART06,
    IMG_GF1_PART07,
    IMG_GF1_PART08,
    IMG_GF1_PART09,
    IMG_GF1_PART100,
    IMG_GF1_PART1010,
    IMG_GF1_PART1011,
    IMG_GF1_PART101,
    IMG_GF1_PART102,
    IMG_GF1_PART103,
    IMG_GF1_PART104,
    IMG_GF1_PART105,
    IMG_GF1_PART106,
    IMG_GF1_PART107,
    IMG_GF1_PART108,
    IMG_GF1_PART109,
    IMG_GF1_PART10,
    IMG_GF1_PART110,
    IMG_GF1_PART1110,
    IMG_GF1_PART1111,
    IMG_GF1_PART111,
    IMG_GF1_PART112,
    IMG_GF1_PART113,
    IMG_GF1_PART114,
    IMG_GF1_PART115,
    IMG_GF1_PART116,
    IMG_GF1_PART117,
    IMG_GF1_PART118,
    IMG_GF1_PART119,
    IMG_GF1_PART11,
    IMG_GF1_PART120,
    IMG_GF1_PART1210,
    IMG_GF1_PART1211,
    IMG_GF1_PART121,
    IMG_GF1_PART122,
    IMG_GF1_PART123,
    IMG_GF1_PART124,
    IMG_GF1_PART125,
    IMG_GF1_PART126,
    IMG_GF1_PART127,
    IMG_GF1_PART128,
    IMG_GF1_PART129,
    IMG_GF1_PART12,
    IMG_GF1_PART130,
    IMG_GF1_PART1310,
    IMG_GF1_PART1311,
    IMG_GF1_PART131,
    IMG_GF1_PART132,
    IMG_GF1_PART133,
    IMG_GF1_PART134,
    IMG_GF1_PART135,
    IMG_GF1_PART136,
    IMG_GF1_PART137,
    IMG_GF1_PART138,
    IMG_GF1_PART139,
    IMG_GF1_PART13,
    IMG_GF1_PART140,
    IMG_GF1_PART1410,
    IMG_GF1_PART1411,
    IMG_GF1_PART141,
    IMG_GF1_PART142,
    IMG_GF1_PART143,
    IMG_GF1_PART144,
    IMG_GF1_PART145,
    IMG_GF1_PART146,
    IMG_GF1_PART147,
    IMG_GF1_PART148,
    IMG_GF1_PART149,
    IMG_GF1_PART14,
    IMG_GF1_PART150,
    IMG_GF1_PART1510,
    IMG_GF1_PART1511,
    IMG_GF1_PART151,
    IMG_GF1_PART152,
    IMG_GF1_PART153,
    IMG_GF1_PART154,
    IMG_GF1_PART155,
    IMG_GF1_PART156,
    IMG_GF1_PART157,
    IMG_GF1_PART158,
    IMG_GF1_PART159,
    IMG_GF1_PART15,
    IMG_GF1_PART16,
    IMG_GF1_PART17,
    IMG_GF1_PART18,
    IMG_GF1_PART19,
    IMG_GF1_PART20,
    IMG_GF1_PART210,
    IMG_GF1_PART211,
    IMG_GF1_PART21,
    IMG_GF1_PART22,
    IMG_GF1_PART23,
    IMG_GF1_PART24,
    IMG_GF1_PART25,
    IMG_GF1_PART26,
    IMG_GF1_PART27,
    IMG_GF1_PART28,
    IMG_GF1_PART29,
    IMG_GF1_PART30,
    IMG_GF1_PART310,
    IMG_GF1_PART311,
    IMG_GF1_PART31,
    IMG_GF1_PART32,
    IMG_GF1_PART33,
    IMG_GF1_PART34,
    IMG_GF1_PART35,
    IMG_GF1_PART36,
    IMG_GF1_PART37,
    IMG_GF1_PART38,
    IMG_GF1_PART39,
    IMG_GF1_PART40,
    IMG_GF1_PART410,
    IMG_GF1_PART411,
    IMG_GF1_PART41,
    IMG_GF1_PART42,
    IMG_GF1_PART43,
    IMG_GF1_PART44,
    IMG_GF1_PART45,
    IMG_GF1_PART46,
    IMG_GF1_PART47,
    IMG_GF1_PART48,
    IMG_GF1_PART49,
    IMG_GF1_PART50,
    IMG_GF1_PART510,
    IMG_GF1_PART511,
    IMG_GF1_PART51,
    IMG_GF1_PART52,
    IMG_GF1_PART53,
    IMG_GF1_PART54,
    IMG_GF1_PART55,
    IMG_GF1_PART56,
    IMG_GF1_PART57,
    IMG_GF1_PART58,
    IMG_GF1_PART59,
    IMG_GF1_PART60,
    IMG_GF1_PART610,
    IMG_GF1_PART611,
    IMG_GF1_PART61,
    IMG_GF1_PART62,
    IMG_GF1_PART63,
    IMG_GF1_PART64,
    IMG_GF1_PART65,
    IMG_GF1_PART66,
    IMG_GF1_PART67,
    IMG_GF1_PART68,
    IMG_GF1_PART69,
    IMG_GF1_PART70,
    IMG_GF1_PART710,
    IMG_GF1_PART711,
    IMG_GF1_PART71,
    IMG_GF1_PART72,
    IMG_GF1_PART73,
    IMG_GF1_PART74,
    IMG_GF1_PART75,
    IMG_GF1_PART76,
    IMG_GF1_PART77,
    IMG_GF1_PART78,
    IMG_GF1_PART79,
    IMG_GF1_PART80,
    IMG_GF1_PART810,
    IMG_GF1_PART811,
    IMG_GF1_PART81,
    IMG_GF1_PART82,
    IMG_GF1_PART83,
    IMG_GF1_PART84,
    IMG_GF1_PART85,
    IMG_GF1_PART86,
    IMG_GF1_PART87,
    IMG_GF1_PART88,
    IMG_GF1_PART89,
    IMG_GF1_PART90,
    IMG_GF1_PART910,
    IMG_GF1_PART911,
    IMG_GF1_PART91,
    IMG_GF1_PART92,
    IMG_GF1_PART93,
    IMG_GF1_PART94,
    IMG_GF1_PART95,
    IMG_GF1_PART96,
    IMG_GF1_PART97,
    IMG_GF1_PART98,
    IMG_GF1_PART99,
    IMG_GF2_25,
    IMG_GF2_30,
    IMG_GF2_30X,
    IMG_GF2_31,
    IMG_GF2_31X,
    IMG_GF2_MA1,
    IMG_GF2_MA2,
    IMG_GF2_MA3,
    IMG_GF2_MA4,
    IMG_GF2_MA5,
    IMG_GF2_PART00,
    IMG_GF2_PART010,
    IMG_GF2_PART011,
    IMG_GF2_PART01,
    IMG_GF2_PART02,
    IMG_GF2_PART03,
    IMG_GF2_PART04,
    IMG_GF2_PART05,
    IMG_GF2_PART06,
    IMG_GF2_PART07,
    IMG_GF2_PART08,
    IMG_GF2_PART09,
    IMG_GF2_PART100,
    IMG_GF2_PART1010,
    IMG_GF2_PART1011,
    IMG_GF2_PART101,
    IMG_GF2_PART102,
    IMG_GF2_PART103,
    IMG_GF2_PART104,
    IMG_GF2_PART105,
    IMG_GF2_PART106,
    IMG_GF2_PART107,
    IMG_GF2_PART108,
    IMG_GF2_PART109,
    IMG_GF2_PART10,
    IMG_GF2_PART110,
    IMG_GF2_PART1110,
    IMG_GF2_PART1111,
    IMG_GF2_PART111,
    IMG_GF2_PART112,
    IMG_GF2_PART113,
    IMG_GF2_PART114,
    IMG_GF2_PART115,
    IMG_GF2_PART116,
    IMG_GF2_PART117,
    IMG_GF2_PART118,
    IMG_GF2_PART119,
    IMG_GF2_PART11,
    IMG_GF2_PART120,
    IMG_GF2_PART1210,
    IMG_GF2_PART1211,
    IMG_GF2_PART121,
    IMG_GF2_PART122,
    IMG_GF2_PART123,
    IMG_GF2_PART124,
    IMG_GF2_PART125,
    IMG_GF2_PART126,
    IMG_GF2_PART127,
    IMG_GF2_PART128,
    IMG_GF2_PART129,
    IMG_GF2_PART12,
    IMG_GF2_PART130,
    IMG_GF2_PART1310,
    IMG_GF2_PART1311,
    IMG_GF2_PART131,
    IMG_GF2_PART132,
    IMG_GF2_PART133,
    IMG_GF2_PART134,
    IMG_GF2_PART135,
    IMG_GF2_PART136,
    IMG_GF2_PART137,
    IMG_GF2_PART138,
    IMG_GF2_PART139,
    IMG_GF2_PART13,
    IMG_GF2_PART140,
    IMG_GF2_PART1410,
    IMG_GF2_PART1411,
    IMG_GF2_PART141,
    IMG_GF2_PART142,
    IMG_GF2_PART143,
    IMG_GF2_PART144,
    IMG_GF2_PART145,
    IMG_GF2_PART146,
    IMG_GF2_PART147,
    IMG_GF2_PART148,
    IMG_GF2_PART149,
    IMG_GF2_PART14,
    IMG_GF2_PART150,
    IMG_GF2_PART1510,
    IMG_GF2_PART1511,
    IMG_GF2_PART151,
    IMG_GF2_PART152,
    IMG_GF2_PART153,
    IMG_GF2_PART154,
    IMG_GF2_PART155,
    IMG_GF2_PART156,
    IMG_GF2_PART157,
    IMG_GF2_PART158,
    IMG_GF2_PART159,
    IMG_GF2_PART15,
    IMG_GF2_PART16,
    IMG_GF2_PART17,
    IMG_GF2_PART18,
    IMG_GF2_PART19,
    IMG_GF2_PART20,
    IMG_GF2_PART210,
    IMG_GF2_PART211,
    IMG_GF2_PART21,
    IMG_GF2_PART22,
    IMG_GF2_PART23,
    IMG_GF2_PART24,
    IMG_GF2_PART25,
    IMG_GF2_PART26,
    IMG_GF2_PART27,
    IMG_GF2_PART28,
    IMG_GF2_PART29,
    IMG_GF2_PART30,
    IMG_GF2_PART310,
    IMG_GF2_PART311,
    IMG_GF2_PART31,
    IMG_GF2_PART32,
    IMG_GF2_PART33,
    IMG_GF2_PART34,
    IMG_GF2_PART35,
    IMG_GF2_PART36,
    IMG_GF2_PART37,
    IMG_GF2_PART38,
    IMG_GF2_PART39,
    IMG_GF2_PART40,
    IMG_GF2_PART410,
    IMG_GF2_PART411,
    IMG_GF2_PART41,
    IMG_GF2_PART42,
    IMG_GF2_PART43,
    IMG_GF2_PART44,
    IMG_GF2_PART45,
    IMG_GF2_PART46,
    IMG_GF2_PART47,
    IMG_GF2_PART48,
    IMG_GF2_PART49,
    IMG_GF2_PART50,
    IMG_GF2_PART510,
    IMG_GF2_PART511,
    IMG_GF2_PART51,
    IMG_GF2_PART52,
    IMG_GF2_PART53,
    IMG_GF2_PART54,
    IMG_GF2_PART55,
    IMG_GF2_PART56,
    IMG_GF2_PART57,
    IMG_GF2_PART58,
    IMG_GF2_PART59,
    IMG_GF2_PART60,
    IMG_GF2_PART610,
    IMG_GF2_PART611,
    IMG_GF2_PART61,
    IMG_GF2_PART62,
    IMG_GF2_PART63,
    IMG_GF2_PART64,
    IMG_GF2_PART65,
    IMG_GF2_PART66,
    IMG_GF2_PART67,
    IMG_GF2_PART68,
    IMG_GF2_PART69,
    IMG_GF2_PART70,
    IMG_GF2_PART710,
    IMG_GF2_PART711,
    IMG_GF2_PART71,
    IMG_GF2_PART72,
    IMG_GF2_PART73,
    IMG_GF2_PART74,
    IMG_GF2_PART75,
    IMG_GF2_PART76,
    IMG_GF2_PART77,
    IMG_GF2_PART78,
    IMG_GF2_PART79,
    IMG_GF2_PART80,
    IMG_GF2_PART810,
    IMG_GF2_PART811,
    IMG_GF2_PART81,
    IMG_GF2_PART82,
    IMG_GF2_PART83,
    IMG_GF2_PART84,
    IMG_GF2_PART85,
    IMG_GF2_PART86,
    IMG_GF2_PART87,
    IMG_GF2_PART88,
    IMG_GF2_PART89,
    IMG_GF2_PART90,
    IMG_GF2_PART910,
    IMG_GF2_PART911,
    IMG_GF2_PART91,
    IMG_GF2_PART92,
    IMG_GF2_PART93,
    IMG_GF2_PART94,
    IMG_GF2_PART95,
    IMG_GF2_PART96,
    IMG_GF2_PART97,
    IMG_GF2_PART98,
    IMG_GF2_PART99,
    IMG_GF3_25,
    IMG_GF3_30,
    IMG_GF3_31,
    IMG_GF3_PART00,
    IMG_GF3_PART010,
    IMG_GF3_PART011,
    IMG_GF3_PART01,
    IMG_GF3_PART02,
    IMG_GF3_PART03,
    IMG_GF3_PART04,
    IMG_GF3_PART05,
    IMG_GF3_PART06,
    IMG_GF3_PART07,
    IMG_GF3_PART08,
    IMG_GF3_PART09,
    IMG_GF3_PART100,
    IMG_GF3_PART1010,
    IMG_GF3_PART1011,
    IMG_GF3_PART101,
    IMG_GF3_PART102,
    IMG_GF3_PART103,
    IMG_GF3_PART104,
    IMG_GF3_PART105,
    IMG_GF3_PART106,
    IMG_GF3_PART107,
    IMG_GF3_PART108,
    IMG_GF3_PART109,
    IMG_GF3_PART10,
    IMG_GF3_PART110,
    IMG_GF3_PART1110,
    IMG_GF3_PART1111,
    IMG_GF3_PART111,
    IMG_GF3_PART112,
    IMG_GF3_PART113,
    IMG_GF3_PART114,
    IMG_GF3_PART115,
    IMG_GF3_PART116,
    IMG_GF3_PART117,
    IMG_GF3_PART118,
    IMG_GF3_PART119,
    IMG_GF3_PART11,
    IMG_GF3_PART120,
    IMG_GF3_PART1210,
    IMG_GF3_PART1211,
    IMG_GF3_PART121,
    IMG_GF3_PART122,
    IMG_GF3_PART123,
    IMG_GF3_PART124,
    IMG_GF3_PART125,
    IMG_GF3_PART126,
    IMG_GF3_PART127,
    IMG_GF3_PART128,
    IMG_GF3_PART129,
    IMG_GF3_PART12,
    IMG_GF3_PART130,
    IMG_GF3_PART1310,
    IMG_GF3_PART1311,
    IMG_GF3_PART131,
    IMG_GF3_PART132,
    IMG_GF3_PART133,
    IMG_GF3_PART134,
    IMG_GF3_PART135,
    IMG_GF3_PART136,
    IMG_GF3_PART137,
    IMG_GF3_PART138,
    IMG_GF3_PART139,
    IMG_GF3_PART13,
    IMG_GF3_PART140,
    IMG_GF3_PART1410,
    IMG_GF3_PART1411,
    IMG_GF3_PART141,
    IMG_GF3_PART142,
    IMG_GF3_PART143,
    IMG_GF3_PART144,
    IMG_GF3_PART145,
    IMG_GF3_PART146,
    IMG_GF3_PART147,
    IMG_GF3_PART148,
    IMG_GF3_PART149,
    IMG_GF3_PART14,
    IMG_GF3_PART150,
    IMG_GF3_PART1510,
    IMG_GF3_PART1511,
    IMG_GF3_PART151,
    IMG_GF3_PART152,
    IMG_GF3_PART153,
    IMG_GF3_PART154,
    IMG_GF3_PART155,
    IMG_GF3_PART156,
    IMG_GF3_PART157,
    IMG_GF3_PART158,
    IMG_GF3_PART159,
    IMG_GF3_PART15,
    IMG_GF3_PART16,
    IMG_GF3_PART17,
    IMG_GF3_PART18,
    IMG_GF3_PART19,
    IMG_GF3_PART20,
    IMG_GF3_PART210,
    IMG_GF3_PART211,
    IMG_GF3_PART21,
    IMG_GF3_PART22,
    IMG_GF3_PART23,
    IMG_GF3_PART24,
    IMG_GF3_PART25,
    IMG_GF3_PART26,
    IMG_GF3_PART27,
    IMG_GF3_PART28,
    IMG_GF3_PART29,
    IMG_GF3_PART30,
    IMG_GF3_PART310,
    IMG_GF3_PART311,
    IMG_GF3_PART31,
    IMG_GF3_PART32,
    IMG_GF3_PART33,
    IMG_GF3_PART34,
    IMG_GF3_PART35,
    IMG_GF3_PART36,
    IMG_GF3_PART37,
    IMG_GF3_PART38,
    IMG_GF3_PART39,
    IMG_GF3_PART40,
    IMG_GF3_PART410,
    IMG_GF3_PART411,
    IMG_GF3_PART41,
    IMG_GF3_PART42,
    IMG_GF3_PART43,
    IMG_GF3_PART44,
    IMG_GF3_PART45,
    IMG_GF3_PART46,
    IMG_GF3_PART47,
    IMG_GF3_PART48,
    IMG_GF3_PART49,
    IMG_GF3_PART50,
    IMG_GF3_PART510,
    IMG_GF3_PART511,
    IMG_GF3_PART51,
    IMG_GF3_PART52,
    IMG_GF3_PART53,
    IMG_GF3_PART54,
    IMG_GF3_PART55,
    IMG_GF3_PART56,
    IMG_GF3_PART57,
    IMG_GF3_PART58,
    IMG_GF3_PART59,
    IMG_GF3_PART60,
    IMG_GF3_PART610,
    IMG_GF3_PART611,
    IMG_GF3_PART61,
    IMG_GF3_PART62,
    IMG_GF3_PART63,
    IMG_GF3_PART64,
    IMG_GF3_PART65,
    IMG_GF3_PART66,
    IMG_GF3_PART67,
    IMG_GF3_PART68,
    IMG_GF3_PART69,
    IMG_GF3_PART70,
    IMG_GF3_PART710,
    IMG_GF3_PART711,
    IMG_GF3_PART71,
    IMG_GF3_PART72,
    IMG_GF3_PART73,
    IMG_GF3_PART74,
    IMG_GF3_PART75,
    IMG_GF3_PART76,
    IMG_GF3_PART77,
    IMG_GF3_PART78,
    IMG_GF3_PART79,
    IMG_GF3_PART80,
    IMG_GF3_PART810,
    IMG_GF3_PART811,
    IMG_GF3_PART81,
    IMG_GF3_PART82,
    IMG_GF3_PART83,
    IMG_GF3_PART84,
    IMG_GF3_PART85,
    IMG_GF3_PART86,
    IMG_GF3_PART87,
    IMG_GF3_PART88,
    IMG_GF3_PART89,
    IMG_GF3_PART90,
    IMG_GF3_PART910,
    IMG_GF3_PART911,
    IMG_GF3_PART91,
    IMG_GF3_PART92,
    IMG_GF3_PART93,
    IMG_GF3_PART94,
    IMG_GF3_PART95,
    IMG_GF3_PART96,
    IMG_GF3_PART97,
    IMG_GF3_PART98,
    IMG_GF3_PART99,
    IMG_GF4_10,
    IMG_GF4_11,
    IMG_GF4_1_2,
    IMG_GF4_12,
    IMG_GF4_13,
    IMG_GF4_14,
    IMG_GF4_15,
    IMG_GF4_16,
    IMG_GF4_17,
    IMG_GF4_18,
    IMG_GF4_19,
    IMG_GF4_1,
    IMG_GF4_20,
    IMG_GF4_2_1,
    IMG_GF4_21,
    IMG_GF4_2_2,
    IMG_GF4_22,
    IMG_GF4_23,
    IMG_GF4_24,
    IMG_GF4_25_2,
    IMG_GF4_26,
    IMG_GF4_27,
    IMG_GF4_2,
    IMG_GF4_3,
    IMG_GF4_4_2,
    IMG_GF4_4,
    IMG_GF4_5_1,
    IMG_GF4_5_2,
    IMG_GF4_5,
    IMG_GF4_6,
    IMG_GF4_7,
    IMG_GF4_8,
    IMG_GF4_9,
    IMG_GF4_BEAM1,
    IMG_GF4_BEAM2,
    IMG_GF4_BEAM3,
    IMG_GF4_BEAM4,
    IMG_GF4_CLOGO1,
    IMG_GF4_CLOGO2,
    IMG_GF4_CLOGO3,
    IMG_GF4_CLOGO4,
    IMG_GF4_CLOGO5,
    IMG_GF4_CLOGO6,
    IMG_GF4_CLOGO7,
    IMG_GF4_CLOGO8,
    IMG_GF4_CLOGO9,
    IMG_GF4_COVER,
    IMG_GF4_HMENU1,
    IMG_GF4_HMENU2,
    IMG_GF4_HMENU3,
    IMG_GF4_HMENU4,
    IMG_GF4_HMENU5,
    IMG_GF4_MA1,
    IMG_GF4_MA2,
    IMG_GF4_MA3,
    IMG_GF4_MA4,
    IMG_GF4_MA5,
    IMG_GF4_SBR,
    IMG_GF4_SHADE,
    IMG_GF4_XP1,
    IMG_GF4_XP2,
    IMG_GF4_XP3,
    IMG_GF4_XP4,
    IMG_GF4_XP5,
    IMG_GF4_XP6,
    IMG_GM1_B1,
    IMG_GM1_B2,
    IMG_GM1_B3,
    IMG_GM1_B4,
    IMG_GM1_EXIT,
    IMG_GM1_GAMEOVER,
    IMG_GM1_GO,
    IMG_GM1_KBON,
    IMG_GM1_MARKNEW,
    IMG_GM1_PICTURE,
    IMG_GM1_S0,
    IMG_GM1_S1,
    IMG_GM1_S2,
    IMG_GM1_S3,
    IMG_GM1_S4,
    IMG_GM1_S5,
    IMG_GM1_S6,
    IMG_GM1_S7,
    IMG_GM1_S8,
    IMG_GM1_S9,
    IMG_GM1_SBON,
    IMG_GM1_SHERIFF,
    IMG_GM1_SHIELD0,
    IMG_GM1_SHIELD1,
    IMG_GM1_SHIELD2,
    IMG_GM1_SHIELD3,
    IMG_GM1_SHIELD4,
    IMG_GM1_SHOOT,
    IMG_GM1_STAGE,
    IMG_GM1_T10,
    IMG_GM1_T11,
    IMG_GM1_T12,
    IMG_GM1_T1,
    IMG_GM1_T2,
    IMG_GM1_T3,
    IMG_GM1_T4,
    IMG_GM1_T5,
    IMG_GM1_T6,
    IMG_GM1_T7,
    IMG_GM1_T8,
    IMG_GM1_T9,
    IMG_GM1_TFIRE,
    IMG_GM1_TOKILL,
    IMG_GM1_XP1,
    IMG_GM1_XP2,
    IMG_GM1_XP3,
    IMG_GM1_XP4,
    IMG_GM1_XP5,
    IMG_GM1_XP6,
    IMG_HF1_PICTURE2,
    IMG_HF2_PICTURE2,
    IMG_HF3_PICTURE2,
    IMG_HF4_PICTURE2,
    IMG_HF5_PICTURE2,
    IMG_HF6_PICTURE2,
    IMG_HF8_PICTURE2,
    IMG_HP1_HYP1,
    IMG_HP1_HYP2,
    IMG_HP1_HYP3,
    IMG_HP1_HYP4,
    IMG_HP1_HYP5,
    IMG_HS1_F10,
    IMG_HS1_F11,
    IMG_HS1_F1,
    IMG_HS1_F2,
    IMG_HS1_F3,
    IMG_HS1_F4,
    IMG_HS1_F5,
    IMG_HS1_F6,
    IMG_HS1_F7,
    IMG_HS1_F8,
    IMG_HS1_F9,
    IMG_HS1_P0,
    IMG_HS1_P1,
    IMG_HS1_P2,
    IMG_HS1_P3,
    IMG_HS1_P4,
    IMG_HS1_P5,
    IMG_HS1_P6,
    IMG_HS1_P7,
    IMG_HS1_P8,
    IMG_HS1_S0,
    IMG_HS1_S1,
    IMG_HS1_S2,
    IMG_HS1_S3,
    IMG_HS1_S4,
    IMG_HS1_S5,
    IMG_HS1_S6,
    IMG_HS1_S7,
    IMG_HS1_S8,
    IMG_HS2_C10,
    IMG_HS2_C1,
    IMG_HS2_C2,
    IMG_HS2_C3,
    IMG_HS2_C4,
    IMG_HS2_C5,
    IMG_HS2_C6,
    IMG_HS2_C7,
    IMG_HS2_C8,
    IMG_HS2_C9,
    IMG_HS2_S,
    IMG_HS3_G1,
    IMG_HS3_S1,
    IMG_HS4_L10,
    IMG_HS4_L11,
    IMG_HS4_L12,
    IMG_HS4_L13,
    IMG_HS4_L14,
    IMG_HS4_L15,
    IMG_HS4_L1,
    IMG_HS4_L2,
    IMG_HS4_L3,
    IMG_HS4_L4,
    IMG_HS4_L5,
    IMG_HS4_L6,
    IMG_HS4_L7,
    IMG_HS4_L8,
    IMG_HS4_L9,
    IMG_LD0_LD0_1,
    IMG_LD0_LD0_2,
    IMG_LD0_LD0_3,
    IMG_LD0_LD0,
    IMG_LD1_LD1_1,
    IMG_LD1_LD1_2,
    IMG_LD1_LD1_3,
    IMG_LD1_LD1,
    IMG_LD2_LD2_1,
    IMG_LD2_LD2_2,
    IMG_LD2_LD2_3,
    IMG_LD2_LD2,
    IMG_LD3_LD3_1,
    IMG_LD3_LD3_2,
    IMG_LD3_LD3_3,
    IMG_LD3_LD3,
    IMG_LD4_LD4_1,
    IMG_LD4_LD4_2,
    IMG_LD4_LD4_3,
    IMG_LD4_LD4,
    IMG_LD5_LD5_1,
    IMG_LD5_LD5_2,
    IMG_LD5_LD5_3,
    IMG_LD5_LD5,
    IMG_LG1_PIC1,
    IMG_LG1_PIC2,
    IMG_LG1_PIC3,
    IMG_LG1_PIC4,
    IMG_LG1_PIC5,
    IMG_LP1_LAND10,
    IMG_LP1_LAND11,
    IMG_LP1_LAND12,
    IMG_LP1_LAND13,
    IMG_LP1_LAND14,
    IMG_LP1_LAND15,
    IMG_LP1_LAND16,
    IMG_LP1_LAND17,
    IMG_LP1_LAND18,
    IMG_LP1_LAND19,
    IMG_LP1_LAND20,
    IMG_LP1_LAND2,
    IMG_LP1_LAND3,
    IMG_LP1_LAND4,
    IMG_LP1_LAND5,
    IMG_LP1_LAND6,
    IMG_LP1_LAND7,
    IMG_LP1_LAND8,
    IMG_LP1_LAND9,
    IMG_ME1_MENU,
    IMG_ME2_MENU,
    IMG_ME4_MENU,
    IMG_ME5_MENU,
    IMG_ME6_MENU,
    IMG_ME7_MENU,
    IMG_OP1_OPEN1,
    IMG_OP1_OPEN2,
    IMG_OP1_OPEN3,
    IMG_OP1_OPEN4,
    IMG_OP1_OPEN5,
    IMG_OP1_OPEN6,
    IMG_PA_ROT,
    IMG_PL1_PLANET1,
    IMG_PL2_PLANET2,
    IMG_PL3_PLANET3,
    IMG_PL4_PLANET4,
    IMG_PL5_PLANET5,
    IMG_PL6_PLANET6,
    IMG_PL8_PLANET8,
    IMG_PP1_CONST,
    IMG_PP2_CONST,
    IMG_PP3_CONST,
    IMG_PT1_PATTERN,
    IMG_PT2_PATTERN,
    IMG_RD1_A1,
    IMG_RD1_ACT0,
    IMG_RD1_ACT1,
    IMG_RD1_ACT2,
    IMG_RD1_ACT3,
    IMG_RD1_ACT4,
    IMG_RD1_ACT5,
    IMG_RD1_BOW,
    IMG_RD1_DEAD10,
    IMG_RD1_DEAD1,
    IMG_RD1_DEAD2,
    IMG_RD1_DEAD3,
    IMG_RD1_DEAD4,
    IMG_RD1_DEAD5,
    IMG_RD1_DEAD6,
    IMG_RD1_DEAD7,
    IMG_RD1_DEAD8,
    IMG_RD1_DEAD9,
    IMG_RD1_F1,
    IMG_RD1_FAIL1,
    IMG_RD1_FAIL2,
    IMG_RD1_FAIL3,
    IMG_RD1_HIT,
    IMG_RD1_MBACK,
    IMG_RD1_RFAIL,
    IMG_RD1_ST1,
    IMG_RD1_ST2,
    IMG_RD1_ST3,
    IMG_RD1_ST4,
    IMG_RD1_STSH,
    IMG_RD1_SYMBOL1,
    IMG_RD1_SYMBOL1S,
    IMG_RD1_SYMBOL2,
    IMG_RD1_SYMBOL2S,
    IMG_RD1_SYMBOL3,
    IMG_RD1_SYMBOL3S,
    IMG_RD1_TLINE1,
    IMG_RD1_TLINE2,
    IMG_RD1_TLINE3,
    IMG_RD1_TYP1,
    IMG_RD1_TYP2,
    IMG_RD1_TYP3,
    IMG_RD1_TYP4,
    IMG_RD1_TYP5,
    IMG_RF1_1_2,
    IMG_RF1_13,
    IMG_RF1_1,
    IMG_RF1_7,
    IMG_SC1_PICTURE,
    IMG_SC1_SHUTTLE,
    IMG_SF1_SPART00,
    IMG_SF1_SPART01,
    IMG_SF1_SPART02,
    IMG_SF1_SPART03,
    IMG_SF1_SPART04,
    IMG_SF1_SPART05,
    IMG_SF1_SPART06,
    IMG_SF1_SPART07,
    IMG_SF1_SPART08,
    IMG_SF1_SPART09,
    IMG_SF1_SPART0C,
    IMG_SF1_SPART0E,
    IMG_SF1_SPART0G,
    IMG_SF1_SPART0L,
    IMG_SF1_SPART0Q,
    IMG_SF1_SPART0S,
    IMG_SF1_SPART10,
    IMG_SF1_SPART11,
    IMG_SF1_SPART12,
    IMG_SF1_SPART13,
    IMG_SF1_SPART14,
    IMG_SF1_SPART15,
    IMG_SF1_SPART16,
    IMG_SF1_SPART17,
    IMG_SF1_SPART18,
    IMG_SF1_SPART19,
    IMG_SF1_SPART1C,
    IMG_SF1_SPART1E,
    IMG_SF1_SPART1G,
    IMG_SF1_SPART1L,
    IMG_SF1_SPART1Q,
    IMG_SF1_SPART1S,
    IMG_SF1_SPART20,
    IMG_SF1_SPART21,
    IMG_SF1_SPART22,
    IMG_SF1_SPART23,
    IMG_SF1_SPART24,
    IMG_SF1_SPART25,
    IMG_SF1_SPART26,
    IMG_SF1_SPART27,
    IMG_SF1_SPART28,
    IMG_SF1_SPART29,
    IMG_SF1_SPART2C,
    IMG_SF1_SPART2E,
    IMG_SF1_SPART2G,
    IMG_SF1_SPART2L,
    IMG_SF1_SPART2Q,
    IMG_SF1_SPART2S,
    IMG_SF1_SPART30,
    IMG_SF1_SPART31,
    IMG_SF1_SPART32,
    IMG_SF1_SPART33,
    IMG_SF1_SPART34,
    IMG_SF1_SPART35,
    IMG_SF1_SPART36,
    IMG_SF1_SPART37,
    IMG_SF1_SPART38,
    IMG_SF1_SPART39,
    IMG_SF1_SPART3C,
    IMG_SF1_SPART3E,
    IMG_SF1_SPART3G,
    IMG_SF1_SPART3L,
    IMG_SF1_SPART3Q,
    IMG_SF1_SPART3S,
    IMG_SF1_SPART40,
    IMG_SF1_SPART41,
    IMG_SF1_SPART42,
    IMG_SF1_SPART43,
    IMG_SF1_SPART44,
    IMG_SF1_SPART45,
    IMG_SF1_SPART46,
    IMG_SF1_SPART47,
    IMG_SF1_SPART48,
    IMG_SF1_SPART49,
    IMG_SF1_SPART4C,
    IMG_SF1_SPART4E,
    IMG_SF1_SPART4G,
    IMG_SF1_SPART4L,
    IMG_SF1_SPART4Q,
    IMG_SF1_SPART4S,
    IMG_SF1_SPART50,
    IMG_SF1_SPART51,
    IMG_SF1_SPART52,
    IMG_SF1_SPART53,
    IMG_SF1_SPART54,
    IMG_SF1_SPART55,
    IMG_SF1_SPART56,
    IMG_SF1_SPART57,
    IMG_SF1_SPART58,
    IMG_SF1_SPART59,
    IMG_SF1_SPART5C,
    IMG_SF1_SPART5E,
    IMG_SF1_SPART5G,
    IMG_SF1_SPART5L,
    IMG_SF1_SPART5Q,
    IMG_SF1_SPART5S,
    IMG_SF1_SPART60,
    IMG_SF1_SPART61,
    IMG_SF1_SPART62,
    IMG_SF1_SPART63,
    IMG_SF1_SPART64,
    IMG_SF1_SPART65,
    IMG_SF1_SPART66,
    IMG_SF1_SPART67,
    IMG_SF1_SPART68,
    IMG_SF1_SPART69,
    IMG_SF1_SPART6C,
    IMG_SF1_SPART6E,
    IMG_SF1_SPART6G,
    IMG_SF1_SPART6L,
    IMG_SF1_SPART6Q,
    IMG_SF1_SPART6S,
    IMG_SF1_SPART70,
    IMG_SF1_SPART71,
    IMG_SF1_SPART72,
    IMG_SF1_SPART73,
    IMG_SF1_SPART74,
    IMG_SF1_SPART75,
    IMG_SF1_SPART76,
    IMG_SF1_SPART77,
    IMG_SF1_SPART78,
    IMG_SF1_SPART79,
    IMG_SF1_SPART7C,
    IMG_SF1_SPART7E,
    IMG_SF1_SPART7G,
    IMG_SF1_SPART7L,
    IMG_SF1_SPART7Q,
    IMG_SF1_SPART7S,
    IMG_SF1_SPART80,
    IMG_SF1_SPART81,
    IMG_SF1_SPART82,
    IMG_SF1_SPART83,
    IMG_SF1_SPART84,
    IMG_SF1_SPART85,
    IMG_SF1_SPART86,
    IMG_SF1_SPART87,
    IMG_SF1_SPART88,
    IMG_SF1_SPART89,
    IMG_SF1_SPART8C,
    IMG_SF1_SPART8E,
    IMG_SF1_SPART8G,
    IMG_SF1_SPART8L,
    IMG_SF1_SPART8Q,
    IMG_SF1_SPART8S,
    IMG_SF1_SPART90,
    IMG_SF1_SPART91,
    IMG_SF1_SPART92,
    IMG_SF1_SPART93,
    IMG_SF1_SPART94,
    IMG_SF1_SPART95,
    IMG_SF1_SPART96,
    IMG_SF1_SPART97,
    IMG_SF1_SPART98,
    IMG_SF1_SPART99,
    IMG_SF1_SPART9C,
    IMG_SF1_SPART9E,
    IMG_SF1_SPART9G,
    IMG_SF1_SPART9L,
    IMG_SF1_SPART9Q,
    IMG_SF1_SPART9S,
    IMG_SF1_SPARTC0,
    IMG_SF1_SPARTC1,
    IMG_SF1_SPARTC2,
    IMG_SF1_SPARTC3,
    IMG_SF1_SPARTC4,
    IMG_SF1_SPARTC5,
    IMG_SF1_SPARTC6,
    IMG_SF1_SPARTC7,
    IMG_SF1_SPARTC8,
    IMG_SF1_SPARTC9,
    IMG_SF1_SPARTCC,
    IMG_SF1_SPARTCE,
    IMG_SF1_SPARTCG,
    IMG_SF1_SPARTCL,
    IMG_SF1_SPARTCQ,
    IMG_SF1_SPARTCS,
    IMG_SF1_SPARTE0,
    IMG_SF1_SPARTE1,
    IMG_SF1_SPARTE2,
    IMG_SF1_SPARTE3,
    IMG_SF1_SPARTE4,
    IMG_SF1_SPARTE5,
    IMG_SF1_SPARTE6,
    IMG_SF1_SPARTE7,
    IMG_SF1_SPARTE8,
    IMG_SF1_SPARTE9,
    IMG_SF1_SPARTEC,
    IMG_SF1_SPARTEE,
    IMG_SF1_SPARTEG,
    IMG_SF1_SPARTEL,
    IMG_SF1_SPARTEQ,
    IMG_SF1_SPARTES,
    IMG_SF1_SPARTG0,
    IMG_SF1_SPARTG1,
    IMG_SF1_SPARTG2,
    IMG_SF1_SPARTG3,
    IMG_SF1_SPARTG4,
    IMG_SF1_SPARTG5,
    IMG_SF1_SPARTG6,
    IMG_SF1_SPARTG7,
    IMG_SF1_SPARTG8,
    IMG_SF1_SPARTG9,
    IMG_SF1_SPARTGC,
    IMG_SF1_SPARTGE,
    IMG_SF1_SPARTGG,
    IMG_SF1_SPARTGL,
    IMG_SF1_SPARTGQ,
    IMG_SF1_SPARTGS,
    IMG_SF1_SPARTL0,
    IMG_SF1_SPARTL1,
    IMG_SF1_SPARTL2,
    IMG_SF1_SPARTL3,
    IMG_SF1_SPARTL4,
    IMG_SF1_SPARTL5,
    IMG_SF1_SPARTL6,
    IMG_SF1_SPARTL7,
    IMG_SF1_SPARTL8,
    IMG_SF1_SPARTL9,
    IMG_SF1_SPARTLC,
    IMG_SF1_SPARTLE,
    IMG_SF1_SPARTLG,
    IMG_SF1_SPARTLL,
    IMG_SF1_SPARTLQ,
    IMG_SF1_SPARTLS,
    IMG_SF1_SPARTQ0,
    IMG_SF1_SPARTQ1,
    IMG_SF1_SPARTQ2,
    IMG_SF1_SPARTQ3,
    IMG_SF1_SPARTQ4,
    IMG_SF1_SPARTQ5,
    IMG_SF1_SPARTQ6,
    IMG_SF1_SPARTQ7,
    IMG_SF1_SPARTQ8,
    IMG_SF1_SPARTQ9,
    IMG_SF1_SPARTQC,
    IMG_SF1_SPARTQE,
    IMG_SF1_SPARTQG,
    IMG_SF1_SPARTQL,
    IMG_SF1_SPARTQQ,
    IMG_SF1_SPARTQS,
    IMG_SF1_SPARTS0,
    IMG_SF1_SPARTS1,
    IMG_SF1_SPARTS2,
    IMG_SF1_SPARTS3,
    IMG_SF1_SPARTS4,
    IMG_SF1_SPARTS5,
    IMG_SF1_SPARTS6,
    IMG_SF1_SPARTS7,
    IMG_SF1_SPARTS8,
    IMG_SF1_SPARTS9,
    IMG_SF1_SPARTSC,
    IMG_SF1_SPARTSE,
    IMG_SF1_SPARTSG,
    IMG_SF1_SPARTSL,
    IMG_SF1_SPARTSQ,
    IMG_SF1_SPARTSS,
    IMG_SF1_STONE0,
    IMG_SF1_STONE10,
    IMG_SF1_STONE19_0,
    IMG_SF1_STONE2_0,
    IMG_SF1_STONE29B,
    IMG_SF1_STONE29,
    IMG_SF1_STONE2,
    IMG_SF1_STONE8,
    IMG_SF1_STONE9,
    IMG_SF2_SPART00,
    IMG_SF2_SPART01,
    IMG_SF2_SPART02,
    IMG_SF2_SPART03,
    IMG_SF2_SPART04,
    IMG_SF2_SPART05,
    IMG_SF2_SPART06,
    IMG_SF2_SPART07,
    IMG_SF2_SPART08,
    IMG_SF2_SPART09,
    IMG_SF2_SPART0C,
    IMG_SF2_SPART0E,
    IMG_SF2_SPART0G,
    IMG_SF2_SPART0L,
    IMG_SF2_SPART0Q,
    IMG_SF2_SPART0S,
    IMG_SF2_SPART10,
    IMG_SF2_SPART11,
    IMG_SF2_SPART12,
    IMG_SF2_SPART13,
    IMG_SF2_SPART14,
    IMG_SF2_SPART15,
    IMG_SF2_SPART16,
    IMG_SF2_SPART17,
    IMG_SF2_SPART18,
    IMG_SF2_SPART19,
    IMG_SF2_SPART1C,
    IMG_SF2_SPART1E,
    IMG_SF2_SPART1G,
    IMG_SF2_SPART1L,
    IMG_SF2_SPART1Q,
    IMG_SF2_SPART1S,
    IMG_SF2_SPART20,
    IMG_SF2_SPART21,
    IMG_SF2_SPART22,
    IMG_SF2_SPART23,
    IMG_SF2_SPART24,
    IMG_SF2_SPART25,
    IMG_SF2_SPART26,
    IMG_SF2_SPART27,
    IMG_SF2_SPART28,
    IMG_SF2_SPART29,
    IMG_SF2_SPART2C,
    IMG_SF2_SPART2E,
    IMG_SF2_SPART2G,
    IMG_SF2_SPART2L,
    IMG_SF2_SPART2Q,
    IMG_SF2_SPART2S,
    IMG_SF2_SPART30,
    IMG_SF2_SPART31,
    IMG_SF2_SPART32,
    IMG_SF2_SPART33,
    IMG_SF2_SPART34,
    IMG_SF2_SPART35,
    IMG_SF2_SPART36,
    IMG_SF2_SPART37,
    IMG_SF2_SPART38,
    IMG_SF2_SPART39,
    IMG_SF2_SPART3C,
    IMG_SF2_SPART3E,
    IMG_SF2_SPART3G,
    IMG_SF2_SPART3L,
    IMG_SF2_SPART3Q,
    IMG_SF2_SPART3S,
    IMG_SF2_SPART40,
    IMG_SF2_SPART41,
    IMG_SF2_SPART42,
    IMG_SF2_SPART43,
    IMG_SF2_SPART44,
    IMG_SF2_SPART45,
    IMG_SF2_SPART46,
    IMG_SF2_SPART47,
    IMG_SF2_SPART48,
    IMG_SF2_SPART49,
    IMG_SF2_SPART4C,
    IMG_SF2_SPART4E,
    IMG_SF2_SPART4G,
    IMG_SF2_SPART4L,
    IMG_SF2_SPART4Q,
    IMG_SF2_SPART4S,
    IMG_SF2_SPART50,
    IMG_SF2_SPART51,
    IMG_SF2_SPART52,
    IMG_SF2_SPART53,
    IMG_SF2_SPART54,
    IMG_SF2_SPART55,
    IMG_SF2_SPART56,
    IMG_SF2_SPART57,
    IMG_SF2_SPART58,
    IMG_SF2_SPART59,
    IMG_SF2_SPART5C,
    IMG_SF2_SPART5E,
    IMG_SF2_SPART5G,
    IMG_SF2_SPART5L,
    IMG_SF2_SPART5Q,
    IMG_SF2_SPART5S,
    IMG_SF2_SPART60,
    IMG_SF2_SPART61,
    IMG_SF2_SPART62,
    IMG_SF2_SPART63,
    IMG_SF2_SPART64,
    IMG_SF2_SPART65,
    IMG_SF2_SPART66,
    IMG_SF2_SPART67,
    IMG_SF2_SPART68,
    IMG_SF2_SPART69,
    IMG_SF2_SPART6C,
    IMG_SF2_SPART6E,
    IMG_SF2_SPART6G,
    IMG_SF2_SPART6L,
    IMG_SF2_SPART6Q,
    IMG_SF2_SPART6S,
    IMG_SF2_SPART70,
    IMG_SF2_SPART71,
    IMG_SF2_SPART72,
    IMG_SF2_SPART73,
    IMG_SF2_SPART74,
    IMG_SF2_SPART75,
    IMG_SF2_SPART76,
    IMG_SF2_SPART77,
    IMG_SF2_SPART78,
    IMG_SF2_SPART79,
    IMG_SF2_SPART7C,
    IMG_SF2_SPART7E,
    IMG_SF2_SPART7G,
    IMG_SF2_SPART7L,
    IMG_SF2_SPART7Q,
    IMG_SF2_SPART7S,
    IMG_SF2_SPART80,
    IMG_SF2_SPART81,
    IMG_SF2_SPART82,
    IMG_SF2_SPART83,
    IMG_SF2_SPART84,
    IMG_SF2_SPART85,
    IMG_SF2_SPART86,
    IMG_SF2_SPART87,
    IMG_SF2_SPART88,
    IMG_SF2_SPART89,
    IMG_SF2_SPART8C,
    IMG_SF2_SPART8E,
    IMG_SF2_SPART8G,
    IMG_SF2_SPART8L,
    IMG_SF2_SPART8Q,
    IMG_SF2_SPART8S,
    IMG_SF2_SPART90,
    IMG_SF2_SPART91,
    IMG_SF2_SPART92,
    IMG_SF2_SPART93,
    IMG_SF2_SPART94,
    IMG_SF2_SPART95,
    IMG_SF2_SPART96,
    IMG_SF2_SPART97,
    IMG_SF2_SPART98,
    IMG_SF2_SPART99,
    IMG_SF2_SPART9C,
    IMG_SF2_SPART9E,
    IMG_SF2_SPART9G,
    IMG_SF2_SPART9L,
    IMG_SF2_SPART9Q,
    IMG_SF2_SPART9S,
    IMG_SF2_SPARTC0,
    IMG_SF2_SPARTC1,
    IMG_SF2_SPARTC2,
    IMG_SF2_SPARTC3,
    IMG_SF2_SPARTC4,
    IMG_SF2_SPARTC5,
    IMG_SF2_SPARTC6,
    IMG_SF2_SPARTC7,
    IMG_SF2_SPARTC8,
    IMG_SF2_SPARTC9,
    IMG_SF2_SPARTCC,
    IMG_SF2_SPARTCE,
    IMG_SF2_SPARTCG,
    IMG_SF2_SPARTCL,
    IMG_SF2_SPARTCQ,
    IMG_SF2_SPARTCS,
    IMG_SF2_SPARTE0,
    IMG_SF2_SPARTE1,
    IMG_SF2_SPARTE2,
    IMG_SF2_SPARTE3,
    IMG_SF2_SPARTE4,
    IMG_SF2_SPARTE5,
    IMG_SF2_SPARTE6,
    IMG_SF2_SPARTE7,
    IMG_SF2_SPARTE8,
    IMG_SF2_SPARTE9,
    IMG_SF2_SPARTEC,
    IMG_SF2_SPARTEE,
    IMG_SF2_SPARTEG,
    IMG_SF2_SPARTEL,
    IMG_SF2_SPARTEQ,
    IMG_SF2_SPARTES,
    IMG_SF2_SPARTG0,
    IMG_SF2_SPARTG1,
    IMG_SF2_SPARTG2,
    IMG_SF2_SPARTG3,
    IMG_SF2_SPARTG4,
    IMG_SF2_SPARTG5,
    IMG_SF2_SPARTG6,
    IMG_SF2_SPARTG7,
    IMG_SF2_SPARTG8,
    IMG_SF2_SPARTG9,
    IMG_SF2_SPARTGC,
    IMG_SF2_SPARTGE,
    IMG_SF2_SPARTGG,
    IMG_SF2_SPARTGL,
    IMG_SF2_SPARTGQ,
    IMG_SF2_SPARTGS,
    IMG_SF2_SPARTL0,
    IMG_SF2_SPARTL1,
    IMG_SF2_SPARTL2,
    IMG_SF2_SPARTL3,
    IMG_SF2_SPARTL4,
    IMG_SF2_SPARTL5,
    IMG_SF2_SPARTL6,
    IMG_SF2_SPARTL7,
    IMG_SF2_SPARTL8,
    IMG_SF2_SPARTL9,
    IMG_SF2_SPARTLC,
    IMG_SF2_SPARTLE,
    IMG_SF2_SPARTLG,
    IMG_SF2_SPARTLL,
    IMG_SF2_SPARTLQ,
    IMG_SF2_SPARTLS,
    IMG_SF2_SPARTQ0,
    IMG_SF2_SPARTQ1,
    IMG_SF2_SPARTQ2,
    IMG_SF2_SPARTQ3,
    IMG_SF2_SPARTQ4,
    IMG_SF2_SPARTQ5,
    IMG_SF2_SPARTQ6,
    IMG_SF2_SPARTQ7,
    IMG_SF2_SPARTQ8,
    IMG_SF2_SPARTQ9,
    IMG_SF2_SPARTQC,
    IMG_SF2_SPARTQE,
    IMG_SF2_SPARTQG,
    IMG_SF2_SPARTQL,
    IMG_SF2_SPARTQQ,
    IMG_SF2_SPARTQS,
    IMG_SF2_SPARTS0,
    IMG_SF2_SPARTS1,
    IMG_SF2_SPARTS2,
    IMG_SF2_SPARTS3,
    IMG_SF2_SPARTS4,
    IMG_SF2_SPARTS5,
    IMG_SF2_SPARTS6,
    IMG_SF2_SPARTS7,
    IMG_SF2_SPARTS8,
    IMG_SF2_SPARTS9,
    IMG_SF2_SPARTSC,
    IMG_SF2_SPARTSE,
    IMG_SF2_SPARTSG,
    IMG_SF2_SPARTSL,
    IMG_SF2_SPARTSQ,
    IMG_SF2_SPARTSS,
    IMG_SF2_STONE0,
    IMG_SF2_STONE10,
    IMG_SF2_STONE19_0,
    IMG_SF2_STONE2_0,
    IMG_SF2_STONE29B,
    IMG_SF2_STONE29,
    IMG_SF2_STONE2,
    IMG_SF2_STONE8,
    IMG_SF2_STONE9,
    IMG_SF3_SPART00,
    IMG_SF3_SPART01,
    IMG_SF3_SPART02,
    IMG_SF3_SPART03,
    IMG_SF3_SPART04,
    IMG_SF3_SPART05,
    IMG_SF3_SPART06,
    IMG_SF3_SPART07,
    IMG_SF3_SPART08,
    IMG_SF3_SPART09,
    IMG_SF3_SPART0C,
    IMG_SF3_SPART0E,
    IMG_SF3_SPART0G,
    IMG_SF3_SPART0L,
    IMG_SF3_SPART0Q,
    IMG_SF3_SPART0S,
    IMG_SF3_SPART10,
    IMG_SF3_SPART11,
    IMG_SF3_SPART12,
    IMG_SF3_SPART13,
    IMG_SF3_SPART14,
    IMG_SF3_SPART15,
    IMG_SF3_SPART16,
    IMG_SF3_SPART17,
    IMG_SF3_SPART18,
    IMG_SF3_SPART19,
    IMG_SF3_SPART1C,
    IMG_SF3_SPART1E,
    IMG_SF3_SPART1G,
    IMG_SF3_SPART1L,
    IMG_SF3_SPART1Q,
    IMG_SF3_SPART1S,
    IMG_SF3_SPART20,
    IMG_SF3_SPART21,
    IMG_SF3_SPART22,
    IMG_SF3_SPART23,
    IMG_SF3_SPART24,
    IMG_SF3_SPART25,
    IMG_SF3_SPART26,
    IMG_SF3_SPART27,
    IMG_SF3_SPART28,
    IMG_SF3_SPART29,
    IMG_SF3_SPART2C,
    IMG_SF3_SPART2E,
    IMG_SF3_SPART2G,
    IMG_SF3_SPART2L,
    IMG_SF3_SPART2Q,
    IMG_SF3_SPART2S,
    IMG_SF3_SPART30,
    IMG_SF3_SPART31,
    IMG_SF3_SPART32,
    IMG_SF3_SPART33,
    IMG_SF3_SPART34,
    IMG_SF3_SPART35,
    IMG_SF3_SPART36,
    IMG_SF3_SPART37,
    IMG_SF3_SPART38,
    IMG_SF3_SPART39,
    IMG_SF3_SPART3C,
    IMG_SF3_SPART3E,
    IMG_SF3_SPART3G,
    IMG_SF3_SPART3L,
    IMG_SF3_SPART3Q,
    IMG_SF3_SPART3S,
    IMG_SF3_SPART40,
    IMG_SF3_SPART41,
    IMG_SF3_SPART42,
    IMG_SF3_SPART43,
    IMG_SF3_SPART44,
    IMG_SF3_SPART45,
    IMG_SF3_SPART46,
    IMG_SF3_SPART47,
    IMG_SF3_SPART48,
    IMG_SF3_SPART49,
    IMG_SF3_SPART4C,
    IMG_SF3_SPART4E,
    IMG_SF3_SPART4G,
    IMG_SF3_SPART4L,
    IMG_SF3_SPART4Q,
    IMG_SF3_SPART4S,
    IMG_SF3_SPART50,
    IMG_SF3_SPART51,
    IMG_SF3_SPART52,
    IMG_SF3_SPART53,
    IMG_SF3_SPART54,
    IMG_SF3_SPART55,
    IMG_SF3_SPART56,
    IMG_SF3_SPART57,
    IMG_SF3_SPART58,
    IMG_SF3_SPART59,
    IMG_SF3_SPART5C,
    IMG_SF3_SPART5E,
    IMG_SF3_SPART5G,
    IMG_SF3_SPART5L,
    IMG_SF3_SPART5Q,
    IMG_SF3_SPART5S,
    IMG_SF3_SPART60,
    IMG_SF3_SPART61,
    IMG_SF3_SPART62,
    IMG_SF3_SPART63,
    IMG_SF3_SPART64,
    IMG_SF3_SPART65,
    IMG_SF3_SPART66,
    IMG_SF3_SPART67,
    IMG_SF3_SPART68,
    IMG_SF3_SPART69,
    IMG_SF3_SPART6C,
    IMG_SF3_SPART6E,
    IMG_SF3_SPART6G,
    IMG_SF3_SPART6L,
    IMG_SF3_SPART6Q,
    IMG_SF3_SPART6S,
    IMG_SF3_SPART70,
    IMG_SF3_SPART71,
    IMG_SF3_SPART72,
    IMG_SF3_SPART73,
    IMG_SF3_SPART74,
    IMG_SF3_SPART75,
    IMG_SF3_SPART76,
    IMG_SF3_SPART77,
    IMG_SF3_SPART78,
    IMG_SF3_SPART79,
    IMG_SF3_SPART7C,
    IMG_SF3_SPART7E,
    IMG_SF3_SPART7G,
    IMG_SF3_SPART7L,
    IMG_SF3_SPART7Q,
    IMG_SF3_SPART7S,
    IMG_SF3_SPART80,
    IMG_SF3_SPART81,
    IMG_SF3_SPART82,
    IMG_SF3_SPART83,
    IMG_SF3_SPART84,
    IMG_SF3_SPART85,
    IMG_SF3_SPART86,
    IMG_SF3_SPART87,
    IMG_SF3_SPART88,
    IMG_SF3_SPART89,
    IMG_SF3_SPART8C,
    IMG_SF3_SPART8E,
    IMG_SF3_SPART8G,
    IMG_SF3_SPART8L,
    IMG_SF3_SPART8Q,
    IMG_SF3_SPART8S,
    IMG_SF3_SPART90,
    IMG_SF3_SPART91,
    IMG_SF3_SPART92,
    IMG_SF3_SPART93,
    IMG_SF3_SPART94,
    IMG_SF3_SPART95,
    IMG_SF3_SPART96,
    IMG_SF3_SPART97,
    IMG_SF3_SPART98,
    IMG_SF3_SPART99,
    IMG_SF3_SPART9C,
    IMG_SF3_SPART9E,
    IMG_SF3_SPART9G,
    IMG_SF3_SPART9L,
    IMG_SF3_SPART9Q,
    IMG_SF3_SPART9S,
    IMG_SF3_SPARTC0,
    IMG_SF3_SPARTC1,
    IMG_SF3_SPARTC2,
    IMG_SF3_SPARTC3,
    IMG_SF3_SPARTC4,
    IMG_SF3_SPARTC5,
    IMG_SF3_SPARTC6,
    IMG_SF3_SPARTC7,
    IMG_SF3_SPARTC8,
    IMG_SF3_SPARTC9,
    IMG_SF3_SPARTCC,
    IMG_SF3_SPARTCE,
    IMG_SF3_SPARTCG,
    IMG_SF3_SPARTCL,
    IMG_SF3_SPARTCQ,
    IMG_SF3_SPARTCS,
    IMG_SF3_SPARTE0,
    IMG_SF3_SPARTE1,
    IMG_SF3_SPARTE2,
    IMG_SF3_SPARTE3,
    IMG_SF3_SPARTE4,
    IMG_SF3_SPARTE5,
    IMG_SF3_SPARTE6,
    IMG_SF3_SPARTE7,
    IMG_SF3_SPARTE8,
    IMG_SF3_SPARTE9,
    IMG_SF3_SPARTEC,
    IMG_SF3_SPARTEE,
    IMG_SF3_SPARTEG,
    IMG_SF3_SPARTEL,
    IMG_SF3_SPARTEQ,
    IMG_SF3_SPARTES,
    IMG_SF3_SPARTG0,
    IMG_SF3_SPARTG1,
    IMG_SF3_SPARTG2,
    IMG_SF3_SPARTG3,
    IMG_SF3_SPARTG4,
    IMG_SF3_SPARTG5,
    IMG_SF3_SPARTG6,
    IMG_SF3_SPARTG7,
    IMG_SF3_SPARTG8,
    IMG_SF3_SPARTG9,
    IMG_SF3_SPARTGC,
    IMG_SF3_SPARTGE,
    IMG_SF3_SPARTGG,
    IMG_SF3_SPARTGL,
    IMG_SF3_SPARTGQ,
    IMG_SF3_SPARTGS,
    IMG_SF3_SPARTL0,
    IMG_SF3_SPARTL1,
    IMG_SF3_SPARTL2,
    IMG_SF3_SPARTL3,
    IMG_SF3_SPARTL4,
    IMG_SF3_SPARTL5,
    IMG_SF3_SPARTL6,
    IMG_SF3_SPARTL7,
    IMG_SF3_SPARTL8,
    IMG_SF3_SPARTL9,
    IMG_SF3_SPARTLC,
    IMG_SF3_SPARTLE,
    IMG_SF3_SPARTLG,
    IMG_SF3_SPARTLL,
    IMG_SF3_SPARTLQ,
    IMG_SF3_SPARTLS,
    IMG_SF3_SPARTQ0,
    IMG_SF3_SPARTQ1,
    IMG_SF3_SPARTQ2,
    IMG_SF3_SPARTQ3,
    IMG_SF3_SPARTQ4,
    IMG_SF3_SPARTQ5,
    IMG_SF3_SPARTQ6,
    IMG_SF3_SPARTQ7,
    IMG_SF3_SPARTQ8,
    IMG_SF3_SPARTQ9,
    IMG_SF3_SPARTQC,
    IMG_SF3_SPARTQE,
    IMG_SF3_SPARTQG,
    IMG_SF3_SPARTQL,
    IMG_SF3_SPARTQQ,
    IMG_SF3_SPARTQS,
    IMG_SF3_SPARTS0,
    IMG_SF3_SPARTS1,
    IMG_SF3_SPARTS2,
    IMG_SF3_SPARTS3,
    IMG_SF3_SPARTS4,
    IMG_SF3_SPARTS5,
    IMG_SF3_SPARTS6,
    IMG_SF3_SPARTS7,
    IMG_SF3_SPARTS8,
    IMG_SF3_SPARTS9,
    IMG_SF3_SPARTSC,
    IMG_SF3_SPARTSE,
    IMG_SF3_SPARTSG,
    IMG_SF3_SPARTSL,
    IMG_SF3_SPARTSQ,
    IMG_SF3_SPARTSS,
    IMG_SF3_STONE0,
    IMG_SF3_STONE10,
    IMG_SF3_STONE19_0,
    IMG_SF3_STONE2_0,
    IMG_SF3_STONE29B,
    IMG_SF3_STONE29,
    IMG_SF3_STONE2,
    IMG_SF3_STONE8,
    IMG_SF3_STONE9,
    IMG_SF4_SPART00,
    IMG_SF4_SPART01,
    IMG_SF4_SPART02,
    IMG_SF4_SPART03,
    IMG_SF4_SPART04,
    IMG_SF4_SPART05,
    IMG_SF4_SPART06,
    IMG_SF4_SPART07,
    IMG_SF4_SPART08,
    IMG_SF4_SPART09,
    IMG_SF4_SPART0C,
    IMG_SF4_SPART0E,
    IMG_SF4_SPART0G,
    IMG_SF4_SPART0L,
    IMG_SF4_SPART0Q,
    IMG_SF4_SPART0S,
    IMG_SF4_SPART10,
    IMG_SF4_SPART11,
    IMG_SF4_SPART12,
    IMG_SF4_SPART13,
    IMG_SF4_SPART14,
    IMG_SF4_SPART15,
    IMG_SF4_SPART16,
    IMG_SF4_SPART17,
    IMG_SF4_SPART18,
    IMG_SF4_SPART19,
    IMG_SF4_SPART1C,
    IMG_SF4_SPART1E,
    IMG_SF4_SPART1G,
    IMG_SF4_SPART1L,
    IMG_SF4_SPART1Q,
    IMG_SF4_SPART1S,
    IMG_SF4_SPART20,
    IMG_SF4_SPART21,
    IMG_SF4_SPART22,
    IMG_SF4_SPART23,
    IMG_SF4_SPART24,
    IMG_SF4_SPART25,
    IMG_SF4_SPART26,
    IMG_SF4_SPART27,
    IMG_SF4_SPART28,
    IMG_SF4_SPART29,
    IMG_SF4_SPART2C,
    IMG_SF4_SPART2E,
    IMG_SF4_SPART2G,
    IMG_SF4_SPART2L,
    IMG_SF4_SPART2Q,
    IMG_SF4_SPART2S,
    IMG_SF4_SPART30,
    IMG_SF4_SPART31,
    IMG_SF4_SPART32,
    IMG_SF4_SPART33,
    IMG_SF4_SPART34,
    IMG_SF4_SPART35,
    IMG_SF4_SPART36,
    IMG_SF4_SPART37,
    IMG_SF4_SPART38,
    IMG_SF4_SPART39,
    IMG_SF4_SPART3C,
    IMG_SF4_SPART3E,
    IMG_SF4_SPART3G,
    IMG_SF4_SPART3L,
    IMG_SF4_SPART3Q,
    IMG_SF4_SPART3S,
    IMG_SF4_SPART40,
    IMG_SF4_SPART41,
    IMG_SF4_SPART42,
    IMG_SF4_SPART43,
    IMG_SF4_SPART44,
    IMG_SF4_SPART45,
    IMG_SF4_SPART46,
    IMG_SF4_SPART47,
    IMG_SF4_SPART48,
    IMG_SF4_SPART49,
    IMG_SF4_SPART4C,
    IMG_SF4_SPART4E,
    IMG_SF4_SPART4G,
    IMG_SF4_SPART4L,
    IMG_SF4_SPART4Q,
    IMG_SF4_SPART4S,
    IMG_SF4_SPART50,
    IMG_SF4_SPART51,
    IMG_SF4_SPART52,
    IMG_SF4_SPART53,
    IMG_SF4_SPART54,
    IMG_SF4_SPART55,
    IMG_SF4_SPART56,
    IMG_SF4_SPART57,
    IMG_SF4_SPART58,
    IMG_SF4_SPART59,
    IMG_SF4_SPART5C,
    IMG_SF4_SPART5E,
    IMG_SF4_SPART5G,
    IMG_SF4_SPART5L,
    IMG_SF4_SPART5Q,
    IMG_SF4_SPART5S,
    IMG_SF4_SPART60,
    IMG_SF4_SPART61,
    IMG_SF4_SPART62,
    IMG_SF4_SPART63,
    IMG_SF4_SPART64,
    IMG_SF4_SPART65,
    IMG_SF4_SPART66,
    IMG_SF4_SPART67,
    IMG_SF4_SPART68,
    IMG_SF4_SPART69,
    IMG_SF4_SPART6C,
    IMG_SF4_SPART6E,
    IMG_SF4_SPART6G,
    IMG_SF4_SPART6L,
    IMG_SF4_SPART6Q,
    IMG_SF4_SPART6S,
    IMG_SF4_SPART70,
    IMG_SF4_SPART71,
    IMG_SF4_SPART72,
    IMG_SF4_SPART73,
    IMG_SF4_SPART74,
    IMG_SF4_SPART75,
    IMG_SF4_SPART76,
    IMG_SF4_SPART77,
    IMG_SF4_SPART78,
    IMG_SF4_SPART79,
    IMG_SF4_SPART7C,
    IMG_SF4_SPART7E,
    IMG_SF4_SPART7G,
    IMG_SF4_SPART7L,
    IMG_SF4_SPART7Q,
    IMG_SF4_SPART7S,
    IMG_SF4_SPART80,
    IMG_SF4_SPART81,
    IMG_SF4_SPART82,
    IMG_SF4_SPART83,
    IMG_SF4_SPART84,
    IMG_SF4_SPART85,
    IMG_SF4_SPART86,
    IMG_SF4_SPART87,
    IMG_SF4_SPART88,
    IMG_SF4_SPART89,
    IMG_SF4_SPART8C,
    IMG_SF4_SPART8E,
    IMG_SF4_SPART8G,
    IMG_SF4_SPART8L,
    IMG_SF4_SPART8Q,
    IMG_SF4_SPART8S,
    IMG_SF4_SPART90,
    IMG_SF4_SPART91,
    IMG_SF4_SPART92,
    IMG_SF4_SPART93,
    IMG_SF4_SPART94,
    IMG_SF4_SPART95,
    IMG_SF4_SPART96,
    IMG_SF4_SPART97,
    IMG_SF4_SPART98,
    IMG_SF4_SPART99,
    IMG_SF4_SPART9C,
    IMG_SF4_SPART9E,
    IMG_SF4_SPART9G,
    IMG_SF4_SPART9L,
    IMG_SF4_SPART9Q,
    IMG_SF4_SPART9S,
    IMG_SF4_SPARTC0,
    IMG_SF4_SPARTC1,
    IMG_SF4_SPARTC2,
    IMG_SF4_SPARTC3,
    IMG_SF4_SPARTC4,
    IMG_SF4_SPARTC5,
    IMG_SF4_SPARTC6,
    IMG_SF4_SPARTC7,
    IMG_SF4_SPARTC8,
    IMG_SF4_SPARTC9,
    IMG_SF4_SPARTCC,
    IMG_SF4_SPARTCE,
    IMG_SF4_SPARTCG,
    IMG_SF4_SPARTCL,
    IMG_SF4_SPARTCQ,
    IMG_SF4_SPARTCS,
    IMG_SF4_SPARTE0,
    IMG_SF4_SPARTE1,
    IMG_SF4_SPARTE2,
    IMG_SF4_SPARTE3,
    IMG_SF4_SPARTE4,
    IMG_SF4_SPARTE5,
    IMG_SF4_SPARTE6,
    IMG_SF4_SPARTE7,
    IMG_SF4_SPARTE8,
    IMG_SF4_SPARTE9,
    IMG_SF4_SPARTEC,
    IMG_SF4_SPARTEE,
    IMG_SF4_SPARTEG,
    IMG_SF4_SPARTEL,
    IMG_SF4_SPARTEQ,
    IMG_SF4_SPARTES,
    IMG_SF4_SPARTG0,
    IMG_SF4_SPARTG1,
    IMG_SF4_SPARTG2,
    IMG_SF4_SPARTG3,
    IMG_SF4_SPARTG4,
    IMG_SF4_SPARTG5,
    IMG_SF4_SPARTG6,
    IMG_SF4_SPARTG7,
    IMG_SF4_SPARTG8,
    IMG_SF4_SPARTG9,
    IMG_SF4_SPARTGC,
    IMG_SF4_SPARTGE,
    IMG_SF4_SPARTGG,
    IMG_SF4_SPARTGL,
    IMG_SF4_SPARTGQ,
    IMG_SF4_SPARTGS,
    IMG_SF4_SPARTL0,
    IMG_SF4_SPARTL1,
    IMG_SF4_SPARTL2,
    IMG_SF4_SPARTL3,
    IMG_SF4_SPARTL4,
    IMG_SF4_SPARTL5,
    IMG_SF4_SPARTL6,
    IMG_SF4_SPARTL7,
    IMG_SF4_SPARTL8,
    IMG_SF4_SPARTL9,
    IMG_SF4_SPARTLC,
    IMG_SF4_SPARTLE,
    IMG_SF4_SPARTLG,
    IMG_SF4_SPARTLL,
    IMG_SF4_SPARTLQ,
    IMG_SF4_SPARTLS,
    IMG_SF4_SPARTQ0,
    IMG_SF4_SPARTQ1,
    IMG_SF4_SPARTQ2,
    IMG_SF4_SPARTQ3,
    IMG_SF4_SPARTQ4,
    IMG_SF4_SPARTQ5,
    IMG_SF4_SPARTQ6,
    IMG_SF4_SPARTQ7,
    IMG_SF4_SPARTQ8,
    IMG_SF4_SPARTQ9,
    IMG_SF4_SPARTQC,
    IMG_SF4_SPARTQE,
    IMG_SF4_SPARTQG,
    IMG_SF4_SPARTQL,
    IMG_SF4_SPARTQQ,
    IMG_SF4_SPARTQS,
    IMG_SF4_SPARTS0,
    IMG_SF4_SPARTS1,
    IMG_SF4_SPARTS2,
    IMG_SF4_SPARTS3,
    IMG_SF4_SPARTS4,
    IMG_SF4_SPARTS5,
    IMG_SF4_SPARTS6,
    IMG_SF4_SPARTS7,
    IMG_SF4_SPARTS8,
    IMG_SF4_SPARTS9,
    IMG_SF4_SPARTSC,
    IMG_SF4_SPARTSE,
    IMG_SF4_SPARTSG,
    IMG_SF4_SPARTSL,
    IMG_SF4_SPARTSQ,
    IMG_SF4_SPARTSS,
    IMG_SF4_STONE0,
    IMG_SF4_STONE10,
    IMG_SF4_STONE19_0,
    IMG_SF4_STONE2_0,
    IMG_SF4_STONE29BOLD,
    IMG_SF4_STONE29B,
    IMG_SF4_STONE29OLD,
    IMG_SF4_STONE29,
    IMG_SF4_STONE2,
    IMG_SF4_STONE8OLD,
    IMG_SF4_STONE8,
    IMG_SF4_STONE9OLD,
    IMG_SF4_STONE9,
    IMG_SF5_SPART00,
    IMG_SF5_SPART01,
    IMG_SF5_SPART02,
    IMG_SF5_SPART03,
    IMG_SF5_SPART04,
    IMG_SF5_SPART05,
    IMG_SF5_SPART06,
    IMG_SF5_SPART07,
    IMG_SF5_SPART08,
    IMG_SF5_SPART09,
    IMG_SF5_SPART0C,
    IMG_SF5_SPART0E,
    IMG_SF5_SPART0G,
    IMG_SF5_SPART0L,
    IMG_SF5_SPART0Q,
    IMG_SF5_SPART0S,
    IMG_SF5_SPART10,
    IMG_SF5_SPART11,
    IMG_SF5_SPART12,
    IMG_SF5_SPART13,
    IMG_SF5_SPART14,
    IMG_SF5_SPART15,
    IMG_SF5_SPART16,
    IMG_SF5_SPART17,
    IMG_SF5_SPART18,
    IMG_SF5_SPART19,
    IMG_SF5_SPART1C,
    IMG_SF5_SPART1E,
    IMG_SF5_SPART1G,
    IMG_SF5_SPART1L,
    IMG_SF5_SPART1Q,
    IMG_SF5_SPART1S,
    IMG_SF5_SPART20,
    IMG_SF5_SPART21,
    IMG_SF5_SPART22,
    IMG_SF5_SPART23,
    IMG_SF5_SPART24,
    IMG_SF5_SPART25,
    IMG_SF5_SPART26,
    IMG_SF5_SPART27,
    IMG_SF5_SPART28,
    IMG_SF5_SPART29,
    IMG_SF5_SPART2C,
    IMG_SF5_SPART2E,
    IMG_SF5_SPART2G,
    IMG_SF5_SPART2L,
    IMG_SF5_SPART2Q,
    IMG_SF5_SPART2S,
    IMG_SF5_SPART30,
    IMG_SF5_SPART31,
    IMG_SF5_SPART32,
    IMG_SF5_SPART33,
    IMG_SF5_SPART34,
    IMG_SF5_SPART35,
    IMG_SF5_SPART36,
    IMG_SF5_SPART37,
    IMG_SF5_SPART38,
    IMG_SF5_SPART39,
    IMG_SF5_SPART3C,
    IMG_SF5_SPART3E,
    IMG_SF5_SPART3G,
    IMG_SF5_SPART3L,
    IMG_SF5_SPART3Q,
    IMG_SF5_SPART3S,
    IMG_SF5_SPART40,
    IMG_SF5_SPART41,
    IMG_SF5_SPART42,
    IMG_SF5_SPART43,
    IMG_SF5_SPART44,
    IMG_SF5_SPART45,
    IMG_SF5_SPART46,
    IMG_SF5_SPART47,
    IMG_SF5_SPART48,
    IMG_SF5_SPART49,
    IMG_SF5_SPART4C,
    IMG_SF5_SPART4E,
    IMG_SF5_SPART4G,
    IMG_SF5_SPART4L,
    IMG_SF5_SPART4Q,
    IMG_SF5_SPART4S,
    IMG_SF5_SPART50,
    IMG_SF5_SPART51,
    IMG_SF5_SPART52,
    IMG_SF5_SPART53,
    IMG_SF5_SPART54,
    IMG_SF5_SPART55,
    IMG_SF5_SPART56,
    IMG_SF5_SPART57,
    IMG_SF5_SPART58,
    IMG_SF5_SPART59,
    IMG_SF5_SPART5C,
    IMG_SF5_SPART5E,
    IMG_SF5_SPART5G,
    IMG_SF5_SPART5L,
    IMG_SF5_SPART5Q,
    IMG_SF5_SPART5S,
    IMG_SF5_SPART60,
    IMG_SF5_SPART61,
    IMG_SF5_SPART62,
    IMG_SF5_SPART63,
    IMG_SF5_SPART64,
    IMG_SF5_SPART65,
    IMG_SF5_SPART66,
    IMG_SF5_SPART67,
    IMG_SF5_SPART68,
    IMG_SF5_SPART69,
    IMG_SF5_SPART6C,
    IMG_SF5_SPART6E,
    IMG_SF5_SPART6G,
    IMG_SF5_SPART6L,
    IMG_SF5_SPART6Q,
    IMG_SF5_SPART6S,
    IMG_SF5_SPART70,
    IMG_SF5_SPART71,
    IMG_SF5_SPART72,
    IMG_SF5_SPART73,
    IMG_SF5_SPART74,
    IMG_SF5_SPART75,
    IMG_SF5_SPART76,
    IMG_SF5_SPART77,
    IMG_SF5_SPART78,
    IMG_SF5_SPART79,
    IMG_SF5_SPART7C,
    IMG_SF5_SPART7E,
    IMG_SF5_SPART7G,
    IMG_SF5_SPART7L,
    IMG_SF5_SPART7Q,
    IMG_SF5_SPART7S,
    IMG_SF5_SPART80,
    IMG_SF5_SPART81,
    IMG_SF5_SPART82,
    IMG_SF5_SPART83,
    IMG_SF5_SPART84,
    IMG_SF5_SPART85,
    IMG_SF5_SPART86,
    IMG_SF5_SPART87,
    IMG_SF5_SPART88,
    IMG_SF5_SPART89,
    IMG_SF5_SPART8C,
    IMG_SF5_SPART8E,
    IMG_SF5_SPART8G,
    IMG_SF5_SPART8L,
    IMG_SF5_SPART8Q,
    IMG_SF5_SPART8S,
    IMG_SF5_SPART90,
    IMG_SF5_SPART91,
    IMG_SF5_SPART92,
    IMG_SF5_SPART93,
    IMG_SF5_SPART94,
    IMG_SF5_SPART95,
    IMG_SF5_SPART96,
    IMG_SF5_SPART97,
    IMG_SF5_SPART98,
    IMG_SF5_SPART99,
    IMG_SF5_SPART9C,
    IMG_SF5_SPART9E,
    IMG_SF5_SPART9G,
    IMG_SF5_SPART9L,
    IMG_SF5_SPART9Q,
    IMG_SF5_SPART9S,
    IMG_SF5_SPARTC0,
    IMG_SF5_SPARTC1,
    IMG_SF5_SPARTC2,
    IMG_SF5_SPARTC3,
    IMG_SF5_SPARTC4,
    IMG_SF5_SPARTC5,
    IMG_SF5_SPARTC6,
    IMG_SF5_SPARTC7,
    IMG_SF5_SPARTC8,
    IMG_SF5_SPARTC9,
    IMG_SF5_SPARTCC,
    IMG_SF5_SPARTCE,
    IMG_SF5_SPARTCG,
    IMG_SF5_SPARTCL,
    IMG_SF5_SPARTCQ,
    IMG_SF5_SPARTCS,
    IMG_SF5_SPARTE0,
    IMG_SF5_SPARTE1,
    IMG_SF5_SPARTE2,
    IMG_SF5_SPARTE3,
    IMG_SF5_SPARTE4,
    IMG_SF5_SPARTE5,
    IMG_SF5_SPARTE6,
    IMG_SF5_SPARTE7,
    IMG_SF5_SPARTE8,
    IMG_SF5_SPARTE9,
    IMG_SF5_SPARTEC,
    IMG_SF5_SPARTEE,
    IMG_SF5_SPARTEG,
    IMG_SF5_SPARTEL,
    IMG_SF5_SPARTEQ,
    IMG_SF5_SPARTES,
    IMG_SF5_SPARTG0,
    IMG_SF5_SPARTG1,
    IMG_SF5_SPARTG2,
    IMG_SF5_SPARTG3,
    IMG_SF5_SPARTG4,
    IMG_SF5_SPARTG5,
    IMG_SF5_SPARTG6,
    IMG_SF5_SPARTG7,
    IMG_SF5_SPARTG8,
    IMG_SF5_SPARTG9,
    IMG_SF5_SPARTGC,
    IMG_SF5_SPARTGE,
    IMG_SF5_SPARTGG,
    IMG_SF5_SPARTGL,
    IMG_SF5_SPARTGQ,
    IMG_SF5_SPARTGS,
    IMG_SF5_SPARTL0,
    IMG_SF5_SPARTL1,
    IMG_SF5_SPARTL2,
    IMG_SF5_SPARTL3,
    IMG_SF5_SPARTL4,
    IMG_SF5_SPARTL5,
    IMG_SF5_SPARTL6,
    IMG_SF5_SPARTL7,
    IMG_SF5_SPARTL8,
    IMG_SF5_SPARTL9,
    IMG_SF5_SPARTLC,
    IMG_SF5_SPARTLE,
    IMG_SF5_SPARTLG,
    IMG_SF5_SPARTLL,
    IMG_SF5_SPARTLQ,
    IMG_SF5_SPARTLS,
    IMG_SF5_SPARTQ0,
    IMG_SF5_SPARTQ1,
    IMG_SF5_SPARTQ2,
    IMG_SF5_SPARTQ3,
    IMG_SF5_SPARTQ4,
    IMG_SF5_SPARTQ5,
    IMG_SF5_SPARTQ6,
    IMG_SF5_SPARTQ7,
    IMG_SF5_SPARTQ8,
    IMG_SF5_SPARTQ9,
    IMG_SF5_SPARTQC,
    IMG_SF5_SPARTQE,
    IMG_SF5_SPARTQG,
    IMG_SF5_SPARTQL,
    IMG_SF5_SPARTQQ,
    IMG_SF5_SPARTQS,
    IMG_SF5_SPARTS0,
    IMG_SF5_SPARTS1,
    IMG_SF5_SPARTS2,
    IMG_SF5_SPARTS3,
    IMG_SF5_SPARTS4,
    IMG_SF5_SPARTS5,
    IMG_SF5_SPARTS6,
    IMG_SF5_SPARTS7,
    IMG_SF5_SPARTS8,
    IMG_SF5_SPARTS9,
    IMG_SF5_SPARTSC,
    IMG_SF5_SPARTSE,
    IMG_SF5_SPARTSG,
    IMG_SF5_SPARTSL,
    IMG_SF5_SPARTSQ,
    IMG_SF5_SPARTSS,
    IMG_SF5_STONE0,
    IMG_SF5_STONE10,
    IMG_SF5_STONE19_0,
    IMG_SF5_STONE2_0,
    IMG_SF5_STONE29B,
    IMG_SF5_STONE29,
    IMG_SF5_STONE2,
    IMG_SF5_STONE8,
    IMG_SF5_STONE9,
    IMG_SF6_SPART00,
    IMG_SF6_SPART01,
    IMG_SF6_SPART02,
    IMG_SF6_SPART03,
    IMG_SF6_SPART04,
    IMG_SF6_SPART05,
    IMG_SF6_SPART06,
    IMG_SF6_SPART07,
    IMG_SF6_SPART08,
    IMG_SF6_SPART09,
    IMG_SF6_SPART0C,
    IMG_SF6_SPART0E,
    IMG_SF6_SPART0G,
    IMG_SF6_SPART0L,
    IMG_SF6_SPART0Q,
    IMG_SF6_SPART0S,
    IMG_SF6_SPART10,
    IMG_SF6_SPART11,
    IMG_SF6_SPART12,
    IMG_SF6_SPART13,
    IMG_SF6_SPART14,
    IMG_SF6_SPART15,
    IMG_SF6_SPART16,
    IMG_SF6_SPART17,
    IMG_SF6_SPART18,
    IMG_SF6_SPART19,
    IMG_SF6_SPART1C,
    IMG_SF6_SPART1E,
    IMG_SF6_SPART1G,
    IMG_SF6_SPART1L,
    IMG_SF6_SPART1Q,
    IMG_SF6_SPART1S,
    IMG_SF6_SPART20,
    IMG_SF6_SPART21,
    IMG_SF6_SPART22,
    IMG_SF6_SPART23,
    IMG_SF6_SPART24,
    IMG_SF6_SPART25,
    IMG_SF6_SPART26,
    IMG_SF6_SPART27,
    IMG_SF6_SPART28,
    IMG_SF6_SPART29,
    IMG_SF6_SPART2C,
    IMG_SF6_SPART2E,
    IMG_SF6_SPART2G,
    IMG_SF6_SPART2L,
    IMG_SF6_SPART2Q,
    IMG_SF6_SPART2S,
    IMG_SF6_SPART30,
    IMG_SF6_SPART31,
    IMG_SF6_SPART32,
    IMG_SF6_SPART33,
    IMG_SF6_SPART34,
    IMG_SF6_SPART35,
    IMG_SF6_SPART36,
    IMG_SF6_SPART37,
    IMG_SF6_SPART38,
    IMG_SF6_SPART39,
    IMG_SF6_SPART3C,
    IMG_SF6_SPART3E,
    IMG_SF6_SPART3G,
    IMG_SF6_SPART3L,
    IMG_SF6_SPART3Q,
    IMG_SF6_SPART3S,
    IMG_SF6_SPART40,
    IMG_SF6_SPART41,
    IMG_SF6_SPART42,
    IMG_SF6_SPART43,
    IMG_SF6_SPART44,
    IMG_SF6_SPART45,
    IMG_SF6_SPART46,
    IMG_SF6_SPART47,
    IMG_SF6_SPART48,
    IMG_SF6_SPART49,
    IMG_SF6_SPART4C,
    IMG_SF6_SPART4E,
    IMG_SF6_SPART4G,
    IMG_SF6_SPART4L,
    IMG_SF6_SPART4Q,
    IMG_SF6_SPART4S,
    IMG_SF6_SPART50,
    IMG_SF6_SPART51,
    IMG_SF6_SPART52,
    IMG_SF6_SPART53,
    IMG_SF6_SPART54,
    IMG_SF6_SPART55,
    IMG_SF6_SPART56,
    IMG_SF6_SPART57,
    IMG_SF6_SPART58,
    IMG_SF6_SPART59,
    IMG_SF6_SPART5C,
    IMG_SF6_SPART5E,
    IMG_SF6_SPART5G,
    IMG_SF6_SPART5L,
    IMG_SF6_SPART5Q,
    IMG_SF6_SPART5S,
    IMG_SF6_SPART60,
    IMG_SF6_SPART61,
    IMG_SF6_SPART62,
    IMG_SF6_SPART63,
    IMG_SF6_SPART64,
    IMG_SF6_SPART65,
    IMG_SF6_SPART66,
    IMG_SF6_SPART67,
    IMG_SF6_SPART68,
    IMG_SF6_SPART69,
    IMG_SF6_SPART6C,
    IMG_SF6_SPART6E,
    IMG_SF6_SPART6G,
    IMG_SF6_SPART6L,
    IMG_SF6_SPART6Q,
    IMG_SF6_SPART6S,
    IMG_SF6_SPART70,
    IMG_SF6_SPART71,
    IMG_SF6_SPART72,
    IMG_SF6_SPART73,
    IMG_SF6_SPART74,
    IMG_SF6_SPART75,
    IMG_SF6_SPART76,
    IMG_SF6_SPART77,
    IMG_SF6_SPART78,
    IMG_SF6_SPART79,
    IMG_SF6_SPART7C,
    IMG_SF6_SPART7E,
    IMG_SF6_SPART7G,
    IMG_SF6_SPART7L,
    IMG_SF6_SPART7Q,
    IMG_SF6_SPART7S,
    IMG_SF6_SPART80,
    IMG_SF6_SPART81,
    IMG_SF6_SPART82,
    IMG_SF6_SPART83,
    IMG_SF6_SPART84,
    IMG_SF6_SPART85,
    IMG_SF6_SPART86,
    IMG_SF6_SPART87,
    IMG_SF6_SPART88,
    IMG_SF6_SPART89,
    IMG_SF6_SPART8C,
    IMG_SF6_SPART8E,
    IMG_SF6_SPART8G,
    IMG_SF6_SPART8L,
    IMG_SF6_SPART8Q,
    IMG_SF6_SPART8S,
    IMG_SF6_SPART90,
    IMG_SF6_SPART91,
    IMG_SF6_SPART92,
    IMG_SF6_SPART93,
    IMG_SF6_SPART94,
    IMG_SF6_SPART95,
    IMG_SF6_SPART96,
    IMG_SF6_SPART97,
    IMG_SF6_SPART98,
    IMG_SF6_SPART99,
    IMG_SF6_SPART9C,
    IMG_SF6_SPART9E,
    IMG_SF6_SPART9G,
    IMG_SF6_SPART9L,
    IMG_SF6_SPART9Q,
    IMG_SF6_SPART9S,
    IMG_SF6_SPARTC0,
    IMG_SF6_SPARTC1,
    IMG_SF6_SPARTC2,
    IMG_SF6_SPARTC3,
    IMG_SF6_SPARTC4,
    IMG_SF6_SPARTC5,
    IMG_SF6_SPARTC6,
    IMG_SF6_SPARTC7,
    IMG_SF6_SPARTC8,
    IMG_SF6_SPARTC9,
    IMG_SF6_SPARTCC,
    IMG_SF6_SPARTCE,
    IMG_SF6_SPARTCG,
    IMG_SF6_SPARTCL,
    IMG_SF6_SPARTCQ,
    IMG_SF6_SPARTCS,
    IMG_SF6_SPARTE0,
    IMG_SF6_SPARTE1,
    IMG_SF6_SPARTE2,
    IMG_SF6_SPARTE3,
    IMG_SF6_SPARTE4,
    IMG_SF6_SPARTE5,
    IMG_SF6_SPARTE6,
    IMG_SF6_SPARTE7,
    IMG_SF6_SPARTE8,
    IMG_SF6_SPARTE9,
    IMG_SF6_SPARTEC,
    IMG_SF6_SPARTEE,
    IMG_SF6_SPARTEG,
    IMG_SF6_SPARTEL,
    IMG_SF6_SPARTEQ,
    IMG_SF6_SPARTES,
    IMG_SF6_SPARTG0,
    IMG_SF6_SPARTG1,
    IMG_SF6_SPARTG2,
    IMG_SF6_SPARTG3,
    IMG_SF6_SPARTG4,
    IMG_SF6_SPARTG5,
    IMG_SF6_SPARTG6,
    IMG_SF6_SPARTG7,
    IMG_SF6_SPARTG8,
    IMG_SF6_SPARTG9,
    IMG_SF6_SPARTGC,
    IMG_SF6_SPARTGE,
    IMG_SF6_SPARTGG,
    IMG_SF6_SPARTGL,
    IMG_SF6_SPARTGQ,
    IMG_SF6_SPARTGS,
    IMG_SF6_SPARTL0,
    IMG_SF6_SPARTL1,
    IMG_SF6_SPARTL2,
    IMG_SF6_SPARTL3,
    IMG_SF6_SPARTL4,
    IMG_SF6_SPARTL5,
    IMG_SF6_SPARTL6,
    IMG_SF6_SPARTL7,
    IMG_SF6_SPARTL8,
    IMG_SF6_SPARTL9,
    IMG_SF6_SPARTLC,
    IMG_SF6_SPARTLE,
    IMG_SF6_SPARTLG,
    IMG_SF6_SPARTLL,
    IMG_SF6_SPARTLQ,
    IMG_SF6_SPARTLS,
    IMG_SF6_SPARTQ0,
    IMG_SF6_SPARTQ1,
    IMG_SF6_SPARTQ2,
    IMG_SF6_SPARTQ3,
    IMG_SF6_SPARTQ4,
    IMG_SF6_SPARTQ5,
    IMG_SF6_SPARTQ6,
    IMG_SF6_SPARTQ7,
    IMG_SF6_SPARTQ8,
    IMG_SF6_SPARTQ9,
    IMG_SF6_SPARTQC,
    IMG_SF6_SPARTQE,
    IMG_SF6_SPARTQG,
    IMG_SF6_SPARTQL,
    IMG_SF6_SPARTQQ,
    IMG_SF6_SPARTQS,
    IMG_SF6_SPARTS0,
    IMG_SF6_SPARTS1,
    IMG_SF6_SPARTS2,
    IMG_SF6_SPARTS3,
    IMG_SF6_SPARTS4,
    IMG_SF6_SPARTS5,
    IMG_SF6_SPARTS6,
    IMG_SF6_SPARTS7,
    IMG_SF6_SPARTS8,
    IMG_SF6_SPARTS9,
    IMG_SF6_SPARTSC,
    IMG_SF6_SPARTSE,
    IMG_SF6_SPARTSG,
    IMG_SF6_SPARTSL,
    IMG_SF6_SPARTSQ,
    IMG_SF6_SPARTSS,
    IMG_SF6_STONE0,
    IMG_SF6_STONE10,
    IMG_SF6_STONE19_0,
    IMG_SF6_STONE2_0,
    IMG_SF6_STONE24,
    IMG_SF6_STONE25,
    IMG_SF6_STONE29B,
    IMG_SF6_STONE29,
    IMG_SF6_STONE2,
    IMG_SF6_STONE8,
    IMG_SF6_STONE9,
    IMG_SF8_SPART00,
    IMG_SF8_SPART01,
    IMG_SF8_SPART02,
    IMG_SF8_SPART03,
    IMG_SF8_SPART04,
    IMG_SF8_SPART05,
    IMG_SF8_SPART06,
    IMG_SF8_SPART07,
    IMG_SF8_SPART08,
    IMG_SF8_SPART09,
    IMG_SF8_SPART0C,
    IMG_SF8_SPART0E,
    IMG_SF8_SPART0G,
    IMG_SF8_SPART0L,
    IMG_SF8_SPART0Q,
    IMG_SF8_SPART0S,
    IMG_SF8_SPART10,
    IMG_SF8_SPART11,
    IMG_SF8_SPART12,
    IMG_SF8_SPART13,
    IMG_SF8_SPART14,
    IMG_SF8_SPART15,
    IMG_SF8_SPART16,
    IMG_SF8_SPART17,
    IMG_SF8_SPART18,
    IMG_SF8_SPART19,
    IMG_SF8_SPART1C,
    IMG_SF8_SPART1E,
    IMG_SF8_SPART1G,
    IMG_SF8_SPART1L,
    IMG_SF8_SPART1Q,
    IMG_SF8_SPART1S,
    IMG_SF8_SPART20,
    IMG_SF8_SPART21,
    IMG_SF8_SPART22,
    IMG_SF8_SPART23,
    IMG_SF8_SPART24,
    IMG_SF8_SPART25,
    IMG_SF8_SPART26,
    IMG_SF8_SPART27,
    IMG_SF8_SPART28,
    IMG_SF8_SPART29,
    IMG_SF8_SPART2C,
    IMG_SF8_SPART2E,
    IMG_SF8_SPART2G,
    IMG_SF8_SPART2L,
    IMG_SF8_SPART2Q,
    IMG_SF8_SPART2S,
    IMG_SF8_SPART30,
    IMG_SF8_SPART31,
    IMG_SF8_SPART32,
    IMG_SF8_SPART33,
    IMG_SF8_SPART34,
    IMG_SF8_SPART35,
    IMG_SF8_SPART36,
    IMG_SF8_SPART37,
    IMG_SF8_SPART38,
    IMG_SF8_SPART39,
    IMG_SF8_SPART3C,
    IMG_SF8_SPART3E,
    IMG_SF8_SPART3G,
    IMG_SF8_SPART3L,
    IMG_SF8_SPART3Q,
    IMG_SF8_SPART3S,
    IMG_SF8_SPART40,
    IMG_SF8_SPART41,
    IMG_SF8_SPART42,
    IMG_SF8_SPART43,
    IMG_SF8_SPART44,
    IMG_SF8_SPART45,
    IMG_SF8_SPART46,
    IMG_SF8_SPART47,
    IMG_SF8_SPART48,
    IMG_SF8_SPART49,
    IMG_SF8_SPART4C,
    IMG_SF8_SPART4E,
    IMG_SF8_SPART4G,
    IMG_SF8_SPART4L,
    IMG_SF8_SPART4Q,
    IMG_SF8_SPART4S,
    IMG_SF8_SPART50,
    IMG_SF8_SPART51,
    IMG_SF8_SPART52,
    IMG_SF8_SPART53,
    IMG_SF8_SPART54,
    IMG_SF8_SPART55,
    IMG_SF8_SPART56,
    IMG_SF8_SPART57,
    IMG_SF8_SPART58,
    IMG_SF8_SPART59,
    IMG_SF8_SPART5C,
    IMG_SF8_SPART5E,
    IMG_SF8_SPART5G,
    IMG_SF8_SPART5L,
    IMG_SF8_SPART5Q,
    IMG_SF8_SPART5S,
    IMG_SF8_SPART60,
    IMG_SF8_SPART61,
    IMG_SF8_SPART62,
    IMG_SF8_SPART63,
    IMG_SF8_SPART64,
    IMG_SF8_SPART65,
    IMG_SF8_SPART66,
    IMG_SF8_SPART67,
    IMG_SF8_SPART68,
    IMG_SF8_SPART69,
    IMG_SF8_SPART6C,
    IMG_SF8_SPART6E,
    IMG_SF8_SPART6G,
    IMG_SF8_SPART6L,
    IMG_SF8_SPART6Q,
    IMG_SF8_SPART6S,
    IMG_SF8_SPART70,
    IMG_SF8_SPART71,
    IMG_SF8_SPART72,
    IMG_SF8_SPART73,
    IMG_SF8_SPART74,
    IMG_SF8_SPART75,
    IMG_SF8_SPART76,
    IMG_SF8_SPART77,
    IMG_SF8_SPART78,
    IMG_SF8_SPART79,
    IMG_SF8_SPART7C,
    IMG_SF8_SPART7E,
    IMG_SF8_SPART7G,
    IMG_SF8_SPART7L,
    IMG_SF8_SPART7Q,
    IMG_SF8_SPART7S,
    IMG_SF8_SPART80,
    IMG_SF8_SPART81,
    IMG_SF8_SPART82,
    IMG_SF8_SPART83,
    IMG_SF8_SPART84,
    IMG_SF8_SPART85,
    IMG_SF8_SPART86,
    IMG_SF8_SPART87,
    IMG_SF8_SPART88,
    IMG_SF8_SPART89,
    IMG_SF8_SPART8C,
    IMG_SF8_SPART8E,
    IMG_SF8_SPART8G,
    IMG_SF8_SPART8L,
    IMG_SF8_SPART8Q,
    IMG_SF8_SPART8S,
    IMG_SF8_SPART90,
    IMG_SF8_SPART91,
    IMG_SF8_SPART92,
    IMG_SF8_SPART93,
    IMG_SF8_SPART94,
    IMG_SF8_SPART95,
    IMG_SF8_SPART96,
    IMG_SF8_SPART97,
    IMG_SF8_SPART98,
    IMG_SF8_SPART99,
    IMG_SF8_SPART9C,
    IMG_SF8_SPART9E,
    IMG_SF8_SPART9G,
    IMG_SF8_SPART9L,
    IMG_SF8_SPART9Q,
    IMG_SF8_SPART9S,
    IMG_SF8_SPARTC0,
    IMG_SF8_SPARTC1,
    IMG_SF8_SPARTC2,
    IMG_SF8_SPARTC3,
    IMG_SF8_SPARTC4,
    IMG_SF8_SPARTC5,
    IMG_SF8_SPARTC6,
    IMG_SF8_SPARTC7,
    IMG_SF8_SPARTC8,
    IMG_SF8_SPARTC9,
    IMG_SF8_SPARTCC,
    IMG_SF8_SPARTCE,
    IMG_SF8_SPARTCG,
    IMG_SF8_SPARTCL,
    IMG_SF8_SPARTCQ,
    IMG_SF8_SPARTCS,
    IMG_SF8_SPARTE0,
    IMG_SF8_SPARTE1,
    IMG_SF8_SPARTE2,
    IMG_SF8_SPARTE3,
    IMG_SF8_SPARTE4,
    IMG_SF8_SPARTE5,
    IMG_SF8_SPARTE6,
    IMG_SF8_SPARTE7,
    IMG_SF8_SPARTE8,
    IMG_SF8_SPARTE9,
    IMG_SF8_SPARTEC,
    IMG_SF8_SPARTEE,
    IMG_SF8_SPARTEG,
    IMG_SF8_SPARTEL,
    IMG_SF8_SPARTEQ,
    IMG_SF8_SPARTES,
    IMG_SF8_SPARTG0,
    IMG_SF8_SPARTG1,
    IMG_SF8_SPARTG2,
    IMG_SF8_SPARTG3,
    IMG_SF8_SPARTG4,
    IMG_SF8_SPARTG5,
    IMG_SF8_SPARTG6,
    IMG_SF8_SPARTG7,
    IMG_SF8_SPARTG8,
    IMG_SF8_SPARTG9,
    IMG_SF8_SPARTGC,
    IMG_SF8_SPARTGE,
    IMG_SF8_SPARTGG,
    IMG_SF8_SPARTGL,
    IMG_SF8_SPARTGQ,
    IMG_SF8_SPARTGS,
    IMG_SF8_SPARTL0,
    IMG_SF8_SPARTL1,
    IMG_SF8_SPARTL2,
    IMG_SF8_SPARTL3,
    IMG_SF8_SPARTL4,
    IMG_SF8_SPARTL5,
    IMG_SF8_SPARTL6,
    IMG_SF8_SPARTL7,
    IMG_SF8_SPARTL8,
    IMG_SF8_SPARTL9,
    IMG_SF8_SPARTLC,
    IMG_SF8_SPARTLE,
    IMG_SF8_SPARTLG,
    IMG_SF8_SPARTLL,
    IMG_SF8_SPARTLQ,
    IMG_SF8_SPARTLS,
    IMG_SF8_SPARTQ0,
    IMG_SF8_SPARTQ1,
    IMG_SF8_SPARTQ2,
    IMG_SF8_SPARTQ3,
    IMG_SF8_SPARTQ4,
    IMG_SF8_SPARTQ5,
    IMG_SF8_SPARTQ6,
    IMG_SF8_SPARTQ7,
    IMG_SF8_SPARTQ8,
    IMG_SF8_SPARTQ9,
    IMG_SF8_SPARTQC,
    IMG_SF8_SPARTQE,
    IMG_SF8_SPARTQG,
    IMG_SF8_SPARTQL,
    IMG_SF8_SPARTQQ,
    IMG_SF8_SPARTQS,
    IMG_SF8_SPARTS0,
    IMG_SF8_SPARTS1,
    IMG_SF8_SPARTS2,
    IMG_SF8_SPARTS3,
    IMG_SF8_SPARTS4,
    IMG_SF8_SPARTS5,
    IMG_SF8_SPARTS6,
    IMG_SF8_SPARTS7,
    IMG_SF8_SPARTS8,
    IMG_SF8_SPARTS9,
    IMG_SF8_SPARTSC,
    IMG_SF8_SPARTSE,
    IMG_SF8_SPARTSG,
    IMG_SF8_SPARTSL,
    IMG_SF8_SPARTSQ,
    IMG_SF8_SPARTSS,
    IMG_SF8_STONE0,
    IMG_SF8_STONE10,
    IMG_SF8_STONE19_0,
    IMG_SF8_STONE2_0,
    IMG_SF8_STONE29B,
    IMG_SF8_STONE29,
    IMG_SF8_STONE2,
    IMG_SF8_STONE8,
    IMG_SF8_STONE9,
    IMG_SG1_GUILD,
    IMG_SG1_LGHT1,
    IMG_SG1_LGHT2,
    IMG_SG1_LGHT3,
    IMG_SG1_LGHT4,
    IMG_SG1_TRANS1,
    IMG_SG1_TRANS2,
    IMG_SG1_TRANS3,
    IMG_SG1_UFO1,
    IMG_SG1_UFO2,
    IMG_SG2_EYES2,
    IMG_SG2_EYES,
    IMG_SG2_INS1,
    IMG_SG2_INS2,
    IMG_SG2_INS3,
    IMG_SG3_DUST,
    IMG_SG3_LAMP0,
    IMG_SG3_LAMP1,
    IMG_SG3_MIRR0,
    IMG_SG3_MIRR1,
    IMG_SG3_MIRR2,
    IMG_SG3_MIRR3,
    IMG_SG3_SCREEN1,
    IMG_SG3_SCREEN2,
    IMG_SG3_SCREEN3,
    IMG_SG3_SCREEN4,
    IMG_SG3_TALK1,
    IMG_SG3_TALK2,
    IMG_SG3_TALK3,
    IMG_SG3_TALK4,
    IMG_SG3_TALK5,
    IMG_SG3_TALK6,
    IMG_SP1_DMG1,
    IMG_SP1_DMG2,
    IMG_SP1_DMG3,
    IMG_SP1_DMG4,
    IMG_SP1_DMG5,
    IMG_SP1_DMG6,
    IMG_SP1_DMG7,
    IMG_SP1_DMG8,
    IMG_SP1_SHIP1,
    IMG_SP1_SHP1,
    IMG_SP1_SHP2,
    IMG_SP1_SHP3,
    IMG_SP1_SHP4,
    IMG_SP1_SHP5,
    IMG_SU1_ALLOW0,
    IMG_SU1_ALLOW1,
    IMG_SU1_B_0,
    IMG_SU1_B_1,
    IMG_SU1_BUILD,
    IMG_SU1_CTA,
    IMG_SU1_CTB,
    IMG_SU1_CTC,
    IMG_SU1_CTRL1,
    IMG_SU1_CTRL2,
    IMG_SU1_CTRL3,
    IMG_SU1_DEAD10,
    IMG_SU1_DEAD1,
    IMG_SU1_DEAD2,
    IMG_SU1_DEAD3,
    IMG_SU1_DEAD4,
    IMG_SU1_DEAD5,
    IMG_SU1_DEAD6,
    IMG_SU1_DEAD7,
    IMG_SU1_DEAD8,
    IMG_SU1_DEAD9,
    IMG_SU1_EXPORT,
    IMG_SU1_MARK,
    IMG_SU1_MENU,
    IMG_SU1_PARAGRAPH,
    IMG_SU1_PEOPLE,
    IMG_SU1_STONE1_0,
    IMG_SU1_STONE10,
    IMG_SU1_STONE11_0,
    IMG_SU1_STONE11_1,
    IMG_SU1_STONE11_2,
    IMG_SU1_STONE11_3,
    IMG_SU1_STONE1_1,
    IMG_SU1_STONE11,
    IMG_SU1_STONE12_0,
    IMG_SU1_STONE12_1,
    IMG_SU1_STONE12_2,
    IMG_SU1_STONE12_3,
    IMG_SU1_STONE1_2,
    IMG_SU1_STONE12,
    IMG_SU1_STONE13_0,
    IMG_SU1_STONE13_1,
    IMG_SU1_STONE13_2,
    IMG_SU1_STONE13_3,
    IMG_SU1_STONE1_3,
    IMG_SU1_STONE13,
    IMG_SU1_STONE14_0,
    IMG_SU1_STONE14_1,
    IMG_SU1_STONE14_2,
    IMG_SU1_STONE14,
    IMG_SU1_STONE15_0,
    IMG_SU1_STONE15_1,
    IMG_SU1_STONE15_2,
    IMG_SU1_STONE15,
    IMG_SU1_STONE16_0,
    IMG_SU1_STONE16_1,
    IMG_SU1_STONE16B,
    IMG_SU1_STONE16,
    IMG_SU1_STONE17_0,
    IMG_SU1_STONE17_1,
    IMG_SU1_STONE17,
    IMG_SU1_STONE18,
    IMG_SU1_STONE19_0,
    IMG_SU1_STONE1,
    IMG_SU1_STONE2_0,
    IMG_SU1_STONE20,
    IMG_SU1_STONE22,
    IMG_SU1_STONE23_0,
    IMG_SU1_STONE23_1,
    IMG_SU1_STONE23_2,
    IMG_SU1_STONE23_3,
    IMG_SU1_STONE23,
    IMG_SU1_STONE2,
    IMG_SU1_STONE3_0,
    IMG_SU1_STONE3_1,
    IMG_SU1_STONE3_2,
    IMG_SU1_STONE3,
    IMG_SU1_STONE4_0,
    IMG_SU1_STONE4_1,
    IMG_SU1_STONE4_2,
    IMG_SU1_STONE4_3,
    IMG_SU1_STONE4B,
    IMG_SU1_STONE4,
    IMG_SU1_STONE5_0,
    IMG_SU1_STONE5_1,
    IMG_SU1_STONE5_2,
    IMG_SU1_STONE5_3,
    IMG_SU1_STONE5_4,
    IMG_SU1_STONE5_5,
    IMG_SU1_STONE5A10,
    IMG_SU1_STONE5A11,
    IMG_SU1_STONE5A12,
    IMG_SU1_STONE5A13,
    IMG_SU1_STONE5A14,
    IMG_SU1_STONE5A15,
    IMG_SU1_STONE5A16,
    IMG_SU1_STONE5A17,
    IMG_SU1_STONE5A18,
    IMG_SU1_STONE5A1,
    IMG_SU1_STONE5A2,
    IMG_SU1_STONE5A3,
    IMG_SU1_STONE5A4,
    IMG_SU1_STONE5A5,
    IMG_SU1_STONE5A6,
    IMG_SU1_STONE5A7,
    IMG_SU1_STONE5A8,
    IMG_SU1_STONE5A9,
    IMG_SU1_STONE5,
    IMG_SU1_STONE6,
    IMG_SU1_STONE7,
    IMG_SU1_STONEXX,
    IMG_SU1_TRI,
    IMG_SU2_STONE7,
    IMG_TD1_TRADE1,
    IMG_TD1_TRADE2,
    IMG_TD1_TRADE3,
    IMG_TD1_TRADE4,
    IMG_TD1_TRADE5,
    IMG_TD1_TRADE6,
    IMG_TD1_TRADE7,
    IMG_TD1_TRADE8,
    IMG_TD1_TRADEBACK,
    IMG_TD2_TR0,
    IMG_TD2_TR1,
    IMG_TD2_TR2,
    IMG_TD2_TR3,
    IMG_TD2_TR4,
    IMG_TD2_TR5,
    IMG_TD2_TR6,
    IMG_TD2_TR7,
    IMG_TD2_TR8,
    IMG_TS1_ALL1,
    IMG_TS1_ALL2,
    IMG_TS1_ALL3,
    IMG_TS1_CLOGO1,
    IMG_TS1_CLOGO2,
    IMG_TS1_CLOGO3,
    IMG_TS1_CLOGO4,
    IMG_TS1_CLOGO5,
    IMG_TS1_CLOGO6,
    IMG_TS1_CLOGO7,
    IMG_TS1_CLOGO8,
    IMG_TS1_CLOGO9,
    IMG_TS1_DEST,
    IMG_TS1_FLAG0,
    IMG_TS1_FLAG10,
    IMG_TS1_FLAG11,
    IMG_TS1_FLAG12,
    IMG_TS1_FLAG13,
    IMG_TS1_FLAG14,
    IMG_TS1_FLAG15,
    IMG_TS1_FLAG16,
    IMG_TS1_FLAG1,
    IMG_TS1_FLAG2,
    IMG_TS1_FLAG3,
    IMG_TS1_FLAG4,
    IMG_TS1_FLAG5,
    IMG_TS1_FLAG6,
    IMG_TS1_FLAG7,
    IMG_TS1_FLAG8,
    IMG_TS1_FLAG9,
    IMG_TS1_HEADER1,
    IMG_TS1_HEADER2,
    IMG_TS1_HEADER,
    IMG_TS1_ICON1,
    IMG_TS1_ICON2,
    IMG_TS1_ICON3,
    IMG_TS1_ICON4,
    IMG_TS1_ICON5,
    IMG_TS1_ID1,
    IMG_TS1_ID2,
    IMG_TS1_M11,
    IMG_TS1_M12,
    IMG_TS1_M13,
    IMG_TS1_M14,
    IMG_TS1_M2_1,
    IMG_TS1_M21,
    IMG_TS1_M2_2,
    IMG_TS1_M22,
    IMG_TS1_M23,
    IMG_TS1_M24,
    IMG_TS1_M25,
    IMG_TS1_M26,
    IMG_TS1_M27,
    IMG_TS1_M28,
    IMG_TS1_MK1,
    IMG_TS1_MK2,
    IMG_TS1_MK3,
    IMG_TS1_QMARK,
    IMG_TS1_SHICON,
    IMG_TS1_SML_MENU,
    IMG_TS1_SPHERE1,
    IMG_TS1_SPHERE2,
    IMG_TS1_SPHERE3,
    IMG_TS1_SPHERE4,
    IMG_TS1_SPHERE5,
    IMG_TS1_SPHERE6,
    IMG_TS1_SPHERE8,
    IMG_TS1_ST1,
    IMG_TS1_ST2,
    IMG_TS1_ST3,
    IMG_TS1_ST4,
    IMG_TS1_ST5,
    IMG_TS1_ST6,
    IMG_TS1_ST8,
    IMG_TS1_SUN1,
    IMG_TS1_SUN2,
    IMG_TS1_SUN3,
    IMG_TS1_SUN4,
    IMG_TS1_SUN5,
    IMG_TS1_SUN6,
    IMG_TS1_SUN7,
    IMG_TS1_SUN8,
    IMG_TS1_SUN9,
    IMG_TS1_WORM,
    IMG_TS2_ALL1,
    IMG_TS2_ALL2,
    IMG_TS2_ALL3,
    IMG_TS2_CLOGO1,
    IMG_TS2_CLOGO2,
    IMG_TS2_CLOGO3,
    IMG_TS2_CLOGO4,
    IMG_TS2_CLOGO5,
    IMG_TS2_CLOGO6,
    IMG_TS2_CLOGO7,
    IMG_TS2_CLOGO8,
    IMG_TS2_CLOGO9,
    IMG_TS2_M2_1,
    IMG_TS2_M2_2,
    IMG_TS2_PATTERN,
    IMG_TS2_SHICON,
    IMG_TS2_SML_MENU,
    IMG_TS2_SPHERE1,
    IMG_TS2_SPHERE2,
    IMG_TS2_SPHERE3,
    IMG_TS2_SPHERE4,
    IMG_TS2_SPHERE5,
    IMG_TS2_SPHERE6,
    IMG_TS2_SPHERE8,
    IMG_VS0_VPIC,
    IMG_VS1_VESSEL,
    IMG_VS2_VESSEL,
    IMG_VS3_VESSEL,
    IMG_VS4_VESSEL,
    IMG_VS5_VESSEL,
    IMG_VS6_VESSEL,
    IMG_WD1_DONE,
    IMG_WD1_WELL,

    "",
};
