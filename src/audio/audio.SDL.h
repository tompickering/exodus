#ifndef GUARD_AUDIO_SDL_H
#define GUARD_AUDIO_SDL_H

#include "audio.h"

#include <string>

using std::string;

class AudioManagerSDL : public AudioManager {
    public:
        virtual bool init() override;
        virtual void load_resources() override;
        virtual void play_sfx(SFX) override;
        virtual void target_music(string) override;
};

#endif
