#ifndef OCTOPUS_H
#define OCTOPUS_H

#include "../GameObject.h"
#include "Tentacle.h"
#include "Turntable.h"

class Octopus : public GameObject {
 public:
  Octopus(bn::fixed_point initialPosition);
  void update(bool isInsideBeat);
  void bounce();
  void hurt();
  void attack();
  void setTargetPosition(bn::fixed_point newTargetPosition);
  bool isBusy() { return isHurt() || isAttacking(); }

 private:
  bn::sprite_ptr sprite;
  bn::optional<bn::sprite_animate_action<5>> idleAnimation;
  bn::optional<bn::sprite_animate_action<8>> hurtAnimation;
  bn::optional<bn::sprite_animate_action<5>> attackAnimation;
  bn::vector<bn::unique_ptr<Tentacle>, 8> tentacles;
  bn::vector<bn::unique_ptr<Turntable>, 4> turntables;
  bn::fixed_point targetPosition;

  bool isHurt() { return hurtAnimation.has_value(); }
  bool isAttacking() { return attackAnimation.has_value(); }
  void setPosition(bn::fixed_point newPosition);
  bn::fixed_point getPosition() { return sprite.position(); }

  void updateAnimations();
  void setIdleState();
  void setHurtState();
  void setAttackState();
  void resetAnimations();
};

#endif  // OCTOPUS_H
