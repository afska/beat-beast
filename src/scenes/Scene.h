#ifndef SCENE_H
#define SCENE_H

#include "bn_display.h"
#include "bn_random.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_unique_ptr.h"
#include "bn_vector.h"

class Scene {
 public:
  virtual void init() = 0;
  virtual void update() = 0;

  virtual ~Scene() = default;

  void setNextScene(bn::unique_ptr<Scene> scene) {
    nextScene = bn::move(scene);
  }
  bool hasNextScene() { return nextScene != nullptr; }
  bn::unique_ptr<Scene> getNextScene() { return bn::move(nextScene); }

 protected:
  bn::unique_ptr<Scene> nextScene;
};

#endif  // SCENE_H
