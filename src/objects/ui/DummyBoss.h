#ifndef DUMMY_BOSS_H
#define DUMMY_BOSS_H

#include "../GameObject.h"

class DummyBoss : public GameObject {
 public:
  DummyBoss(bn::fixed_point initialPosition);

  bool update();
  void explode();
  void hurt();
  bool hasBeenHit() { return isExploding; }
  bn::fixed_point getPosition() { return sprite.position(); }
  void setPosition(bn::fixed_point position) { sprite.set_position(position); }

 private:
  bn::sprite_ptr sprite;
  int animationIndex = -1;
  bool isAppearing = true;
  bool isExploding = false;
  bool isDestroying = false;
  bn::optional<bn::sprite_animate_action<2>> idleAnimation;
  bn::optional<bn::sprite_animate_action<8>> hurtAnimation;

  void updateAnimations();
  void setIdleState();
  void setHurtState();
  void resetAnimations();
};

#endif  // DUMMY_BOSS_H
