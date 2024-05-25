#ifndef VINYL_H
#define VINYL_H

#include "GameObject.h"

class Vinyl : GameObject {
 public:
  Vinyl(bn::fixed_point initialPosition, bn::fixed_point normalizedDirection);

  bool update();

 private:
  bn::sprite_ptr sprite;
  bn::fixed_point direction;
};

#endif  // VINYL_H
