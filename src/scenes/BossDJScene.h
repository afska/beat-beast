#ifndef BOSS_DJ_SCENE_H
#define BOSS_DJ_SCENE_H

#include "Scene.h"

#include "../objects/Bullet.h"
#include "../objects/Horse.h"
#include "../objects/Vinyl.h"
#include "../phys/PhysWorld.h"
#include "bn_regular_bg_position_hbe_ptr.h"

class BossDJScene : Scene {
 public:
  BossDJScene();
  void init() override;
  void update() override;

 private:
  bn::vector<bn::sprite_ptr, 32> textSprites;
  bn::sprite_text_generator textGenerator;
  bn::unique_ptr<PhysWorld> physWorld;
  bn::unique_ptr<Horse> horse;
  bn::regular_bg_ptr background;
  bn::vector<bn::unique_ptr<Bullet>, 32> bullets;
  bn::vector<bn::unique_ptr<Vinyl>, 32> vinyls;
  int lastBeat = 0;
  bool isNewBeat = false;
  bn::fixed layer1 = 0;
  bn::fixed layer2 = 0;
  bn::array<bn::fixed, bn::display::height()> horizontalDeltas;
  bn::regular_bg_position_hbe_ptr horizontalHBE;
  bn::fixed_point direction = bn::fixed_point(0, 0);
  int beatCount = 0;
  bn::random random;

  void processInput();
  void processBeats();
  void updateBackground();
  void updateSprites();
};

#endif  // BOSS_DJ_SCENE_H
