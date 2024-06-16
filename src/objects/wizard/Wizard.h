#ifndef WIZARD_H
#define WIZARD_H

#include "../GameObject.h"

class Wizard : public GameObject {
 public:
  Wizard(bn::fixed_point initialPosition);

  void bounce();
  bool update(bn::fixed_point playerPosition, bool isInsideBeat);
  void setTargetPosition(bn::fixed_point newTargetPosition,
                         unsigned beatDurationMs);

 private:
  bn::sprite_ptr sprite;
  bn::fixed_point targetPosition;
  bn::fixed speedX = 1;
  bn::fixed speedY = 1;
  int animationIndex = -1;
};

#endif  // WIZARD_H
