#ifndef WIZARD_H
#define WIZARD_H

#include "../GameObject.h"

class Wizard : public GameObject {
 public:
  Wizard(bn::fixed_point initialPosition);

  bool update(bn::fixed_point playerPosition, bool isInsideBeat);
  void bounce();
  void hurt();
  void attack();
  void setTargetPosition(bn::fixed_point newTargetPosition,
                         unsigned beatDurationMs);
  bn::fixed_point getShootingPoint() {
    return sprite.position() + bn::fixed_point(0, 0);
  }
  bool isBusy() { return isHurt() || isAttacking(); }
  bool isHurt() { return hurtAnimation.has_value(); }
  void disappearInto(bn::fixed_point _disappearPosition) {
    disappearPosition = _disappearPosition;
  }

 private:
  bn::sprite_ptr sprite;
  bn::optional<bn::sprite_animate_action<2>> idleAnimation;
  bn::optional<bn::sprite_animate_action<8>> hurtAnimation;
  bn::optional<bn::sprite_animate_action<2>> attackAnimation;
  bn::fixed_point targetPosition;
  bn::optional<bn::fixed_point> disappearPosition;
  bn::fixed speedX = 1;
  bn::fixed speedY = 1;
  int animationIndex = -1;

  bool isAttacking() { return attackAnimation.has_value(); }

  void updateAnimations();
  void setIdleState();
  void setHurtState();
  void setAttackState();
  void resetAnimations();
};

#endif  // WIZARD_H
