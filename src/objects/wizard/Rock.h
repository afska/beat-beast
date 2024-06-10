#ifndef ROCK_H
#define ROCK_H

#include "../../rhythm/models/Event.h"
#include "../GameObject.h"

class Rock : public GameObject {
 public:
  Rock(bn::fixed_point initialPosition, Event* _event);

  void smash();
  bool isBreaking() { return isSmashing; }

  bool update(int msecs,
              unsigned beatDurationMs,
              unsigned oneDivBeatDurationMs,
              int horseX);

 private:
  bn::sprite_ptr sprite;
  Event* event;
  bool isSmashing = false;
};

#endif  // ROCK_H
