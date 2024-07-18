#ifndef WAVE_H
#define WAVE_H

#include "../../rhythm/models/Event.h"
#include "../RhythmicBullet.h"

class Wave : public RhythmicBullet {
 public:
  Wave(bn::fixed_point initialPosition,
       bn::fixed_point _direction,
       Event* _event,
       bn::fixed _speedMultiplier = 1);

  bool update(int msecs,
              bool isInsideBeat,
              bn::fixed_point playerPosition) override;
  void explode(bn::fixed_point nextTarget) override;
  void setCamera(bn::camera_ptr camera) { sprite.set_camera(camera); }

 private:
  bn::sprite_ptr sprite;
  bn::fixed_point direction;
  Event* event;
  bn::fixed speedMultiplier;
  bn::fixed scale = 0.1;
  bn::sprite_animate_action<2> animation;
};

#endif  // WAVE_H
