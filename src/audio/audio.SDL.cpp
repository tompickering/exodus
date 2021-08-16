#include "audio.SDL.h"

#include <SDL2/SDL_mixer.h>

#include "assetpaths.h"

#include "shared.h"

bool AudioManagerSDL::init() {
    L.info("AudioManager init...");
    enabled = false;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        L.error("SDL_mixer failed to initialise");
        return false;
    }

    target_track = nullptr;
    playing_track = nullptr;

    enabled = true;
    return true;
}

void AudioManagerSDL::load_resources() {
    if (!enabled)
        return;

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

    for (unsigned int i = 0;; ++i) {
        if (ASSETS_SFX[i][0] == '\0')
            break;
        strncpy(mus_path, ASSETS_SFX[i], ASSET_PATH_LEN_MAX);
        strcat(mus_path, ".wav");
        sfx_data[ASSETS_SFX[i]] = Mix_LoadWAV(mus_path);
        if (sfx_data[ASSETS_SFX[i]]) {
            L.debug("Loaded %s", mus_path);
        } else {
            L.warn("Could not load %s", mus_path);
        }
    }
}

void AudioManagerSDL::target_music(const char* track) {
    if (!enabled)
        return;

    if (!track) {
        L.debug("Stopping music: %s", track);
        Mix_PauseMusic();
        return;
    }

    L.debug("Targetting music: %s", track);
    if (track == target_track) {
        L.debug("Track already playing: %s", track);
    } else {
        target_track = track;
        Mix_PlayMusic((Mix_Music*)music_data[target_track], -1);
        playing_track = target_track;
    }
}

void AudioManagerSDL::play_sfx(const char* sfx) {
    if (!enabled)
        return;
    if (!sfx)
        return;
    L.debug("Playing SFX: %s", sfx);
    Mix_PlayChannel(-1, (Mix_Chunk*)sfx_data[sfx], 0);
}

void AudioManagerSDL::fade_out(int ms) {
    if (!enabled)
        return;
    if (!Mix_PlayingMusic())
        return;
    Mix_FadeOutMusic(ms);
}
