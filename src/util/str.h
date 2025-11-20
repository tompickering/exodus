#ifndef GUARD_UTIL_STR_H
#define GUARD_UTIL_STR_H

#define INPUT_CAP(t, w) { \
    int overfill = draw_manager.calc_text_overfill(w, Font::Default, t); \
    if (overfill > 0) { \
        input_manager.input_text_overfill(overfill); \
    } \
}


void capitalise(char*, int);
const char* tmp_caps(const char*);

#endif
