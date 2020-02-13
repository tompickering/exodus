#ifndef GUARD_DRAW_H
#define GUARD_DRAW_H

#include <map>

#include "../shared.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 512

using std::map;

class DrawManager {
    public:
        virtual bool init() = 0;
        virtual void load_resources() = 0;
        virtual void update(MousePos, MousePos) = 0;
        virtual void draw_text(const char*, int, int, int, int,
                               unsigned char, unsigned char, unsigned char) = 0;
    protected:
        map<const char*, void*> sprite_data;
        virtual void* get_sprite_data(const char*) = 0;
        void *font;
};

#ifdef SDL
#include "draw.SDL.h"
#endif

#endif
