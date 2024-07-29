#ifndef HEAD_H
#define HEAD_H

#include "../GameObject.h"

class Head : public GameObject {
 public:
  Head(bn::sprite_item _spriteItem,
       bn::fixed_point initialPosition,
       int _blinkDelay);

  void update();
  void blink();
  void talk();
  void stopTalking();
  void setPosition(bn::fixed_point newPosition) {
    sprite.set_position(newPosition);
  }
  bn::fixed_point getPosition() { return sprite.position(); }
  void disableMosaic() { sprite.set_mosaic_enabled(false); }
  void enableMosaic() { sprite.set_mosaic_enabled(true); }
  void setTargetScale(bn::fixed newTargetScale) {
    targetScale = newTargetScale;
  }
  void freeze() { setFreezeState(); }
  void hide() { sprite.set_visible(false); }

 private:
  bn::sprite_item spriteItem;
  bn::sprite_ptr sprite;
  int blinkDelay;
  bn::optional<bn::sprite_animate_action<2>> idleAnimation;
  bn::optional<bn::sprite_animate_action<3>> blinkAnimation;
  bn::optional<bn::sprite_animate_action<2>> talkAnimation;
  bn::optional<bn::sprite_animate_action<2>> freezeAnimation;
  int blinkWait = 0;
  bn::fixed targetScale = 1;

  bool isTalking() { return talkAnimation.has_value(); }

  void updateAnimations();
  void setIdleState();
  void setBlinkState();
  void setTalkState();
  void setFreezeState();
  void resetAnimations();
};

#endif  // HEAD_H
