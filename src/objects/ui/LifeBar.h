#ifndef LIFE_BAR_H
#define LIFE_BAR_H

#include "../GameObject.h"

// (LifeBar uses the top-left system internally)

class LifeBar : public GameObject {
 public:
  LifeBar(bn::fixed_point initialPosition);

  bool update();
  void addLife();
  void removeLife();

 private:
  bn::sprite_ptr icon;
  bn::sprite_ptr border;
  bn::sprite_ptr fill;
  int animationIndex = 0;
};

#endif  // LIFE_BAR_H
