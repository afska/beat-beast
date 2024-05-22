#ifndef HORSE_H
#define HORSE_H

#include "GameObject.h"

class Horse : GameObject {
 public:
  Horse(bn::fixed initialX, bn::fixed initialY);

  void update();
  void bounce();
  void setPosition(bn::fixed x, bn::fixed y);

 private:
  bn::sprite_ptr mainSprite;
  bn::sprite_ptr gunSprite;
  bn::sprite_ptr ammoSprite;
  bn::fixed x;
  bn::fixed y;

  bn::sprite_animate_action<7> runAnimation;
  bn::optional<bn::sprite_animate_action<7>> gunAnimation;
  bn::optional<bn::sprite_animate_action<3>> ammoAnimation;
  int bounceFrame = 0;

  void updateAnimations();
};

#endif  // HORSE_H
