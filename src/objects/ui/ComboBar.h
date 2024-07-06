#ifndef COMBO_BAR_H
#define COMBO_BAR_H

#include "../TopLeftGameObject.h"

class ComboBar : public TopLeftGameObject {
 public:
  ComboBar(bn::fixed_point _topLeftPosition,
           unsigned _maxCombo = 30,
           unsigned _divisor = 2);

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
  bool animationFlag = false;

  void updateFill(unsigned visualCombo);
};

#endif  // COMBO_BAR_H
