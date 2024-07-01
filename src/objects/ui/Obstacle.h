#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "../GameObject.h"

class Obstacle : public GameObject {
 public:
  Obstacle(bn::fixed_point initialPosition,
           bn::fixed_point _direction,
           int _eventMsecs);

  bool update(int msecs,
              unsigned beatDurationMs,
              unsigned oneDivBeatDurationMs,
              int horseX);

 private:
  bn::sprite_ptr sprite;
  bn::fixed_point direction;
  int eventMsecs;
  bn::fixed scale = 0.1;
};

#endif  // OBSTACLE_H
