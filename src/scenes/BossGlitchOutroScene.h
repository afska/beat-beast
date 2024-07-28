#ifndef BOSS_GLITCH_OUTRO_SCENE_H
#define BOSS_GLITCH_OUTRO_SCENE_H

#include "UIScene.h"

#include "../objects/Horse.h"
#include "../objects/glitch/Cerberus.h"

class BossGlitchOutroScene : public UIScene {
 public:
  BossGlitchOutroScene(const GBFS_FILE* _fs);

  void init() override;
  void update() override;

 private:
  bn::unique_ptr<Horse> horse;
  unsigned state = 0;
  int lastBeat = 0;
  bn::optional<bn::unique_ptr<Cerberus>> cerberus;

  void updateDialog();

 protected:
  bool canSkipAutoWrite() override { return false; }
};

#endif  // BOSS_GLITCH_OUTRO_SCENE_H
