#ifndef PLATFORM_FIRE_H
#define PLATFORM_FIRE_H

#include "../TopLeftGameObject.h"

#include "../../rhythm/models/Event.h"

class PlatformFire : public TopLeftGameObject {
 public:
  PlatformFire(bn::fixed_point _topLeftPosition,
               Event* _event,
               bn::camera_ptr camera);

  void start(Event* event);
  bool didStart() { return didStartAnimation; }
  bool update(int msecs);

 private:
  bn::sprite_ptr sprite2;
  bn::optional<bn::sprite_animate_action<2>> animation1;
  bn::optional<bn::sprite_animate_action<2>> animation2;
  Event* event;
  Event* startEvent = NULL;
  bool didStartAnimation = false;

  bool hasReallyStarted(int msecs) {
    return startEvent != NULL && msecs >= startEvent->timestamp;
  }

  void setPosition(bn::fixed_point newPosition);
};
#endif  // PLATFORM_FIRE_H
