#include "PhysWorld.h"

void PhysWorld::addObject(bn::fixed_rect& obj) {
  objects.push_back(obj);
}

bool PhysWorld::testCollision(bn::fixed_rect& obj, bn::fixed_point& velocity) {
  bn::fixed_rect_t new_obj = bn::fixed_rect_t(obj);
  new_obj.set_position(new_obj.position() + velocity);
  for (bn::fixed_rect& other_obj : objects) {
    if (new_obj.intersects(other_obj)) {
      return true;
    }
  }

  return false;
}
