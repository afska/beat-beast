#ifndef PLAYER_H
#define PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "PlaybackState.h"

#define RATE_LEVELS 3
#define AUDIO_SYNC_LIMIT 2

void player_init();
void player_unload();
void player_play(const char* name);
// void player_playSfx(const char* name);
void player_enableLoop();
void player_seek(unsigned int msecs);
void player_setRate(int rate);
void player_stop();
bool player_isPlaying();
void player_onVBlank();
void player_update(int expectedAudioChunk,
                   void (*onAudioChunks)(unsigned int current));

#ifdef __cplusplus
}
#endif

#endif  // PLAYER_H
