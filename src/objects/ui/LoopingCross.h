#ifndef LOOPING_CROSS_H
#define LOOPING_CROSS_H

#include "../GameObject.h"

class LoopingCross : public GameObject {
 public:
  LoopingCross(bn::fixed_point initialPosition);

  void update();

 private:
  bn::sprite_ptr sprite;
  int animationIndex = 0;
};

#endif  // LOOPING_CROSS_H
