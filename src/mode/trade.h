#ifndef GUARD_TRADE_H
#define GUARD_TRADE_H

#include "mode_base.h"

#include "draw/draw.h"

struct TradeRow {
    const char* img;
    const char* name;
    int cost;
    int buy;
    int stock;
    int freight;
    SprID id_stock;
    SprID id_freight;
};

class Trade : ModeBase {
    public:
        Trade();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:
        int active_row;
        TradeRow rows[8];
        bool sell;
};

#endif
