#ifndef BOSS_GLITCH_SCENE_H
#define BOSS_GLITCH_SCENE_H

#include "BossScene.h"

#include "../objects/glitch/BlackHole3d.h"
#include "../objects/glitch/Bullet3d.h"
#include "../objects/glitch/Cerberus.h"
#include "../objects/glitch/FireBall3d.h"
#include "../objects/glitch/GameNote3d.h"
#include "../objects/glitch/MegaBall3d.h"
#include "../objects/glitch/Vinyl3d.h"
#include "../objects/riffer/PlatformFire.h"
#include "../objects/wizard/Lightning.h"
#include "bn_regular_bg_position_hbe_ptr.h"

class BossGlitchScene : public BossScene {
 public:
  BossGlitchScene(const GBFS_FILE* _fs);

  void updateBossFight() override;

 private:
  bn::optional<bn::regular_bg_ptr> errBackground;
  bn::optional<bn::regular_bg_ptr> videoBackground;
  bn::optional<bn::unique_ptr<Horse>> ghostHorse;
  bn::vector<bn::unique_ptr<Bullet3d>, 64> bullets;
  bn::vector<bn::unique_ptr<Attack3d>, 32> enemyBullets;
  bn::vector<bn::unique_ptr<Lightning>, 32> lightnings;
  bn::vector<bn::unique_ptr<PlatformFire>, 32> platformFires;
  bn::optional<bn::unique_ptr<Cerberus>> cerberus;
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
  bool oneTimeFlag = false;
  bn::fixed offsetY = 0;
  bn::fixed frozenVideoFrame = 0;
  bn::fixed actualVideoFrame = 0;
  bool pauseVideo = false;
  bool mosaicVideo = false;
  bool invertColors = false;
  bn::fixed hueShift = 0;
  int selectedGlitch = 0;
  bool permanentHueShift = false;

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
  void startGlitch(int type);
  void cleanupGlitch();
};

#endif  // BOSS_GLITCH_SCENE_H
