#ifndef HORSE_H
#define HORSE_H

#include "GameObject.h"
#include "bn_fixed_point.h"

class Horse : GameObject {
 public:
  Horse(bn::fixed_point initialPosition);

  void update();
  void bounce();
  void aim(int direction);
  void setPosition(bn::fixed_point newPosition);
  void setFlipX(bool flipX);
  bn::fixed_point getPosition() { return position; }

 private:
  bn::sprite_ptr mainSprite;
  bn::sprite_ptr gunSprite;
  bn::fixed_point position;

  bn::sprite_animate_action<7> runAnimation;
  bn::optional<bn::sprite_animate_action<8>> gunAnimation;
  int bounceFrame = 0;

  void updateAnimations();
};

#endif  // HORSE_H
