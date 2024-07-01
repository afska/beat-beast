#ifndef CHECK_H
#define CHECK_H

#include "../GameObject.h"

class Check : public GameObject {
 public:
  Check(bn::fixed_point initialPosition);

  bool update();

 private:
  bn::sprite_ptr sprite;
  int animationIndex = 0;
};

#endif  // CHECK_H
