#ifndef BOSS_DJ_SCENE_H
#define BOSS_DJ_SCENE_H

#include "BossScene.h"

#include "../objects/Bullet.h"
#include "../objects/dj/FloatingVinyl.h"
#include "../objects/dj/MegaBall.h"
#include "../objects/dj/Octopus.h"
#include "../objects/dj/Vinyl.h"
#include "bn_regular_bg_position_hbe_ptr.h"

class BossDJScene : public BossScene {
 public:
  BossDJScene(const GBFS_FILE* _fs);

  void updateBossFight() override;

 private:
  bn::regular_bg_ptr background;
  bn::unique_ptr<Octopus> octopus;
  bn::vector<bn::unique_ptr<Bullet>, 32> bullets;
  bn::vector<bn::unique_ptr<RhythmicBullet>, 32> enemyBullets;
  bn::vector<bn::unique_ptr<Vinyl>, 32> vinyls;
  bn::fixed layer1 = 0;
  bn::fixed layer2 = 0;
  bn::array<bn::fixed, bn::display::height()> horizontalDeltas;
  bn::regular_bg_position_hbe_ptr horizontalHBE;
  bool didWin = false;
  bool didShowMessage = false;

  void processInput();
  void processChart();
  void updateBackground();
  void updateSprites();

  void causeDamage(unsigned amount);
  void playRandomSound();
};

#endif  // BOSS_DJ_SCENE_H
