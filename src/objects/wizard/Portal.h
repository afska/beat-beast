#ifndef PORTAL_H
#define PORTAL_H

#include "../../rhythm/models/Event.h"
#include "../GameObject.h"

class Portal : public GameObject {
 public:
  Portal(bn::fixed_point initialPosition, Event* _event);

  bool update(int msecs,
              unsigned beatDurationMs,
              unsigned oneDivBeatDurationMs,
              int horseX);

 private:
  bn::sprite_ptr sprite;
  Event* event;
  bn::sprite_animate_action<10> animation;
};

#endif  // PORTAL_H
