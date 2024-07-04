#ifndef SAVEFILE_H
#define SAVEFILE_H

#include "bn_sram.h"

namespace SaveFile {

struct __attribute__((__packed__)) SaveFileData {
  unsigned magicNumber;
  int audioLag;
  bool rumble;
  bool bgBlink;
  unsigned char intensity;
};

extern SaveFileData data;

bool initialize();
void load();
void save();
void wipe();

}  // namespace SaveFile

#endif  // SAVEFILE_H
