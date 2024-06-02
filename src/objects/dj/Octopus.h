#ifndef OCTOPUS_H
#define OCTOPUS_H

#include "../GameObject.h"

class Octopus : public GameObject {
 public:
  Octopus();
  void update();

 private:
  bn::sprite_ptr sprite;
  bn::optional<bn::sprite_animate_action<5>> idleAnimation;

  void updateAnimations();

  void setIdleState();

  void resetAnimations();
  bn::sprite_animate_action<5> createIdleAnimation();
};

#endif  // OCTOPUS_H
