#ifndef CERBERUS_H
#define CERBERUS_H

#include "../TopLeftGameObject.h"

class Cerberus : public TopLeftGameObject {
 public:
  Cerberus(bn::fixed_point initialPosition);

  bool update(bn::fixed_point playerPosition, bool isInsideBeat);
  void bounce();
  void setTargetPosition(bn::fixed_point newTargetPosition,
                         unsigned beatDurationMs);
  void disableMosaic() {
    mainSprite.set_mosaic_enabled(false);
    secondarySprite.set_mosaic_enabled(false);
  }

  void enableMosaic() {
    mainSprite.set_mosaic_enabled(true);
    secondarySprite.set_mosaic_enabled(true);
  }

 private:
  bn::sprite_ptr secondarySprite;
  bn::fixed_point targetPosition;
  bn::fixed speedX = 1;
  bn::fixed speedY = 1;

  void updateSubsprites(bn::fixed_point playerPosition);

  // TODO: ANIMATIONS
};

#endif  // CERBERUS_H
