#include "anim.h"

#include "shared.h"

Anim::Anim() : nframes(0) {
}

Anim::Anim(int _nframes, ...) : nframes(_nframes) {
    va_list args;
    va_start(args, _nframes);
    for (int i = 0; i < nframes; ++i)
        frames[i] = va_arg(args, const char*);
    va_end(args);
}

const char* Anim::frame(int frame) {
    if (frame < 0) {
        L.warn("Anim frame < 0: %f", frame);
        frame = 0;
    }

    frame %= nframes;

    return frames[frame];
}

const char* Anim::interp(float interp) {
    if (interp < 0) {
        L.warn("Anim interp < 0: %f", interp);
        interp = 0;
    }
    if (interp > 1) {
        L.warn("Anim interp > 1: %f", interp);
        interp = 1;
    }

    // Ensure we can't overflow our index
    if (interp == 1) {
        interp = 0.999;
    }

    int idx = (int)(interp * ((float)nframes));
    if (idx < 0) {
        L.warn("Anim frame < 0: %d", idx);
        idx = 0;
    }

    return frames[idx];
}

bool Anim::is_static() {
    return nframes <= 1;
}

int Anim::get_n_frames() {
    return nframes;
}
