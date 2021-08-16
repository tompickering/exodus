#ifndef GUARD_AUDIO_H
#define GUARD_AUDIO_H

#include <map>
#include <string>

using std::map;
using std::string;

class AudioManager {
    public:
        AudioManager() : enabled(false), target_track(nullptr), playing_track(nullptr) {}
        virtual bool init() = 0;
        virtual void load_resources() = 0;
        virtual void play_sfx(const char*) = 0;
        virtual void target_music(const char*) = 0;
        virtual void fade_out(int) = 0;
        virtual bool is_playing(const char* track) const { return playing_track && (track == playing_track); }
        virtual const char* get_playing() const { return playing_track; }
    protected:
        bool enabled;
        map<const char*, void*> sfx_data;
        map<const char*, void*> music_data;
        const char *target_track;
        const char *playing_track;
};

#ifdef SDL
#include "audio.SDL.h"
#endif

#endif
