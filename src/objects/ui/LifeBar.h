#ifndef LIFE_BAR_H
#define LIFE_BAR_H

#include "../GameObject.h"

#define MAX_LIVES 5

class LifeBar : public GameObject {
 public:
  LifeBar(bn::fixed_point initialPosition);

  bool update();
  void addLife();
  void removeLife();

 private:
  bn::vector<bn::sprite_ptr, MAX_LIVES> sprites;
  int animationIndex = -1;
};

#endif  // LIFE_BAR_H
