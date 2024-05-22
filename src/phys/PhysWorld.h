#ifndef PHYS_WORLD_H
#define PHYS_WORLD_H

#include <bn_fixed_point.h>
#include <bn_fixed_rect.h>
#include <bn_vector.h>

#define MAX_OBJECTS 1024

class PhysWorld {
 public:
  void addObject(bn::fixed_rect& obj);
  bool testCollision(bn::fixed_rect& obj, bn::fixed_point& velocity);

 private:
  bn::vector<bn::fixed_rect, MAX_OBJECTS> objects;
};

#endif
