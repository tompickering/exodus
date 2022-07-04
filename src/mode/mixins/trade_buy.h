#ifndef GUARD_TRADE_BUY_H
#define GUARD_TRADE_BUY_H

#define TRADEBUY_OPTIONS 4

enum TradeBuyGoodsType {
    TBGT_Food,
    TBGT_Inf,
    TBGT_Gli,
    TBGT_Art,
};

struct TradeBuyGoods {
    TradeBuyGoodsType type;
    int avail;
};

class TradeBuy {
    public:
        TradeBuy();
        bool tradebuy_start(int, int, int, int);
        bool tradebuy_update();
    private:
        TradeBuyGoods tradebuy_available[TRADEBUY_OPTIONS];
};

#endif
