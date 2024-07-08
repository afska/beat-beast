#ifndef SCENE_H
#define SCENE_H

#include "bn_camera_ptr.h"
#include "bn_display.h"
#include "bn_random.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_unique_ptr.h"
#include "bn_vector.h"

#include "../savefile/GameState.h"

#include "../utils/gbfs/gbfs.h"

class Scene {
 public:
  Scene(GameState::Screen _screen, const GBFS_FILE* _fs)
      : screen(_screen), fs(_fs) {}

  virtual void init() = 0;
  virtual void update() = 0;

  virtual ~Scene() = default;

  GameState::Screen getScreen() { return screen; }
  void setNextScreen(GameState::Screen _nextScreen) {
    ___nextScreen = _nextScreen;
  }
  bool hasNextScreen() { return ___nextScreen != GameState::Screen::NO; }
  GameState::Screen getNextScreen() { return ___nextScreen; }

 protected:
  GameState::Screen screen;
  GameState::Screen ___nextScreen = GameState::Screen::NO;
  const GBFS_FILE* fs;
};

#endif  // SCENE_H
