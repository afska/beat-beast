#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "GameObject.h"

class Explosion : public GameObject {
 public:
  Explosion(bn::fixed_point position);

  bool update();

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<8> animation;
};

#endif  // EXPLOSION_H
