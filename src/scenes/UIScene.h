#ifndef UI_SCENE_H
#define UI_SCENE_H

#include "Scene.h"

#include "../objects/ui/Menu.h"
#include "../player/player_sfx.h"
#include "../utils/PixelBlink.h"

class UIScene : public Scene {
 public:
  UIScene(GameState::Screen _screen, const GBFS_FILE* _fs);

  virtual void update() override;

  virtual ~UIScene() = default;

 protected:
  bn::vector<bn::sprite_ptr, 32> textSprites;
  bn::sprite_text_generator textGenerator;
  bn::sprite_text_generator textGeneratorAccent;
  bn::unique_ptr<PixelBlink> pixelBlink;
  bn::optional<bn::regular_bg_ptr> background;
  bn::optional<bn::sprite_ptr> talkbox1;
  bn::optional<bn::sprite_ptr> talkbox2;
  bn::optional<bn::sprite_ptr> talkbox3;
  bn::optional<bn::sprite_ptr> talkbox4;
  bn::optional<bn::sprite_ptr> icon;
  unsigned videoFrame = 0;
  int extraSpeed = 0;
  // bn::unique_ptr<Menu> menu;
};

#endif  // UI_SCENE_H
