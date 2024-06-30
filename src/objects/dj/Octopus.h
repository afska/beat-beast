#ifndef OCTOPUS_H
#define OCTOPUS_H

#include "../GameObject.h"
#include "Tentacle.h"
#include "Turntable.h"

class Octopus : public GameObject {
 public:
  Octopus(bn::fixed_point initialPosition);
  void update(bn::fixed_point playerPosition, bool isInsideBeat);
  void bounce();
  void hurt();
  void attack();
  void megaAttack();
  void setTargetPosition(bn::fixed_point newTargetPosition,
                         unsigned beatDurationMs);
  bn::fixed_point getShootingPoint() {
    return sprite.position() + bn::fixed_point(-18, 21);
  }
  bool isBusy() { return isHurt() || isAttacking(); }
  Turntable* getUpperTurntable() { return turntables[0].get(); }
  Turntable* getLowerTurntable() { return turntables[1].get(); }
  void spin();

 private:
  bn::sprite_ptr sprite;
  bn::optional<bn::sprite_animate_action<5>> idleAnimation;
  bn::optional<bn::sprite_animate_action<8>> hurtAnimation;
  bn::optional<bn::sprite_animate_action<3>> attackAnimation;
  bn::optional<bn::sprite_animate_action<20>> megaAttackAnimation;
  bn::vector<bn::unique_ptr<Tentacle>, 8> tentacles;
  bn::vector<bn::unique_ptr<Turntable>, 4> turntables;
  bn::fixed_point targetPosition;
  bn::fixed speedX = 1;
  bn::fixed speedY = 1;
  bool isSpinning = false;

  bool isHurt() { return hurtAnimation.has_value(); }
  bool isAttacking() { return attackAnimation.has_value(); }
  void setPosition(bn::fixed_point newPosition);
  bn::fixed_point getPosition() { return sprite.position(); }

  void updateAnimations();
  void setIdleState();
  void setHurtState();
  void setAttackState();
  void setMegaAttackState();
  void resetAnimations();
};

#endif  // OCTOPUS_H
