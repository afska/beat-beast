#ifndef ALLY_DRAGON_H
#define ALLY_DRAGON_H

#include "../GameObject.h"
#include "../Horse.h"

class AllyDragon : public GameObject {
 public:
  AllyDragon(bn::fixed_point initialPosition);

  bool update(Horse* horse);
  void flap();
  void megaflap();
  bool isReady() { return _isReady; }
  void setPosition(bn::fixed_point newPosition) {
    sprite.set_position(newPosition);
  }
  bn::fixed_point getPosition() { return sprite.position(); }
  void disappearInto(bn::fixed_point _disappearPosition) {
    disappearPosition = _disappearPosition;
  }
  void stopFalling() { _stopFalling = true; }

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<9> animation;
  bn::fixed xSpeed = 1.5;
  bn::fixed gravity = 0.2;
  bn::fixed flapForce = 4;
  bn::fixed velocityY = 0;
  bn::fixed scale = 1;
  bool isFlapping = false;
  bool _isReady = false;
  bool _stopFalling = false;
  bn::optional<bn::fixed_point> disappearPosition;
};

#endif  // ALLY_DRAGON_H
