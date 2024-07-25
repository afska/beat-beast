#ifndef BULLET_3D_H
#define BULLET_3D_H

#include "../RhythmicBullet.h"

#define BULLET_3D_OFF_BEAT_SPEED 1.5
#define BULLET_3D_ON_BEAT_SPEED 2.5
#define BULLET_3D_BONUS_DMG 0.25

class Bullet3d : public RhythmicBullet {
 public:
  Bullet3d(bn::fixed_point _initialPosition,
           bn::fixed_point _direction,
           bn::sprite_item _bullet,
           bn::fixed dmg = 1,
           bn::fixed scale = 1,
           bn::fixed _offBeatSpeed = BULLET_3D_OFF_BEAT_SPEED,
           bn::fixed _onBeatSpeed = BULLET_3D_ON_BEAT_SPEED);

  bn::fixed_point getPosition() { return sprite.position(); }
  bool update(int msecs,
              bool isInsideBeat,
              bn::fixed_point playerPosition) override;
  void setCamera(bn::camera_ptr camera) { sprite.set_camera(camera); }

 private:
  bn::fixed_point initialPosition;
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<2> animation;
  bn::fixed_point direction;
  bn::fixed offBeatSpeed;
  bn::fixed onBeatSpeed;
};

#endif  // BULLET_3D_H
