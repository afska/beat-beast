#ifndef BOSS_WIZARD_SCENE_H
#define BOSS_WIZARD_SCENE_H

#include "BossScene.h"

#include "../objects/Bullet.h"

class BossWizardScene : public BossScene {
 public:
  BossWizardScene(const GBFS_FILE* _fs);

  void updateBossFight() override;

 private:
  bn::regular_bg_ptr background3;
  bn::regular_bg_ptr background2;
  bn::regular_bg_ptr background1;
  bn::regular_bg_ptr background0;
  bn::vector<bn::unique_ptr<Bullet>, 32> bullets;
  bn::vector<bn::unique_ptr<RhythmicBullet>, 32> enemyBullets;

  void processInput();
  void processChart();
  void updateBackground();
  void updateSprites();

  void causeDamage(unsigned amount);
};

#endif  // BOSS_WIZARD_SCENE_H