#ifndef BOSS_RIFFER_SCENE_H
#define BOSS_RIFFER_SCENE_H

#include "BossScene.h"

#include "../objects/Bullet.h"
#include "../objects/wizard/Lightning.h"
#include "../objects/wizard/Meteorite.h"
#include "../objects/wizard/MiniRock.h"
#include "../objects/wizard/Portal.h"
#include "../objects/wizard/Rock.h"
#include "../objects/wizard/Wizard.h"

class BossRifferScene : public BossScene {
 public:
  BossRifferScene(const GBFS_FILE* _fs);

  void updateBossFight() override;

 private:
  bn::optional<bn::regular_bg_ptr> background3;
  bn::optional<bn::regular_bg_ptr> background2;
  bn::optional<bn::regular_bg_ptr> background1;
  bn::optional<bn::regular_bg_ptr> background0;
  bn::vector<bn::unique_ptr<Bullet>, 64> bullets;
  bn::vector<bn::unique_ptr<RhythmicBullet>, 32> enemyBullets;

  void processInput();
  void processChart();
  void updateBackground();
  void updateSprites();

  void causeDamage(unsigned amount);
};

#endif  // BOSS_RIFFER_SCENE_H
