#ifndef PIXEL_BLINK_H
#define PIXEL_BLINK_H

#include "bn_fixed.h"

class PixelBlink {
 public:
  PixelBlink(bn::fixed _targetValue);

  void blink();

  bool update();

 private:
  bn::fixed targetValue;
  bn::fixed step = 0;
  bool isBlinking = false;
};

#endif  // PIXEL_BLINK_H
