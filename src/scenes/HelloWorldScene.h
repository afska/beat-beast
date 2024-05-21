#ifndef HELLO_WORLD_SCENE_H
#define HELLO_WORLD_SCENE_H

#include "Scene.h"

#include "../phys/phys.h"
#include "bn_regular_bg_position_hbe_ptr.h"

class HelloWorldScene : Scene {
 public:
  HelloWorldScene();
  void init() override;
  void update() override;

 private:
  bn::vector<bn::sprite_ptr, 32> textSprites;
  bn::sprite_text_generator textGenerator;
  bn::unique_ptr<PhysWorld> physWorld;
  bn::fixed_rect gunBox;
  bn::regular_bg_ptr background;
  bn::sprite_ptr horse;
  bn::sprite_ptr gun;
  bn::sprite_ptr otherGun;
  bn::sprite_animate_action<7> runAnimation;
  int bounceFrame = 0;
  int lastBeat = 0;
  bn::fixed layer1 = 0;
  bn::fixed layer2 = 0;
  bn::array<bn::fixed, bn::display::height()> horizontalDeltas;
  bn::regular_bg_position_hbe_ptr horizontalHBE;
};

#endif  // HELLO_WORLD_SCENE_H
