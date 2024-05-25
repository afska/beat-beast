#ifndef HORSE_H
#define HORSE_H

#include "GameObject.h"
#include "bn_fixed_point.h"

class Horse : GameObject {
 public:
  Horse(bn::fixed_point initialPosition);

  void update();
  void bounce();
  void shoot();
  void aim(bn::fixed_point newDirection);
  void setPosition(bn::fixed_point newPosition);
  void setFlipX(bool flipX);
  bn::fixed_point getPosition() { return position; }

 private:
  bn::sprite_ptr mainSprite;
  bn::sprite_ptr gunSprite;
  bn::fixed_point position;
  bn::fixed targetAngle = 0;

  bn::sprite_animate_action<8> runAnimation;
  bn::optional<bn::sprite_animate_action<8>> gunAnimation;
  int bounceFrame = 0;

  void updateAnimations();
  void updateAngle();
};

#endif  // HORSE_H
