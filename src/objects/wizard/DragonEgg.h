#ifndef DRAGON_EGG_H
#define DRAGON_EGG_H

#include "../GameObject.h"

class DragonEgg : public GameObject {
 public:
  DragonEgg(bn::fixed_point initialPosition);

  bool update();
  void explode();
  bn::fixed_point getPosition() { return sprite.position(); }
  void setPosition(bn::fixed_point position) { sprite.set_position(position); }

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<2> animation;
  int animationIndex = -1;
  bool didExplode = false;
};

#endif  // DRAGON_EGG_H
