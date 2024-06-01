#ifndef CROSS_H
#define CROSS_H

#include "../GameObject.h"

class Cross : public GameObject {
 public:
  Cross(bn::fixed_point initialPosition);

  bool update();

 private:
  bn::sprite_ptr sprite;
  int animationIndex = 0;
};

#endif  // CROSS_H
