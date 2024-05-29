#ifndef CROSS_H
#define CROSS_H

#include "../GameObject.h"

class Cross : GameObject {
 public:
  Cross(bn::fixed_point initialPositionn);

  bool update();

 private:
  bn::sprite_ptr sprite;
  int animationIndex = 0;
};

#endif  // CROSS_H
