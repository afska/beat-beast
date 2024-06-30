#ifndef BOSS_WIZARD_SCENE_H
#define BOSS_WIZARD_SCENE_H

#include "BossScene.h"

#include "../objects/Bullet.h"
#include "../objects/wizard/AllyDragon.h"
#include "../objects/wizard/BlackHole.h"
#include "../objects/wizard/CircularFireBall.h"
#include "../objects/wizard/DragonEgg.h"
#include "../objects/wizard/FireBall.h"
#include "../objects/wizard/FlyingDragon.h"
#include "../objects/wizard/Lightning.h"
#include "../objects/wizard/Meteorite.h"
#include "../objects/wizard/MiniRock.h"
#include "../objects/wizard/Portal.h"
#include "../objects/wizard/Rock.h"
#include "../objects/wizard/Wizard.h"

class BossWizardScene : public BossScene {
 public:
  BossWizardScene(const GBFS_FILE* _fs);

  void updateBossFight() override;

 private:
  bn::optional<bn::regular_bg_ptr> background3;
  bn::optional<bn::regular_bg_ptr> background2;
  bn::optional<bn::regular_bg_ptr> background1;
  bn::optional<bn::regular_bg_ptr> background0;
  bn::vector<bn::unique_ptr<MiniRock>, 32> miniRocks;
  bn::vector<bn::unique_ptr<Rock>, 32> rocks;
  bn::vector<bn::unique_ptr<Bullet>, 32> bullets;
  bn::vector<bn::unique_ptr<RhythmicBullet>, 32> enemyBullets;
  bn::vector<bn::unique_ptr<Lightning>, 32> lightnings;
  bn::vector<bn::unique_ptr<FlyingDragon>, 32> flyingDragons;
  bn::vector<bn::unique_ptr<Portal>, 32> portals;
  bn::optional<bn::unique_ptr<Wizard>> wizard;
  bn::optional<bn::unique_ptr<DragonEgg>> dragonEgg;
  bn::optional<bn::unique_ptr<AllyDragon>> allyDragon;
  bn::optional<bn::unique_ptr<BlackHole>> blackHole;
  bn::fixed flySpeedX = 0;
  int phase = 1;
  bool fadingToWhite = false;

  void processInput();
  void processChart();
  void updateBackground();
  void updateSprites();

  void goToNextPhase();
  void causeDamage(unsigned amount);
};

#endif  // BOSS_WIZARD_SCENE_H
