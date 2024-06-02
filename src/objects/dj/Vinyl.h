#ifndef VINYL_H
#define VINYL_H

#include "../../rhythm/models/Event.h"
#include "../GameObject.h"

class Vinyl : public GameObject {
 public:
  Vinyl(bn::fixed_point initialPosition,
        bn::fixed_point normalizedDirection,
        Event* _event);

  bool update(int msecs,
              unsigned beatDurationMs,
              unsigned oneDivBeatDurationMs,
              int horseX);

 private:
  bn::sprite_ptr sprite;
  bn::fixed_point direction;
  Event* event;
  bn::fixed scale = 0.1;

  bn::sprite_ptr tentacleSprite;
  bn::fixed tentacleTargetX;
  bn::sprite_animate_action<5> attackAnimation;
};

#endif  // VINYL_H
