#ifndef GUARD_TRADE_H
#define GUARD_TRADE_H

#include "mode_base.h"

#include "draw/draw.h"

#include "mixins/trade_buy.h"

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

class Trade : ModeBase, TradeBuy {
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
            TradeBuy,
        };

        Stage stage;

        TradeQuality quality;
        int active_row;
        TradeRow rows[8];
        bool sell;
        int bought;
        int sold;

        bool food_needed;
        bool arms_offered;

        int get_cost(TradeQuality, int);
        int get_buy(TradeQuality, int);
        int get_freight(int);
        void draw_stock_freight();
        void open_panel();
        void close_panel();
        void start_trade(bool);
        void adjust_trade(bool);
        void adjust_freight(int, bool);

        Freight freight_initial;
};

#endif
