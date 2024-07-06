#ifndef COMBO_BAR_H
#define COMBO_BAR_H

#include "../TopLeftGameObject.h"

#define COMBO_MAX 30

class ComboBar : public TopLeftGameObject {
 public:
  ComboBar(bn::fixed_point _topLeftPosition);

  unsigned getCombo() { return combo; }
  void setCombo(unsigned _combo);
  bool isMaxedOut() { return combo == COMBO_MAX; }

  void update();
  void bump();
  void bounce();

 private:
  unsigned combo;
  int animationIndex = -1;
  int animationOffset = 0;
  unsigned animationWait = 0;
  bool animationFlag = false;

  void updateFill(unsigned visualCombo);
};

#endif  // COMBO_BAR_H
