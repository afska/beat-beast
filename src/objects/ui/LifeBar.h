#ifndef LIFE_BAR_H
#define LIFE_BAR_H

#include "../TopLeftGameObject.h"
#include "LoopingCross.h"

class LifeBar : public TopLeftGameObject {
 public:
  LifeBar(bn::fixed_point _topLeftPosition,
          unsigned _maxLife,
          bn::sprite_item _icon,
          bn::sprite_item _fill);

  unsigned getLife() { return life; }
  unsigned getMaxLife() { return maxLife; }
  bool setLife(unsigned _life);

  void update();
  void bounce();

 private:
  unsigned maxLife;
  bn::sprite_ptr icon;
  bn::sprite_ptr fill;
  unsigned life;        // [0, maxLife]
  unsigned visualLife;  // [0, MAX_DIFFERENT_VALUES]
  int animationIndex = -1;
  int animationOffset = 0;
  unsigned animationWait = 0;
  bool animationFlag = false;
  bn::fixed_point defaultFillPosition;
  bn::optional<bn::unique_ptr<LoopingCross>> loopingCross;

  void updateFill(unsigned visualLife);
};

#endif  // LIFE_BAR_H
