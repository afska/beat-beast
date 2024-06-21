#ifndef ALLY_DRAGON_H
#define ALLY_DRAGON_H

#include "../GameObject.h"

class AllyDragon : public GameObject {
 public:
  AllyDragon(bn::fixed_point initialPosition);

  bool update(bn::fixed_point playerPosition);
  bool isReady() { return _isReady; }

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<9> animation;
  bn::fixed xSpeed = 1.5;
  bn::fixed gravity = 0.2;
  bn::fixed flapForce = 2.5;
  bn::fixed velocityY = 0;
  bn::fixed scale = 1;
  bool isFlapping = false;
  bool _isReady = false;
};

#endif  // ALLY_DRAGON_H
