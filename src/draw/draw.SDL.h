#ifndef GUARD_DRAW_SDL_H
#define GUARD_DRAW_SDL_H

#include "draw.h"

#include <SDL2/SDL.h>

class DrawManagerSDL : public DrawManager {
    public:
        virtual bool init() override;
        virtual void load_resources() override;
        virtual void update(MousePos, MousePos) override;
        virtual void clear() override;
        virtual void save_background() override;
        virtual void draw(const char*) override;
        virtual void draw(const char*, int, int, int, int) override;
        virtual void draw_text(const char*, Justify, int, int, RGB) override;
    protected:
        virtual void* get_sprite_data(const char*) override;
    private:
        SDL_Window *win;
        SDL_Surface *surf;
        SDL_Surface *background;
};

#endif
