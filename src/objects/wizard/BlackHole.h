#ifndef BLACK_HOLE_H
#define BLACK_HOLE_H

#include "../GameObject.h"

class BlackHole : public GameObject {
 public:
  BlackHole(bn::fixed_point initialPosition,
            bn::fixed _initialScale = 0.25,
            bn::fixed _scaleInSpeed = 0.00225);

  bool update();
  bn::fixed_point getPosition() { return sprite.position(); }
  void setPosition(bn::fixed_point position) { sprite.set_position(position); }
  void goAway();
  bn::optional<bn::fixed_point> getTargetPosition() { return targetPosition; }
  void setTargetPosition(bn::fixed_point _targetPosition);
  void disappear() { isDisappearing = true; }

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<6> animation;
  bn::fixed scaleInSpeed;
  bn::optional<bn::fixed_point> targetPosition;
  bn::fixed scale;
  bn::fixed targetScale = 1.9;
  int animationIndex = -1;
  bool isGoingAway = false;
  bool isMoving = false;
  bool isDisappearing = false;
};

#endif  // BLACK_HOLE_H
