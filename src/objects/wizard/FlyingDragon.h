#ifndef FLYING_DRAGON_H
#define FLYING_DRAGON_H

#include "../GameObject.h"

#include "../../rhythm/models/Event.h"

class FlyingDragon : public GameObject {
 public:
  FlyingDragon(bn::fixed_point initialPosition,
               Event* _event,
               bn::fixed _xSpeed = 1.5,
               bn::fixed _gravity = 0.2,
               bn::fixed _flapForce = 2.5);

  bool isExploding() { return hurtAnimation.has_value(); }
  bool update(int msecs, bool isInsideBeat);
  void explode();

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<5> animation;
  bn::optional<bn::sprite_animate_action<2>> hurtAnimation;
  Event* event;
  bn::fixed xSpeed;
  bn::fixed gravity;
  bn::fixed flapForce;
  bn::fixed velocityY = 0;
  bn::fixed scale = 1;
  bool isFlapping = false;
};

#endif  // FLYING_DRAGON_H
