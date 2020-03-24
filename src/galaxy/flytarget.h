#ifndef GUARD_FLYTARGET_H
#define GUARD_FLYTARGET_H

class FlyTarget {
    public:
        FlyTarget() : x(0), y(0) {};
        FlyTarget(int, int);
        const int x;
        const int y;
};

#endif
