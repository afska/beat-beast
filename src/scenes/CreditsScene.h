#ifndef CREDITS_SCENE_H
#define CREDITS_SCENE_H

#include "Scene.h"

class CreditsScene : public Scene {
 public:
  CreditsScene(const GBFS_FILE* _fs);

  void init() override;
  void update() override;

 private:
  bn::optional<bn::regular_bg_ptr> background;
  bn::vector<bn::vector<bn::sprite_ptr, 64>, 64> textLines;
  bn::sprite_text_generator textGenerator;
  bn::sprite_text_generator textGeneratorAccent;
  int wait = 0;
  int index = -1;

  void scrollLines();
  void addLine();
};

#endif  // CREDITS_SCENE_H
