#ifndef OCTOPUS_H
#define OCTOPUS_H

#include "../GameObject.h"
#include "Tentacle.h"
#include "Turntable.h"

class Octopus : public GameObject {
 public:
  Octopus(bn::fixed_point initialPosition);
  void update(bool isInsideBeat);
  void bounce();
  void setTargetPosition(bn::fixed_point newTargetPosition);

 private:
  bn::sprite_ptr sprite;
  bn::optional<bn::sprite_animate_action<5>> idleAnimation;
  bn::vector<bn::unique_ptr<Tentacle>, 8> tentacles;
  bn::vector<bn::unique_ptr<Turntable>, 4> turntables;
  bn::fixed_point targetPosition;

  void setPosition(bn::fixed_point newPosition);
  bn::fixed_point getPosition() { return sprite.position(); }

  void updateAnimations();
  void setIdleState();
  void resetAnimations();
  bn::sprite_animate_action<5> createIdleAnimation();
};

#endif  // OCTOPUS_H
