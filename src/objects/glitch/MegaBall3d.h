#ifndef MEGA_BALL_3D_H
#define MEGA_BALL_3D_H

#include "../../rhythm/models/Event.h"
#include "Attack3d.h"

class MegaBall3d : public Attack3d {
 public:
  MegaBall3d(int _channel,
             bn::fixed_point _initialPosition,
             bn::fixed_point _targetPosition,
             bn::fixed _hitZoneStart,
             bn::fixed _hitZoneEnd,
             bn::fixed _frames,
             Event* _event);

  bool update(int msecs,
              bool isInsideBeat,
              bn::fixed_point playerPosition) override;
  void explode(bn::fixed_point nextTarget) override;

 private:
  bn::sprite_ptr sprite;
  bn::sprite_ptr sprite2;
  bn::fixed_point targetPosition;
  bn::fixed hitZoneStart, hitZoneEnd;
  bn::fixed frames;
  Event* event;
  bn::fixed speedX, speedY, speedZ;
  bn::sprite_animate_action<2> animation1;
  bn::sprite_animate_action<7> animation2;
};

#endif  // MEGA_BALL_3D_H
