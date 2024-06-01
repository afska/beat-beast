#include "SaveFile.h"

BN_DATA_EWRAM SaveFile::SaveFileData SaveFile::data;

#define MAGIC_NUMBER 13579513

bool SaveFile::initialize() {
  load();

  if (data.magicNumber != MAGIC_NUMBER) {
    data.magicNumber = MAGIC_NUMBER;
    data.audioLag = 0;
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
