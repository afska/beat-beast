#ifndef SAVEFILE_H
#define SAVEFILE_H

#include "bn_sram.h"

namespace SaveFile {

const int TOTAL_LEVELS = 4;
const int TOTAL_DIFFICULTY_LEVELS = 3;

struct __attribute__((__packed__)) LevelProgress {
  unsigned char health;
  unsigned short damage;
  unsigned char sync;
  unsigned short wins;
  unsigned short deaths;
};

struct __attribute__((__packed__)) GameProgress {
  LevelProgress levels[TOTAL_LEVELS];
};

struct __attribute__((__packed__)) SaveFileData {
  // settings
  unsigned magicNumber;
  int audioLag;
  bool didCalibrate;
  bool rumble;
  bool bgBlink;
  unsigned char intensity;
  unsigned char contrast;
  unsigned char _padding1[3];

  // state
  unsigned char selectedDifficultyLevel;
  unsigned char selectedLevel;
  bool isInsideFinal;
  bool didFinishGame;
  unsigned char _padding2[12];

  // progress
  GameProgress progress[TOTAL_DIFFICULTY_LEVELS];
};

extern SaveFileData data;

bool initialize();
bool isValid();
void load();
void save();
void wipe();

}  // namespace SaveFile

#endif  // SAVEFILE_H
