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

#define HORSE_INITIAL_Y 50

class BossRifferScene : public BossScene {
 public:
  BossRifferScene(const GBFS_FILE* _fs);

  void updateBossFight() override;

 private:
  bn::fixed velocityY = 0;
  bn::fixed scrollLimit1, scrollLimit2;
  bn::top_left_fixed_rect viewport;
  bn::vector<bn::top_left_fixed_rect, 32> platforms;
  bn::fixed_point lastSafePosition;
  bn::fixed_point lastSafeViewportPosition;

  bn::optional<bn::regular_bg_ptr> background3;
  bn::optional<bn::regular_bg_ptr> background0;
  bn::vector<bn::unique_ptr<Bullet>, 64> bullets;
  bn::vector<bn::unique_ptr<RhythmicBullet>, 32> enemyBullets;

  void processInput();
  void processChart();
  void updateBackground();
  void updateSprites();

  bool snapToPlatform(bool requireYAlignment = true);
  void moveViewport(bn::fixed deltaX, bn::fixed deltaY);
  void causeDamage(bn::fixed amount);
};

#endif  // BOSS_RIFFER_SCENE_H
