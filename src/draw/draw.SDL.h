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
        virtual void draw(const char*, DrawArea) override;
        virtual void draw(const char*, DrawTransform) override;
        virtual void draw(SprID, const char*) override;
        virtual void draw(SprID, const char*, DrawTransform) override;
        virtual void pixelswap_clear() override;
        virtual void pixelswap_draw(const char*) override;
        virtual void pixelswap_draw(const char*, DrawArea) override;
        virtual void pixelswap_draw(const char*, DrawTransform) override;
        virtual void pixelswap_draw(SprID, const char*) override;
        virtual void pixelswap_draw(SprID, const char*, DrawTransform) override;
        virtual void pixelswap_start() override;
        virtual void pixelswap_start(DrawArea*) override;
        virtual bool pixelswap_active() override;
        virtual void pixelswap_update() override;
        virtual void draw_text(const char*, Justify, int, int, RGB) override;
        virtual void draw_text(const char*, Justify, int, int, RGB, RGB) override;
        virtual void draw_text(Font, const char*, Justify, int, int, RGB) override;
        virtual void draw_text(Font, const char*, Justify, int, int, RGB, RGB) override;
        virtual void pixelswap_draw_text(const char*, Justify, int, int, RGB) override;
        virtual void pixelswap_draw_text(const char*, Justify, int, int, RGB, RGB) override;
        virtual void pixelswap_draw_text(Font, const char*, Justify, int, int, RGB) override;
        virtual void pixelswap_draw_text(Font, const char*, Justify, int, int, RGB, RGB) override;
        virtual void fade_black(float, int) override;
        virtual bool fade_active() override;
    protected:
        virtual void* get_sprite_data(const char*) override;
    private:
        SDL_Window *win;
        SDL_Surface *surf;
        SDL_Surface *background;
        SDL_Surface *temp_surf;
        unsigned int pixelswap_stage;
        void draw(SDL_Surface*, const char*, DrawArea*, SprID*);
        void draw(SDL_Surface*, const char*, DrawTransform, SprID*);
        void draw_text(SDL_Surface*, Font, const char*, Justify, int, int, RGB*, RGB*);
};

#endif
