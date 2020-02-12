#ifndef GUARD_DRAW_SDL_H
#define GUARD_DRAW_SDL_H

#include "draw.h"

#include <SDL2/SDL.h>

class DrawManagerSDL : public DrawManager {
    public:
        virtual bool init() override;
        virtual void update() override;
        virtual void draw_text(const char*, int, int, int, int,
                               unsigned char, unsigned char, unsigned char) override;
    protected:
        virtual void* get_sprite_data(string) override;
    private:
        SDL_Window *win;
        SDL_Surface *surf;
};

#endif
