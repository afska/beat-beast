#ifndef HELLO_WORLD_SCENE_H
#define HELLO_WORLD_SCENE_H

#include "Scene.h"

#include "bn_sprite_text_generator.h"

class HelloWorldScene : Scene {
 public:
  HelloWorldScene();
  void init() override;
  void update() override;

 private:
  bn::vector<bn::sprite_ptr, 32> textSprites;
  bn::sprite_text_generator textGenerator;
};

#endif  // HELLO_WORLD_SCENE_H
