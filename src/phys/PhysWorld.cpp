#include "PhysWorld.h"

void PhysWorld::addObject(bn::fixed_rect& obj) {
  objects.push_back(obj);
}

bool PhysWorld::testCollision(bn::fixed_rect& obj, bn::fixed_point& velocity) {
  bn::fixed_rect newObj = bn::fixed_rect(obj);
  newObj.set_position(newObj.position() + velocity);
  for (bn::fixed_rect& otherObj : objects) {
    if (newObj.intersects(otherObj)) {
      return true;
    }
  }

  return false;
}
