#ifndef PLAYER_SFX_H
#define PLAYER_SFX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

void player_sfx_init();
void player_sfx_unload();
void player_sfx_play(const char* name);
void player_sfx_stop();
bool player_sfx_isPlaying();
void player_sfx_onVBlank();
void player_sfx_update();

#ifdef __cplusplus
}
#endif

#endif  // PLAYER_SFX_H
