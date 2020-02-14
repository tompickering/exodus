#ifndef GUARD_DRAW_H
#define GUARD_DRAW_H

#include <map>
#include <vector>

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
    int w;
    int h;
} DrawArea;

typedef struct {
    SprID id;
    DrawArea area;
} DrawnSprite;

class DrawManager {
    public:
        virtual bool init() = 0;
        virtual void load_resources() = 0;
        virtual void update(MousePos, MousePos) = 0;
        virtual void save_background() = 0;
        virtual void clear();
        virtual SprID new_sprite_id();
        virtual void draw_text(const char*, int, int, int, int,
                               unsigned char, unsigned char, unsigned char) = 0;
    protected:
        map<const char*, void*> sprite_data;
        virtual void* get_sprite_data(const char*) = 0;
        void *font;
        SprID next_id;
        vector<DrawnSprite> drawn_spr_info;
};

#ifdef SDL
#include "draw.SDL.h"
#endif

#endif
