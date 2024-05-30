#ifndef HORSE_H
#define HORSE_H

#include "GameObject.h"

class Horse : GameObject {
 public:
  Horse(bn::fixed_point initialPosition);

  void update();
  void bounce();
  void shoot();
  void jump();
  void aim(bn::fixed_point newDirection);
  void setPosition(bn::fixed_point newPosition, bool isNowMoving);
  void setFlipX(bool flipX);
  bn::fixed_point getPosition() { return position; }
  bn::fixed_point getCenteredPosition() { return mainSprite.position(); }
  bn::fixed_point getShootingPoint();
  bn::fixed_point getShootingDirection();
  int getBounceFrame() { return bounceFrame; }

 private:
  bn::sprite_ptr mainSprite;
  bn::sprite_ptr gunSprite;
  bn::fixed_point position;
  bn::fixed targetAngle = 0;
  bool isMoving = false;

  bool isJumping() { return jumpingAnimation.has_value(); }

  bn::optional<bn::sprite_animate_action<2>> idleAnimation;
  bn::optional<bn::sprite_animate_action<8>> runningAnimation;
  bn::optional<bn::sprite_animate_action<4>> jumpingAnimation;
  bn::optional<bn::sprite_animate_action<8>> gunAnimation;
  int bounceFrame = 0;
  int jumpFrame = 0;

  void updateAnimations();
  void updateAngle();

  void setIdleOrRunningState();
  void setIdleState();
  void setRunningState();
  void setJumpingState();

  void resetAnimations();
  bn::sprite_animate_action<2> createIdleAnimation();
  bn::sprite_animate_action<8> createRunningAnimation();
  bn::sprite_animate_action<4> createJumpingAnimation();
};

#endif  // HORSE_H
