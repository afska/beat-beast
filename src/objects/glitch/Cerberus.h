#ifndef CERBERUS_H
#define CERBERUS_H

#include "../TopLeftGameObject.h"

class Cerberus : public TopLeftGameObject {
 public:
  Cerberus(bn::fixed_point initialPosition);

  bool update();
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
  bn::sprite_animate_action<3> mainAnimation;
  bn::sprite_animate_action<3> secondaryAnimation;

  bn::fixed_point targetPosition;
  bn::fixed speedX = 1;
  bn::fixed speedY = 1;

  void updateSubsprites();
};

#endif  // CERBERUS_H
