#ifndef GUARD_AUDIO_SDL_H
#define GUARD_AUDIO_SDL_H

#include "audio.h"

#include <string>

using std::string;

class AudioManagerSDL : public AudioManager {
    public:
        virtual bool init() override;
        virtual void load_resources() override;
        virtual void play_sfx(const char*) override;
        virtual void target_music(const char*) override;
        virtual void fade_out(int) override;
};

#endif
