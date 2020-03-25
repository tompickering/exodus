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
        virtual void clear(DrawTarget) override;
        virtual void save_background() override;
        virtual void draw_init_image() override;
        virtual void draw(const char*) override;
        virtual void draw(const char*, DrawArea) override;
        virtual void draw(const char*, DrawTransform) override;
        virtual void draw(SprID, const char*) override;
        virtual void draw(SprID, const char*, DrawTransform) override;
        virtual void draw(DrawTarget, const char*) override;
        virtual void draw(DrawTarget, const char*, DrawArea) override;
        virtual void draw(DrawTarget, const char*, DrawTransform) override;
        virtual void draw(DrawTarget, SprID, const char*) override;
        virtual void draw(DrawTarget, SprID, const char*, DrawTransform) override;
        virtual void pixelswap_start() override;
        virtual void pixelswap_start(DrawArea*) override;
        virtual bool pixelswap_active() override;
        virtual void pixelswap_update() override;
        virtual void fill(DrawArea, RGB) override;
        virtual void fill(DrawTarget, DrawArea, RGB) override;
        virtual void pattern_fill(DrawArea) override;
        virtual void draw_text(const char*, Justify, int, int, RGB) override;
        virtual void draw_text(const char*, Justify, int, int, RGB, RGB) override;
        virtual void draw_text(Font, const char*, Justify, int, int, RGB) override;
        virtual void draw_text(Font, const char*, Justify, int, int, RGB, RGB) override;
        virtual void draw_text(SprID, const char*, Justify, int, int, RGB) override;
        virtual void draw_text(SprID, Font, const char*, Justify, int, int, RGB) override;
        virtual void draw_text(DrawTarget, const char*, Justify, int, int, RGB) override;
        virtual void draw_text(DrawTarget, const char*, Justify, int, int, RGB, RGB) override;
        virtual void draw_text(DrawTarget, Font, const char*, Justify, int, int, RGB) override;
        virtual void draw_text(DrawTarget, Font, const char*, Justify, int, int, RGB, RGB) override;
        virtual void draw_text(DrawTarget, SprID, const char*, Justify, int, int, RGB) override;
        virtual void draw_text(DrawTarget, SprID, Font, const char*, Justify, int, int, RGB) override;
        virtual void fade_start(float, int) override;
        virtual void fade_black(float, int) override;
        virtual void fade_white(float, int) override;
        virtual bool fade_active() override;
        virtual void show_cursor(bool) override;
    protected:
        virtual void* get_sprite_data(const char*) override;
    private:
        SDL_Window *win;
        SDL_Surface *surf;
        SDL_Surface *background;
        SDL_Surface *src_surf_0; // Used for pixelswap target, fade target
        SDL_Surface *src_surf_1; // Used for fade source
        SDL_Surface *pattern;
        unsigned int pixelswap_stage;
        SDL_Surface *load_normalised_image(const char*);
        SDL_Surface *get_target(DrawTarget);
        void draw(DrawTarget, const char*, DrawArea*, SprID*);
        void draw(DrawTarget, const char*, DrawTransform, SprID*);
        void draw_text(DrawTarget, SprID, Font, const char*, Justify, int, int, RGB*, RGB*);
        void repair_dirty_area(SprID);
        void update_dirty_area(SprID, DrawArea);
};

#endif
