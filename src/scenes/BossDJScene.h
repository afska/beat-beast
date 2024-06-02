#ifndef BOSS_DJ_SCENE_H
#define BOSS_DJ_SCENE_H

#include "Scene.h"

#include "../objects/Bullet.h"
#include "../objects/Horse.h"
#include "../objects/dj/Octopus.h"
#include "../objects/dj/Vinyl.h"
#include "../objects/ui/Cross.h"
#include "../objects/ui/LifeBar.h"
#include "../phys/PhysWorld.h"
#include "../rhythm/ChartReader.h"
#include "bn_regular_bg_position_hbe_ptr.h"

class BossDJScene : public Scene {
 public:
  BossDJScene(const GBFS_FILE* _fs);
  void init() override;
  void update() override;

 private:
  bn::vector<bn::sprite_ptr, 32> textSprites;
  bn::sprite_text_generator textGenerator;
  bn::unique_ptr<ChartReader> chartReader;
  bn::unique_ptr<Horse> horse;
  bn::unique_ptr<Octopus> octopus;
  bn::regular_bg_ptr background;
  bn::vector<bn::unique_ptr<Bullet>, 32> bullets;
  bn::vector<bn::unique_ptr<Vinyl>, 32> vinyls;
  bn::unique_ptr<LifeBar> lifeBar;
  bn::optional<bn::unique_ptr<Cross>> cross;
  int lastBeat = -1;
  bool isNewBeat = false;
  bn::fixed layer1 = 0;
  bn::fixed layer2 = 0;
  bn::array<bn::fixed, bn::display::height()> horizontalDeltas;
  bn::regular_bg_position_hbe_ptr horizontalHBE;
  bn::random random;

  void processInput();
  void processChart();
  void updateBackground();
  void updateSprites();

  void showCross();
  void throwVinyl(bn::unique_ptr<Vinyl> vinyl);
};

#endif  // BOSS_DJ_SCENE_H
