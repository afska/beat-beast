#ifndef RIFFER_H
#define RIFFER_H

#include "../TopLeftGameObject.h"

class Riffer : public TopLeftGameObject {
 public:
  enum HandAnimation { PLAYING, ROCKING };

  Riffer(bn::fixed_point initialPosition);

  bool update(bn::fixed_point playerPosition, bool isInsideBeat);
  void bounce();
  void hurt();
  void swing();
  void swingEnd();
  void breakGuitar();
  void headbang();
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
    guitar.set_camera(camera);
  }

 private:
  bn::sprite_ptr guitar;
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
  int brokenGuitarShakeAnimationIndex = -1;
  bool waitingSwingEnd = false;
  HandAnimation handAnimation = PLAYING;

  bool isHeadbanging() { return headbangAnimation.has_value(); }

  void updateSubsprites();
  void updateAnimations();
  void setIdleState();
  void setHurtState();
  void setAttackState();
  void resetAnimations();
};

#endif  // RIFFER_H
