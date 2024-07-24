#ifndef BOSS_GLITCH_SCENE_H
#define BOSS_GLITCH_SCENE_H

#include "BossScene.h"

#include "../objects/Bullet.h"
#include "../objects/glitch/Cerberus.h"
#include "bn_regular_bg_position_hbe_ptr.h"

class BossGlitchScene : public BossScene {
 public:
  BossGlitchScene(const GBFS_FILE* _fs);

  void updateBossFight() override;

 private:
  bn::optional<bn::regular_bg_ptr> errBackground;
  bn::optional<bn::regular_bg_ptr> videoBackground;
  bn::optional<Cerberus> cerberus;
  bn::vector<bn::unique_ptr<Bullet>, 64> bullets;
  bn::vector<bn::unique_ptr<RhythmicBullet>, 32> enemyBullets;
  bn::fixed videoFrame = 0;
  bn::fixed extraSpeed = 0;
  int channel = 0;
  bn::array<bn::fixed, bn::display::height()> horizontalDeltas;
  bn::optional<bn::regular_bg_position_hbe_ptr> horizontalHBE;

  void processInput();
  void processChart();
  void updateBackground();
  void updateSprites();

  void updateHorseChannel();

  void causeDamage(bn::fixed amount);
};

#endif  // BOSS_GLITCH_SCENE_H
