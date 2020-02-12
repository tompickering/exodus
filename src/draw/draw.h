#ifndef GUARD_DRAW_H
#define GUARD_DRAW_H

#include <map>
#include <vector>

#include "../timer/timer.h"
#include "../shared.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 512

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
 */

typedef unsigned int SprID;

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

class DrawManager {
    public:
        DrawManager();
        virtual bool init() = 0;
        virtual void load_resources() = 0;
        virtual void update(MousePos, MousePos) = 0;
        virtual void save_background() = 0;
        virtual void clear();
        virtual SprID new_sprite_id();
        virtual void release_sprite_id(SprID);
        virtual DrawArea* get_drawn_area(SprID);
        virtual void draw(const char*) = 0;
        virtual void draw(const char*, DrawArea) = 0;
        virtual void draw(const char*, DrawTransform) = 0;
        virtual void draw(SprID, const char*, DrawTransform) = 0;
        virtual void pixelswap_clear() = 0;
        virtual void pixelswap_draw(const char*) = 0;
        virtual void pixelswap_draw(const char*, DrawArea) = 0;
        virtual void pixelswap_draw(const char*, DrawTransform) = 0;
        virtual void pixelswap_draw(SprID, const char*, DrawTransform) = 0;
        virtual void pixelswap_start() = 0;
        virtual void pixelswap_start(DrawArea*) = 0;
        virtual bool pixelswap_active() = 0;
        virtual void pixelswap_update() = 0;
        virtual void draw_text(const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(const char*, Justify, int, int, RGB, RGB) = 0;
    protected:
        map<const char*, void*> sprite_data;
        virtual void* get_sprite_data(const char*) = 0;
        void *font;
        SprID next_id;
        vector<DrawnSprite> drawn_spr_info;
        DrawArea pixelswap_region;
        TIMER pixelswap_timer;
};

#ifdef SDL
#include "draw.SDL.h"
#endif

#endif
