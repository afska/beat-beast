#ifndef CHART_H
#define CHART_H

#include "tonc_core.h"

#include "./Event.h"

enum DifficultyLevel { EASY, NORMAL, HARD };

typedef struct {
  DifficultyLevel difficulty;  // u8

  u32 rhythmEventCount;
  Event* rhythmEvents;  // ("rhythmEventCount" times)

  u32 eventCount;
  Event* events;  // ("eventCount" times)
} Chart;

#endif  // CHART_H
