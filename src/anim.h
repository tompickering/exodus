#ifndef GUARD_ANIM_H
#define GUARD_ANIM_H

#define MAX_ANIM_FRAMES 30

class Anim {
    public:
        Anim();
        Anim(int, ...);
        const char* frame(int);
        const char* interp(float);
        bool is_static();
        int get_n_frames();
    private:
        char nframes;
        const char* frames[MAX_ANIM_FRAMES];
};

#endif
