#ifndef VINYL_H
#define VINYL_H

#include "../rhythm/models/Event.h"
#include "GameObject.h"

class Vinyl : public GameObject {
 public:
  Vinyl(bn::fixed_point initialPosition,
        bn::fixed_point normalizedDirection,
        Event* _event);

  bool update(int msecs,
              u32 beatDurationMs,
              u32 oneDivBeatDurationMs,
              int horseX);

 private:
  bn::sprite_ptr sprite;
  bn::fixed_point direction;
  Event* event;
};

#endif  // VINYL_H
