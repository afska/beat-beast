#ifndef MINI_ROCK_H
#define MINI_ROCK_H

#include "../../rhythm/models/Event.h"
#include "../GameObject.h"

class MiniRock : public GameObject {
 public:
  MiniRock(bn::fixed_point initialPosition, Event* _event);

  bool update(int msecs,
              unsigned beatDurationMs,
              unsigned oneDivBeatDurationMs,
              int horseX);

 private:
  bn::sprite_ptr sprite;
  Event* event;
};

#endif  // MINI_ROCK_H
