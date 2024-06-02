#ifndef TURNTABLE_H
#define TURNTABLE_H

#include "../GameObject.h"

class Turntable : public GameObject {
 public:
  Turntable(bn::fixed_point position);
  void update();

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<3> animation;
};

#endif  // TURNTABLE_H
