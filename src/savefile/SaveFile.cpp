#include "SaveFile.h"

#include "../../../butano/butano/hw/include/bn_hw_core.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../utils/Rumble.h"

BN_DATA_EWRAM SaveFile::SaveFileData SaveFile::data;

#define MAGIC_NUMBER 13579513

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
      data._padding1[i] = 0;

    data.selectedDifficultyLevel = 0;
    data.selectedLevel = 0;
    data.isInsideFinal = false;
    data.didFinishGame = false;
    for (int i = 0; i < 12; i++)
      data._padding2[i] = 0;

    for (int d = 0; d < TOTAL_DIFFICULTY_LEVELS; d++) {
      for (int l = 0; l < TOTAL_LEVELS; l++) {
        data.progress[d].levels[l].health = 0;
        data.progress[d].levels[l].damage = 0;
        data.progress[d].levels[l].sync = 0;
        data.progress[d].levels[l].wins = 0;
        data.progress[d].levels[l].deaths = 0;
      }
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
