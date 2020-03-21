#include "audio.SDL.h"

#include <SDL2/SDL_mixer.h>

#include "../assetpaths.h"

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

void AudioManagerSDL::load_resources() {
    char mus_path[ASSET_PATH_LEN_MAX];

    for (unsigned int i = 0;; ++i) {
        if (ASSETS_MUS[i][0] == '\0')
            break;
        strncpy(mus_path, ASSETS_MUS[i], ASSET_PATH_LEN_MAX);
        strcat(mus_path, ".ogg");
        music_data[ASSETS_MUS[i]] = Mix_LoadMUS(mus_path);
        if (music_data[ASSETS_MUS[i]]) {
            L.debug("Loaded %s", mus_path);
        } else {
            L.warn("Could not load %s", mus_path);
        }
    }
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
