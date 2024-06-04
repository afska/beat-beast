#ifndef TURNTABLE_H
#define TURNTABLE_H

#include "../GameObject.h"

class Turntable : public GameObject {
 public:
  Turntable(bn::fixed_point position);
  void update(bn::fixed_point playerPosition);
  void attack();
  void stopAttack();
  void setPosition(bn::fixed_point newPosition) {
    sprite.set_position(newPosition);
  }
  bool getIsAttacking() { return isAttacking; }

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<3> animation;
  bool isAttacking = false;
};

#endif  // TURNTABLE_H
