#ifndef START_SCENE_H
#define START_SCENE_H

#include "Scene.h"

#include "../objects/Horse.h"
#include "../objects/ui/Menu.h"
#include "../objects/ui/SettingsMenu.h"

class StartScene : public Scene {
 public:
  StartScene(const GBFS_FILE* _fs);

  void init() override;
  void update() override;

 private:
  bn::optional<bn::regular_bg_ptr> background;
  bn::unique_ptr<Horse> horse;
  bn::vector<bn::sprite_ptr, 64> textSprites;
  bn::sprite_text_generator textGenerator;
  bn::sprite_text_generator textGeneratorAccent;
  bn::unique_ptr<Menu> menu;
  bn::unique_ptr<SettingsMenu> settingsMenu;
  bn::sprite_ptr logo1;
  bn::sprite_ptr logo2;
  bn::sprite_ptr logo3;
  bn::sprite_ptr logo4;
  bn::fixed videoFrame = 0;
  int lastBeat = 0;
  bn::fixed extraSpeed = 0;

  void updateVideo();
  void processMenuOption(int option);
};

#endif  // START_SCENE_H
