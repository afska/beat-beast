#ifndef LIGHTNING_H
#define LIGHTNING_H

#include "../TopLeftGameObject.h"

#include "../../rhythm/models/Event.h"

class Lightning : public TopLeftGameObject {
 public:
  Lightning(bn::fixed_point _topLeftPosition, Event* _event);

  void start(Event* event);
  bool hasStarted() { return startEvent != NULL; }
  bool update(int msecs);

 private:
  bn::sprite_ptr sprite2;
  bn::sprite_ptr sprite3;
  bn::sprite_animate_action<8> animation1;
  bn::sprite_animate_action<8> animation2;
  bn::sprite_animate_action<8> animation3;
  Event* event;
  Event* startEvent = NULL;
  bool hasStartedAnimation = false;

  void setPosition(bn::fixed_point newPosition);
};
#endif  // LIGHTNING_H
