#ifndef HORSE_H
#define HORSE_H

#include "TopLeftGameObject.h"

#define HORSE_SPEED 1.5
#define HORSE_JUMP_SPEEDX_BONUS 1.25

class Horse : public TopLeftGameObject {
 public:
  bool showGun = true;
  bool canShoot = false;
  bool fakeJump = true;
  bool customScale = false;

  Horse(bn::fixed_point _topLeftPosition);

  void update();
  void bounce();
  void shoot();
  void jump();
  void hurt();
  void aim(bn::fixed_point newDirection);
  void setPosition(bn::fixed_point newPosition, bool isNowMoving);
  bn::fixed_point getPosition() { return topLeftPosition; }
  void setFlipX(bool flipX);
  bool getFlipX() { return mainSprite.horizontal_flip(); }
  bool isBusy() { return isJumping() || isHurt(); }
  bool isJumping() { return jumpingAnimation.has_value(); }
  bool isHurt() { return hurtAnimation.has_value(); }
  bool getIsMoving() { return isMoving; }
  bn::fixed_point getShootingPoint();
  bn::fixed_point getShootingDirection();
  bool canReallyShoot() { return canShoot && shootCooldown == 0; }
  bool failShoot() {
    if (shootCooldown == 0) {
      shootCooldown = 30;
      return true;
    }
    return false;
  }
  int getBounceFrame() { return bounceFrame; }
  void enableAlternativeRunAnimation() {
    useAlternativeRunAnimation = true;
    if (isMoving) {
      setIsMoving(false);
      setIsMoving(true);
    }
  }
  void clearCustomRunTiles() { useAlternativeRunAnimation = false; }
  void disappearInto(bn::fixed_point _disappearPosition) {
    disappearPosition = _disappearPosition;
  }
  void setIdleOrRunningState();

 private:
  bn::sprite_ptr gunSprite;
  bn::fixed targetAngle = 0;
  bool isMoving = false;
  bn::optional<bn::fixed_point> disappearPosition;

  bn::optional<bn::sprite_animate_action<2>> idleAnimation;
  bn::optional<bn::sprite_animate_action<8>> runningAnimation;
  bn::optional<bn::sprite_animate_action<4>> jumpingAnimation;
  bn::optional<bn::sprite_animate_action<8>> hurtAnimation;
  bn::optional<bn::sprite_animate_action<8>> gunAnimation;
  bool useAlternativeRunAnimation = false;
  int bounceFrame = 0;
  int jumpFrame = 0;
  int hurtFrame = 0;
  int shootCooldown = 0;

  void setIsMoving(bool isNowMoving);

  void updateAngle();

  void updateAnimations();
  void setIdleState();
  void setRunningState();
  void setJumpingState();
  void setHurtState();
  void resetAnimations();
};

#endif  // HORSE_H
