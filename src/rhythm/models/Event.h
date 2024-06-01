#ifndef EVENT_H
#define EVENT_H

#include "tonc_core.h"

typedef struct {
  int timestamp;
  u32 data;

  bool handled;

  // REGULAR events: 31 types. Can be used simultaneosly.
  // SPECIAL events: * types. One per beat.
  // All events are user-defined (depend on the scene).

  bool isRegular() { return (data & (1 << 31)) == 0; }
  bool isSpecial() { return !isRegular(); }
  bool isBeat() { return isSpecial() && getType() == 0; }
  bool isTick() { return isBeat() || (isSpecial() && getType() == 1); }
  u8 getType() { return data & ~(1 << 31); }
} Event;

#endif  // EVENT_H
