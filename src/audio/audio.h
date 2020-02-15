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
        virtual void play_sfx(SFX) = 0;
        virtual void target_music(string) = 0;
    protected:
        map<SFX, void*> sfx_data;
        string target_track;
        string playing_track;
};

#ifdef SDL
#include "audio.SDL.h"
#endif

#endif
