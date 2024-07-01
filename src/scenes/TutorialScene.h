#ifndef TUTORIAL_SCENE_H
#define TUTORIAL_SCENE_H

#include "UIScene.h"

#include "../objects/Bullet.h"
#include "../objects/Horse.h"

class TutorialScene : public UIScene {
 public:
  TutorialScene(const GBFS_FILE* _fs);

  void init() override;
  void update() override;

 private:
  bn::unique_ptr<Horse> horse;
  unsigned state = 0;
  bn::vector<bn::unique_ptr<Bullet>, 32> bullets;
  int lastBeat = 0;
  bool isNewBeat = false;

  void updateDialog();

 protected:
  bool canSkipAutoWrite() override { return true; }
};

#endif  // TUTORIAL_SCENE_H
