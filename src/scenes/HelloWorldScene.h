#ifndef HELLO_WORLD_SCENE_H
#define HELLO_WORLD_SCENE_H

#include "Scene.h"

class HelloWorldScene : Scene {
 public:
  void init() override;
  void update() override;

 private:
  bn::vector<bn::sprite_ptr, 32> textSprites;
};

#endif  // HELLO_WORLD_SCENE_H
