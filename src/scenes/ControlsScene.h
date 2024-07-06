#ifndef CONTROLS_SCENE_H
#define CONTROLS_SCENE_H

#include "UIScene.h"

#include "../objects/Horse.h"

class ControlsScene : public UIScene {
 public:
  ControlsScene(const GBFS_FILE* _fs, GameState::Screen _nextScreen);

  void init() override;
  void update() override;

 private:
  bn::unique_ptr<Horse> horse;
  GameState::Screen nextScreen;
  unsigned state = 0;

  void updateDialog();

 protected:
  bool canSkipAutoWrite() override { return true; }
  bool canPause() override { return false; }
};

#endif  // CONTROLS_SCENE_H
