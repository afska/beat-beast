#ifndef SCENE_H
#define SCENE_H

#include "bn_display.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_string.h"
#include "bn_vector.h"

class Scene {
 public:
  virtual void init() = 0;
  virtual void update() = 0;

  virtual ~Scene() = default;
};

#endif  // SCENE_H
