#ifndef BUTANO_3D_H
#define BUTANO_3D_H

#include "../../rhythm/models/Event.h"
#include "Attack3d.h"

class Butano3d : public Attack3d {
 public:
  Butano3d(int _channel,
           bn::fixed_point _initialPosition,
           bn::fixed_point _targetPosition,
           bn::fixed _frames,
           Event* _event);

  bool update(int msecs,
              bool isInsideBeat,
              bn::fixed_point playerPosition) override;
  void explode(bn::fixed_point nextTarget) override;
  void hurt();

 private:
  bn::sprite_ptr sprite;
  bn::fixed_point targetPosition;
  bn::fixed frames;
  Event* event;
  bn::fixed speedX, speedY, speedZ;
  bn::optional<bn::sprite_animate_action<8>> hurtAnimation;
};

#endif  // BUTANO_3D_H
