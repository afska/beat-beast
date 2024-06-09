#ifndef START_SCENE_H
#define START_SCENE_H

#include "Scene.h"

class StartScene : public Scene {
 public:
  StartScene(const GBFS_FILE* _fs);
  void init() override;
  void update() override;

 private:
};

#endif  // START_SCENE_H
