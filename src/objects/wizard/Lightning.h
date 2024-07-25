#ifndef LIGHTNING_H
#define LIGHTNING_H

#include "../TopLeftGameObject.h"

#include "../../rhythm/models/Event.h"

class Lightning : public TopLeftGameObject {
 public:
  bool causedDamage = false;
  Lightning(bn::sprite_item _spriteItem1,
  bn::sprite_item _spriteItem2,
  bn::sprite_item _spriteItem3,
  bn::sprite_item _spriteItem4,
  bn::sprite_item _spriteItem5,
            bn::fixed_point _topLeftPosition,
            Event* _event);

  void start(Event* event);
  bool needsToStart() { return hasPendingStartAnimation; }
  bool didStart() { return didStartAnimation; }
  bool update(int msecs);

  void start1();
  void start2();

 private:
  bn::sprite_item spriteItem1;
  bn::sprite_item spriteItem2;
  bn::sprite_item spriteItem3;
  bn::sprite_item spriteItem4;
  bn::sprite_item spriteItem5;
  bn::sprite_ptr sprite2;
  bn::sprite_ptr sprite3;
  bn::sprite_ptr sprite4;
  bn::sprite_ptr sprite5;
  bn::optional<bn::sprite_animate_action<8>> animation1;
  bn::optional<bn::sprite_animate_action<8>> animation2;
  bn::optional<bn::sprite_animate_action<8>> animation3;
  bn::optional<bn::sprite_animate_action<8>> animation4;
  bn::optional<bn::sprite_animate_action<8>> animation5;
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
