#ifndef UI_SCENE_H
#define UI_SCENE_H

#include "Scene.h"

#include "../objects/ui/Menu.h"
#include "../player/player_sfx.h"
#include "../utils/PixelBlink.h"

class UIScene : public Scene {
 public:
  UIScene(GameState::Screen _screen, const GBFS_FILE* _fs);

  virtual void init() override;
  bool updateUI();

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
  GameState::Screen skipScreen = GameState::Screen::NO;
  bool isWriting = false;
  bool hasMoreMessages = false;
  bool wantsToContinue = false;
  unsigned characterIndex = 0;
  bool characterWait = false;
  bn::unique_ptr<Menu> menu;
  bool pauseVideo = false;
  bn::fixed videoFrame = 0;
  bn::fixed extraSpeed = 0;
  bn::sprite_item dialogIcon;

  virtual void updateVideo();

  bool finishedWriting() {
    if (hasFinishedWriting) {
      hasFinishedWriting = false;
      return true;
    }
    return false;
  }

  virtual bool canSkipAutoWrite() { return true; }
  virtual bool canPause() { return true; }

  void write(bn::vector<bn::string<64>, 2> lines,
             bool _hasMoreMessages = false);
  void ask(bn::vector<Menu::Option, 32> options,
           bn::fixed scaleX = 1,
           bn::fixed scaleY = 1,
           bn::fixed positionX = 0,
           bn::fixed positionY = 0,
           bool withSound = true);
  void closeMenu(bool withSound = true);
  void closeText();
  void setDialogIcon(bn::sprite_item);

 private:
  bool hasFinishedWriting = false;
  bool isPaused = false;

  void setUpBlending();
  void startWriting();
  void stopWriting();
  void autoWrite();
  void finishAutoWrite();
  bn::string<64> removeSeparator(bn::string<64> str, char separator);

  bool processPauseInput();
  void pause();
  void showPauseMenu();
  void unpause();
  void processMenuOption(int option);
};

#endif  // UI_SCENE_H
