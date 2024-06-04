#ifndef TURNTABLE_H
#define TURNTABLE_H

#include "../GameObject.h"

class Turntable : public GameObject {
 public:
  Turntable(bn::fixed_point position);
  void update(bn::fixed_point playerPosition);
  void attack();
  void stopAttack();
  void addDamage();
  bn::sprite_ptr getSprite() { return sprite; }
  bool getIsAttacking() { return isAttacking; }

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<3> animation;
  bn::vector<bn::sprite_ptr, 4> damageSprites;
  bn::vector<bn::sprite_animate_action<6>, 4> damageSpriteAnimations;
  bool isAttacking = false;
};

#endif  // TURNTABLE_H
