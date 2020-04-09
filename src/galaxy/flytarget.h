#ifndef GUARD_FLYTARGET_H
#define GUARD_FLYTARGET_H

#define FT_MAX_NAME 20

class FlyTarget {
    public:
        FlyTarget() : FlyTarget(-1, -1, (char*)"NONAME") {};
        FlyTarget(int, int, const char*);
        const int x;
        const int y;
        char name[FT_MAX_NAME + 1];
};

#endif
