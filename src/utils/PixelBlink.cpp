#include "PixelBlink.h"

#include "bn_bgs_mosaic.h"
#include "bn_sprites_mosaic.h"

PixelBlink::PixelBlink(bn::fixed _targetValue) {
  this->targetValue = _targetValue;
}

void PixelBlink::blink() {
  step = 0;
  isBlinking = true;
}

bool PixelBlink::update() {
  if (!isBlinking && step == 0)
    return false;

  bool hasEnded = false;

  if (isBlinking) {
    step += 0.1;
    if (step >= targetValue) {
      step = targetValue;
      isBlinking = false;
      hasEnded = true;
    }
  } else {
    step -= 0.1;
    if (step < 0)
      step = 0;
  }

  bn::bgs_mosaic::set_stretch(step);
  bn::sprites_mosaic::set_stretch(step);

  return hasEnded;
}
