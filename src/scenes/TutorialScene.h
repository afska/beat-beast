#ifndef TUTORIAL_SCENE_H
#define TUTORIAL_SCENE_H

#include "UIScene.h"

#include "../objects/Bullet.h"
#include "../objects/Horse.h"
#include "../objects/ui/Check.h"
#include "../objects/ui/Cross.h"
#include "../objects/ui/QuestionMark.h"

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

  unsigned state = 0;
  int lastBeat = 0;
  int lastTick = 0;
  bool isNewBeat = false;
  bool isNewTick = false;
  bool isNewBeatNow = false;
  bool isNewTickNow = false;
  bool isInsideTick = false;
  bool isInsideBeat = false;

  bool didUnlockMove = false;
  bool didUnlockJump = false;
  bool didUnlockAim = false;
  bool didUnlockTargetLock = false;

  void processInput();
  int processBeats();
  void updateSprites(int msecs);

  void updateDialog();
  void shoot();
  void reportFailedShot();
  void reportSuccess();

 protected:
  bool canSkipAutoWrite() override { return true; }
};

#endif  // TUTORIAL_SCENE_H
