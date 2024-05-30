#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "bn_fixed_point.h"
#include "bn_fixed_rect.h"
#include "bn_log.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_ptr.h"
#include "bn_string.h"

class GameObject {
 public:
  bn::fixed_rect getBoundingBox() { return boundingBox; }

 protected:
  bn::fixed_rect boundingBox;
};

#endif  // GAME_OBJECT_H
