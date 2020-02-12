#ifndef GUARD_DRAW_H
#define GUARD_DRAW_H

#include <map>
#include <string>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 512

using std::map;
using std::string;

class DrawManager {
    public:
        virtual bool init() = 0;
        virtual void update() = 0;
    protected:
        map<string, void*> sprite_data;
        virtual void* get_sprite_data(string) = 0;
        void *font;
};

#ifdef SDL
#include "draw.SDL.h"
#endif

#endif
