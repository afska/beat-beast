#ifndef CIRCULAR_FIRE_BALL_H
#define CIRCULAR_FIRE_BALL_H

#include "../RhythmicBullet.h"

#include "../../rhythm/models/Event.h"

class CircularFireBall : public RhythmicBullet {
 public:
  CircularFireBall(bn::fixed_point _initialPosition, Event* _event);

  bool update(int msecs,
              bool isInsideBeat,
              bn::fixed_point playerPosition) override;
  void explode(bn::fixed_point nextTarget) override;

 private:
  Event* event;
  bn::fixed_point direction;
  bn::fixed scale = 0.1;
  bn::sprite_ptr sprite;
  bn::sprite_animate_action<3> animation;
  int waitFramesUntilTargetFixed = 15;
  bn::fixed_point fixedTarget;
};

#endif  // CIRCULAR_FIRE_BALL_H
