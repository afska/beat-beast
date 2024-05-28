#ifndef EVENT_H
#define EVENT_H

#include "tonc_core.h"

typedef struct {
  int timestamp;
  u8 data;

  bool handled = false;

  // REGULAR events are predictible (launched ahead of time) and
  // can be launched simultaneously: up to 7 regular events per
  // beat. SPECIAL events are immediate and they can be up to
  // 127 different types. All events are user-defined.

  bool isRegular() { return (data & (1 << 7)) == 0; }
  bool isSpecial() { return !isRegular(); }
  u8 getType() { return data >> 1; }
} Event;

#endif  // EVENT_H
