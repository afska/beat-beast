#ifndef HORSE_H
#define HORSE_H

#include "TopLeftGameObject.h"

class Horse : public TopLeftGameObject {
 public:
  Horse(bn::fixed_point initialPosition);

  void update();
  void bounce();
  void shoot();
  void jump();
  void hurt();
  void aim(bn::fixed_point newDirection);
  void setPosition(bn::fixed_point newPosition, bool isNowMoving);
  bn::fixed_point getPosition() { return topLeftPosition; }
  void setFlipX(bool flipX);
  bool isBusy() { return isJumping() || isHurt(); }
  bool isHurt() { return hurtAnimation.has_value(); }
  bn::fixed_point getShootingPoint();
  bn::fixed_point getShootingDirection();
  int getBounceFrame() { return bounceFrame; }

 private:
  bn::sprite_ptr gunSprite;
  bn::fixed targetAngle = 0;
  bool isMoving = false;

  bool isJumping() { return jumpingAnimation.has_value(); }
  void setIsMoving(bool isNowMoving);

  bn::optional<bn::sprite_animate_action<2>> idleAnimation;
  bn::optional<bn::sprite_animate_action<8>> runningAnimation;
  bn::optional<bn::sprite_animate_action<4>> jumpingAnimation;
  bn::optional<bn::sprite_animate_action<8>> hurtAnimation;
  bn::optional<bn::sprite_animate_action<8>> gunAnimation;
  int bounceFrame = 0;
  int jumpFrame = 0;
  int hurtFrame = 0;

  void updateAngle();

  void updateAnimations();
  void setIdleOrRunningState();
  void setIdleState();
  void setRunningState();
  void setJumpingState();
  void setHurtState();
  void resetAnimations();
};

#endif  // HORSE_H
