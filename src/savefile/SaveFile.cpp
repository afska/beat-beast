#include "SaveFile.h"

#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../utils/Rumble.h"

#include "../../../butano/butano/hw/include/bn_hw_core.h"

BN_DATA_EWRAM SaveFile::SaveFileData SaveFile::data;

#define MAGIC_NUMBER 135795135

bool SaveFile::initialize() {
  load();

  if (!isValid()) {
    data.magicNumber = MAGIC_NUMBER;
    data.audioLag = 0;
    data.didCalibrate = false;
    data.rumble = true;
    data.bgBlink = false;
    data.intensity = 0;
    data.contrast = 0;
    for (int i = 0; i < 3; i++)
      data._padding1[i] = 0xEE;

    data.selectedDifficultyLevel = 1;
    data.selectedLevel = 0;
    data.isInsideFinal = false;
    data.didFinishGame = false;
    data.didFinishImpossible = false;
    for (int i = 0; i < 11; i++)
      data._padding2[i] = 0xEE;

    for (int d = 0; d < TOTAL_DIFFICULTY_LEVELS; d++) {
      for (int l = 0; l < TOTAL_LEVELS; l++) {
        data.progress[d].levels[l].health = 0;
        data.progress[d].levels[l].damage = 0;
        data.progress[d].levels[l].sync = 0;

        data.progress[d].levels[l].didWin = false;
        data.progress[d].levels[l].wins = 0;
        data.progress[d].levels[l].deaths = 0;
      }
      for (int i = 0; i < 3; i++)
        data.progress[d]._padding[i] = 0xEE;
    }

    save();

    return true;
  } else
    return false;
}

bool SaveFile::isValid() {
  if (data.magicNumber != MAGIC_NUMBER)
    return false;
  if (data.audioLag < 0 || data.audioLag > 500)
    return false;
  if (data.intensity >= 5 || data.contrast >= 5)
    return false;
  if (data.selectedDifficultyLevel >= TOTAL_DIFFICULTY_LEVELS)
    return false;
  if (data.selectedLevel >= TOTAL_LEVELS)
    return false;

  return true;
}

bool SaveFile::didCompleteTutorial() {
  return data.progress[1].levels[0].didWin;
}

bool SaveFile::hasUnlockedFinal(int difficultyLevel) {
  for (int i = 0; i < TOTAL_LEVELS - 1; i++) {
    if (!data.progress[difficultyLevel].levels[i].didWin)
      return false;
  }
  return true;
}

void SaveFile::load() {
  bn::sram::read(data);
}

void SaveFile::save() {
  bn::sram::write(data);
}

void SaveFile::wipe() {
  data.magicNumber = 1;
  save();

  player_stop();
  player_sfx_stop();
  RUMBLE_stop();

  bn::hw::core::reset();
}
