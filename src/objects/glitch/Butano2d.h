#ifndef BUTANO_2D_H
#define BUTANO_2D_H

#include "../GameObject.h"

class Butano2d : public GameObject {
 public:
  Butano2d(bn::fixed_point initialPosition);

  bool update();
  void explode();
  void hurt();
  bool hasBeenHit() { return isExploding; }
  bn::fixed_point getPosition() { return sprite.position(); }
  void setPosition(bn::fixed_point position) { sprite.set_position(position); }

 private:
  bn::sprite_ptr sprite;
  int animationIndex = -1;
  bool isExploding = false;
  bool isDestroying = false;
  bn::optional<bn::sprite_animate_action<2>> idleAnimation;
  bn::optional<bn::sprite_animate_action<8>> hurtAnimation;
  bn::optional<bn::sprite_animate_action<2>> explodeAnimation;
  bn::fixed extraScale = 0;

  void updateAnimations();
  void setIdleState();
  void setHurtState();
  void setExplodingState();
  void resetAnimations();
};

#endif  // BUTANO_2D_H
