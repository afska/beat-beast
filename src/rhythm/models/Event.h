#ifndef EVENT_H
#define EVENT_H

#include "tonc_core.h"

typedef struct {
  int timestamp;
  u8 data;

  bool handled;

  // REGULAR events: 7 types. Can be used simultaneosly.
  // SPECIAL events: 127 types. One per beat.
  // All events are user-defined (depend on the scene).

  bool isRegular() { return (data & (1 << 7)) == 0; }
  bool isSpecial() { return !isRegular(); }
  bool isBeat() { return isSpecial() && getType() == 0; }
  bool isTick() { return isBeat() || (isSpecial() && getType() == 1); }
  u8 getType() { return data & ~(1 << 7); }
} Event;

#endif  // EVENT_H
