#ifndef LIGHTNING_H
#define LIGHTNING_H

#include "../TopLeftGameObject.h"

class Lightning : public TopLeftGameObject {
 public:
  Lightning(bn::fixed_point _topLeftPosition);

  bool update();

 private:
  bn::sprite_ptr sprite2;
  bn::sprite_ptr sprite3;
  bn::sprite_animate_action<8> animation1;
  bn::sprite_animate_action<8> animation2;
  bn::sprite_animate_action<8> animation3;

  void setPosition(bn::fixed_point newPosition);
};
#endif  // LIGHTNING_H
