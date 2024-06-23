#ifndef AUTO_FIRE_H
#define AUTO_FIRE_H

#include "../TopLeftGameObject.h"

class AutoFire : public TopLeftGameObject {
 public:
  AutoFire(bn::fixed_point _topLeftPosition);

  void update();

 private:
  bn::sprite_animate_action<2> animation;
};

#endif  // AUTO_FIRE_H
