#ifndef BOSS_GLITCH_SCENE_H
#define BOSS_GLITCH_SCENE_H

#include "BossScene.h"

#include "../objects/glitch/Bullet3d.h"
#include "../objects/glitch/Cerberus.h"
#include "../objects/glitch/Vinyl3d.h"
#include "bn_regular_bg_position_hbe_ptr.h"

class BossGlitchScene : public BossScene {
 public:
  BossGlitchScene(const GBFS_FILE* _fs);

  void updateBossFight() override;

 private:
  bn::optional<bn::regular_bg_ptr> errBackground;
  bn::optional<bn::regular_bg_ptr> videoBackground;
  bn::optional<bn::unique_ptr<Horse>> ghostHorse;
  bn::optional<Cerberus> cerberus;
  bn::vector<bn::unique_ptr<Bullet3d>, 64> bullets;
  bn::vector<bn::unique_ptr<Attack3d>, 32> enemyBullets;
  bn::fixed videoFrame = 0;
  bn::fixed extraSpeed = 0;
  int channel = 0;
  bn::array<bn::fixed, bn::display::height()> horizontalDeltas;
  bn::optional<bn::regular_bg_position_hbe_ptr> horizontalHBE;
  int glitchType = -1;
  int glitchFrames = 0;
  bool animatedFlag = false;
  int halfAnimatedFlag = 0;
  int sixteenthAnimatedFlag = 0;
  bn::fixed offsetY = 0;
  bn::fixed frozenVideoFrame = 0;
  bn::fixed actualVideoFrame = 0;
  bool pauseVideo = false;
  bool mosaicVideo = false;
  bool invertColors = false;
  bn::fixed hueShift = 0;
  int selectedGlitch = 0;

  void processInput();
  void processChart();
  void updateBackground();
  void updateSprites();
  void updateGlitches();

  void updateHorseChannel();
  bn::fixed_point getShootingPoint();
  bn::fixed_point getShootingDirection();
  bn::fixed getZSpeed();

  void causeDamage(bn::fixed amount);
};

#endif  // BOSS_GLITCH_SCENE_H
