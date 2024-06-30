#ifndef LIGHTNING_H
#define LIGHTNING_H

#include "../TopLeftGameObject.h"

#include "../../rhythm/models/Event.h"

class Lightning : public TopLeftGameObject {
 public:
  bool causedDamage = false;
  Lightning(bn::fixed_point _topLeftPosition, Event* _event);

  void start(Event* event);
  bool needsToStart() { return hasPendingStartAnimation; }
  bool didStart() { return didStartAnimation; }
  bool update(int msecs);

  void start1();
  void start2();

 private:
  bn::sprite_ptr sprite2;
  bn::sprite_ptr sprite3;
  bn::optional<bn::sprite_animate_action<8>> animation1;
  bn::optional<bn::sprite_animate_action<8>> animation2;
  bn::optional<bn::sprite_animate_action<8>> animation3;
  Event* event;
  Event* startEvent = NULL;
  bool hasPendingStartAnimation = false;
  bool didStartAnimation = false;

  bool hasReallyStarted(int msecs) {
    return startEvent != NULL && msecs >= startEvent->timestamp;
  }

  void setPosition(bn::fixed_point newPosition);
};
#endif  // LIGHTNING_H
