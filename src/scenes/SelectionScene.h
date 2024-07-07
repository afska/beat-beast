#ifndef SELECTION_SCENE_H
#define SELECTION_SCENE_H

#include "Scene.h"

#include "../objects/Horse.h"

class SelectionScene : public Scene {
 public:
  SelectionScene(const GBFS_FILE* _fs);

  void init() override;
  void update() override;

 private:
  bn::optional<bn::regular_bg_ptr> background;
  bn::unique_ptr<Horse> horse;
  bn::vector<bn::sprite_ptr, 64> textSprites;
  bn::sprite_text_generator textGenerator;
  bn::sprite_text_generator textGeneratorAccent;
  bn::fixed videoFrame = 0;
  int lastBeat = 0;

  void processBeats();
  void updateVideo();
};

#endif  // SELECTION_SCENE_H
