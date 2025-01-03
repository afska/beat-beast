#ifndef RIFFER_H
#define RIFFER_H

#include "../TopLeftGameObject.h"

class Riffer : public TopLeftGameObject {
 public:
  Riffer(bn::fixed_point initialPosition);

  bool update(bn::fixed_point playerPosition, bool isInsideBeat);
  void bounce();
  void hurt();
  void swing();
  void swingEnd();
  void breakGuitar();
  void headbang();
  void setAngryHands();
  void unsetAngryHands();
  void startThrow();
  bool getAngryHands() { return hasAngryHands; }
  bn::optional<bn::sprite_ptr> getBrokenGuitar1() {
    if (!isThrowing)
      return bn::optional<bn::sprite_ptr>{};
    return brokenGuitar1;
  }
  void resetBrokenGuitar1() { brokenGuitar1.reset(); }
  void resetBrokenGuitar2() {
    isThrowing = false;
    brokenGuitar2.reset();
  }
  void recoverGuitar();
  void setTargetPosition(bn::fixed_point newTargetPosition,
                         unsigned beatDurationMs);
  bn::fixed_point getShootingPoint() {
    return mainSprite.position() + bn::fixed_point(-18, 36);
  }
  bool isBusy() { return isHurt() || isHeadbanging(); }
  bool isHurt() { return hurtAnimation.has_value(); }
  void setCamera(bn::camera_ptr camera) {
    mainSprite.set_camera(camera);
    handL.set_camera(camera);
    handR.set_camera(camera);
    guitar->set_camera(camera);
  }
  void spin();
  bool didFinalSpinEnd() {
    return isSpinning && mainSprite.position().y() > 120;
  }

 private:
  bn::optional<bn::sprite_ptr> guitar;
  bn::sprite_ptr handL;
  bn::sprite_ptr handR;
  bn::optional<bn::sprite_ptr> brokenGuitar1;
  bn::optional<bn::sprite_ptr> brokenGuitar2;
  bn::optional<bn::sprite_animate_action<2>> idleAnimation;
  bn::optional<bn::sprite_animate_action<8>> hurtAnimation;
  bn::optional<bn::sprite_animate_action<2>> headbangAnimation;
  bn::fixed_point targetPosition;
  bn::fixed speedX = 1;
  bn::fixed speedY = 1;
  int animationIndex = -1;
  int swingAnimationIndex = -1;
  int handLAnimationIndex = -1;
  int handRAnimationIndex = -1;
  int angryHandsAnimationIndex = -1;
  int brokenGuitarShakeAnimationIndex = -1;
  int headbangAnimationIndex = -1;
  bool waitingSwingEnd = false;
  bool hasAngryHands = false;
  bool isThrowing = false;
  bool isHeadbangingNow = false;
  bool isSpinning = false;

  bool isHeadbanging() { return headbangAnimation.has_value(); }

  void updateSubsprites(bn::fixed_point playerPosition);
  void updateAnimations();
  void setIdleState();
  void setHurtState();
  void setAttackState();
  void resetAnimations();
};

#endif  // RIFFER_H
