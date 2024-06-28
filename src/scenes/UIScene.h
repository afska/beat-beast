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
  bn::vector<bn::sprite_ptr, 128> textSprites;
  bn::vector<bn::sprite_ptr, 128> menuSprites;
  bn::sprite_text_generator textGenerator;
  bn::sprite_text_generator textGeneratorAccent;
  bn::sprite_text_generator menuTextGenerator;
  bn::sprite_text_generator menuTextGeneratorAccent;
  bn::unique_ptr<PixelBlink> pixelBlink;
  bn::optional<bn::regular_bg_ptr> background;
  bn::optional<bn::sprite_ptr> talkbox1;
  bn::optional<bn::sprite_ptr> talkbox2;
  bn::optional<bn::sprite_ptr> talkbox3;
  bn::optional<bn::sprite_ptr> talkbox4;
  bn::optional<bn::sprite_ptr> icon;
  bn::optional<bn::sprite_ptr> continueIcon;
  bn::vector<bn::string<64>, 2> textLines;
  bool isWriting = false;
  bool hasFinishedWriting = false;
  bool hasMoreMessages = false;
  bool wantsToContinue = false;
  unsigned characterIndex = 0;
  bool characterWait = false;

  unsigned videoFrame = 0;
  int extraSpeed = 0;
  bn::unique_ptr<Menu> menu;

  void write(bn::vector<bn::string<64>, 2> lines,
             bool _hasMoreMessages = false);
  void ask(bn::vector<Menu::Option, 32> options);
  void closeMenu();
  void closeText();

 private:
  void updateVideo();
  void startWriting();
  void stopWriting();
  void autoWrite();
  bn::string<64> removeSeparator(bn::string<64> str, char separator);
};

#endif  // UI_SCENE_H
