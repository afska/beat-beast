#ifndef PORTAL_H
#define PORTAL_H

#include "../../rhythm/models/Event.h"
#include "../GameObject.h"
#include "../Horse.h"

class Portal : public GameObject {
 public:
  Portal(bn::fixed_point initialPosition, Event* _event, bool _followY = false);

  bool update(int msecs,
              unsigned beatDurationMs,
              unsigned oneDivBeatDurationMs,
              Horse* horse);

 private:
  bn::sprite_ptr sprite;
  Event* event;
  bn::sprite_animate_action<10> animation;
  bool followY;
};

#endif  // PORTAL_H
