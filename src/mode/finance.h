#ifndef GUARD_FINANCE_H
#define GUARD_FINANCE_H

#include "mode_base.h"

#include "draw/draw.h"

#include "state/exodus_state.h"

enum FinanceReport {
    FR_LastMonth,
    FR_OverLastMonth,
    FR_ThisMonth,
};

class Finance : ModeBase {
    public:
        Finance();
        virtual void enter() override;
        virtual ExodusMode update(float) override;
    private:
        void draw_finance(FinanceReport);
        FinanceReport current_report;
};

#endif
