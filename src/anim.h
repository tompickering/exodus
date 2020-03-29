#ifndef GUARD_ANIM_H
#define GUARD_ANIM_H

#define MAX_ANIM_FRAMES 30

class Anim {
    public:
        Anim(int, ...);
        const char* frame(int);
        const char* interp(float);
    private:
        char nframes;
        const char* frames[MAX_ANIM_FRAMES];
};

#endif
