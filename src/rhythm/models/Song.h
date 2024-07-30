#ifndef SONG_H
#define SONG_H

#include <tonc_core.h>

#include "../../utils/gbfs/gbfs.h"
#include "Chart.h"
#include "Event.h"
#include "parse.h"

#include "bn_string.h"

typedef struct {
  u8 bpm;
  u8 tickcount;
  u32 duration;
  u32 oneDivBeatDurationMs;
  u8 chartCount;
  Chart charts[3];
} Song;

Song SONG_parse(const GBFS_FILE* fs,
                bn::string<32> fileName,
                DifficultyLevel difficultyLevel);
Chart SONG_findChartByDifficultyLevel(Song song,
                                      DifficultyLevel difficultyLevel);

#endif  // SONG_H
