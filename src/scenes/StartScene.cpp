#include "StartScene.h"

#include "../assets/StartVideo.h"
#include "../assets/fonts/common_fixed_8x16_sprite_font.h"
#include "../assets/fonts/common_fixed_8x16_sprite_font_accent.h"
#include "../player/player.h"
#include "../savefile/SaveFile.h"
#include "../utils/Math.h"
#include "bn_blending.h"
#include "bn_sprite_items_start_logo1.h"
#include "bn_sprite_items_start_logo2.h"
#include "bn_sprite_items_start_logo3.h"
#include "bn_sprite_items_start_logo4.h"

#define HORSE_X 40
#define HORSE_Y 90
#define BPM 85
#define BEAT_PREDICTION_WINDOW 100

StartScene::StartScene(const GBFS_FILE* _fs)
    : Scene(GameState::Screen::START, _fs),
      horse(bn::unique_ptr{new Horse({0, 0})}),
      textGenerator(common_fixed_8x16_sprite_font),
      textGeneratorAccent(common_fixed_8x16_sprite_font_accent),
      menu(bn::unique_ptr{new Menu(textGenerator, textGeneratorAccent)}),
      settingsMenu(
          bn::unique_ptr{new SettingsMenu(textGenerator, textGeneratorAccent)}),
      logo1(bn::sprite_items::start_logo1.create_sprite(
          Math::toAbsTopLeft({9, 19}, 64, 32))),
      logo2(bn::sprite_items::start_logo2.create_sprite(logo1.x() + 64,
                                                        logo1.y())),
      logo3(bn::sprite_items::start_logo3.create_sprite(logo2.x() + 64,
                                                        logo2.y())),
      logo4(bn::sprite_items::start_logo4.create_sprite(logo3.x() + 64 - 32 / 2,
                                                        logo3.y())) {
  horse->showGun = false;
  horse->setPosition({HORSE_X, HORSE_Y}, true);
  horse->update();
  updateVideo();
  logo1.set_blending_enabled(true);
  logo2.set_blending_enabled(true);
  logo3.set_blending_enabled(true);
  logo4.set_blending_enabled(true);
}

void StartScene::init() {
  bn::vector<Menu::Option, 10> options;
  options.push_back(Menu::Option{.text = "Play"});
  options.push_back(Menu::Option{.text = "Settings"});
  if (SaveFile::data.didFinishGame)
    options.push_back(Menu::Option{.text = "Credits"});
  menu->start(options, false);

  if (!PlaybackState.isLooping) {
    player_playGSM("lazer.gsm");
    player_setLoop(true);
  }
}

void StartScene::update() {
  horse->setPosition({HORSE_X, HORSE_Y}, true);
  horse->update();

  if (!credits) {
    menu->update();
    settingsMenu->update();
    if (menu->hasConfirmedOption()) {
      auto confirmedOption = menu->receiveConfirmedOption();
      processMenuOption(confirmedOption);
    }
    if (settingsMenu->getNextScreen() != GameState::Screen::NO)
      setNextScreen(settingsMenu->getNextScreen());
    if (settingsMenu->isClosing()) {
      menu->clickSound();
      settingsMenu->stop();
      init();
    }
  }

  const int PER_MINUTE = 71583;            // (1/60000) * 0xffffffff
  int audioLag = SaveFile::data.audioLag;  // (0 on real hardware)
  int msecs = PlaybackState.msecs - audioLag + BEAT_PREDICTION_WINDOW;
  int beat = Math::fastDiv(msecs * BPM, PER_MINUTE);
  bool isNewBeat = beat != lastBeat;
  lastBeat = beat;
  if (isNewBeat && !credits)
    extraSpeed = 10;

  if (isNewBeat && !credits) {
    horse->jump();
  }

  updateVideo();

  if (credits && (int)PlaybackState.msecs >= 4050 - SaveFile::data.audioLag) {
    setNextScreen(GameState::Screen::CREDITS);
  }
}

void StartScene::updateVideo() {
  background.reset();
  background = StartVideo::getFrame(videoFrame.floor_integer())
                   .create_bg((256 - Math::SCREEN_WIDTH) / 2,
                              (256 - Math::SCREEN_HEIGHT) / 2);
  background.get()->set_mosaic_enabled(true);
  extraSpeed = (bn::max(extraSpeed - 1, bn::fixed(0)));
  videoFrame += (1 + extraSpeed / 2) / 2;
  if (videoFrame >= 150)
    videoFrame = 0;

  auto alpha = 0.7 - bn::fixed(extraSpeed) / 20;
  if (alpha > 1)
    alpha = 1;
  if (alpha < 0)
    alpha = 0;
  bn::blending::set_transparency_alpha(alpha);
}

void StartScene::processMenuOption(int option) {
  switch (option) {
    case 0: {
      if (SaveFile::didCompleteTutorial()) {
        setNextScreen(GameState::Screen::SELECTION);
      } else {
        GameState::data.isPlaying = true;
        setNextScreen(GameState::Screen::STORY);
      }
      break;
    }
    case 1: {  // Settings
      menu->stop();
      menu->clickSound();
      settingsMenu->start();
      break;
    }
    case 2: {  // Credits
      player_playGSM("bonus.gsm");
      credits = true;
      break;
    }
    default: {
    }
  }
}
