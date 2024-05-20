#ifndef SAVEFILE_H
#define SAVEFILE_H

#include "bn_sram.h"

namespace SaveFile {

struct SaveFileData {
  unsigned magicNumber;
  int audioLag;
};

extern SaveFileData data;

bool initialize();
void load();
void save();

}  // namespace SaveFile

#endif  // SAVEFILE_H
