#ifndef GUARD_TRADE_BUY_H
#define GUARD_TRADE_BUY_H

#define TRADEBUY_OPTIONS 4

#include "draw/draw.h"

enum TradeBuyGoodsType {
    TBGT_Food,
    TBGT_Inf,
    TBGT_Gli,
    TBGT_Art,
};

struct TradeBuyGoods {
    TradeBuyGoodsType type;
    int avail;
    int cost;
    int buy;
    SprID id_offer;
    SprID id_buy;
    SprID id_adj;
};

class TradeBuy {
    public:
        TradeBuy();
        bool tradebuy_start(int, int, int, int);
        bool tradebuy_update();
    private:
        SprID id_tradebuy_header_flag;
        SprID id_tradebuy_header_l;
        SprID id_tradebuy_header_r;
        SprID id_tradebuy_panel;
        SprID id_tradebuy_black;
        SprID id_tradebuy_exit;

        TradeBuyGoods tradebuy_available[TRADEBUY_OPTIONS];

        int tradebuy_text_x(int);
        int tradebuy_text_y(int);

        int tradebuy_row_y(int);

        void tradebuy_open();
        void tradebuy_close();
};

#endif
