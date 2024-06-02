#ifndef LIFE_BAR_H
#define LIFE_BAR_H

#include "../TopLeftGameObject.h"

#define MAX_LIFE 20

class LifeBar : public TopLeftGameObject {
 public:
  LifeBar(bn::fixed_point initialPosition,
          bn::sprite_item _icon,
          bn::sprite_item _fill);

  unsigned getLife() { return life; }
  void setLife(unsigned _life);

  void update();
  void bounce();

 private:
  bn::sprite_ptr icon;
  bn::sprite_ptr fill;
  unsigned life = MAX_LIFE;
  int animationIndex = -1;
  int animationOffset = 0;
  unsigned animationWait = 0;
  bool animationFlag = false;
  bn::fixed_point defaultFillPosition;

  void updateFill(unsigned visualLife);
};

#endif  // LIFE_BAR_H
