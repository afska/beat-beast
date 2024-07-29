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
  bn::vector<bn::vector<bn::sprite_ptr, 64>, 32> textSprites;
  bn::sprite_text_generator textGenerator;
  bn::sprite_text_generator textGeneratorAccent;
};

#endif  // CREDITS_SCENE_H
