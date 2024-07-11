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
  bn::regular_bg_ptr background3;
  // bn::regular_bg_ptr background2;
  bn::regular_bg_ptr background1;
  bn::regular_bg_ptr background0;
  bn::unique_ptr<Octopus> octopus;
  bn::vector<bn::unique_ptr<Bullet>, 64> bullets;
  bn::vector<bn::unique_ptr<RhythmicBullet>, 32> enemyBullets;
  bn::vector<bn::unique_ptr<Vinyl>, 32> vinyls;
  int megaBallSfxCount = 0;

  void processInput();
  void processChart();
  void updateBackground();
  void updateSprites();

  void causeDamage(unsigned amount);
  void addMegaBallSfx();
  void removeMegaBallSfx();
};

#endif  // BOSS_DJ_SCENE_H
