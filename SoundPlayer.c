#include "SoundPlayer.h"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

// Mixer instance
static MIX_Mixer* mixer = NULL;

bool SoundPlayer_Init(void) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    // 🔥 THIS is what you're missing
    if (!MIX_Init()) {
        SDL_Log("MIX_Init failed: %s", SDL_GetError());
        return false;
    }

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);

    if (!mixer) {
        SDL_Log("Mixer creation failed: %s", SDL_GetError());
        return false;
    }

    SDL_ResumeAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);

    SDL_Log("Audio initialized successfully.");
    return true;
}
SoundAsset* SoundPlayer_LoadFromMem(const unsigned char* data, size_t len) {
    SDL_IOStream* src = SDL_IOFromConstMem(data, len);
    if (!src) {
        SDL_Log("IOStream creation failed: %s", SDL_GetError());
        return NULL;
    }

    SoundAsset* asset = MIX_LoadAudio_IO(mixer, src, true, true);

    if (!asset) {
        SDL_Log("Audio load failed: %s", SDL_GetError());
        return NULL;
    }

    return asset;
}

static void SoundPlayer_PlayInternal(SoundAsset* asset, int loopCount) {
    if (!asset || !mixer) {
        SDL_Log("Play failed: invalid asset or mixer");
        return;
    }

    SDL_PropertiesID props = MIX_GetAudioProperties(asset);

    SDL_SetNumberProperty(props, "mixer.audio.loop_count", loopCount);
    SDL_SetBooleanProperty(props, "mixer.audio.playing", false);

    if (!MIX_PlayAudio(mixer, asset)) {
        SDL_Log("Playback failed: %s", SDL_GetError());
    }
}

// 🔊 Play once
void SoundPlayer_Play(SoundAsset* asset) {
    SoundPlayer_PlayInternal(asset, 0);
}

// 🔁 Play in loop (FOREVER)
void SoundPlayer_PlayLoop(SoundAsset* asset) {
    SoundPlayer_PlayInternal(asset, -1);
}

// 🔁 Play N times
void SoundPlayer_PlayLoopCount(SoundAsset* asset, int count) {
    SoundPlayer_PlayInternal(asset, count);
}

bool SoundPlayer_IsPlaying(SoundAsset* asset) {
    if (!asset || !mixer) return false;

    SDL_PropertiesID props = MIX_GetAudioProperties(asset);
    return SDL_GetBooleanProperty(props, "mixer.audio.playing", false);
}

void SoundPlayer_Stop(SoundAsset* asset) {
    if (!asset || !mixer) return;

    SDL_PropertiesID props = MIX_GetAudioProperties(asset);
    SDL_SetBooleanProperty(props, "mixer.audio.playing", false);
}

void SoundPlayer_FreeAsset(SoundAsset* asset) {
    if (asset) {
        MIX_DestroyAudio(asset);
    }
}

void SoundPlayer_Quit(void) {
    if (mixer) {
        MIX_DestroyMixer(mixer);
        mixer = NULL;
    }

    SDL_Quit();
}