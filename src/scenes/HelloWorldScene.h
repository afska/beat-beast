#ifndef HELLO_WORLD_SCENE_H
#define HELLO_WORLD_SCENE_H

#include "Scene.h"

#include "bn_sprite_text_generator.h"
#include "../phys/phys.h"

#include <bn_unique_ptr.h>

class HelloWorldScene : Scene {
 public:
  HelloWorldScene();
  void init() override;
  void update() override;

 private:
  bn::vector<bn::sprite_ptr, 32> textSprites;
  bn::sprite_text_generator textGenerator;
  bn::unique_ptr<PhysWorld> physWorld;
  bn::fixed_rect player_box;
  bn::sprite_ptr sprite;
  bn::sprite_ptr other;
};

#endif  // HELLO_WORLD_SCENE_H
