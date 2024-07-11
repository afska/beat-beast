#ifndef BULLET_H
#define BULLET_H

#include "RhythmicBullet.h"

#define BULLET_OFF_BEAT_SPEED 1.5
#define BULLET_ON_BEAT_SPEED 2.5
#define BULLET_BONUS_DMG 0.25

class Bullet : public RhythmicBullet {
 public:
  Bullet(bn::fixed_point initialPosition,
         bn::fixed_point _direction,
         bn::sprite_item _bullet,
         bn::fixed dmg = 1,
         bn::fixed scale = 1,
         bn::fixed _offBeatSpeed = BULLET_OFF_BEAT_SPEED,
         bn::fixed _onBeatSpeed = BULLET_ON_BEAT_SPEED);

  bn::fixed_point getPosition() { return sprite.position(); }
  bool update(int msecs,
              bool isInsideBeat,
              bn::fixed_point playerPosition) override;

 private:
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<2> animation;
  bn::fixed_point direction;
  bn::fixed offBeatSpeed;
  bn::fixed onBeatSpeed;
};

#endif  // BULLET_H
