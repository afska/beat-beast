#ifndef FLOATING_VINYL_H
#define FLOATING_VINYL_H

#include "../RhythmicBullet.h"

#include "../../rhythm/models/Event.h"

class FloatingVinyl : public RhythmicBullet {
 public:
  FloatingVinyl(bn::fixed_point initialPosition,
                bn::fixed_point _direction,
                Event* _event);

  bool update(int msecs,
              bool isInsideBeat,
              bn::fixed_point playerPosition) override;
  void explode(bn::fixed_point nextTarget) override;

 private:
  bn::sprite_ptr sprite;
  bn::fixed_point direction;
  Event* event;
  bn::fixed scale = 0.1;
  bn::sprite_animate_action<5> animation;
  bn::optional<bn::sprite_animate_action<2>> explodingAnimation;
};

#endif  // FLOATING_VINYL_H
