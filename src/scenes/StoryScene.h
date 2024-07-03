#ifndef STORY_SCENE_H
#define STORY_SCENE_H

#include "UIScene.h"

#include "../objects/Horse.h"

class StoryScene : public UIScene {
 public:
  StoryScene(const GBFS_FILE* _fs);

  void init() override;
  void update() override;

 private:
  bn::unique_ptr<Horse> horse;

  unsigned state = 0;

  void updateDialog();
};

#endif  // STORY_SCENE_H
