#ifndef GUARD_TRADE_H
#define GUARD_TRADE_H

#include "mode_base.h"

#include "draw/draw.h"

struct TradeRow {
    const char* img;
    const char* name;
    const char* icon;
    int cost;
    int buy;
    int stock;
    SprID id_stock;
    SprID id_freight;
    SprID id_unavailable;
};

class Trade : ModeBase {
    public:
        Trade();
        virtual void enter() override;
        virtual void exit() override;
        virtual ExodusMode update(float) override;
    private:

        enum Stage {
            Overview,
            Info,
            DoTrade,
        };

        Stage stage;

        int active_row;
        TradeRow rows[8];
        bool sell;

        int get_cost(TradeQuality, int);
        int get_buy(TradeQuality, int);
        int get_freight(int);
        void draw_stock_freight();
        void open_panel();
        void close_panel();
};

#endif
