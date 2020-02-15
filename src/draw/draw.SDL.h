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
        virtual void draw(const char*, DrawArea*) override;
        virtual void pixelswap_clear() override;
        virtual void pixelswap_draw(const char*) override;
        virtual void pixelswap_draw(const char*, DrawArea*) override;
        virtual void pixelswap_start() override;
        virtual void pixelswap_start(DrawArea*) override;
        virtual bool pixelswap_active() override;
        virtual void pixelswap_update() override;
        virtual void draw_text(const char*, Justify, int, int, RGB) override;
        void draw(SDL_Surface*, const char*, DrawArea*);
    protected:
        virtual void* get_sprite_data(const char*) override;
    private:
        SDL_Window *win;
        SDL_Surface *surf;
        SDL_Surface *background;
        SDL_Surface *pixelswap_src;
        unsigned int pixelswap_stage;
};

#endif
