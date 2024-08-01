#ifndef SAVEFILE_H
#define SAVEFILE_H

#include "bn_sram.h"

namespace SaveFile {

const int TOTAL_LEVELS = 5;
const int TOTAL_DIFFICULTY_LEVELS = 4;

struct __attribute__((__packed__)) LevelProgress {
  unsigned char health;
  unsigned short damage;
  unsigned char sync;

  bool didWin;
  unsigned short wins;
  unsigned short deaths;
};

struct __attribute__((__packed__)) GameProgress {
  LevelProgress levels[TOTAL_LEVELS];
  unsigned char _padding[3];
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
  bool didFinishImpossible;
  unsigned char _padding2[11];

  // progress
  GameProgress progress[TOTAL_DIFFICULTY_LEVELS];
};

extern SaveFileData data;

bool initialize();
bool isValid();
bool didCompleteTutorial();
bool hasUnlockedFinal(int difficultyLevel);
void load();
void save();
void wipe();

}  // namespace SaveFile

#endif  // SAVEFILE_H
