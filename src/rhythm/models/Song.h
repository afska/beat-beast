#ifndef SONG_H
#define SONG_H

#include <tonc_core.h>
#include "bn_string.h"

#include "Chart.h"
#include "Event.h"
#include "parse.h"

#include "../../utils/gbfs/gbfs.h"

typedef struct {
  u8 bpm;
  u8 tickcount;
  u8 chartCount;
  Chart charts[3];
} Song;

Song SONG_parse(const GBFS_FILE* fs, bn::string<32> fileName);
Chart SONG_findChartByDifficultyLevel(Song song,
                                      DifficultyLevel difficultyLevel);

#endif  // SONG_H
