#include "audio.SDL.h"

#include <SDL2/SDL_mixer.h>

#include "../shared.h"

bool AudioManagerSDL::init() {
    L.info("AudioManager init...");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        L.error("SDL_mixer failed to initialise");
        return false;
    }

    target_track = "";
    playing_track = "";

    return true;
}

void AudioManagerSDL::target_music(string track) {
    L.debug("Targetting music: %s", track);
}

void AudioManagerSDL::play_sfx(SFX sfx) {
    L.debug("Playing SFX: %d", (int) sfx);
}
