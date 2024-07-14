#ifndef BOSS_RIFFER_SCENE_H
#define BOSS_RIFFER_SCENE_H

#include "BossScene.h"

#include "../objects/Bullet.h"
#include "../objects/riffer/PlatformFire.h"
#include "../objects/riffer/Riffer.h"
#include "../objects/riffer/Wave.h"

#define HORSE_INITIAL_Y 50

class BossRifferScene : public BossScene {
 public:
  BossRifferScene(const GBFS_FILE* _fs);

  void updateBossFight() override;

 private:
  bn::fixed velocityY = 0;
  bn::fixed scrollLimit1, scrollLimit2;
  bn::camera_ptr camera;
  bn::fixed cameraTargetX = -1;
  bn::vector<bn::top_left_fixed_rect, 32> platforms;
  bn::fixed_point lastSafePosition;
  bn::fixed_point lastSafeViewportPosition;
  bn::fixed currentPlatformY = -1;
  int lastTargetedPlatform = -1;

  bn::optional<bn::regular_bg_ptr> background3;
  bn::optional<bn::regular_bg_ptr> background0;
  bn::unique_ptr<Riffer> riffer;
  bn::vector<bn::unique_ptr<Bullet>, 64> bullets;
  bn::vector<bn::unique_ptr<RhythmicBullet>, 32> enemyBullets;
  bn::vector<bn::unique_ptr<PlatformFire>, 32> platformFires;

  void processInput();
  void processChart();
  void updateBackground();
  void updateSprites();
  void updatePhysics();

  bool snapToPlatform(bool requireYAlignment = true);
  void moveViewport(bn::fixed deltaX, bn::fixed deltaY);
  void causeDamage(bn::fixed amount);

  void addExplosion(bn::fixed_point position) override;
};

#endif  // BOSS_RIFFER_SCENE_H
