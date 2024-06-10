#ifndef LIGHTNING_HINT_H
#define LIGHTNING_HINT_H

#include "../TopLeftGameObject.h"

class LightningHint : public TopLeftGameObject {
 public:
  LightningHint(bn::fixed_point _topLeftPosition);

  bool update();

 private:
  bn::sprite_ptr sprite2;
  bn::sprite_ptr sprite3;
  bn::sprite_animate_action<8> animation1;
  bn::sprite_animate_action<8> animation2;
  bn::sprite_animate_action<8> animation3;

  void setPosition(bn::fixed_point newPosition);
};
#endif  // LIGHTNING_HINT_H
