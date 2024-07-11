#ifndef LIFE_BAR_H
#define LIFE_BAR_H

#include "../TopLeftGameObject.h"
#include "LoopingCross.h"

class LifeBar : public TopLeftGameObject {
 public:
  LifeBar(bn::fixed_point _topLeftPosition,
          bn::fixed _maxLife,
          bn::sprite_item _icon,
          bn::sprite_item _fill);

  bn::fixed getLife() { return life; }
  bn::fixed getMaxLife() { return maxLife; }
  bn::fixed getExtraDamage() { return extraDamage.floor_integer(); }
  bool setLife(bn::fixed _life);

  void update();
  void bounce();

 private:
  bn::fixed maxLife;
  bn::sprite_ptr icon;
  bn::sprite_ptr fill;
  bn::fixed life;       // [0, maxLife]
  unsigned visualLife;  // [0, MAX_DIFFERENT_VALUES]
  int animationIndex = -1;
  int animationOffset = 0;
  unsigned animationWait = 0;
  bool animationFlag = false;
  bn::fixed_point defaultFillPosition;
  bn::optional<bn::unique_ptr<LoopingCross>> loopingCross;
  bn::fixed extraDamage = 0;

  void updateFill(unsigned drawLife);
};

#endif  // LIFE_BAR_H
