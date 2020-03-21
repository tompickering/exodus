#ifndef GUARD_AUDIO_H
#define GUARD_AUDIO_H

#include <map>
#include <string>

using std::map;
using std::string;

enum SFX {
};

class AudioManager {
    public:
        virtual bool init() = 0;
        virtual void load_resources() = 0;
        virtual void play_sfx(SFX) = 0;
        virtual void target_music(const char*) = 0;
    protected:
        bool enabled;
        map<const char*, void*> music_data;
        map<SFX, void*> sfx_data;
        const char *target_track;
        const char *playing_track;
};

#ifdef SDL
#include "audio.SDL.h"
#endif

#endif
