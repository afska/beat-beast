#ifndef BULLET_H
#define BULLET_H

#include "GameObject.h"

class Bullet : public GameObject {
 public:
  Bullet(bn::fixed_point initialPosition, bn::fixed_point normalizedDirection);

  bool update(bool isInsideBeat);

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<2> animation;
  bn::fixed_point direction;
};

#endif  // BULLET_H
