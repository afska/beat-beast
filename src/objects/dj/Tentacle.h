#ifndef TENTACLE_H
#define TENTACLE_H

#include "../GameObject.h"

class Tentacle : public GameObject {
 public:
  Tentacle(bn::fixed_point position, bn::fixed angle, bool flipX);
  void update();
  void setPosition(bn::fixed_point newPosition) {
    sprite.set_position(newPosition);
  }

 private:
  bn::sprite_ptr sprite;
  bn::optional<bn::sprite_animate_action<8>> idleAnimation;

  void updateAnimations();

  void setIdleState();

  void resetAnimations();
  bn::sprite_animate_action<8> createIdleAnimation();
};

#endif  // TENTACLE_H
