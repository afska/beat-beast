#ifndef UI_SCENE_H
#define UI_SCENE_H

#include "Scene.h"

#include "../objects/ui/Menu.h"
#include "../player/player_sfx.h"
#include "../utils/PixelBlink.h"

class UIScene : public Scene {
 public:
  UIScene(GameState::Screen _screen, const GBFS_FILE* _fs);

  virtual ~UIScene() = default;

 protected:
  bn::vector<bn::sprite_ptr, 32> textSprites;
  bn::sprite_text_generator textGenerator;
  bn::sprite_text_generator textGeneratorAccent;
  bn::unique_ptr<PixelBlink> pixelBlink;
  // bn::unique_ptr<Menu> menu;
};

#endif  // UI_SCENE_H
