#ifndef GUARD_MONTH_REPORT_H
#define GUARD_MONTH_REPORT_H

class MonthReport {
    public:
        MonthReport();
        bool monthreport_is_open();
        void monthreport_open();
        void monthreport_close();
    private:
        bool _monthreport_open;
};

#endif
