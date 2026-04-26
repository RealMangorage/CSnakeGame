#ifndef SOUNDPLAYER_H
#define SOUNDPLAYER_H

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

/**
 * In the latest SDL3_mixer (Main Branch):
 * Mix_Chunk -> MIX_Audio
 */
typedef MIX_Audio SoundAsset;

bool SoundPlayer_Init(void);
SoundAsset* SoundPlayer_LoadFromMem(const unsigned char* data, size_t len);

void SoundPlayer_Play(SoundAsset* asset);
void SoundPlayer_PlayLoop(SoundAsset* asset);

bool SoundPlayer_IsPlaying(SoundAsset* asset);
void SoundPlayer_FreeAsset(SoundAsset* asset);
void SoundPlayer_Quit(void);

#endif