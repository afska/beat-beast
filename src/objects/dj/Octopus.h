#ifndef OCTOPUS_H
#define OCTOPUS_H

#include "../GameObject.h"

class Octopus : public GameObject {
 public:
  Octopus();
  void update();

 private:
  bn::sprite_ptr sprite;
};

#endif  // OCTOPUS_H
