#include "SaveFile.h"

#include "../../../butano/butano/hw/include/bn_hw_core.h"
#include "../player/player.h"
#include "../player/player_sfx.h"
#include "../utils/Rumble.h"

BN_DATA_EWRAM SaveFile::SaveFileData SaveFile::data;

#define MAGIC_NUMBER 13579513

bool SaveFile::initialize() {
  load();

  if (data.magicNumber != MAGIC_NUMBER) {
    data.magicNumber = MAGIC_NUMBER;
    data.audioLag = 0;
    data.rumble = true;
    data.bgBlink = false;
    data.intensity = 0;

    save();

    return true;
  } else
    return false;
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
