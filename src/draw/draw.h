#ifndef GUARD_DRAW_H
#define GUARD_DRAW_H

#include <map>
#include <vector>

#include "timer/timer.h"
#include "shared.h"

#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 768

#define RES_X 640
#define RES_Y 512

using std::map;
using std::vector;

/*
 * Many states in Exodus involve drawing moving sprites over a static background.
 * As such, we separate the concepts of 'background' and overlaid 'sprites'.
 * The current contents of the frame buffer can be 'saved' as the new background.
 * To draw a sprite, we request a unique sprite identifier (SprID) and provide this
 * when we draw. The manager keeps track of which sprite UIDs are currently being
 * drawn, along with the area over which they were drawn, and if we re-draw the same
 * sprite (as identified by the ID - not the sprite itself in case we are using the
 * new frame in an animation), then we'll refresh that area of background first.
 *
 * Sprites with IDs should be removed from the screen by calling draw with that
 * ID and a null pointer as the sprite key.
 */

typedef unsigned int SprID;

extern const SprID ID_NONE;
extern const SprID ID_CURSOR;

typedef struct {
    int x;
    int y;
    float anchor_x;
    float anchor_y;
    float scale_x;
    float scale_y;

} DrawTransform;

typedef struct {
    int x;
    int y;
    int w;
    int h;
} DrawArea;

typedef struct {
    SprID id;
    DrawArea area;
} DrawnSprite;

typedef struct {
    SprID id;
    float x;
    float y;
} SpriteClick;

enum Font {
    Default,
    Small,
    Large,
};

enum Justify {
    Left,
    Centre,
    Right,
};

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGB;

enum DrawTarget {
    TGT_Primary,
    TGT_Secondary,
    TGT_Pattern,
};

class DrawManager {
    public:
        DrawManager();
        virtual bool init() = 0;
        virtual void load_resources() = 0;
        virtual void update(MousePos, MousePos);
        virtual void save_background() = 0;
        virtual void clear() = 0;
        virtual void clear(DrawTarget) = 0;
        virtual SprID new_sprite_id();
        virtual void release_sprite_id(SprID);
        virtual DrawArea* get_drawn_area(SprID);
        virtual void draw_init_image() = 0;
        virtual void draw(const char*) = 0;
        virtual void draw(const char*, DrawArea) = 0;
        virtual void draw(const char*, DrawTransform) = 0;
        virtual void draw(SprID, const char*) = 0;
        virtual void draw(SprID, const char*, DrawTransform) = 0;
        virtual void draw(DrawTarget, const char*) = 0;
        virtual void draw(DrawTarget, const char*, DrawArea) = 0;
        virtual void draw(DrawTarget, const char*, DrawTransform) = 0;
        virtual void draw(DrawTarget, SprID, const char*) = 0;
        virtual void draw(DrawTarget, SprID, const char*, DrawTransform) = 0;
        virtual void pixelswap_start() = 0;
        virtual void pixelswap_start(DrawArea*) = 0;
        virtual bool pixelswap_active() = 0;
        virtual void pixelswap_update() = 0;
        virtual void fill(DrawArea area, RGB) = 0;
        virtual void fill(DrawTarget, DrawArea area, RGB) = 0;
        virtual void pattern_fill(DrawArea area) = 0;
        virtual void pattern_fill(DrawTarget, DrawArea area) = 0;
        virtual void draw_text(const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(const char*, Justify, int, int, RGB, RGB) = 0;
        virtual void draw_text(Font, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(Font, const char*, Justify, int, int, RGB, RGB) = 0;
        virtual void draw_text(SprID, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(SprID, Font, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(DrawTarget, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(DrawTarget, const char*, Justify, int, int, RGB, RGB) = 0;
        virtual void draw_text(DrawTarget, Font, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(DrawTarget, Font, const char*, Justify, int, int, RGB, RGB) = 0;
        virtual void draw_text(DrawTarget, SprID, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(DrawTarget, SprID, Font, const char*, Justify, int, int, RGB) = 0;
        virtual void fade_start(float, int) = 0;
        virtual void fade_black(float, int) = 0;
        virtual void fade_white(float, int) = 0;
        virtual bool fade_active() = 0;
        virtual void show_cursor(bool);
        virtual SpriteClick get_clicked_sprite();
        virtual SpriteClick query_click(SprID);
        virtual void clear_sprite_ids();
    protected:
        map<const char*, void*> sprite_data;
        virtual void* get_sprite_data(const char*) = 0;
        map<Font, void*> font_data;
        SprID next_id;
        vector<DrawnSprite> drawn_spr_info;
        DrawArea pixelswap_region;
        TIMER pixelswap_timer;
        TIMER fade_timer;
        float fade_seconds;
        int fade_stage;
        int fade_stages;
        bool draw_cursor;
    private:
        MousePos click_pos;
        SpriteClick sprite_click;
};

#ifdef SDL
#include "draw.SDL.h"
#endif

#endif
