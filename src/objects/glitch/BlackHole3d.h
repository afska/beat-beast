#ifndef BLACK_HOLE_3D_H
#define BLACK_HOLE_3D_H

#include "Attack3d.h"

#include "../../rhythm/models/Event.h"

class BlackHole3d : public Attack3d {
 public:
  BlackHole3d(int _channel,
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
  bn::fixed_point targetPosition;
  bn::fixed hitZoneStart, hitZoneEnd;
  bn::fixed frames;
  Event* event;
  bn::fixed speedX, speedY, speedZ;
  bn::sprite_animate_action<6> animation;
};

#endif  // BLACK_HOLE_3D_H
