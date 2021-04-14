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
 *
 * Sprite IDs can also be used to draw particular regions of a sprite. This is
 * achieved by first attaining an ID and then calling set_source_region with
 * that ID and a pointer to the source region to use when drawing with that ID.
 * The region may be updated with subsequent calls to the function.
 * Passing nullptr with the ID will terminate this behaviour.
 */

typedef unsigned int SprID;

extern const SprID ID_NONE;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGB;

typedef struct {
    int x;
    int y;
    float anchor_x;
    float anchor_y;
    float scale_x;
    float scale_y;

} DrawTransform;

typedef struct DrawArea {
    int x;
    int y;
    int w;
    int h;
    bool overlaps(DrawArea& o) {
        if (x+w < o.x)   return false;
        if (x > o.x+o.w) return false;
        if (y+h < o.y)   return false;
        if (y > o.y+o.h) return false;
        return true;
    }
} DrawArea;

enum DrawType {
    DRAWTYPE_Unknown,
    DRAWTYPE_Sprite,
    DRAWTYPE_Text,
    DRAWTYPE_Fill,
    DRAWTYPE_Pattern,
};

typedef struct {
    SprID id;
    DrawArea area;
    /*
     * FIXME: We store some information to record *what* was actually drawn
     * so we can later use it to repair if necessary, after an overlaid sprite
     * is moved / removed. However, we currently only record sufficient data
     * for sprite, fill and pattern-type draws - not text, as this would require
     * us to allocate large amounts of space per-entry to remember the text content
     * and draw style.
     */
    DrawType type;
    const char *sprite;
    RGB colour;
} DrawnSprite;

typedef struct {
    SprID id;
    float x;
    float y;
} SpriteClick;

enum Font {
    Default,
    Tiny,
    Small,
    Large,
};

enum Justify {
    Left,
    Centre,
    Right,
};

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
        virtual void update(float, MousePos, MousePos);
        virtual void save_background() = 0;
        virtual void save_background(DrawArea) = 0;
        virtual void restore_background(DrawArea) = 0;
        virtual void clear() = 0;
        virtual void clear(DrawTarget) = 0;
        virtual SprID new_sprite_id();
        virtual void release_sprite_id(SprID);
        virtual void refresh_sprite_id(SprID);
        virtual DrawnSprite* get_drawn_info(SprID);
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
        virtual void pixelswap_start(const DrawArea*) = 0;
        virtual bool pixelswap_active() = 0;
        virtual void pixelswap_update() = 0;
        virtual void draw_rect(DrawArea area, float, RGB) = 0;
        virtual void fill(DrawArea area, RGB) = 0;
        virtual void fill(SprID, DrawArea area, RGB) = 0;
        virtual void fill(DrawTarget, DrawArea area, RGB) = 0;
        virtual void fill_pattern(DrawArea area) = 0;
        virtual void fill_pattern(SprID, DrawArea area) = 0;
        virtual void fill_pattern(DrawTarget, DrawArea area) = 0;
        virtual void draw_text(const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(const char*, Justify, int, int, RGB, RGB) = 0;
        virtual void draw_text(Font, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(Font, SprID, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(Font, const char*, Justify, int, int, RGB, RGB) = 0;
        virtual void draw_text(SprID, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(SprID, Font, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(DrawTarget, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(DrawTarget, const char*, Justify, int, int, RGB, RGB) = 0;
        virtual void draw_text(DrawTarget, Font, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(DrawTarget, Font, const char*, Justify, int, int, RGB, RGB) = 0;
        virtual void draw_text(DrawTarget, SprID, const char*, Justify, int, int, RGB) = 0;
        virtual void draw_text(DrawTarget, SprID, Font, const char*, Justify, int, int, RGB) = 0;
        virtual void set_source_region(SprID, DrawArea*);
        virtual DrawArea* get_source_region(SprID);
        virtual void fade_start(float, int) = 0;
        virtual void fade_black(float, int) = 0;
        virtual void fade_white(float, int) = 0;
        virtual bool fade_active() = 0;
        virtual void show_cursor(bool);
        virtual SpriteClick get_clicked_sprite();
        virtual SpriteClick query_click(SprID);
        virtual SpriteClick query_mouseover(SprID);
        virtual void consume_click();
        virtual bool mouse_over(SprID);
        virtual bool mouse_in_area(DrawArea);
        virtual void clear_sprite_ids();
        virtual bool clicked();
        virtual int right(SprID);
        virtual void get_upscale(float&, float&);
        virtual void set_selectable(SprID);
        virtual void unset_selectable(SprID);
    protected:
        map<const char*, void*> sprite_data;
        virtual void* get_sprite_data(const char*) = 0;
        virtual void adjust_selectable_text_col(SprID, RGB&);
        map<SprID, DrawArea> id_source_regions;
        map<Font, void*> font_data;
        SprID next_id;
        vector<DrawnSprite> drawn_spr_info;
        DrawArea pixelswap_region;
        float pixelswap_time;
        float fade_time;
        float fade_seconds;
        int fade_stage;
        int fade_stages;
        bool draw_cursor;
        bool clicked_this_frame;
    private:
        MousePos mouse_pos;
        MousePos click_pos;
        SpriteClick sprite_click;
        SprID prev_mouseover_selectable_text_id;
        SprID mouseover_selectable_text_id;
        float mouseover_selectable_text_time;
        vector<SprID> selectable_text_ids;
        virtual RGB text_pulse_col(float);
};

#ifdef SDL
#include "draw.SDL.h"
#endif

#endif
