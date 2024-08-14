#ifndef COMBO_BAR_H
#define COMBO_BAR_H

#include "../TopLeftGameObject.h"

class ComboBar : public TopLeftGameObject {
 public:
  ComboBar(bn::fixed_point _topLeftPosition,
           unsigned _maxCombo = 10  // HACK: DON'T CHANGE THIS
  );

  void relocate(bn::fixed_point _topLeftPosition);
  unsigned getCombo() { return combo; }
  void setCombo(unsigned _combo);
  bool isMaxedOut() { return combo == maxCombo; }

  void update();
  void bump();
  void bounce();

 private:
  unsigned combo;
  unsigned maxCombo;
  unsigned divisor;
  int animationIndex = -1;
  int animationOffset = 0;
  unsigned animationWait = 0;
  int animatedFlag = 1;

  void updateFill(unsigned visualCombo);
};

#endif  // COMBO_BAR_H
