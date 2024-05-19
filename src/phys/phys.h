#ifndef PHYS_H
#define PHYS_H

#include <bn_vector.h>
#include <bn_fixed_point.h>
#include <bn_fixed_rect.h>

#define MAX_OBJECTS 1024

class PhysWorld {
  public:
    void add_object(bn::fixed_rect &obj);
    bool test_collision(bn::fixed_rect &obj, bn::fixed_point &velocity);
  private:
    bn::vector<bn::fixed_rect, MAX_OBJECTS> objects;
};

#endif
