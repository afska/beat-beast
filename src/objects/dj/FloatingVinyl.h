#ifndef FLOATING_VINYL_H
#define FLOATING_VINYL_H

#include "../../rhythm/models/Event.h"
#include "../RhythmicBullet.h"

class FloatingVinyl : public RhythmicBullet {
 public:
  FloatingVinyl(bn::fixed_point initialPosition,
                bn::fixed_point _direction,
                Event* _event);

  bool update(int msecs, bool isInsideBeat) override;
  void explode() override { isExploding = true; }

 private:
  bn::sprite_ptr sprite;
  bn::fixed_point direction;
  Event* event;
  bn::fixed scale = 0.1;
  bn::sprite_animate_action<5> animation;
  bool isExploding = false;
};

#endif  // FLOATING_VINYL_H
