#include "audio.SDL.h"

#include <SDL2/SDL_mixer.h>

#include "../shared.h"

bool AudioManagerSDL::init() {
    L.info("AudioManager init...");
    enabled = false;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        L.error("SDL_mixer failed to initialise");
        return false;
    }

    target_track = "";
    playing_track = "";

    enabled = true;
    return true;
}

void AudioManagerSDL::target_music(string track) {
    if (!enabled)
        return;
    L.debug("Targetting music: %s", track);
}

void AudioManagerSDL::play_sfx(SFX sfx) {
    if (!enabled)
        return;
    L.debug("Playing SFX: %d", (int) sfx);
}
