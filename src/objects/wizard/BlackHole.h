#ifndef BLACK_HOLE_H
#define BLACK_HOLE_H

#include "../GameObject.h"

class BlackHole : public GameObject {
 public:
  BlackHole(bn::fixed_point initialPosition);

  bool update();
  bn::fixed_point getPosition() { return sprite.position(); }
  void setPosition(bn::fixed_point position) { sprite.set_position(position); }

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<6> animation;
  bn::fixed scale = 0.05;
  int animationIndex = -1;
};

#endif  // BLACK_HOLE_H
