#ifndef PLATFORM_FIRE_H
#define PLATFORM_FIRE_H

#include "../TopLeftGameObject.h"

#include "../../rhythm/models/Event.h"

class PlatformFire : public TopLeftGameObject {
 public:
  PlatformFire(bn::sprite_item _spriteItem,
               bn::fixed_point _topLeftPosition,
               Event* _event,
               bn::optional<bn::camera_ptr> camera,
               bool _isInfinite = false);

  void start(Event* event);
  bool didStart() { return didStartAnimation; }
  bool update(int msecs);
  Event* getEvent() { return event; }
  bool getIsInfinite() { return isInfinite; }

  bool hasReallyStarted(int msecs) {
    return startEvent != NULL && msecs >= startEvent->timestamp;
  }

 private:
  bn::sprite_item spriteItem;
  bn::sprite_ptr sprite2;
  bn::optional<bn::sprite_animate_action<2>> animation1;
  bn::optional<bn::sprite_animate_action<2>> animation2;
  Event* event;
  bool isInfinite = false;
  Event* startEvent = NULL;
  bool didStartAnimation = false;

  void setPosition(bn::fixed_point newPosition);
};
#endif  // PLATFORM_FIRE_H
