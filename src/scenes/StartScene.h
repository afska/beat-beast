#ifndef START_SCENE_H
#define START_SCENE_H

#include "../objects/ui/Menu.h"
#include "Scene.h"

class StartScene : public Scene {
 public:
  StartScene(const GBFS_FILE* _fs);
  void init() override;
  void update() override;

 private:
  bn::optional<bn::regular_bg_ptr> background;
  bn::vector<bn::sprite_ptr, 32> textSprites;
  bn::sprite_text_generator textGenerator;
  bn::sprite_text_generator textGeneratorAccent;
  bn::unique_ptr<Menu> menu;
  unsigned videoFrame = 0;

  void processMenuOption(int option);
};

#endif  // START_SCENE_H
