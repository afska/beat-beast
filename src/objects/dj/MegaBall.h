#ifndef MEGA_BALL_H
#define MEGA_BALL_H

#include "../../rhythm/models/Event.h"
#include "../RhythmicBullet.h"

class MegaBall : public RhythmicBullet {
 public:
  MegaBall(bn::fixed_point _initialPosition, Event* _event);

  bool update(int msecs,
              bool isInsideBeat,
              bn::fixed_point playerPosition) override;
  void explode(bn::fixed_point nextTarget) override;

 private:
  Event* event;
  bn::fixed_point direction;
  bn::fixed scale = 0.1;
  bn::sprite_ptr sprite1;
  bn::sprite_ptr sprite2;
  bn::fixed_point returnPoint;
  bn::sprite_animate_action<2> animation1;
  bn::sprite_animate_action<7> animation2;
  u32 waitFrames = 10;
  u32 damageWaitFrames = 40;
};

#endif  // MEGA_BALL_H
