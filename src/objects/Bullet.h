#ifndef BULLET_H
#define BULLET_H

#include "RhythmicBullet.h"

class Bullet : public RhythmicBullet {
 public:
  Bullet(bn::fixed_point initialPosition,
         bn::fixed_point _direction,
         bn::sprite_item _bullet);

  bn::fixed_point getPosition() { return sprite.position(); }
  bool update(int msecs,
              bool isInsideBeat,
              bn::fixed_point playerPosition) override;

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<2> animation;
  bn::fixed_point direction;
};

#endif  // BULLET_H
