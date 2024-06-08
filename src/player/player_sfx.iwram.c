#include "player_sfx.h"

#include <gba_dma.h>
#include <gba_input.h>
#include <gba_interrupt.h>
#include <gba_sound.h>
#include <gba_systemcalls.h>
#include <gba_timers.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>  // for memset

#include "../utils/gbfs/gbfs.h"

#define TIMER_16MHZ 0
#define FIFO_ADDR_A 0x040000a0
#define CHANNEL_A_MUTE 0xfcff  /*0b1111110011111111*/
#define CHANNEL_A_UNMUTE 0x300 /*0b0000001100000000*/
#define AUDIO_CHUNK_SIZE_PCM 304
#define FRACUMUL_PRECISION 0xFFFFFFFF
#define AS_MSECS_PCM 118273043  // 0xffffffff * (1000/36314)
#define AS_CURSOR_PCM 1348619731
#define REG_DMA1CNT_L *(vu16*)(REG_BASE + 0x0c4)
#define REG_DMA1CNT_H *(vu16*)(REG_BASE + 0x0c6)

#define CODE_ROM __attribute__((section(".code")))
#define CODE_EWRAM __attribute__((section(".ewram")))
#define INLINE static inline __attribute__((always_inline))

// ------------------------------------
// SFX player (for uncompressed s8 PCM)
// ------------------------------------
// Audio is taken from the embedded GBFS file in ROM.
// FIFO channel A, Timer 0 and DMA1 are used.
// The sample rate is 36314hz.
// Each PCM chunk is 304 bytes and represents 304 samples.
// Two chunks are copied per frame.
// Playback rate can be either 1 or 1.11.

static bool did_run = false;
static bool is_looping = false;

#define AS_MSECS AS_MSECS_PCM

#define AUDIO_PROCESS(ON_STOP)                          \
  did_run = true;                                       \
  buffer = double_buffers[cur_buffer];                  \
                                                        \
  if (src != NULL) {                                    \
    if (src_pos < src_len) {                            \
      for (u32 i = 0; i < 608 / 4; i++)                 \
        ((u32*)buffer)[i] = ((u32*)(src + src_pos))[i]; \
      src_pos += 608;                                   \
      if (src_pos > src_len) {                          \
        ON_STOP;                                        \
      }                                                 \
    } else {                                            \
      ON_STOP;                                          \
    }                                                   \
  }

uint32_t fracumul(uint32_t x, uint32_t frac) __attribute__((long_call));
static const GBFS_FILE* fs;
static const unsigned char* src = NULL;
static u32 src_len = 0;
static u32 src_pos = 0;
static s8 double_buffers[2][608] __attribute__((aligned(4)));
static u32 cur_buffer = 0;
static s8* buffer;

INLINE void mute() {
  DSOUNDCTRL = DSOUNDCTRL & CHANNEL_A_MUTE;
}

INLINE void unmute() {
  DSOUNDCTRL = DSOUNDCTRL | CHANNEL_A_UNMUTE;
}

INLINE void turn_on_sound() {
  SETSNDRES(1);
  SNDSTAT = SNDSTAT_ENABLE;
  DSOUNDCTRL = 0xff0c; /*0b1111111100001100*/
  mute();
}

INLINE void init() {
  /* TMxCNT_L is count; TMxCNT_H is control */
  REG_TM0CNT_H = 0;
  REG_TM0CNT_L = 0x10000 - (924 / 2);        // 0x10000 - (16777216 / 36314)
  REG_TM0CNT_H = TIMER_16MHZ | TIMER_START;  //            cpuFreq  / sampleRate

  mute();
}

INLINE void stop() {
  mute();
  src = NULL;
  for (u32 i = 0; i < 2; i++) {
    u32* bufferPtr = (u32*)double_buffers[i];
    for (u32 j = 0; j < 608 / 4; j++)
      bufferPtr[j] = 0;
  }
}

INLINE void play(const char* name) {
  stop();

  src = gbfs_get_obj(fs, name, &src_len);
  src_pos = 0;
}

INLINE void disable_audio_dma() {
  // ----------------------------------------------------
  // This convoluted process was taken from the official manual.
  // It's supposed to disable DMA1 in a "safe" way, avoiding DMA lockups.
  //
  // 32-bit write
  // enabled = 1; start timing = immediately; transfer type = 32 bits;
  // repeat = off; destination = fixed; other bits = no change
  REG_DMA1CNT = (REG_DMA1CNT & 0xcdff /*0b00000000000000001100110111111111*/) |
                (0x0004 << 16) | DMA_ENABLE | DMA32 | DMA_DST_FIXED;
  //
  // wait 4 cycles
  asm volatile("eor r0, r0; eor r0, r0" ::: "r0");
  asm volatile("eor r0, r0; eor r0, r0" ::: "r0");
  //
  // 16-bit write
  // enabled = 0; start timing = immediately; transfer type = 32 bits;
  // repeat = off; destination = fixed; other bits = no change
  REG_DMA1CNT_H = (REG_DMA1CNT_H & 0x4dff /*0b0100110111111111*/) |
                  0x500 /*0b0000010100000000*/;  // DMA32 | DMA_DST_FIXED
  //
  // wait 4 more cycles
  asm volatile("eor r0, r0; eor r0, r0" ::: "r0");
  asm volatile("eor r0, r0; eor r0, r0" ::: "r0");
}

INLINE void dsound_start_audio_copy(const void* source) {
  // disable DMA1
  disable_audio_dma();

  // setup DMA1 for audio
  REG_DMA1SAD = (intptr_t)source;
  REG_DMA1DAD = (intptr_t)FIFO_ADDR_A;
  REG_DMA1CNT = DMA_DST_FIXED | DMA_SRC_INC | DMA_REPEAT | DMA32 | DMA_SPECIAL |
                DMA_ENABLE | 1;
}

INLINE void loadFile(const char* name) {
  play(name);
  is_looping = false;
}

CODE_ROM void player_sfx_init() {
  fs = find_first_gbfs_file(0);
  turn_on_sound();
  init();
}

CODE_ROM void player_sfx_unload() {
  disable_audio_dma();
}

CODE_ROM void player_sfx_play(const char* name) {
  loadFile(name);
}

CODE_ROM void player_sfx_setLoop(bool enable) {
  is_looping = enable;
}

CODE_ROM void player_sfx_stop() {
  stop();
  is_looping = false;
}

CODE_ROM bool player_sfx_isPlaying() {
  return src != NULL;
}

void player_sfx_onVBlank() {
  dsound_start_audio_copy(double_buffers[cur_buffer]);

  if (!did_run)
    return;

  if (src != NULL)
    unmute();

  cur_buffer = !cur_buffer;
  did_run = false;
}

void player_sfx_update() {
  // > audio processing (back buffer)
  AUDIO_PROCESS({
    if (is_looping) {
      src_pos = 0;
      did_run = false;
    } else
      player_sfx_stop();
  });
}
