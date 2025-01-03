#ifndef BOSS_GLITCH_OUTRO_SCENE_H
#define BOSS_GLITCH_OUTRO_SCENE_H

#include "UIScene.h"

#include "../objects/Bullet.h"
#include "../objects/Horse.h"
#include "../objects/glitch/Butano2d.h"
#include "../objects/glitch/Cerberus.h"
#include "../objects/ui/Cross.h"
#include "../objects/ui/GunReload.h"
#include "../objects/ui/LifeBar.h"
#include "../objects/ui/QuestionMark.h"
#include "../player/player.h"

class BossGlitchOutroScene : public UIScene {
 public:
  BossGlitchOutroScene(const GBFS_FILE* _fs);

  void init() override;
  void update() override;

 private:
  bn::unique_ptr<Horse> horse;
  unsigned state = 0;

  bool didUnlockShooting = false;
  int countdown = 0;
  bool bounce = true;
  bool didStartBonusLoop = false;
  bool didStartBonusSong = false;

  int msecs = 0;
  int lastBeat = 0;
  int lastTick = 0;
  bool isNewBeat = false;
  bool isNewTick = false;
  bool isNewBeatNow = false;
  bool isNewTickNow = false;
  bool isInsideTick = false;
  bool isInsideBeat = false;

  bn::optional<bn::unique_ptr<Cerberus>> cerberus;
  bn::unique_ptr<Butano2d> butano2d;
  bn::optional<bn::unique_ptr<LifeBar>> enemyLifeBar;
  bn::vector<bn::unique_ptr<Bullet>, 16> bullets;
  bn::optional<bn::unique_ptr<Cross>> cross;
  bn::optional<bn::unique_ptr<QuestionMark>> questionMark;

  void processInput();
  void processBeats();
  void updateSprites();

  void updateDialog();

  template <typename F, typename Type, int MaxSize>
  inline void iterate(bn::vector<Type, MaxSize>& vector, F action) {
    for (auto it = vector.begin(); it != vector.end();) {
      bool erase = action(it->get());
      if (vector.empty())
        return;

      if (erase)
        it = vector.erase(it);
      else
        ++it;
    }
  }

  void shoot();
  void reportFailedShot();

 protected:
  bool canSkipAutoWrite() override { return false; }
  void onRestart() override {
    if (didStartBonusLoop)
      player_stop();
  }
};

#endif  // BOSS_GLITCH_OUTRO_SCENE_H
