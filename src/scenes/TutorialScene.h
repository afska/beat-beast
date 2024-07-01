#ifndef TUTORIAL_SCENE_H
#define TUTORIAL_SCENE_H

#include "UIScene.h"

#include "../objects/Bullet.h"
#include "../objects/Horse.h"
#include "../objects/ui/Check.h"
#include "../objects/ui/Cross.h"
#include "../objects/ui/DummyBoss.h"
#include "../objects/ui/LifeBar.h"
#include "../objects/ui/Obstacle.h"
#include "../objects/ui/QuestionMark.h"
#include "../objects/ui/Radio.h"

class TutorialScene : public UIScene {
 public:
  TutorialScene(const GBFS_FILE* _fs);

  void init() override;
  void update() override;

 private:
  bn::unique_ptr<Horse> horse;
  bn::vector<bn::unique_ptr<Bullet>, 32> bullets;
  bn::optional<bn::unique_ptr<Cross>> cross;
  bn::optional<bn::unique_ptr<Check>> check;
  bn::optional<bn::unique_ptr<QuestionMark>> questionMark;
  bn::optional<bn::unique_ptr<DummyBoss>> dummyBoss;
  bn::optional<bn::unique_ptr<LifeBar>> enemyLifeBar;
  bn::vector<bn::sprite_ptr, 64> tmpText;
  bn::vector<bn::unique_ptr<Obstacle>, 32> obstacles;
  bn::vector<bn::unique_ptr<Radio>, 32> radios;

  unsigned state = 0;
  int count = 0;

  bool didUnlockMove = false;
  bool didUnlockJump = false;
  bool didUnlockAim = false;
  bool didUnlockSubBeats = false;
  bool didUnlockTargetLock = false;

  int msecs = 0;
  int lastBeat = 0;
  int lastTick = 0;
  bool isNewBeat = false;
  bool isNewTick = false;
  bool isNewBeatNow = false;
  bool isNewTickNow = false;
  bool isInsideTick = false;
  bool isInsideBeat = false;
  bool didShootUpperRightWhileLookingLeft = false;
  bool didShootUpperLeftWhileLookingRight = false;

  void processInput();
  void processBeats();
  void updateSprites();

  void updateDialog();
  void shoot();
  void reportFailedShot();
  void reportSuccess();
  void addRadio();
  void addMultipleRadios();
  bool updateCount();
  void updateCountdown();

  template <typename F, typename Type, int MaxSize>
  inline void iterate(bn::vector<Type, MaxSize>& vector, F action) {
    for (auto it = vector.begin(); it != vector.end();) {
      bool erase = action(it->get());

      if (erase)
        it = vector.erase(it);
      else
        ++it;
    }
  }

 protected:
  bool canSkipAutoWrite() override { return false; }
};

#endif  // TUTORIAL_SCENE_H
